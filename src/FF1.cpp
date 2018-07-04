#include "DS.hpp"

template <int deviceCount>
struct FF_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = deviceCount
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	int state[deviceCount] = {};	
	DS_Schmitt schmittTrigger[deviceCount];

	FF_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}

	void step() override {
		if (inputs[INPUT].active) {
			if (schmittTrigger[0].redge(this, inputs[INPUT].value))
				state[0] = !state[0];
		}
		outputs[OUTPUT_1].value = state[0]?voltage1:voltage0;
		for (int i = 1; i < deviceCount; i++) {
			if (schmittTrigger[i].redge(this, state[i-1]?voltage0:voltage1))
						state[i] = !state[i];
			outputs[OUTPUT_1 + i].value = state[i]?voltage1:voltage0;
		}
	}
};

struct FF110 : ModuleWidget {
	FF110(FF_1<10> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/FF-110.svg")));

		addInput(Port::create<sub_port_blue>(Vec(2.5,19), Port::INPUT, module, FF_1<10>::INPUT));

		for (int i = 0; i < 10; i++) {
			int offset = 29 * i;

			addOutput(Port::create<sub_port_blue>(Vec(2.5,77 + offset), Port::OUTPUT, module, FF_1<10>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

struct FF120 : ModuleWidget {
	FF120(FF_1<20> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/FF-120.svg")));

		addInput(Port::create<sub_port_blue>(Vec(17.5,19), Port::INPUT, module, FF_1<20>::INPUT));

		for (int i = 0; i < 20; i+=2) {
			int offset = 15 * i;

			addOutput(Port::create<sub_port_blue>(Vec(4,53 + offset), Port::OUTPUT, module, FF_1<20>::OUTPUT_1 + i));
			addOutput(Port::create<sub_port_blue>(Vec(31,68 + offset), Port::OUTPUT, module, FF_1<20>::OUTPUT_1 + i + 1));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelFF110 = Model::create<FF_1<10>, FF110>("SubmarineFree", "FF-110", "FF-110 10-Stage Flip-Flop Counter", LOGIC_TAG, MULTIPLE_TAG);
Model *modelFF120 = Model::create<FF_1<20>, FF120>("SubmarineFree", "FF-120", "FF-120 20-Stage Flip-Flop Counter", LOGIC_TAG, MULTIPLE_TAG);
