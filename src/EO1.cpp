//SubTag W27

/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright 2017 by Andrew Belt */
#include <string.h>
#include "SubmarineFree.hpp"

#define BUFFER_SIZE 800
#define PRE_SIZE 100

struct EO_102 : Module {
	enum ParamIds {
		PARAM_SCALE_1,
		PARAM_SCALE_2,
		PARAM_OFFSET_1,
		PARAM_OFFSET_2,
		PARAM_TRIGGER,
		PARAM_TIME,
		PARAM_INDEX_1,
		PARAM_INDEX_2,
		PARAM_INDEX_3,
		PARAM_RUNMODE,
		PARAM_RUN,
		PARAM_PRE,
		PARAM_MODE_1,
		PARAM_MODE_2,
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
		LIGHT_TRIGGER,
		NUM_LIGHTS
	};
	
	float buffer[2][BUFFER_SIZE] = {};
	int bufferIndex = 0;
	float frameIndex = 0;
	
	float preBuffer[2][PRE_SIZE] = {};
	int preBufferIndex = 0;
	float preFrameIndex = 0;
	int preCount = 0;

	dsp::SchmittTrigger trigger;
	dsp::PulseGenerator triggerLight;
	float runMode;
	int traceMode[2];
	int traceStep;	

	EO_102() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < 2; i++) {
			configParam(PARAM_MODE_1 + i, 0.0f, 1.0f, 0.0f);
			configParam(PARAM_OFFSET_1 + i, -10.0f, 10.0f, 0.0f);
			configParam(PARAM_SCALE_1 + i, -5.0f, 5.0f, 0.0f);
		}
		configParam(PARAM_TIME, -6.0f, -16.0f, -14.0f);
		configParam(PARAM_PRE, 0.0f, 1.0f * PRE_SIZE, 0.0f);
		configParam(PARAM_TRIGGER, -10.0f, 10.0f, 0.0f);
		configParam(PARAM_RUNMODE, 0.0f, 1.0f, 0.0f);
		configParam(PARAM_RUN, 0.0f, 1.0f, 1.0f);
		configParam(PARAM_INDEX_1, 0.0f, 1.0f, 0.0f);
		configParam(PARAM_INDEX_2, 0.0f, 1.0f, 1.0f);
		configParam(PARAM_INDEX_3, 0.0f, 1.0f, 0.2f);
	}
	void process(const ProcessArgs &args) override;
	void startFrame(void);
};

void EO_102::startFrame() {
	triggerLight.trigger(0.1f);
	frameIndex = 0;
	preCount = (int)(params[PARAM_PRE].getValue() + 0.5f);
	for (int i = 0; i < 2; i++) {
		for (int s = 0; s < preCount; s++) {
			buffer[i][s] = preBuffer[i][(preBufferIndex + (PRE_SIZE * 2) - preCount + s) % PRE_SIZE];
		}
		traceMode[i] = (int)(params[PARAM_MODE_1 + i].getValue() + 0.5f);
	}
	bufferIndex = preCount;
	traceStep = 1;
}

