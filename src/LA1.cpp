//SubTag DS W20

/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright © 2016 by Andrew Belt */
#include <string.h>
#include "DS.hpp"

#define BUFFER_SIZE 512

struct LA_108 : DS_Module {
	enum ParamIds {
		PARAM_TRIGGER,
		PARAM_EDGE,
		PARAM_TIME,
		PARAM_INDEX_1,
		PARAM_INDEX_2,
		PARAM_RUN,
		PARAM_RESET,
		PARAM_PRE,
		PARAM_COLORS,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		INPUT_2,
		INPUT_3,
		INPUT_4,
		INPUT_5,
		INPUT_6,
		INPUT_7,
		INPUT_8,
		INPUT_EXT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_1,
		LIGHT_2,
		LIGHT_3,
		LIGHT_4,
		LIGHT_5,
		LIGHT_6,
		LIGHT_7,
		LIGHT_8,
		LIGHT_EXT,
		NUM_LIGHTS
	};
	
	float buffer[8][BUFFER_SIZE] = {};
	int bufferIndex = 0;
	float frameIndex = 0;
	
	float preBuffer[8][32] = {};
	int preBufferIndex = 0;
	float preFrameIndex = 0;
	int preCount = 0;

	DS_Schmitt trigger;

	LA_108() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_TRIGGER, 0.0f, 8.0f, 0.0f, "Trigger input", "", 0.f, 1.f, 1.f);
		configParam(PARAM_EDGE, 0.0f, 1.0f, 0.0f, "Trigger on falling edge");
		configParam(PARAM_RUN, 0.0f, 1.0f, 0.0f, "One shot mode");
		configParam(PARAM_RESET, 0.0f, 1.0f, 0.0f, "Reset");
		configParam(PARAM_TIME, -6.0f, -16.0f, -14.0f, "Time base");
		configParam(PARAM_INDEX_1, 0.0f, 1.0f, 0.0f, "Left index position");
		configParam(PARAM_INDEX_2, 0.0f, 1.0f, 1.0f, "Right index position");
		configParam(PARAM_PRE, 0.0f, 32.0f, 0.0f, "Pre-trigger buffer size");
		configParam(PARAM_COLORS, 0.0f, 1.0f, 0.0f, "Match cable colors");
	}
	void process(const ProcessArgs &args) override;
	void startFrame(void);
};

void LA_108::startFrame() {
	frameIndex = 0;
	preCount = (int)(params[PARAM_PRE].getValue() + 0.5f);
	if (preCount) {
		for (int i = 0; i < 8; i++) {
			for (int s = 0; s < preCount; s++) {
				buffer[i][s] = preBuffer[i][(preBufferIndex + 64 - preCount + s) % 32];
			}
		}
		bufferIndex = preCount;
		return;
	}
	bufferIndex = 0;
}

void LA_108::process(const ProcessArgs &args) {
	// Set trigger lights
	for (int i = 0; i < 9; i++)
		lights[LIGHT_1 + i].setBrightness(params[PARAM_TRIGGER].getValue() == i);
	// Compute time
	float deltaTime = powf(2.0f, params[PARAM_TIME].getValue());
	int frameCount = (int)ceilf(deltaTime * args.sampleRate);
	
	// Add frame to preBuffer
	if (++preFrameIndex >= frameCount) {
		preFrameIndex = 0;
		for (int i = 0; i < 8; i++)
			preBuffer[i][preBufferIndex] = inputs[INPUT_1 + i].getVoltage();
		preBufferIndex++;
		if (preBufferIndex >= 32)
			preBufferIndex = 0;
	}

	// Add frame to buffer
	if (bufferIndex < BUFFER_SIZE) {
		if (++frameIndex >= frameCount) {
			frameIndex = 0;
			for (int i = 0; i < 8; i++)
				buffer[i][bufferIndex] = inputs[INPUT_1 + i].getVoltage();
			bufferIndex++;
		}
	}

	int triggerInput = LA_108::INPUT_1 + (int)(clamp(params[PARAM_TRIGGER].getValue(), 0.0f, 8.0f));
	int edge = (params[PARAM_EDGE].getValue() > 0.5f);
	
	// Are we waiting on the next trigger?
	if (bufferIndex >= BUFFER_SIZE) {
		// Trigger immediately if nothing connected to trigger input
		if (!inputs[triggerInput].isConnected()) {
			startFrame();
			return;
		}

		// Reset the Schmitt trigger so we don't trigger immediately if the input is high
		if (frameIndex == 0) {
			//trigger.set(edge);
		}
		frameIndex++;

		float gate = inputs[triggerInput].getVoltage();
		int triggered = trigger.edge(this, gate, edge);

		if (params[PARAM_RUN].getValue() < 0.5f) { // Continuous run mode
			params[PARAM_RESET].setValue(0.0f);
			// Reset if triggered
			float holdTime = 0.1f;
			if (triggered) {
				startFrame();
				return;
			}

			// Reset if we've waited too long
			if (frameIndex >= args.sampleRate * holdTime) {
				startFrame();
				return;
			}
		}
		else {
			if (params[PARAM_RESET].getValue() > 0.5f) {
				if (triggered) {
					startFrame();
					params[PARAM_RESET].setValue(0.0f);
					return;
				}
			}
		}
	}
}

