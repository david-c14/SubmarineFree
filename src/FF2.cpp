#include "DS.hpp"

template <int x>
struct FF_2 : DS_Module {
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
	
	int state[x] = {};	
	DS_Schmitt schmittTrigger[x];

	FF_2() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
	for (int i = 0; i < x; i++) {
			if (inputs[INPUT_1 + i].active) {
				if (schmittTrigger[i].redge(this, inputs[INPUT_1 + i].value))
					state[i] = !state[i];	
			}
			else {
				if (i) {
					if (schmittTrigger[i].redge(this, state[i-1]?voltage0:voltage1))
						state[i] = !state[i];
				}
			}
			outputs[OUTPUT_1 + i].value = state[i]?voltage1:voltage0;
		}
	}
};

struct FF212 : ModuleWidget {
	FF212(FF_2<12> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/FF-212.svg")));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(Port::create<sub_port_blue>(Vec(4,19 + offset), Port::INPUT, module, FF_2<12>::INPUT_1 + i));

			addOutput(Port::create<sub_port_blue>(Vec(62,19 + offset), Port::OUTPUT, module, FF_2<12>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelFF212 = Model::create<FF_2<12>, FF212>("SubmarineFree", "FF-212", "FF-212 Edge Triggered Flip-Flops", LOGIC_TAG, MULTIPLE_TAG);
