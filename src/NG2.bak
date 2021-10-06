//SubTag DS TM W2

#include "shared/DS.hpp"

struct NG_2 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = 6
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = 6
	};
	enum LightIds {
		NUM_LIGHTS
	};

	NG_2() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override {
		for (int i = 0; i < 6; i++) {
			int channels = inputs[INPUT_1 + i].getChannels();
			unsigned int bitArray = getInput(channels, 0x0000, INPUT_1 + i); 
			setOutput(channels, OUTPUT_1 + i, ~bitArray);
		}
	}
};

struct NG206 : SchemeModuleWidget {
	NG206(NG_2 *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, NG_2::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,59.5 + offset), module, NG_2::OUTPUT_1 + i));
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
		drawBase(vg, "NG-206");
	}
};

Model *modelNG206 = createModel<NG_2, NG206>("NG-206");
