#include "DS.hpp"
#include <random>
#include <chrono>

template <int x>
struct DN_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};
	uint32_t lfsr[x];

	DN_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> distribution(1, 0xffffffffu);
		for (unsigned int i = 0; i < x; i++) {
			lfsr[i] = distribution(generator);
		}
	}

	void step() override {
		for (int i = 0; i < x; i++) {
			if (outputs[OUTPUT_1 + i].active) {
				unsigned int lsb = lfsr[i] & 1;
				lfsr[i] >>= 1;
				if (lsb)
					lfsr[i] ^= 0xc3000000u;	
				outputs[OUTPUT_1 + i].value = lsb?voltage1:voltage0;
			}
		}
	}
};

struct DN112 : ModuleWidget {
	DN112(DN_1<12> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/DN-112.svg")));

		for (int i = 0; i < 12; i++) {
			addOutput(Port::create<BluePort>(Vec(2.5,19 + 29 * i), Port::OUTPUT, module, DN_1<12>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		((DS_Module *)module)->appendContextMenu(menu);
	}
};

Model *modelDN112 = Model::create<DN_1<12>, DN112>("Submarine (Free)", "DN-112", "DN-112 Digital Noise", DIGITAL_TAG, NOISE_TAG, MULTIPLE_TAG);
