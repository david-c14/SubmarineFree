/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright 2017 by Andrew Belt */
#include <string.h>
#include "SubmarineFree.hpp"
#include "dsp/digital.hpp"

#define BUFFER_SIZE 512

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
	
	float preBuffer[2][32] = {};
	int preBufferIndex = 0;
	float preFrameIndex = 0;
	int preCount = 0;

	SchmittTrigger trigger;
	PulseGenerator triggerLight;
	sub_btn *runningButtonWidget;
	float runMode;
	int traceMode[2];
	int traceStep;	

	EO_102() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	void startFrame(void);
};

void EO_102::startFrame() {
	triggerLight.trigger(0.1f);
	frameIndex = 0;
	preCount = (int)(params[PARAM_PRE].value + 0.5f);
	for (int i = 0; i < 2; i++) {
		for (int s = 0; s < preCount; s++) {
			buffer[i][s] = preBuffer[i][(preBufferIndex + 64 - preCount + s) % 32];
		}
		traceMode[i] = (int)(params[PARAM_MODE_1 + i].value + 0.5f);
	}
	bufferIndex = preCount;
	traceStep = 1;
}

void EO_102::step() {
	if (runMode > 0.5f) {
		if (params[PARAM_RUNMODE].value < 0.5f)
			runningButtonWidget->setValue(1.0f);
	}
	runMode = params[PARAM_RUNMODE].value;
	// Compute time
	float deltaTime = powf(2.0f, params[PARAM_TIME].value);
	int frameCount = (int)ceilf(deltaTime * engineGetSampleRate());
	lights[LIGHT_TRIGGER].value = triggerLight.process(engineGetSampleTime());
	
	// Add frame to preBuffer
	for (int i = 0; i < 2; i++) {
		if (params[PARAM_MODE_1 + i].value > 0.5f) {
			if (traceStep) {
				preBuffer[i][preBufferIndex] = fabs(inputs[INPUT_1 + i].value);
			}
			preBuffer[i][preBufferIndex] = std::max(preBuffer[i][preBufferIndex], (float)fabs(inputs[INPUT_1 + i].value));
		}
	}
	if (++preFrameIndex >= frameCount) {
		preFrameIndex = 0;
		for (int i = 0; i < 2; i++) {
			if (params[PARAM_MODE_1 + i].value < 0.5f) {
				preBuffer[i][preBufferIndex] = inputs[INPUT_1 + i].value;
			}
		}
		preBufferIndex++;
		if (preBufferIndex >= 32) {
			preBufferIndex = 0;
		}
	}

	// Add frame to buffer
	if (bufferIndex < BUFFER_SIZE) {
		for (int i = 0; i < 2; i++) {
			if (traceMode[i]) {
				if (traceStep) {
					buffer[i][bufferIndex] = fabs(inputs[INPUT_1 + i].value);
				}
				buffer[i][bufferIndex] = std::max(buffer[i][bufferIndex], (float)fabs(inputs[INPUT_1 + i].value));
			}
		}
		traceStep = 0;
		if (++frameIndex >= frameCount) {
			frameIndex = 0;
			for (int i = 0; i < 2; i++) {
				if (!traceMode[i]) {
					buffer[i][bufferIndex] = inputs[INPUT_1 + i].value;
				}
			}
			bufferIndex++;
			traceStep = 1;
		}
	}

	int triggerInput = INPUT_1;
	if (inputs[INPUT_EXT].active)
		triggerInput = INPUT_EXT;
	
	// Are we waiting on the next trigger?
	if (bufferIndex >= BUFFER_SIZE) {
		// Trigger immediately if nothing connected to trigger input
		if (!inputs[triggerInput].active) {
			startFrame();
			return;
		}

		// Reset the Schmitt trigger so we don't trigger immediately if the input is high
		if (frameIndex == 0) {
			trigger.reset();
		}
		frameIndex++;

		float gate = inputs[triggerInput].value;
		int triggered = trigger.process(rescale(gate, params[PARAM_TRIGGER].value - 0.1f, params[PARAM_TRIGGER].value, 0.0f, 1.0f)); 

		if (params[PARAM_RUN].value > 0.5f) {
			if (triggered) {
				startFrame();
				if (runMode > 0.5f) // Continuous run mode
					if (runningButtonWidget) // This is bad - try to move away from dependency on widget
						runningButtonWidget->setValue(0.0f);
				return;
			}
		}
	}
}

struct EO_Display : TransparentWidget {
	EO_102 *module;

