//SubTag DS TM W2 W6

#include "DS.hpp"
#include <random>
#include <chrono>

template <int x>
struct FF_2 : DS_Module {
	int doResetFlag = 0;
	int doRandomFlag = 0;
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
	
	int state[x] = {};	
	DS_Schmitt schmittTrigger[x];

	FF_2() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		if (doResetFlag) doReset();
		if (doRandomFlag) doRandomize();
		for (int i = 0; i < x; i++) {
			if (inputs[INPUT_1 + i].active) {
				if (schmittTrigger[i].redge(this, inputs[INPUT_1 + i].value))
					state[i] = !state[i];	
			}
			else {
				if (i) {
					if (schmittTrigger[i].fedge(this, state[i-1]?voltage1:voltage0))
						state[i] = !state[i];
				}
			}
			outputs[OUTPUT_1 + i].value = state[i]?voltage1:voltage0;
		}
	}
	void doRandomize() {
		doRandomFlag = 0;
		std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> distribution(0,1);
		for (int i = 0; i < x; i++) {
		 	state[i] = distribution(generator);
			if (i) if (!inputs[INPUT_1 + i].active) schmittTrigger[i].set(state[i-1]);
			outputs[OUTPUT_1 + i].value = state[i]?voltage1:voltage0;
		}
	}
	void doReset() {
		doResetFlag = 0;
		for (int i = 0; i < x; i++) {
			state[i] = 0;
			if (!inputs[INPUT_1 + i].active) schmittTrigger[i].reset();
			outputs[OUTPUT_1 + i].value = voltage0;
		}
	}
	void onRandomize() override {
		if (gPaused) {
			doRandomize();
		}
		else {
			doResetFlag = 0;
			doRandomFlag = 1;
		}
	}
	void onReset() override {
		if (gPaused) {
			doReset();
		}
		else {
			doRandomFlag = 0;
			doResetFlag = 1;
		}
	}
};

struct FF206 : SchemeModuleWidget {
	FF206(FF_2<6> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 6; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<BluePort>(Vec(15,31.5 + offset), module, FF_2<6>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,59.5 + offset), module, FF_2<6>::OUTPUT_1 + i));
		}
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override{
		drawBase(vg, "FF-206");
		nvgStrokeWidth(vg, 2);
		nvgLineCap(vg, NVG_ROUND);
		nvgStrokeColor(vg, gScheme.alternative);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 15, 31.5);
		nvgLineTo(vg, 15, 349.5);
		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.contrast);
		nvgBeginPath(vg);
		for (int i = 0; i < 6; i++) {
			nvgMoveTo(vg, 15, 31.5 + i * 58);
			nvgLineTo(vg, 15, 59.5 + i * 58);
		}
		nvgStroke(vg);
	}
};

struct FF212 : SchemeModuleWidget {
	FF212(FF_2<12> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < 12; i++) {
			int offset = 29 * i;
			addInput(createInputCentered<BluePort>(Vec(16.5,31.5 + offset), module, FF_2<12>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,31.5 + offset), module, FF_2<12>::OUTPUT_1 + i));
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
		drawBase(vg, "FF-212");
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgStrokeColor(vg, gScheme.contrast);
		nvgBeginPath(vg);
		for (int i = 0; i < 12; i++) {
			nvgMoveTo(vg, 16, 31.5 + i * 29);
			nvgLineTo(vg, 35, 31.5 + i * 29);
			nvgMoveTo(vg, 35, 36.5 + i * 29);
			nvgLineTo(vg, 38, 36.5 + i * 29);
			nvgLineTo(vg, 38, 26.5 + i * 29);
			nvgLineTo(vg, 45, 26.5 + i * 29);
			nvgLineTo(vg, 45, 36.5 + i * 29);
			nvgLineTo(vg, 52, 36.5 + i * 29);
			nvgLineTo(vg, 52, 26.5 + i * 29);
			nvgLineTo(vg, 55, 26.5 + i * 29);
			nvgMoveTo(vg, 55, 31.5 + i * 29);
			nvgLineTo(vg, 74, 31.5 + i * 29);
		}
		nvgStroke(vg);
		
		nvgStrokeColor(vg, gScheme.alternative);
		nvgBeginPath(vg);
		for (int i = 0; i < 11; i++) {
			nvgMoveTo(vg, 74, 31.5 + i * 29);
			nvgLineTo(vg, 48, 44.5 + i * 29);
			nvgMoveTo(vg, 36, 50.5 + i * 29);
			nvgLineTo(vg, 16, 60.5 + i * 29);
			nvgMoveTo(vg, 40, 48.5 + i * 29);
			nvgLineTo(vg, 46, 40.5 + i * 29);
			nvgLineTo(vg, 50, 48.5 + i * 29);
			nvgClosePath(vg);
			nvgCircle(vg, 38, 49.5 + i * 29, 2);
		}
		nvgStroke(vg);
	}
};

Model *modelFF206 = createModel<FF_2<6>, FF206>("FF-206");
Model *modelFF212 = createModel<FF_2<12>, FF212>("FF-212");
