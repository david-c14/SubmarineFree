#include "SubmarineFree.hpp"
#include "dsp/digital.hpp"

#define BUFFER_SIZE 512

struct LA_108 : Module {
	enum ParamIds {
		PARAM_TRIGGER,
		PARAM_EDGE,
		PARAM_TIME,
		PARAM_INDEX_1,
		PARAM_INDEX_2,
		PARAM_RUN,
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

	SchmittTrigger trigger;

	LA_108() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void LA_108::step() {
	for (int i = 0; i < 9; i++)
		lights[LIGHT_1 + i].value = (params[PARAM_TRIGGER].value == i);
	// Compute time
	float deltaTime = powf(2.0f, params[PARAM_TIME].value);
	int frameCount = (int)ceilf(deltaTime * engineGetSampleRate());

	// Add frame to buffer
	if (bufferIndex < BUFFER_SIZE) {
		if (++frameIndex > frameCount) {
			frameIndex = 0;
			for (int i = 0; i < 8; i++)
				buffer[i][bufferIndex] = inputs[INPUT_1 + i].value;
			bufferIndex++;
		}
	}

	int triggerInput = LA_108::INPUT_1 + (int)(clamp(params[PARAM_TRIGGER].value, 0.0f, 8.0f));
	
	// Are we waiting on the next trigger?
	if (bufferIndex >= BUFFER_SIZE) {
		// Trigger immediately if nothing connected to trigger input
		if (!inputs[triggerInput].active) {
			bufferIndex = 0;
			frameIndex = 0;
			return;
		}

		// Reset the Schmitt trigger so we don't trigger immediately if the input is high
		if (frameIndex == 0) {
			trigger.reset();
		}
		frameIndex++;

		float gate = inputs[triggerInput].value;
		if (params[PARAM_EDGE].value > 0.5f)
			gate = 5.0f - gate;

		// Reset if triggered
		float holdTime = 0.1f;
		if (trigger.process(gate)) {
			bufferIndex = 0; frameIndex = 0; return;
		}

		// Reset if we've waited too long
		if (frameIndex >= engineGetSampleRate() * holdTime) {
			bufferIndex = 0; frameIndex = 0; return;
		}
	}
}

struct LA_Display : TransparentWidget {
	LA_108 *module;

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
			y = offset - clamp(values[i], 0.0f, 5.0f) * 5.8f;
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

	void draw(NVGcontext *vg) override {
		for (int i = 0; i < 8; i++) {
			if (module->inputs[LA_108::INPUT_1 + i].active) {
				drawTrace(vg, module->buffer[i], 32.5f + 35 * i); 
			}
		}
		drawIndex(vg, clamp(module->params[LA_108::PARAM_INDEX_1].value, 0.0f, 1.0f));
		drawIndex(vg, clamp(module->params[LA_108::PARAM_INDEX_2].value, 0.0f, 1.0f));
	}
};

struct LA108 : ModuleWidget {
	LA108(LA_108 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/LA-108.svg")));

		{
			LA_Display * display = new LA_Display();
			display->module = module;
			display->box.pos = Vec(42, 15);
			display->box.size = Vec(box.size.x - 44, 280);
			addChild(display);
		}

		for (int i = 0; i < 8; i++) {
			addInput(Port::create<sub_port_blue>(Vec(4, 20 + 35 * i), Port::INPUT, module, LA_108::INPUT_1 + i));
			addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 22 + 35 * i), module, LA_108::LIGHT_1 + i));
		}

		addInput(Port::create<sub_port_blue>(Vec(4, 330), Port::INPUT, module, LA_108::INPUT_EXT));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 332), module, LA_108::LIGHT_EXT));

		addParam(ParamWidget::create<sub_knob_med_snap>(Vec(48, 325), module, LA_108::PARAM_TRIGGER, 0.0f, 8.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(88, 325), module, LA_108::PARAM_EDGE, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(108, 325), module, LA_108::PARAM_TIME, -6.0f, -16.0f, -14.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(148, 325), module, LA_108::PARAM_INDEX_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(188, 325), module, LA_108::PARAM_INDEX_2, 0.0f, 1.0f, 1.0f));
	}
};

Model *modelLA108 = Model::create<LA_108, LA108>("SubmarineFree", "LA-108", "LA-108 Logic Analyser", LOGIC_TAG, VISUAL_TAG);