	void drawTrace(NVGcontext *vg, float *values, float offset, float scale, NVGcolor col) {
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
			y = ((values[i] * scaling + offset ) / 20.0f - 0.5f) * -b.size.y;
			if (i == 0)
				nvgMoveTo(vg, x, y);
			else
				nvgLineTo(vg, x, y);
		} 
		if (1) {
			nvgStrokeColor(vg, col);
			nvgLineCap(vg, NVG_ROUND);
			nvgMiterLimit(vg, 2.0f);
			nvgStrokeWidth(vg, 1.5f);
			nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
			nvgStroke(vg);
		}
		else {
			nvgLineTo(vg, b.size.x, (offset / 20.0f - 0.5f) * -b.size.y);
			nvgLineTo(vg, 0, (offset / 20.0f - 0.5f) * -b.size.y);
			nvgClosePath(vg);
			nvgFillColor(vg, col);
			nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
			nvgFill(vg);
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

	void draw(NVGcontext *vg) override {
		NVGcolor col = nvgRGBA(0x28, 0xb0, 0xf3, 0xc0);
		for (int i = 0; i < 2; i++) {
			if (module->inputs[EO_102::INPUT_1 + i].active) {
				drawTrace(vg, module->buffer[i], module->params[EO_102::PARAM_OFFSET_1 + i].value, module->params[EO_102::PARAM_SCALE_1 + i].value, col); 
			}
			col = nvgRGBA(0xe1, 0x02, 0x78, 0xc0);
		}
		drawIndex(vg, clamp(module->params[EO_102::PARAM_INDEX_1].value, 0.0f, 1.0f));
		drawIndex(vg, clamp(module->params[EO_102::PARAM_INDEX_2].value, 0.0f, 1.0f));
		drawMask(vg, clamp(module->params[EO_102::PARAM_PRE].value, 0.0f, 32.0f) / BUFFER_SIZE);
		drawPre(vg, 1.0f * module->preCount / BUFFER_SIZE);
	}
};

struct EO_Measure : TransparentWidget {
	std::shared_ptr<Font> font;
	EO_102 *module;
	char measureText[41];

	EO_Measure() {
		font = Font::load(assetGlobal( "res/fonts/DejaVuSans.ttf"));
	}

	void draw(NVGcontext *vg) override {
		float deltaTime = powf(2.0f, module->params[EO_102::PARAM_TIME].value);
		int frameCount = (int)ceilf(deltaTime * engineGetSampleRate());
		frameCount *= BUFFER_SIZE;
		float width = (float)frameCount * fabs(module->params[EO_102::PARAM_INDEX_1].value - module->params[EO_102::PARAM_INDEX_2].value) / engineGetSampleRate(); 
		
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
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 27, 12, measureText, NULL);
	}
};


struct EO102 : ModuleWidget {
	EO102(EO_102 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/EO-102.svg")));

		{
			EO_Display * display = new EO_Display();
			display->module = module;
			display->box.pos = Vec(42, 15);
			display->box.size = Vec(box.size.x - 44, 280);
			addChild(display);
		}
		{
			EO_Measure * display = new EO_Measure();
			display->module = module;
			display->box.pos = Vec(213, 297);
			display->box.size = Vec(54, 16);
			addChild(display);
		}

		for (int i = 0; i < 2; i++) {
			addInput(Port::create<BluePort>(Vec(4, 20 + 35 * i), Port::INPUT, module, EO_102::INPUT_1 + i));
		}

		addInput(Port::create<BluePort>(Vec(4, 310), Port::INPUT, module, EO_102::INPUT_EXT));
		addParam(ParamWidget::create<SnapKnob<MedKnob<LightKnob>>>(Vec(4, 90), module, EO_102::PARAM_SCALE_1, -5.0f, 5.0f, 0.0f));
		addParam(ParamWidget::create<MedKnob<LightKnob>>(Vec(4, 130), module, EO_102::PARAM_OFFSET_1, -10.0f, 10.0f, 0.0f));
		addParam(ParamWidget::create<SnapKnob<MedKnob<LightKnob>>>(Vec(4, 170), module, EO_102::PARAM_SCALE_2, -5.0f, 5.0f, 0.0f));
		addParam(ParamWidget::create<MedKnob<LightKnob>>(Vec(4, 210), module, EO_102::PARAM_OFFSET_2, -10.0f, 10.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(4, 250), module, EO_102::PARAM_MODE_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(4, 290), module, EO_102::PARAM_MODE_2, 0.0f, 1.0f, 0.0f));

		addParam(ParamWidget::create<MedKnob<LightKnob>>(Vec(39, 301), module, EO_102::PARAM_TRIGGER, -10.0f, 10.0f, 0.0f));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(74, 333), module, EO_102::LIGHT_TRIGGER));
		addParam(ParamWidget::create<sub_sw_2>(Vec(108, 308), module, EO_102::PARAM_RUNMODE, 0.0f, 1.0f, 0.0f));
		module->runningButtonWidget = ParamWidget::create<sub_btn>(Vec(151, 312), module, EO_102::PARAM_RUN, 0.0f, 1.0f, 1.0f);
		addParam(module->runningButtonWidget);
		addParam(ParamWidget::create<MedKnob<LightKnob>>(Vec(171, 301), module, EO_102::PARAM_TIME, -6.0f, -16.0f, -14.0f));
		addParam(ParamWidget::create<SmallKnob<LightKnob>>(Vec(214, 315), module, EO_102::PARAM_INDEX_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<SmallKnob<LightKnob>>(Vec(242, 315), module, EO_102::PARAM_INDEX_2, 0.0f, 1.0f, 1.0f));
		addParam(ParamWidget::create<SnapKnob<SmallKnob<LightKnob>>>(Vec(271, 315), module, EO_102::PARAM_PRE, 0.0f, 32.0f, 0.0f));
	}
};

Model *modelEO102 = Model::create<EO_102, EO102>("SubmarineFree", "EO-102", "EO-102 Envelope Oscilloscope", VISUAL_TAG);
