//SubTag DS TM W2 W6

#include "DS.hpp"

template <int x>
struct OG_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1,
		INPUT_B_1 = x,
		NUM_INPUTS = x + x
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	OG_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override {
		int setCount = 0;
		for (int i = 0; i < x; i++) {
			if (inputs[INPUT_A_1 + i].isConnected())
				if (inputs[INPUT_A_1 + i].getVoltage() > midpoint())
					setCount++;
			if (inputs[INPUT_B_1 + i].isConnected())
				if (inputs[INPUT_B_1 + i].getVoltage() > midpoint())
					setCount++;
			if (outputs[OUTPUT_1 + i].isConnected()) {
				outputs[OUTPUT_1 + i].setVoltage((setCount > 0)?voltage1:voltage0);
				setCount = 0;
			}
		}
	}
};

struct OG104 : SchemeModuleWidget {
	OG104(OG_1<4> *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 4; i++) {
			int offset = 87 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, OG_1<4>::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,59.5 + offset), module, OG_1<4>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,87.5 + offset), module, OG_1<4>::OUTPUT_1 + i));
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
		drawBase(vg, "OG-104");
		nvgBeginPath(vg);
		nvgMoveTo(vg, 15, 31.5);
		nvgLineTo(vg, 15, 348.5);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgStrokeWidth(vg, 1);
		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 4; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 15, 31.5 + i * 87);
			nvgLineTo(vg, 15, 87.5 + i * 87);
			nvgStroke(vg);
		}
	}
};

struct OG106 : SchemeModuleWidget {
	OG106(OG_1<6> *module) {
		setModule(module);
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5,31.5 + offset), module, OG_1<6>::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(16.5,59.5 + offset), module, OG_1<6>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,45.5 + offset), module, OG_1<6>::OUTPUT_1 + i));
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
		drawBase(vg, "OG-106");
		nvgStrokeWidth(vg, 1);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		for (int i = 0; i < 5; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 74.5, 45.5 + i * 58);
			nvgLineTo(vg, 74.5, 74.5 + i * 58);
			nvgLineTo(vg, 29.5, 74.5 + i * 58);
			nvgLineTo(vg, 29.5, 103.5 + i * 58);
			nvgLineTo(vg, 38.5, 103.5 + i * 58);
			nvgStroke(vg);
		}
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 6; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 16, 31.5 + i * 58);
			nvgLineTo(vg, 32, 31.5 + i * 58);
			nvgLineTo(vg, 32, 42.5 + i * 58);
			nvgLineTo(vg, 39, 42.5 + i * 58);
			nvgMoveTo(vg, 16, 59.5 + i * 58);
			nvgLineTo(vg, 32, 59.5 + i * 58);
			nvgLineTo(vg, 32, 48.5 + i * 58);
			nvgLineTo(vg, 39, 48.5 + i * 58);
			nvgMoveTo(vg, 55.5, 45.5 + i * 58);
			nvgLineTo(vg, 74, 45.5 + i * 58);
			nvgMoveTo(vg, 35.5, 34.5 + i * 58);
			nvgQuadTo(vg, 43, 45.5 + i * 58, 35.5, 56.5 + i * 58);
			nvgQuadTo(vg, 50.5, 55.5 + i * 58, 55.5, 45.5 + i * 58);
			nvgQuadTo(vg, 50.5, 35.5 + i * 58, 35.5, 34.5 + i * 58);
			nvgStroke(vg);
		}
	}
};

Model *modelOG104 = createModel<OG_1<4>, OG104>("OG-104");
Model *modelOG106 = createModel<OG_1<6>, OG106>("OG-106");
