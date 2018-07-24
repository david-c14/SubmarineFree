#include "DS.hpp"

template <int x>
struct NG_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = x
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	NG_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].value = (inputs[INPUT_1 + i].value < midpoint())?voltage1:voltage0;
		}
	}
};

struct NG106 : ModuleWidget {
	NG106(NG_1<6> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/NG-106.svg")));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(Port::create<sub_port_blue>(Vec(2.5,19 + offset), Port::INPUT, module, NG_1<6>::INPUT_1 + i));

			addOutput(Port::create<sub_port_blue>(Vec(2.5,47 + offset), Port::OUTPUT, module, NG_1<6>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

struct NG112 : ModuleWidget {
	NG112(NG_1<12> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/NG-112.svg")));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(Port::create<sub_port_blue>(Vec(4,19 + offset), Port::INPUT, module, NG_1<12>::INPUT_1 + i));

			addOutput(Port::create<sub_port_blue>(Vec(62,19 + offset), Port::OUTPUT, module, NG_1<12>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelNG106 = Model::create<NG_1<6>, NG106>("SubmarineFree", "NG-106", "NG-106 NOT Gates", LOGIC_TAG, MULTIPLE_TAG);
Model *modelNG112 = Model::create<NG_1<12>, NG112>("SubmarineFree", "NG-112", "NG-112 NOT Gates", LOGIC_TAG, MULTIPLE_TAG);
