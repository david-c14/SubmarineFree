//SubTag DS TM W2

#include "DS.hpp"
#include <random>
#include <chrono>

static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());

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

struct DN112 : SchemeModuleWidget {
	DN112(DN_1<12> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 12; i++) {
			addOutput(createOutputCentered<BluePort>(Vec(15,31.5 + 29 * i), module, DN_1<12>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "DN-112");
	}
};

Model *modelDN112 = createModel<DN_1<12>, DN112>("DN-112");