struct LA_Display : TransparentWidget {
	LA_108 *module;
	PortWidget *ports[8];

	void drawTrace(NVGcontext *vg, float *values, float offset, NVGcolor col) {
		if (!values)
			return;
		nvgSave(vg);
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		nvgBeginPath(vg);
		for (int i = 0; i < BUFFER_SIZE; i++) {
			float x, y;
			x = (float)i / (BUFFER_SIZE - 1) * b.size.x;
			y = module->voltage0 -clamp(values[i], module->voltage0, module->voltage1);
			y *= 29.0f / (module->voltage1 - module->voltage0);
			y += offset;
			if (i == 0)
				nvgMoveTo(vg, x, y);
			else
				nvgLineTo(vg, x, y);
		} 
		nvgStrokeColor(vg, col);
		nvgLineCap(vg, NVG_ROUND);
		nvgMiterLimit(vg, 2.0f);
		nvgStrokeWidth(vg, 1.5f);
		nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
		nvgStroke(vg);
		nvgResetScissor(vg);
		nvgRestore(vg);	
	}

	void drawIndex(NVGcontext *vg, float value) {
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		value = value * b.size.x;

		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0x40));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, value, 0);
			nvgLineTo(vg, value, b.size.y);
			nvgClosePath(vg);
		}
		nvgStroke(vg);
		nvgResetScissor(vg);
	}

	void drawPre(NVGcontext *vg, float value) {
		if (value == 0.0f)
			return;
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		value = value * b.size.x;

		nvgStrokeColor(vg, nvgRGBA(0xff, 0x40, 0x40, 0x80));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, value, 0);
			nvgLineTo(vg, value, b.size.y);
			nvgClosePath(vg);
		}
		nvgStroke(vg);
		nvgResetScissor(vg);
	}

	void drawMask(NVGcontext *vg, float value) {
		if (value == 0.0f)
			return;
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		value = value * b.size.x;

		nvgFillColor(vg, nvgRGBA(0xff, 0x40, 0x40, 0x40));
		{
			nvgBeginPath(vg);
			nvgRect(vg, 0, 0, value, b.size.y);
			nvgClosePath(vg);
		}
		nvgFill(vg);
		nvgResetScissor(vg);
	}

	void drawEasterEgg(NVGcontext *vg) {
		nvgFillColor(vg, SUBLIGHTBLUE);
		scheme::drawLogoPath(vg, 100, 2.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 20, 37.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 160, 72.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 145, 107.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 75, 142.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 120, 177.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 40, 212.5f, 2.0f, 0);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 60, 247.5f, 2.0f, 0);
		nvgFill(vg);
	}

	void draw(const DrawArgs &args) override {
		if (!module) {
			drawEasterEgg(args.vg);
			return;
		}
		for (int i = 0; i < 8; i++) {
			if (module->inputs[LA_108::INPUT_1 + i].isConnected()) {
				NVGcolor col = getColor(i);
				drawTrace(args.vg, module->buffer[i], 32.5f + 35 * i, col); 
			}
		}
		drawIndex(args.vg, clamp(module->params[LA_108::PARAM_INDEX_1].getValue(), 0.0f, 1.0f));
		drawIndex(args.vg, clamp(module->params[LA_108::PARAM_INDEX_2].getValue(), 0.0f, 1.0f));
		drawMask(args.vg, clamp(module->params[LA_108::PARAM_PRE].getValue(), 0.0f, 32.0f) / BUFFER_SIZE);
		drawPre(args.vg, 1.0f * module->preCount / BUFFER_SIZE);
	}
	
	NVGcolor getColor(int i) {
		if (!module->params[LA_108::PARAM_COLORS].getValue())
			return SUBLIGHTBLUETRANS;
		return APP->scene->rack->getTopCable(ports[i])->color;
	}
};

struct LA_Measure : TransparentWidget {
	LA_108 *module;
	char measureText[41];

	void draw(const DrawArgs &args) override {
		if (!module) {
			return;
		}
		float deltaTime = powf(2.0f, module->params[LA_108::PARAM_TIME].getValue());
		int frameCount = (int)ceilf(deltaTime * APP->engine->getSampleRate());
		frameCount *= BUFFER_SIZE;
		float width = (float)frameCount * fabs(module->params[LA_108::PARAM_INDEX_1].getValue() - module->params[LA_108::PARAM_INDEX_2].getValue()) / APP->engine->getSampleRate(); 
		
		if (width < 0.00000995f)
			sprintf(measureText, "%4.3f\xc2\xb5s", width * 1000000.0f);
		else if (width < 0.0000995f)
			sprintf(measureText, "%4.2f\xc2\xb5s", width * 1000000.0f);
		else if (width < 0.000995f)
			sprintf(measureText, "%4.1f\xc2\xb5s", width * 1000000.0f);
		else if (width < 0.00995f)
			sprintf(measureText, "%4.3fms", width * 1000.0f);
		else if (width < 0.0995f)
			sprintf(measureText, "%4.2fms", width * 1000.0f);
		else if (width < 0.995f)
			sprintf(measureText, "%4.1fms", width * 1000.0f);
		else if (width < 9.95f)
			sprintf(measureText, "%4.3fs", width);
		else if (width < 99.5f)
			sprintf(measureText, "%4.2fs", width);
		else
			sprintf(measureText, "%4.1fs", width);
		nvgFontSize(args.vg, 14);
		nvgFontFaceId(args.vg, gScheme.font()->handle);
		nvgFillColor(args.vg, SUBLIGHTBLUE);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
		nvgText(args.vg, 27, 12, measureText, NULL);
	}
};


