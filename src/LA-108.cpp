#include "SubmarineFree.hpp"

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

	LA_108() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void LA_108::step() {
}

struct LA_Display : TransparentWidget {
	LA_108 *module;

	void draw(NVGcontext *vg) override {
	}
};

struct LA108 : ModuleWidget {
	LA108(LA_108 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/LA-108.svg")));

		{
			LA_Display * display = new LA_Display();
			display->module = module;
			display->box.pos = Vec(45, 15);
			display->box.size = Vec(box.size.x - 47, 304);
			addChild(display);
		}

		for (int i = 0; i < 8; i++) {
			addInput(Port::create<sub_port_blue>(Vec(4, 20 + 35 * i), Port::INPUT, module, LA_108::INPUT_1 + i));
			addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 42 + 38 * i), module, LA_108::LIGHT_1 + i));
		}

		addInput(Port::create<sub_port_blue>(Vec(4, 330), Port::INPUT, module, LA_108::INPUT_EXT));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(30, 351), module, LA_108::LIGHT_EXT));

		addParam(ParamWidget::create<sub_knob_med_snap>(Vec(48, 325), module, LA_108::PARAM_TRIGGER, 0.0f, 8.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_2>(Vec(88, 325), module, LA_108::PARAM_EDGE, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med_snap>(Vec(108, 325), module, LA_108::PARAM_TIME, -6.0f, -16.0f, -14.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(148, 325), module, LA_108::PARAM_INDEX_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(188, 325), module, LA_108::PARAM_INDEX_2, 0.0f, 1.0f, 1.0f));
	}
};

Model *modelLA108 = Model::create<LA_108, LA108>("SubmarineFree", "LA-108", "LA-108 Logic Analyser", LOGIC_TAG, VISUAL_TAG);