void EO_102::process(const ProcessArgs &args) {
	if (runMode > 0.5f) {
		if (params[PARAM_RUNMODE].getValue() < 0.5f) {
			params[PARAM_RUN].setValue(1.0f);
		}
	}
	runMode = params[PARAM_RUNMODE].getValue();
	// Compute time
	float deltaTime = powf(2.0f, params[PARAM_TIME].getValue());
	int frameCount = (int)ceilf(deltaTime * args.sampleRate);
	lights[LIGHT_TRIGGER].setBrightness(triggerLight.process(args.sampleTime));
	
	// Add frame to preBuffer
	for (int i = 0; i < 2; i++) {
		if (params[PARAM_MODE_1 + i].getValue() > 0.5f) {
			if (traceStep) {
				preBuffer[i][preBufferIndex] = fabs(inputs[INPUT_1 + i].getVoltage());
			}
			preBuffer[i][preBufferIndex] = std::max(preBuffer[i][preBufferIndex], (float)fabs(inputs[INPUT_1 + i].getVoltage()));
		}
	}
	if (++preFrameIndex >= frameCount) {
		preFrameIndex = 0;
		for (int i = 0; i < 2; i++) {
			if (params[PARAM_MODE_1 + i].getValue() < 0.5f) {
				preBuffer[i][preBufferIndex] = inputs[INPUT_1 + i].getVoltage();
			}
		}
		preBufferIndex++;
		if (preBufferIndex >= PRE_SIZE) {
			preBufferIndex = 0;
		}
	}

	// Add frame to buffer
	if (bufferIndex < BUFFER_SIZE) {
		for (int i = 0; i < 2; i++) {
			if (traceMode[i]) {
				if (traceStep) {
					buffer[i][bufferIndex] = fabs(inputs[INPUT_1 + i].getVoltage());
				}
				buffer[i][bufferIndex] = std::max(buffer[i][bufferIndex], (float)fabs(inputs[INPUT_1 + i].getVoltage()));
			}
		}
		traceStep = 0;
		if (++frameIndex >= frameCount) {
			frameIndex = 0;
			for (int i = 0; i < 2; i++) {
				if (!traceMode[i]) {
					buffer[i][bufferIndex] = inputs[INPUT_1 + i].getVoltage();
				}
			}
			bufferIndex++;
			traceStep = 1;
		}
	}

	int triggerInput = INPUT_1;
	if (inputs[INPUT_EXT].isConnected())
		triggerInput = INPUT_EXT;
	
	// Are we waiting on the next trigger?
	if (bufferIndex >= BUFFER_SIZE) {
		// Trigger immediately if nothing connected to trigger input
		if (!inputs[triggerInput].isConnected()) {
			startFrame();
			return;
		}

		// Reset the Schmitt trigger so we don't trigger immediately if the input is high
		if (frameIndex == 0) {
			trigger.reset();
		}
		frameIndex++;

		float gate = inputs[triggerInput].getVoltage();
		int triggered = trigger.process(rescale(gate, params[PARAM_TRIGGER].getValue() - 0.1f, params[PARAM_TRIGGER].getValue(), 0.0f, 1.0f)); 

		if (params[PARAM_RUN].getValue() > 0.5f) {
			if (triggered) {
				startFrame();
				if (runMode > 0.5f) {// Continuous run mode
					params[PARAM_RUN].setValue(0);
				}
				return;
			}
		}
	}
}

struct EO_Display : TransparentWidget {
	EO_102 *module;

	void drawTrace(NVGcontext *vg, float *values, float offset, float scale, NVGcolor col, int mode) {
		if (!values)
			return;
		float scaling = powf(2.0, scale);
		nvgSave(vg);
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		nvgBeginPath(vg);
		for (int i = 0; i < BUFFER_SIZE; i++) {
			float x, y;
			x = (float)i / (BUFFER_SIZE - 1) * b.size.x;
			y = ((values[i] * scaling + offset ) / 20.0f - 0.8f) * -b.size.y;
			if (i == 0)
				nvgMoveTo(vg, x, y);
			else
				nvgLineTo(vg, x, y);
		} 
		if (mode) {
			nvgLineTo(vg, b.size.x, (offset / 20.0f - 0.8f) * -b.size.y);
			nvgLineTo(vg, 0, (offset / 20.0f - 0.8f) * -b.size.y);
			nvgClosePath(vg);
			nvgFillColor(vg, col);
			nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
			nvgFill(vg);
		}
		else {
			nvgStrokeColor(vg, col);
			nvgLineCap(vg, NVG_ROUND);
			nvgMiterLimit(vg, 2.0f);
			nvgStrokeWidth(vg, 1.5f);
			nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
			nvgStroke(vg);
		}
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
	void drawIndexV(NVGcontext *vg, float value) {
		Rect b = Rect(Vec(0, 0), box.size);
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
		value = (1-value) * b.size.y;

		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0x40));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, 0, value);
			nvgLineTo(vg, b.size.x, value);
			nvgClosePath(vg);
		}
		nvgStroke(vg);
		nvgResetScissor(vg);
	}

	void drawTrigger(NVGcontext *vg, float value, float offset, float scale) {
		Rect b = Rect(Vec(0, 0), box.size);
		float scaling = powf(2.0f, scale);
		float y = ((value * scaling + offset ) / 20.0f - 0.8f) * -b.size.y;
		if (y < 0) return;
		if (y > b.size.y) return;
		nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);

		nvgStrokeColor(vg, nvgRGBA(0xff, 0xff, 0xff, 0x40));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, 0, y);
			nvgLineTo(vg, b.size.x, y);
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
		scheme::drawLogoPath(vg, 0, 0, 15, 0);
		nvgFillColor(vg, SUBLIGHTREDTRANS);
		nvgFill(vg);
		scheme::drawLogoPath(vg, 100, 50, 12, 0);
		nvgStrokeColor(vg, SUBLIGHTBLUETRANS);
		nvgStroke(vg);
	}

	void draw(const DrawArgs &args) override {
		if (!module) {
			drawEasterEgg(args.vg);
			return;
		}
		NVGcolor col = SUBLIGHTBLUETRANS;
		for (int i = 0; i < 2; i++) {
			if (module->inputs[EO_102::INPUT_1 + i].isConnected()) {
				drawTrace(args.vg, module->buffer[i], module->params[EO_102::PARAM_OFFSET_1 + i].getValue(), module->params[EO_102::PARAM_SCALE_1 + i].getValue(), col, module->traceMode[i]); 
			}
			col = SUBLIGHTREDTRANS;
		}
		drawIndex(args.vg, clamp(module->params[EO_102::PARAM_INDEX_1].getValue(), 0.0f, 1.0f));
		drawIndex(args.vg, clamp(module->params[EO_102::PARAM_INDEX_2].getValue(), 0.0f, 1.0f));
		drawIndexV(args.vg, clamp(module->params[EO_102::PARAM_INDEX_3].getValue(), 0.0f, 1.0f));
		if (module->inputs[EO_102::INPUT_EXT].isConnected())
			drawTrigger(args.vg, module->params[EO_102::PARAM_TRIGGER].getValue(), 0.0f, 1.0f);
		else
			drawTrigger(args.vg, module->params[EO_102::PARAM_TRIGGER].getValue(), module->params[EO_102::PARAM_OFFSET_1].getValue(), module->params[EO_102::PARAM_SCALE_1].getValue());
		drawMask(args.vg, clamp(module->params[EO_102::PARAM_PRE].getValue(), 0.0f, 1.0f * PRE_SIZE) / BUFFER_SIZE);
		drawPre(args.vg, 1.0f * module->preCount / BUFFER_SIZE);
	}
};

