//SubTag DS TM W2 W6

#include "DS.hpp"

template <int x>
struct NG_1 : DS_Module {
	enum ParamIds {
		NUM_PARAMS
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

	NG_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override {
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].value = (inputs[INPUT_1 + i].value < midpoint())?voltage1:voltage0;
		}
	}
};

struct NG106 : SchemeModuleWidget {
	NG106(NG_1<6> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, NG_1<6>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,59.5 + offset), module, NG_1<6>::OUTPUT_1 + i));
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
		drawBase(vg, "NG-106");
		nvgStrokeWidth(vg, 1);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 6; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 15, 31.5 + i * 58);
			nvgLineTo(vg, 15, 59.5 + i * 58);
			nvgStroke(vg);
		}
	}
};

struct NG112 : SchemeModuleWidget {
	NG112(NG_1<12> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5,31.5 + offset), module, NG_1<12>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,31.5 + offset), module, NG_1<12>::OUTPUT_1 + i));
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
		drawBase(vg, "NG-112");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);	
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		for (int i = 0; i < 12; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 16, 31.5 + i * 29);
			nvgLineTo(vg, 39, 31.5 + i * 29);
			nvgMoveTo(vg, 59, 31.5 + i * 29);
			nvgLineTo(vg, 74, 31.5 + i * 29);
			nvgMoveTo(vg, 39, 20 + i * 29);
			nvgLineTo(vg, 55, 31.5 + i * 29);
			nvgLineTo(vg, 39, 43 + i * 29);
			nvgClosePath(vg);
			nvgCircle(vg, 57, 31.5 + i * 29, 2);
			nvgStroke(vg);
		}
	}
};

Model *modelNG106 = createModel<NG_1<6>, NG106>("NG-106");
Model *modelNG112 = createModel<NG_1<12>, NG112>("NG-112");
