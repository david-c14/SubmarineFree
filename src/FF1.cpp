//SubTag DS TM W2 W4

#include "DS.hpp"
#include <random>
#include <chrono>

template <int deviceCount>
struct FF_1 : DS_Module {
	int doResetFlag = 0;
	int doRandomFlag = 0;
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = deviceCount
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	int state[deviceCount] = {};	
	DS_Schmitt schmittTrigger[deviceCount];

	FF_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override {
		if (doResetFlag) doReset();
		if (doRandomFlag) doRandomize();
		if (inputs[INPUT].isConnected()) {
			if (schmittTrigger[0].redge(this, inputs[INPUT].getVoltage()))
				state[0] = !state[0];
		}
		outputs[OUTPUT_1].setVoltage(state[0]?voltage1:voltage0);
		for (int i = 1; i < deviceCount; i++) {
			if (schmittTrigger[i].fedge(this, state[i-1]?voltage1:voltage0))
						state[i] = !state[i];
			outputs[OUTPUT_1 + i].setVoltage(state[i]?voltage1:voltage0);
		}
	}
	void doRandomize() {
		doRandomFlag = 0;
		std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_int_distribution<int> distribution(0,1);
		state[0] = distribution(generator);
		outputs[OUTPUT_1].setVoltage(state[0]?voltage1:voltage0);
		for (int i = 1; i < deviceCount; i++) {
			state[i] = distribution(generator);
			schmittTrigger[i].set(state[i-1]);
			outputs[OUTPUT_1 + i].setVoltage(state[i]?voltage1:voltage0);
		}
	}
	void doReset() {
		doResetFlag = 0;
		for (int i = 0; i < deviceCount; i++) {
			state[i] = 0;
			if (i) schmittTrigger[i].reset();
			outputs[OUTPUT_1 + i].setVoltage(voltage0);
		}
	}
	void onRandomize() override {
		if (APP->engine->isPaused()) {
			doRandomize();
		}
		else {
			doResetFlag = 0;
			doRandomFlag = 1;
		}
	}
	void onReset() override {
		if (APP->engine->isPaused()) {
			doReset();
		}
		else {
			doRandomFlag = 0;
			doResetFlag = 1;
		}
	}
};

struct FF110 : SchemeModuleWidget {
	FF110(FF_1<10> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BluePort>(Vec(15,31.5), module, FF_1<10>::INPUT));

		for (int i = 0; i < 10; i++) {
			int offset = 29 * i;

			addOutput(createOutputCentered<BluePort>(Vec(15,89.5 + offset), module, FF_1<10>::OUTPUT_1 + i));
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
		drawBase(vg, "FF-110");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 15, 89.5);
		nvgLineTo(vg, 15, 350.5);
		nvgStroke(vg);
		drawText(vg, 15, 52, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CLOCK");
	}
};

struct FF120 : SchemeModuleWidget {
	FF120(FF_1<20> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(60, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BluePort>(Vec(30,31.5), module, FF_1<20>::INPUT));

		for (int i = 0; i < 20; i+=2) {
			int offset = 15 * i;

			addOutput(createOutputCentered<BluePort>(Vec(16.5,65.5 + offset), module, FF_1<20>::OUTPUT_1 + i));
			addOutput(createOutputCentered<BluePort>(Vec(43.5,80.5 + offset), module, FF_1<20>::OUTPUT_1 + i + 1));
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
		drawBase(vg, "FF-120");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 16.5, 65.5);
		nvgLineTo(vg, 43.5, 80.5);
		for (int i = 0; i < 9; i++) {
			nvgLineTo(vg, 16.5, 95.5 + i * 30);
			nvgLineTo(vg, 43.5, 110.0 + i * 30);
		}
		nvgStroke(vg);
		drawText(vg, 30, 52, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CLOCK");
	}
};

Model *modelFF110 = createModel<FF_1<10>, FF110>("FF-110");
Model *modelFF120 = createModel<FF_1<20>, FF120>("FF-120");