struct EO_Measure : TransparentWidget {
	EO_102 *module;
	char measureText[41] = "";
	NVGcolor col;

	virtual void updateText() {
	} 

	void draw(const DrawArgs &args) override {
		updateText();
		nvgFontSize(args.vg, 14);
		nvgFontFaceId(args.vg, gScheme.font()->handle);
		nvgFillColor(args.vg, col);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
		nvgText(args.vg, box.size.x / 2, 12, measureText, NULL);
	}
};

struct EO_Measure_Horz : EO_Measure {
	void updateText() override {
		if (!module) {
			return;
		} 
		float deltaTime = powf(2.0f, module->params[EO_102::PARAM_TIME].getValue());
		int frameCount = (int)ceilf(deltaTime * APP->engine->getSampleRate());
		frameCount *= BUFFER_SIZE;
		float width = (float)frameCount * fabs(module->params[EO_102::PARAM_INDEX_1].getValue() - module->params[EO_102::PARAM_INDEX_2].getValue()) / APP->engine->getSampleRate(); 
		
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
	}
};

struct EO_Measure_Vert : EO_Measure {
	int index = 0;
	void updateText() override {
		if (!module) {
			return; 
		}
		float height = ((module->params[EO_102::PARAM_INDEX_3].getValue() - 0.2f) * 20.0f - module->params[EO_102::PARAM_OFFSET_1 + index].getValue()) / powf(2, module->params[EO_102::PARAM_SCALE_1 + index].getValue());
		
		float ah = fabs(height);
		if (ah < 0.00000995f)
			sprintf(measureText, "%4.3f\xc2\xb5V", height * 1000000.0f);
		else if (ah < 0.0000995f)
			sprintf(measureText, "%4.2f\xc2\xb5V", height * 1000000.0f);
		else if (ah < 0.000995f)
			sprintf(measureText, "%4.1f\xc2\xb5V", height * 1000000.0f);
		else if (ah < 0.00995f)
			sprintf(measureText, "%4.3fmV", height * 1000.0f);
		else if (ah < 0.0995f)
			sprintf(measureText, "%4.2fmV", height * 1000.0f);
		else if (ah < 0.995f)
			sprintf(measureText, "%4.1fmV", height * 1000.0f);
		else if (ah < 9.95f)
			sprintf(measureText, "%4.3fV", height);
		else if (ah < 99.5f)
			sprintf(measureText, "%4.2fV", height);
		else
			sprintf(measureText, "%4.1fV", height);
	}
};

