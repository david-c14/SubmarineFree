//SubTag DS TM W2

#include "shared/DS.hpp"

struct XG_2 : DS_Module {
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

	XG_2() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(INPUT_A_1, "Signal 1A");
		configInput(INPUT_A_2, "Signal 2A");
		configInput(INPUT_B_1, "Signal 1B");
		configInput(INPUT_B_2, "Signal 2B");
		configInput(INPUT_C_1, "Signal 1C");
		configInput(INPUT_C_2, "Signal 2C");
		configInput(INPUT_D_1, "Signal 1D");
		configInput(INPUT_D_2, "Signal 2D");
		configOutput(OUTPUT_1, "Signal 1");
		configOutput(OUTPUT_2, "Signal 2");
	}

	void process(const ProcessArgs &args) override {
		for (unsigned int i = 0; i < 2; i++) {
			unsigned int accumulator; 
			if (!inputs[INPUT_A_1 + i].isConnected()) {
				setOutput(1, OUTPUT_1 + i, 0x0);
				continue;
			}
			int channels = inputs[INPUT_A_1 + i].getChannels();
			accumulator = getInput(channels, 0x0, INPUT_A_1 + i);
			accumulator ^= getInput(channels, 0x0, INPUT_B_1 + i);
			accumulator ^= getInput(channels, 0x0, INPUT_C_1 + i);
			accumulator ^= getInput(channels, 0x0, INPUT_D_1 + i);
			setOutput(channels, OUTPUT_1, accumulator);
		}
	}
};

struct XG202 : SchemeModuleWidget {
	XG202(XG_2 *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 2; i++) {
			int offset = 188 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, XG_2::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,59.5 + offset), module, XG_2::INPUT_B_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,87.5 + offset), module, XG_2::INPUT_C_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,115.5 + offset), module, XG_2::INPUT_D_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,150.5 + offset), module, XG_2::OUTPUT_1 + i));
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
		drawBase(vg, "XG-202");
	}
};

Model *modelXG202 = createModel<XG_2, XG202>("XG-202");
