//SubTag DS TM W2 W6

#include "shared/DS.hpp"

template <int x>
struct AG_1 : DS_Module {
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

	AG_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < x; i++) {
			configInput(INPUT_A_1 + i, "Signal " + std::to_string(i + 1) + "A");
			configInput(INPUT_B_1 + i, "Signal " + std::to_string(i + 1) + "B");
			configOutput(OUTPUT_1 + i, "Signal " + std::to_string(i + 1));
		}
	}

	void process(const ProcessArgs &args) override {
		int connCount = 0;
		int setCount = 0;
		for (int i = 0; i < x; i++) {
			if (inputs[INPUT_A_1 + i].isConnected()) {
				connCount++;
				if (inputs[INPUT_A_1 + i].getVoltage() > midpoint())
					setCount++;
			}
			if (inputs[INPUT_B_1 + i].isConnected()) {
				connCount++;
				if (inputs[INPUT_B_1 + i].getVoltage() > midpoint())
					setCount++;
			}
			if (outputs[OUTPUT_1 + i].isConnected()) {
				if (connCount)
					outputs[OUTPUT_1 + i].setVoltage((connCount == setCount)?voltage1:voltage0);
				else
					outputs[OUTPUT_1 + i].setVoltage(voltage0);
				connCount = 0;
				setCount = 0;
			}
		}
	}
};

struct AG104 : SchemeModuleWidget {
	AG104(AG_1<4> *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 4; i++) {
			int offset = 87 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, AG_1<4>::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(15,59.5 + offset), module, AG_1<4>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,87.5 + offset), module, AG_1<4>::OUTPUT_1 + i));
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
		drawBase(vg, "AG-104");
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

struct AG106 : SchemeModuleWidget {
	AG106(AG_1<6> *module) {
		setModule(module);
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5,31.5 + offset), module, AG_1<6>::INPUT_A_1 + i));
			addInput(createInputCentered<BluePort>(Vec(16.5,59.5 + offset), module, AG_1<6>::INPUT_B_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,45.5 + offset), module, AG_1<6>::OUTPUT_1 + i));
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
		drawBase(vg, "AG-106");
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
			nvgLineTo(vg, 34, 31.5 + i * 58);
			nvgLineTo(vg, 34, 42.5 + i * 58);
			nvgLineTo(vg, 39, 42.5 + i * 58);
			nvgMoveTo(vg, 16, 59.5 + i * 58);
			nvgLineTo(vg, 34, 59.5 + i * 58);
			nvgLineTo(vg, 34, 48.5 + i * 58);
			nvgLineTo(vg, 39, 48.5 + i * 58);
			nvgMoveTo(vg, 55, 45.5 + i * 58);
			nvgLineTo(vg, 74, 45.5 + i * 58);
			nvgMoveTo(vg, 39, 34.5 + i * 58);
			nvgLineTo(vg, 39, 56.5 + i * 58);
			nvgLineTo(vg, 43.5, 56.5 + i * 58);
			nvgArc(vg, 43.5, 45.5 + i * 58, 11, M_PI / 2.0f, -M_PI / 2.0f, NVG_CCW);
			nvgClosePath(vg);
			nvgStroke(vg);
		}
	}
};

Model *modelAG104 = createModel<AG_1<4>, AG104>("AG-104");
Model *modelAG106 = createModel<AG_1<6>, AG106>("AG-106");