struct EO102 : SchemeModuleWidget {
	LightButton *paramRun;
	EO102(EO_102 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(405, 380);
		addChild(new SchemePanel(this->box.size));
		{
			EO_Display * display = new EO_Display();
			display->module = module;
			display->box.pos = Vec(2.5, 14);
			display->box.size = Vec(box.size.x - 5, 236);
			addChild(display);
		}
		{
			EO_Measure_Horz * display = new EO_Measure_Horz();
			display->module = module;
			display->box.pos = Vec(284, 272);
			display->box.size = Vec(54, 16);
			display->col = nvgRGBA(0xff, 0xff, 0xff, 0xff);
			addChild(display);
		}
		{
			EO_Measure_Vert * display = new EO_Measure_Vert();
			display->module = module;
			display->box.pos = Vec(341, 254);
			display->box.size = Vec(62, 16);
			display->index = 0;
			display->col = SUBLIGHTBLUE;
			addChild(display);
		}
		{
			EO_Measure_Vert * display = new EO_Measure_Vert();
			display->module = module;
			display->box.pos = Vec(341, 272);
			display->box.size = Vec(62, 16);
			display->index = 1;
			display->col = SUBLIGHTRED;
			addChild(display);
		}


		for (int i = 0; i < 2; i++) {
			addInput(createInputCentered<BluePort>(Vec(16.5 + 75 * i, 326.5), module, EO_102::INPUT_1 + i));
			addParam(createParamCentered<SubSwitch2>(Vec(16.5 + 75 * i, 280), module, EO_102::PARAM_MODE_1 + i));
			addParam(createParamCentered<MedKnob<LightKnob>>(Vec(50 + 75 * i, 320), module, EO_102::PARAM_OFFSET_1 + i));
			addParam(createParamCentered<SnapKnob<MedKnob<LightKnob>>>(Vec(50 + 75 * i, 270), module, EO_102::PARAM_SCALE_1 + i));
		}
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(172.5, 320), module, EO_102::PARAM_TIME));
		addParam(createParamCentered<SnapKnob<MedKnob<LightKnob>>>(Vec(172.5, 270), module, EO_102::PARAM_PRE));

		addInput(createInputCentered<BluePort>(Vec(211.5, 326.5), module, EO_102::INPUT_EXT));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(245, 320), module, EO_102::PARAM_TRIGGER));
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(226, 333), module, EO_102::LIGHT_TRIGGER));
		addParam(createParamCentered<SubSwitch2>(Vec(211.5, 280), module, EO_102::PARAM_RUNMODE));
		paramRun = createParamCentered<LightButton>(Vec(245, 280), module, EO_102::PARAM_RUN);
		addParam(paramRun);

		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(290, 320), module, EO_102::PARAM_INDEX_1));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(332, 320), module, EO_102::PARAM_INDEX_2));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(376, 320), module, EO_102::PARAM_INDEX_3));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "EO-102");

		// Display panels
		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 2.5, 14, 400, 236, 2);
		nvgRoundedRect(vg, 284, 272, 54, 16, 2);
		nvgRoundedRect(vg, 341, 254, 62, 16, 2);
		nvgRoundedRect(vg, 341, 272, 62, 16, 2);
		nvgFill(vg);
		
		// Rounded backgrounds for labels
		nvgFillColor(vg, gScheme.getContrast(module));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 5, 355, 65, 10, 5);
		nvgRoundedRect(vg, 80, 355, 65, 10, 5);
		nvgRoundedRect(vg, 155, 355, 35, 10, 5);
		nvgRoundedRect(vg, 200, 355, 65, 10, 5);
		nvgRoundedRect(vg, 275, 355, 125, 10, 5);
		nvgFill(vg);

		// Text
		drawText(vg, 37.5, 363, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, SUBLIGHTBLUE, "CHANNEL A");
		drawText(vg, 112.5, 363, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, SUBLIGHTRED, "CHANNEL B");
		drawText(vg, 172.5, 363, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getBackground(module), "TIME");
		drawText(vg, 232.5, 363, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getBackground(module), "TRIGGER");
		drawText(vg, 337.5, 363, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getBackground(module), "INDICES");
		drawText(vg, 16.5, 265, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CV");
		drawText(vg, 16.5, 302, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 11, gScheme.getContrast(module), "\xe2\x99\xaa");
		drawText(vg, 50, 298, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SCALE");
		drawText(vg, 50, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OFFSET");
		drawText(vg, 91.5, 265, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CV");
		drawText(vg, 91.5, 302, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 11, gScheme.getContrast(module), "\xe2\x99\xaa");
		drawText(vg, 125, 298, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SCALE");
		drawText(vg, 125, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OFFSET");
		drawText(vg, 172.5, 298, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "PRE");
		drawText(vg, 172.5, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TIME");
		drawText(vg, 211.5, 265, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CONT.");
		drawText(vg, 211.5, 302, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ONCE");
		drawText(vg, 245, 265, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "RUN");
		drawText(vg, 245, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LEVEL");
		drawText(vg, 290, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LEFT");
		drawText(vg, 332, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "RIGHT");
		drawText(vg, 376, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "HORZ");
	}
};

Model *modelEO102 = createModel<EO_102, EO102>("EO-102");
