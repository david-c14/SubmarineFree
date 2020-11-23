//SubTag DS TM W2

#include "shared/DS.hpp"

struct AG_2 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1,
		INPUT_A_2,
		INPUT_B_1,
		INPUT_B_2,
		INPUT_C_1,
		INPUT_C_2,
		INPUT_D_1,
		INPUT_D_2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		OUTPUT_2,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	AG_2() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override {
		for (unsigned int i = 0; i < 2; i++) {
			unsigned int accumulator; 
			if (!inputs[INPUT_A_1 + i].isConnected()) {
				continue;
			}
			int channels = inputs[INPUT_A_1 + i].getChannels();
			accumulator = getInput(channels, 0xFFFF, INPUT_A_1 + i);
			accumulator &= getInput(channels, 0xFFFF, INPUT_B_1 + i);
			accumulator &= getInput(channels, 0xFFFF, INPUT_C_1 + i);
			accumulator &= getInput(channels, 0xFFFF, INPUT_D_1 + i);
			setOutput(channels, OUTPUT_1, accumulator);
		}
	}
};

struct AG202 : SchemeModuleWidget {
	AG202(AG_2 *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 2; i++) {
			int offset = 174 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, AG_2::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,59.5 + offset), module, AG_2::INPUT_B_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,87.5 + offset), module, AG_2::INPUT_C_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,115.5 + offset), module, AG_2::INPUT_D_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,143.5 + offset), module, AG_2::OUTPUT_1 + i));
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
		drawBase(vg, "AG-202");
	}
};

Model *modelAG202 = createModel<AG_2, AG202>("AG-202");
