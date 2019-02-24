//SubTag DS TM W2 W8

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

struct PG104 : SchemeModuleWidget {
	PG104(PG_1<4> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 4; i++) {
			int offset = 87 * i;
			addInput(createInputCentered<BluePort>(Vec(15, 31.5 + offset), module, PG_1<4>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15, 89.5 + offset), module, PG_1<4>::OUTPUT_1 + i));

			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 59.5 + offset), module, PG_1<4>::PARAM_1 + i, -5.0f, 2.0f, -2.0f));
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
		drawBase(vg, "PG-104");
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 2);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (int i = 0; i < 4; i++) {
			nvgMoveTo(vg, 15, 31.5 + 87 * i);
			nvgLineTo(vg, 15, 87.5 + 87 * i);
		}
		nvgStroke(vg);
	}
};

struct PG112 : SchemeModuleWidget {
	PG112(PG_1<12> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(120, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5, 31.5 + offset), module, PG_1<12>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(104.5, 31.5 + offset), module, PG_1<12>::OUTPUT_1 + i));

			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(45, 31.5 + offset), module, PG_1<12>::PARAM_1 + i, -5.0f, 2.0f, -2.0f));
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
		drawBase(vg, "PG-112");
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (int i = 0; i < 12; i++) {
			nvgMoveTo(vg, 12.5, 31.5 + 29 * i);
			nvgLineTo(vg, 68.5, 31.5 + 29 * i);
			nvgLineTo(vg, 68.5, 19.5 + 29 * i);
			nvgLineTo(vg, 74.5, 19.5 + 29 * i);
			nvgLineTo(vg, 74.5, 31.5 + 29 * i);
			nvgLineTo(vg, 99.5, 31.5 + 29 * i);
		}
		nvgStroke(vg);
	}
};

Model *modelPG104 = Model::create<PG_1<4>, PG104>("Submarine (Free)", "PG-104", "PG-104 Pulse Generators", LOGIC_TAG, MULTIPLE_TAG);
Model *modelPG112 = Model::create<PG_1<12>, PG112>("Submarine (Free)", "PG-112", "PG-112 Pulse Generators", LOGIC_TAG, MULTIPLE_TAG);
