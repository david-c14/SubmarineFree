//SubTag DS W22

/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright © 2016 by Andrew Belt */
#include <string.h>
#include "shared/DS.hpp"

#define BUFFER_SIZE 512

struct LA_216 : DS_Module {
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
		INPUT_EXT,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_1,
		LIGHT_2 = LIGHT_1 + 16,
		LIGHT_EXT = LIGHT_2 + 16,
		NUM_LIGHTS
	};
	
	float buffer[32][BUFFER_SIZE] = {};
	int bufferIndex = 0;
	float frameIndex = 0;
	
	float preBuffer[32][32] = {};
	int preBufferIndex = 0;
	float preFrameIndex = 0;
	int preCount = 0;

	DS_Schmitt trigger;

	LA_216() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configSwitch(PARAM_TRIGGER, 0, 32, 0, "Trigger Input", { "Signal 1-1", "Signal 1-2", "Signal 1-3", "Signal 1-4", "Signal 1-5", "Signal 1-6", "Signal 1-7", "Signal 1-8", 
									 "Signal 1-9", "Signal 1-10", "Signal 1-11", "Signal 1-12", "Signal 1-13", "Signal 1-14", "Signal 1-15", "Signal 1-16",
									 "Signal 2-1", "Signal 2-2", "Signal 2-3", "Signal 2-4", "Signal 2-5", "Signal 2-6", "Signal 2-7", "Signal 2-8", 
									 "Signal 2-9", "Signal 2-10", "Signal 2-11", "Signal 2-12", "Signal 2-13", "Signal 2-14", "Signal 2-15", "Signal 2-16",
									 "External" });
		configSwitch(PARAM_EDGE, 0, 1, 0, "Trigger on", { "Rising Edge", "Falling Edge" });
		configSwitch(PARAM_RUN, 0, 1, 0, "Trigger Mode", { "Continuous", "One-Shot" });
		configSwitch(PARAM_RESET, 0, 1, 0, "Reset", { "Off", "Waiting" });
		configParam(PARAM_TIME, -2.0f, -16.0f, -14.0f, "Time Base", " Samples per point", 2.0f, 65536, 0.0f);
		configParam(PARAM_INDEX_1, 0.0f, 1.0f, 0.0f, "Left Index Position", "%", 0.0f, 100.0f, 0.0f);
		configParam(PARAM_INDEX_2, 0.0f, 1.0f, 1.0f, "Right Index Position", "%", 0.0f, 100.0f, 0.0f);
		configParam(PARAM_PRE, 0.0f, 32.0f, 0.0f, "Pre-trigger Buffer Size", " Points");
		configSwitch(PARAM_COLORS, 0, 1, 0, "Match cable colors", { "Off", "On" });
		configInput(INPUT_1, "Signal 1");
		configInput(INPUT_2, "Signal 2");
		configInput(INPUT_EXT, "External Trigger");
		configLight(LIGHT_1 + 0, "Trigger on Signal 1-1");
		configLight(LIGHT_1 + 1, "Trigger on Signal 1-2");
		configLight(LIGHT_1 + 2, "Trigger on Signal 1-3");
		configLight(LIGHT_1 + 3, "Trigger on Signal 1-4");
		configLight(LIGHT_1 + 4, "Trigger on Signal 1-5");
		configLight(LIGHT_1 + 5, "Trigger on Signal 1-6");
		configLight(LIGHT_1 + 6, "Trigger on Signal 1-7");
		configLight(LIGHT_1 + 7, "Trigger on Signal 1-8");
		configLight(LIGHT_1 + 8, "Trigger on Signal 1-9");
		configLight(LIGHT_1 + 9, "Trigger on Signal 1-10");
		configLight(LIGHT_1 + 10, "Trigger on Signal 1-11");
		configLight(LIGHT_1 + 11, "Trigger on Signal 1-12");
		configLight(LIGHT_1 + 12, "Trigger on Signal 1-13");
		configLight(LIGHT_1 + 13, "Trigger on Signal 1-14");
		configLight(LIGHT_1 + 14, "Trigger on Signal 1-15");
		configLight(LIGHT_1 + 15, "Trigger on Signal 1-16");
		configLight(LIGHT_2 + 0, "Trigger on Signal 2-1");
		configLight(LIGHT_2 + 1, "Trigger on Signal 2-2");
		configLight(LIGHT_2 + 2, "Trigger on Signal 2-3");
		configLight(LIGHT_2 + 3, "Trigger on Signal 2-4");
		configLight(LIGHT_2 + 4, "Trigger on Signal 2-5");
		configLight(LIGHT_2 + 5, "Trigger on Signal 2-6");
		configLight(LIGHT_2 + 6, "Trigger on Signal 2-7");
		configLight(LIGHT_2 + 7, "Trigger on Signal 2-8");
		configLight(LIGHT_2 + 8, "Trigger on Signal 2-9");
		configLight(LIGHT_2 + 9, "Trigger on Signal 2-10");
		configLight(LIGHT_2 + 10, "Trigger on Signal 2-11");
		configLight(LIGHT_2 + 11, "Trigger on Signal 2-12");
		configLight(LIGHT_2 + 12, "Trigger on Signal 2-13");
		configLight(LIGHT_2 + 13, "Trigger on Signal 2-14");
		configLight(LIGHT_2 + 14, "Trigger on Signal 2-15");
		configLight(LIGHT_2 + 15, "Trigger on Signal 2-16");
		configLight(LIGHT_EXT, "Trigger on External Signal");
	}

	void startFrame() {
		frameIndex = 0;
		preCount = (int)(params[PARAM_PRE].getValue() + 0.5f);
		if (preCount) {
			for (int i = 0; i < 32; i++) {
				for (int s = 0; s < preCount; s++) {
					buffer[i][s] = preBuffer[i][(preBufferIndex + 64 - preCount + s) % 32];
				}
			}
			bufferIndex = preCount;
			return;
		}
		bufferIndex = 0;
	}

	void process(const ProcessArgs &args) override {
		// Set trigger lights
		for (int i = 0; i < 33; i++)
			lights[LIGHT_1 + i].setBrightness(params[PARAM_TRIGGER].getValue() == i);
		// Compute time
		float deltaTime = powf(2.0f, params[PARAM_TIME].getValue());
		int frameCount = (int)ceilf(deltaTime * args.sampleRate);
		
		// Add frame to preBuffer
		if (++preFrameIndex >= frameCount) {
			preFrameIndex = 0;
			for (int i = 0; i < 16; i++) {
				preBuffer[i][preBufferIndex] = inputs[INPUT_1].getVoltage(i);
				preBuffer[i + 16][preBufferIndex] = inputs[INPUT_2].getVoltage(i);
			}
			preBufferIndex++;
			if (preBufferIndex >= 32)
				preBufferIndex = 0;
		}
	
		// Add frame to buffer
		if (bufferIndex < BUFFER_SIZE) {
			if (++frameIndex >= frameCount) {
				frameIndex = 0;
				for (int i = 0; i < 16; i++) {
					buffer[i][bufferIndex] = inputs[INPUT_1].getVoltage(i);
					buffer[i + 16][bufferIndex] = inputs[INPUT_2].getVoltage(i);
				}
				bufferIndex++;
			}
		}
	
		int triggerInput = (int)(clamp(params[PARAM_TRIGGER].getValue(), 0.0f, 32.0f));
		int triggerChannel = triggerInput % 16;
		triggerInput = triggerInput / 16;
		int edge = (params[PARAM_EDGE].getValue() > 0.5f);
		
		// Are we waiting on the next trigger?
		if (bufferIndex >= BUFFER_SIZE) {
			// Trigger immediately if nothing connected to trigger input
			if (!inputs[triggerInput].isConnected()) {
				startFrame();
				return;
			}
			if (inputs[triggerInput].getChannels() < (triggerChannel + 1)) {
				startFrame();
				return;
			}
	
			// Reset the Schmitt trigger so we don't trigger immediately if the input is high
			if (frameIndex == 0) {
				//trigger.set(edge);
			}
			frameIndex++;
	
			float gate = inputs[triggerInput].getVoltage(triggerChannel);
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
};

namespace {	
	struct LA_Display : Widget {
		LA_216 *module;
		PortWidget *ports[2];
	
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
				y *= 20.0f / (module->voltage1 - module->voltage0);
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
			nvgStrokeColor(vg, SUBLIGHTBLUE);
			nvgStrokeWidth(vg, 1);
			scheme::drawLogoPath(vg, 0, 1.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 14, 23.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 28, 45.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 42, 67.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 56, 89.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 70, 111.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 84, 133.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 98, 155.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 112, 177.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 126, 199.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 140, 221.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 154, 243.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 168, 265.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 182, 287.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 196, 309.0f, 1.333f, 0);
			nvgStroke(vg);
			scheme::drawLogoPath(vg, 210, 331.0f, 1.333f, 0);
			nvgStroke(vg);
		}
	
		void drawLayer(const DrawArgs &args, int layer) override {
			if (!module) {
				drawEasterEgg(args.vg);
				return;
			}
			if (layer == 1) {
				for (int i = 0; i < 2; i++) {
					if (module->inputs[LA_216::INPUT_1 + i].isConnected()) {
						NVGcolor col = getColor(i);
						for (int c = 0; c < module->inputs[LA_216::INPUT_1 + i].getChannels(); c++) {
							drawTrace(args.vg, module->buffer[i * 16 + c], 21.5f + 22 * c, col); 
						}
					}
				}
				drawIndex(args.vg, clamp(module->params[LA_216::PARAM_INDEX_1].getValue(), 0.0f, 1.0f));
				drawIndex(args.vg, clamp(module->params[LA_216::PARAM_INDEX_2].getValue(), 0.0f, 1.0f));
				drawMask(args.vg, clamp(module->params[LA_216::PARAM_PRE].getValue(), 0.0f, 32.0f) / BUFFER_SIZE);
				drawPre(args.vg, 1.0f * module->preCount / BUFFER_SIZE);
			}
			Widget::drawLayer(args, layer);
		}
		
		NVGcolor getColor(int i) {
			if (!module->params[LA_216::PARAM_COLORS].getValue()) {
				return i?SUBLIGHTREDTRANS:SUBLIGHTBLUETRANS;
			}
			NVGcolor col = APP->scene->rack->getTopCable(ports[i])->color;
			col.a = 1.0f;
			return col;
		}
	};

	struct LA_Measure : Widget {
		LA_216 *module;
		char measureText[41];
	
		void drawLayer(const DrawArgs &args, int layer) override {
			if (module && (layer == 1)) {
				float deltaTime = powf(2.0f, module->params[LA_216::PARAM_TIME].getValue());
				int frameCount = (int)ceilf(deltaTime * APP->engine->getSampleRate());
				frameCount *= BUFFER_SIZE;
				float width = (float)frameCount * fabs(module->params[LA_216::PARAM_INDEX_1].getValue() - module->params[LA_216::PARAM_INDEX_2].getValue()) / APP->engine->getSampleRate(); 
				
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
			Widget::drawLayer(args, layer);
		}
	};

} // end namespace

struct LA216 : SchemeModuleWidget {
	LightButton *resetButton;
	LA216(LA_216 *module) {
		setModule(module);
		this->box.size = Vec(330, 380);
		addChild(new SchemePanel(this->box.size));

		LA_Display * display = new LA_Display();
		display->module = module;
		display->box.pos = Vec(72, 14);
		display->box.size = Vec(box.size.x - 74, 352);
		addChild(display);

		LA_Measure * measure = new LA_Measure();
		measure->module = module;
		measure->box.pos = Vec(5.5, 307);
		measure->box.size = Vec(54, 16);
		addChild(measure);

		for (int i = 0; i < 2; i++) {
			PortWidget *port = createInputCentered<BluePort>(Vec(16.5 + 32 * i, 32.5), module, LA_216::INPUT_1 + i);
			addInput(port);
			display->ports[i] = port;
			for (int c = 0; c < 16; c++) {
				addChild(createLightCentered<TinyLight<BlueLight>>(Vec(63.5 + 4 * i, 26.5 + 22 * c), module, LA_216::LIGHT_1 + i * 16 + c));
			}
		}

		addInput(createInputCentered<BluePort>(Vec(16.5, 65.5), module, LA_216::INPUT_EXT));
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(31.5, 74.5), module, LA_216::LIGHT_EXT));

		addParam(createParamCentered<SnapKnob<MedKnob<LightKnob>>>(Vec(32.5, 208.5), module, LA_216::PARAM_TRIGGER));
		addParam(createParamCentered<SubSwitch2>(Vec(43, 65.5), module, LA_216::PARAM_EDGE));
		addParam(createParamCentered<SubSwitch2>(Vec(20, 115.5), module, LA_216::PARAM_RUN));
		resetButton = createParamCentered<LightButton>(Vec(16.5, 163), module, LA_216::PARAM_RESET);
		addParam(resetButton);
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(32.5, 270), module, LA_216::PARAM_TIME));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(18.5, 337), module, LA_216::PARAM_INDEX_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(46.5, 337), module, LA_216::PARAM_INDEX_2));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(48.5, 163), module, LA_216::PARAM_PRE));
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
			EventWidgetMenuItem *vmi = createMenuItem<EventWidgetMenuItem>("Match Cable Colors");
			vmi->stepHandler = [=]() {
				vmi->rightText = CHECKMARK(module->params[LA_216::PARAM_COLORS].getValue());
			};
			vmi->clickHandler = [=]() {
				bool val = module->params[LA_216::PARAM_COLORS].getValue();
				module->params[LA_216::PARAM_COLORS].setValue(!val);
			};
			menu->addChild(vmi);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LA-216");
		
		//Scope
		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 72, 14, 256, 352, 2);
		nvgRoundedRect(vg, 5, 307, 55, 16, 2);
		nvgFill(vg);

		//Grid
		nvgStrokeColor(vg, nvgRGB(0x33, 0x33, 0x33));
		nvgStrokeWidth(vg, 1);
		nvgBeginPath(vg);
		for (int i = 0; i < 16; i++) {
			nvgMoveTo(vg, 72, 15.5 + 22 * i);
			nvgLineTo(vg, 328, 15.5 + 22 * i);
			nvgMoveTo(vg, 72, 34.5 + 22 * i);
			nvgLineTo(vg, 328, 34.5 + 22 * i);
		}
		nvgStroke(vg);
		
		//Silkscreen
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgBeginPath(vg);
		nvgMoveTo(vg, 53.5, 61.5);
		nvgLineTo(vg, 56.5, 61.5);
		nvgLineTo(vg, 56.5, 55.5);
		nvgLineTo(vg, 59.5, 55.5);
		nvgMoveTo(vg, 53.5, 68.5);
		nvgLineTo(vg, 56.5, 68.5);
		nvgLineTo(vg, 56.5, 74.5);
		nvgLineTo(vg, 59.5, 74.5);
		nvgStroke(vg);

		drawText(vg, 16.5, 93, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "EXT.TR"); 
		drawText(vg, 48.5, 93, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "EDGE"); 
		drawText(vg, 35, 140, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "MODE"); 
		drawText(vg, 30, 113, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CONT"); 
		drawText(vg, 30, 125, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ONCE"); 
		drawText(vg, 16.5, 183, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "RESET"); 
		drawText(vg, 32.5, 240, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TRIGGER"); 
		drawText(vg, 32.5, 302, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TIME"); 
		drawText(vg, 32.5, 360, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "L INDEX R"); 
		drawText(vg, 48.5, 183, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "PRE."); 

	}
};

Model *modelLA216 = createModel<LA_216, LA216>("LA-216");
