//SubTag DS TM W4

#include "DS.hpp"
#include <random>
#include <chrono>

template <int x>
struct BB_1 : DS_Module {
	int doResetFlag = 0;
	int doRandomFlag = 0;
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_CLK,
		INPUT_CV,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	float sample[x] = {};	
	DS_Schmitt schmittTrigger;

	BB_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		if (doResetFlag) doReset();
		if (doRandomFlag) doRandomize();
		int triggered = true;
		if (inputs[INPUT_CLK].active) {
			triggered = schmittTrigger.redge(this, inputs[INPUT_CLK].value);
		}
		if (triggered) {
			for (int i = x - 1; i; i--)
				sample[i] = sample[i - 1];
			sample[0] = inputs[INPUT_CV].value;
		}
		for (int i = 0; i < x; i++)
			outputs[OUTPUT_1 + i].value = sample[i];
	}
	void doRandomize() {
		doRandomFlag = 0;
		std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_real_distribution<float> distribution(voltage0, voltage1);	
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].value = sample[i] = distribution(generator); 
		}
	}
	void doReset() {
		doResetFlag = 0;
		for (int i = 0; i < x; i++)
			outputs[OUTPUT_1 + i].value = sample[i] = 0.0f;
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

struct BB120 : SchemeModuleWidget {
	BB120(BB_1<20> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(60, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BluePort>(Vec(17,31.5), module, BB_1<20>::INPUT_CLK));
		addInput(createInputCentered<SilverPort>(Vec(44,46.5), module, BB_1<20>::INPUT_CV));

		for (int i = 0; i < 20; i+=2) {
			int offset = 15 * i;

			addOutput(createOutputCentered<SilverPort>(Vec(16.5,65.5 + offset), module, BB_1<20>::OUTPUT_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(43.5,80.5 + offset), module, BB_1<20>::OUTPUT_1 + i + 1));
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
		drawBase(vg, "BB-120");
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 43.5, 46.5);
		nvgLineTo(vg, 16.5, 65.5);
		nvgLineTo(vg, 43.5, 80.5);
		for (int i = 0; i < 9; i++) {
			nvgLineTo(vg, 16.5, 95.5 + i * 30);
			nvgLineTo(vg, 43.5, 110.5 + i * 30);
		}
		nvgStroke(vg);
		drawText(vg, 43.5, 32, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "IN");
		drawText(vg, 16.5, 52, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "CLK");
	}
};

Model *modelBB120 = createModel<BB_1<20>, BB120>("BB-120");
