#include "DS.hpp"

template <int x>
struct BB_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_CLK,
		INPUT_CV,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	float sample[x] = {};	
	DS_Schmitt schmittTrigger;

	BB_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		int triggered = true;
		if (inputs[INPUT_CLK].active) {
			triggered = schmittTrigger.redge(this, inputs[INPUT_CLK].value);
		}
		if (triggered) {
			for (int i = x - 1; i; i--)
				sample[i] = sample[i - 1];
			sample[0] = inputs[INPUT_CV].value;
		}
		for (int i = 0; i < x; i++)
			outputs[OUTPUT_1 + i].value = sample[i];
	}
};

struct BB120 : ModuleWidget {
	BB120(BB_1<20> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/BB-120.svg")));

		addInput(Port::create<sub_port_blue>(Vec(4.5,19), Port::INPUT, module, BB_1<20>::INPUT_CLK));
		addInput(Port::create<sub_port>(Vec(31.5,34), Port::INPUT, module, BB_1<20>::INPUT_CV));

		for (int i = 0; i < 20; i+=2) {
			int offset = 15 * i;

			addOutput(Port::create<sub_port>(Vec(4,53 + offset), Port::OUTPUT, module, BB_1<20>::OUTPUT_1 + i));
			addOutput(Port::create<sub_port>(Vec(31,68 + offset), Port::OUTPUT, module, BB_1<20>::OUTPUT_1 + i + 1));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelBB120 = Model::create<BB_1<20>, BB120>("SubmarineFree", "BB-120", "BB-120 20-Stage Bucket Brigade Sample and Hold", LOGIC_TAG, DELAY_TAG, SAMPLE_AND_HOLD_TAG, MULTIPLE_TAG);
