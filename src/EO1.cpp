/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright 2017 by Andrew Belt */
#include <string.h>
#include "DS.hpp"

#define BUFFER_SIZE 512

struct EO_102 : DS_Module {
	enum ParamIds {
		PARAM_TRIGGER,
		PARAM_EDGE,
		PARAM_TIME,
		PARAM_INDEX_1,
		PARAM_INDEX_2,
		PARAM_RUN,
		PARAM_RESET,
		PARAM_PRE,
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
		LIGHT_2,
		LIGHT_EXT,
		NUM_LIGHTS
	};
	
	float buffer[2][BUFFER_SIZE] = {};
	int bufferIndex = 0;
	float frameIndex = 0;
	
	float preBuffer[2][32] = {};
	int preBufferIndex = 0;
	float preFrameIndex = 0;
	int preCount = 0;

	DS_Schmitt trigger;
	sub_btn *resetButtonWidget;

	EO_102() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	void startFrame(void);
};

void EO_102::startFrame() {
	frameIndex = 0;
	preCount = (int)(params[PARAM_PRE].value + 0.5f);
	if (preCount) {
		for (int i = 0; i < 2; i++) {
			for (int s = 0; s < preCount; s++) {
				buffer[i][s] = preBuffer[i][(preBufferIndex + 64 - preCount + s) % 32];
			}
		}
		bufferIndex = preCount;
		return;
	}
	bufferIndex = 0;
}

void EO_102::step() {
	// Set trigger lights
	for (int i = 0; i < 3; i++)
		lights[LIGHT_1 + i].value = (params[PARAM_TRIGGER].value == i);
	// Compute time
	float deltaTime = powf(2.0f, params[PARAM_TIME].value);
	int frameCount = (int)ceilf(deltaTime * engineGetSampleRate());
	
	// Add frame to preBuffer
	if (++preFrameIndex >= frameCount) {
		preFrameIndex = 0;
		for (int i = 0; i < 2; i++)
			preBuffer[i][preBufferIndex] = inputs[INPUT_1 + i].value;
		preBufferIndex++;
		if (preBufferIndex >= 32)
			preBufferIndex = 0;
	}

	// Add frame to buffer
	if (bufferIndex < BUFFER_SIZE) {
		if (++frameIndex >= frameCount) {
			frameIndex = 0;
			for (int i = 0; i < 2; i++)
				buffer[i][bufferIndex] = inputs[INPUT_1 + i].value;
			bufferIndex++;
		}
	}

	int triggerInput = INPUT_1 + (int)(clamp(params[PARAM_TRIGGER].value, 0.0f, 2.0f));
	int edge = (params[PARAM_EDGE].value > 0.5f);
	
	// Are we waiting on the next trigger?
	if (bufferIndex >= BUFFER_SIZE) {
		// Trigger immediately if nothing connected to trigger input
		if (!inputs[triggerInput].active) {
			startFrame();
			return;
		}

		// Reset the Schmitt trigger so we don't trigger immediately if the input is high
		if (frameIndex == 0) {
			//trigger.set(edge);
		}
		frameIndex++;

		float gate = inputs[triggerInput].value;
		int triggered = trigger.edge(this, gate, edge);

		if (params[PARAM_RUN].value < 0.5f) { // Continuous run mode
			resetButtonWidget->setValue(0.0f);
			// Reset if triggered
			if (triggered) {
				startFrame();
				return;
			}
		}
		else {
			if (params[PARAM_RESET].value > 0.5f) {
				if (triggered) {
					startFrame();
					resetButtonWidget->setValue(0.0f);
					return;
				}
			}
		}
	}
}

struct EO_Display : TransparentWidget {
	EO_102 *module;

	void drawTrace(NVGcontext *vg, float *values, float offset) {
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
		nvgStrokeColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xc0));
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

	void draw(NVGcontext *vg) override {
		for (int i = 0; i < 2; i++) {
			if (module->inputs[EO_102::INPUT_1 + i].active) {
				drawTrace(vg, module->buffer[i], 32.5f + 35 * i); 
			}
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
			addInput(Port::create<sub_port_blue>(Vec(4, 20 + 35 * i), Port::INPUT, module, EO_102::INPUT_1 + i));
			addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 22 + 35 * i), module, EO_102::LIGHT_1 + i));
		}

		addInput(Port::create<sub_port_blue>(Vec(4, 310), Port::INPUT, module, EO_102::INPUT_EXT));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 312), module, EO_102::LIGHT_EXT));

		addParam(ParamWidget::create<SnapKnob<MedKnob<LightKnob>>>(Vec(39, 301), module, EO_102::PARAM_TRIGGER, 0.0f, 2.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(82, 308), module, EO_102::PARAM_EDGE, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(108, 308), module, EO_102::PARAM_RUN, 0.0f, 1.0f, 0.0f));
		module->resetButtonWidget = ParamWidget::create<sub_btn>(Vec(151, 312), module, EO_102::PARAM_RESET, 0.0f, 1.0f, 0.0f);
		addParam(module->resetButtonWidget);
		addParam(ParamWidget::create<MedKnob<LightKnob>>(Vec(171, 301), module, EO_102::PARAM_TIME, -6.0f, -16.0f, -14.0f));
		addParam(ParamWidget::create<SmallKnob<LightKnob>>(Vec(214, 315), module, EO_102::PARAM_INDEX_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<SmallKnob<LightKnob>>(Vec(242, 315), module, EO_102::PARAM_INDEX_2, 0.0f, 1.0f, 1.0f));
		addParam(ParamWidget::create<SnapKnob<SmallKnob<LightKnob>>>(Vec(271, 315), module, EO_102::PARAM_PRE, 0.0f, 32.0f, 0.0f));
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelEO102 = Model::create<EO_102, EO102>("SubmarineFree", "EO-102", "EO-102 Envelope Oscilloscope", VISUAL_TAG);
