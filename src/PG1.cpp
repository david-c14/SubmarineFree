#include "DS.hpp"
#include "dsp/digital.hpp"

template <int x>
struct PG_1 : DS_Module {
	enum ParamIds {
		PARAM_1,
		NUM_PARAMS = x
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
	DS_Schmitt schmitt[x];
	PulseGenerator pulse[x];

	PG_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		float deltaTime = 1.0f / engineGetSampleRate();
		for (int i = 0; i < x; i++) {
			if (schmitt[i].redge(this, inputs[INPUT_1 + i].value)) {
				pulse[i].process(deltaTime);
				pulse[i].trigger(powf(10.0f, params[PARAM_1 + i].value));
				outputs[OUTPUT_1 + i].value = voltage1;
			}
			else {
				outputs[OUTPUT_1 + i].value = pulse[i].process(deltaTime)?voltage1:voltage0;
			}
		}
	}
};

struct PG112 : ModuleWidget {
	PG112(PG_1<12> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/PG-112.svg")));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(Port::create<sub_port_blue>(Vec(4,19 + offset), Port::INPUT, module, PG_1<12>::INPUT_1 + i));

			addOutput(Port::create<sub_port_blue>(Vec(92,19 + offset), Port::OUTPUT, module, PG_1<12>::OUTPUT_1 + i));

			addParam(ParamWidget::create<sub_knob_small>(Vec(33,19.5 + offset), module, PG_1<12>::PARAM_1 + i, -5.0f, 2.0f, -2.0f));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelPG112 = Model::create<PG_1<12>, PG112>("SubmarineFree", "PG-112", "PG-112 Pulse Generators", LOGIC_TAG, MULTIPLE_TAG);