struct LA108 : SchemeModuleWidget {
	LightButton *resetButton;
	LA108(LA_108 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(300, 380);
		addChild(new SchemePanel(this->box.size));

		LA_Display * display = new LA_Display();
		display->module = module;
		display->box.pos = Vec(42, 15);
		display->box.size = Vec(box.size.x - 44, 280);
		addChild(display);

		LA_Measure * measure = new LA_Measure();
		measure->module = module;
		measure->box.pos = Vec(213, 297);
		measure->box.size = Vec(54, 16);
		addChild(measure);

		for (int i = 0; i < 8; i++) {
			PortWidget *port = createInputCentered<BluePort>(Vec(16.5, 32.5 + 35 * i), module, LA_108::INPUT_1 + i);
			addInput(port);
			display->ports[i] = port;
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(31.5, 23.5 + 35 * i), module, LA_108::LIGHT_1 + i));
		}

		addInput(createInputCentered<BluePort>(Vec(16.5, 322.5), module, LA_108::INPUT_EXT));
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(31.5, 313.5), module, LA_108::LIGHT_EXT));

		addParam(createParamCentered<SnapKnob<MedKnob<LightKnob>>>(Vec(58, 320), module, LA_108::PARAM_TRIGGER));
		addParam(createParamCentered<SubSwitch2>(Vec(89, 320.5), module, LA_108::PARAM_EDGE));
		addParam(createParamCentered<SubSwitch2>(Vec(115, 320.5), module, LA_108::PARAM_RUN));
		resetButton = createParamCentered<LightButton>(Vec(159, 320), module, LA_108::PARAM_RESET);
		addParam(resetButton);
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(190, 320), module, LA_108::PARAM_TIME));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(226, 327), module, LA_108::PARAM_INDEX_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(254, 327), module, LA_108::PARAM_INDEX_2));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(283, 327), module, LA_108::PARAM_PRE));
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
			EventWidgetMenuItem *vmi = createMenuItem<EventWidgetMenuItem>("Match Cable Colors");
			vmi->stepHandler = [=]() {
				vmi->rightText = CHECKMARK(module->params[LA_108::PARAM_COLORS].getValue());
			};
			vmi->clickHandler = [=]() {
				bool val = module->params[LA_108::PARAM_COLORS].getValue();
				module->params[LA_108::PARAM_COLORS].setValue(!val);
			};
			menu->addChild(vmi);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LA_108");
		
		//Scope
		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 42, 15, 256, 280, 2);
		nvgRoundedRect(vg, 213, 297, 54, 16, 2);
		nvgFill(vg);

		//Grid
		nvgStrokeColor(vg, nvgRGB(0x33, 0x33, 0x33));
		nvgStrokeWidth(vg, 1);
		nvgBeginPath(vg);
		for (int i = 0; i < 8; i++) {
			nvgMoveTo(vg, 42, 18.5 + 35 * i);
			nvgLineTo(vg, 298, 18.5 + 35 * i);
			nvgMoveTo(vg, 42, 47.5 + 35 * i);
			nvgLineTo(vg, 298, 47 + 35 * i);
		}
		nvgStroke(vg);
		
		//Silkscreen
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgBeginPath(vg);
		for (int i = 0; i < 8; i++) {
			nvgMoveTo(vg, 16.5, 32.5 + 35 * i);
			nvgLineTo(vg, 42, 32.5 + 35 * i);
		}
		nvgMoveTo(vg, 98.5, 316.5);
		nvgLineTo(vg, 101.5, 316.5);
		nvgLineTo(vg, 101.5, 310.5);
		nvgLineTo(vg, 104.5, 310.5);
		nvgMoveTo(vg, 98.5, 323.5);
		nvgLineTo(vg, 101.5, 323.5);
		nvgLineTo(vg, 101.5, 329.5);
		nvgLineTo(vg, 104.5, 329.5);
		nvgStroke(vg);

		drawText(vg, 16.5, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "EXT.TR"); 
		drawText(vg, 58, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TRIGGER"); 
		drawText(vg, 93, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "EDGE"); 
		drawText(vg, 128, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "MODE"); 
		drawText(vg, 125, 318, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CONT"); 
		drawText(vg, 125, 330, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ONCE"); 
		drawText(vg, 158, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "RESET"); 
		drawText(vg, 190, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TIME"); 
		drawText(vg, 240, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "L INDEX R"); 
		drawText(vg, 283, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "PRE."); 

	}
};

Model *modelLA108 = createModel<LA_108, LA108>("LA-108");
