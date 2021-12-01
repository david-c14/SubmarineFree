//SubTag DS TM W2 W6

#include "shared/DS.hpp"

template <int x>
struct OA_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_REF_HI,
		INPUT_REF_LO,
		INPUT_A_1,
		INPUT_B_1 = INPUT_A_1 + x,
		NUM_INPUTS = INPUT_B_1 + x 
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	OA_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configInput(INPUT_REF_HI, "Reference Signal 1");
		configInput(INPUT_REF_LO, "Reference Signal 2");
		for (int i = 0; i < x; i++) {
			configInput(INPUT_A_1 + i, "Signal " + std::to_string(i + 1) + "A");
			configInput(INPUT_B_1 + i, "Signal " + std::to_string(i + 1) + "B");
			configOutput(OUTPUT_1 + i, "Signal " + std::to_string(i + 1));
		}
	}

	void process(const ProcessArgs &args) override {
		float v1 = inputs[INPUT_REF_HI].isConnected()?inputs[INPUT_REF_HI].getVoltage(0):voltage1;
		float v0 = inputs[INPUT_REF_LO].isConnected()?inputs[INPUT_REF_LO].getVoltage(0):voltage0;
		for (int i = 0; i < x; i++) {
			if (inputs[INPUT_A_1 + i].isConnected() && inputs[INPUT_B_1 + i].isConnected()) {
				outputs[OUTPUT_1 + i].setVoltage(
					(inputs[INPUT_A_1 + i].getVoltage(0) > inputs[INPUT_B_1 + i].getVoltage(0))?v1:v0);
			}
		}
	}
};

struct OA103 : SchemeModuleWidget {
	OA103(OA_1<3> *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(15, 31.5), module, OA_1<3>::INPUT_REF_HI));
		for (int i = 0; i < 3; i++) {
			int offset = 87 * i;
			addInput(createInputCentered<SilverPort>(Vec(15,75 + offset), module, OA_1<3>::INPUT_A_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(15,103 + offset), module, OA_1<3>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15, 131 + offset), module, OA_1<3>::OUTPUT_1 + i));
		}
		addInput(createInputCentered<SilverPort>(Vec(15, 348.5), module, OA_1<3>::INPUT_REF_LO));
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "OA-103");
		nvgBeginPath(vg);
		nvgMoveTo(vg, 15, 31.5);
		nvgLineTo(vg, 15, 348.5);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgStrokeWidth(vg, 1);
		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 3; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 15, 75 + i * 87);
			nvgLineTo(vg, 15, 131 + i * 87);
			nvgStroke(vg);
		}
	}
};

struct OA105 : SchemeModuleWidget {
	OA105(OA_1<5> *module) {
		setModule(module);
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(45,31.5), module, OA_1<5>::INPUT_REF_HI));
		for (int i = 0; i < 5; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5,60.5 + offset), module, OA_1<5>::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(16.5,88.5 + offset), module, OA_1<5>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,74.5 + offset), module, OA_1<5>::OUTPUT_1 + i));
		}
		addInput(createInputCentered<SilverPort>(Vec(45, 348.5), module, OA_1<5>::INPUT_REF_LO));
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "OA-105");
		nvgStrokeWidth(vg, 1);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 45, 54.5);
		nvgLineTo(vg, 45, 325.5);
		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 5; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 16, 60.5 + i * 58);
			nvgLineTo(vg, 34, 60.5 + i * 58);
			nvgLineTo(vg, 34, 71.5 + i * 58);
			nvgLineTo(vg, 39, 71.5 + i * 58);
			nvgMoveTo(vg, 16, 88.5 + i * 58);
			nvgLineTo(vg, 34, 88.5 + i * 58);
			nvgLineTo(vg, 34, 77.5 + i * 58);
			nvgLineTo(vg, 39, 77.5 + i * 58);

			nvgMoveTo(vg, 55, 74.5 + i * 58);
			nvgLineTo(vg, 74, 74.5 + i * 58);
			nvgMoveTo(vg, 39, 63.5 + i * 58);
			nvgLineTo(vg, 39, 85.5 + i * 58);
			nvgLineTo(vg, 55, 74.5 + i * 58);
			nvgClosePath(vg);
			nvgStroke(vg);
		}
		drawText(vg, 45.5, 51, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 11, gScheme.getContrast(module), "+");
		drawText(vg, 45.5, 330, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 12, gScheme.getContrast(module), "-");
	}
};

Model *modelOA103 = createModel<OA_1<3>, OA103>("OA-103");
Model *modelOA105 = createModel<OA_1<5>, OA105>("OA-105");
