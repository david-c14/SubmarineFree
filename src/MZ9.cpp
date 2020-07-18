//SubTag W2

#include "SubmarineFree.hpp"
#include <chrono>

struct MZ_999 : Module {
	enum ParamIds {
		PARAM_ON,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	
	MZ_999() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_ON, 1.0f, 2.0f, 1.0f, "On");
	}

	void process(const ProcessArgs &args) override {
		unsigned int poly = inputs[INPUT_1].getChannels();
		outputs[OUTPUT_1].setChannels(poly);
		for (unsigned int i = 0; i < poly; i++) {
			outputs[OUTPUT_1].setVoltage(inputs[INPUT_1].getPolyVoltage(i), i);	
		}
	}
};

struct MZ999 : SchemeModuleWidget {
	LightButton *onButton;
	MZ999(MZ_999 *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));


		addInput(createInputCentered<SilverPort>(Vec(15, 285), module, MZ_999::INPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(15, 325), module, MZ_999::OUTPUT_1));
		onButton = createParamCentered<LightButton>(Vec(15, 40), module, MZ_999::PARAM_ON);
		addParam(onButton);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "MZ-999");
		
		drawText(vg, 15, 60, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ON"); 
		drawText(vg, 15, 308, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "IN"); 
		drawText(vg, 15, 350, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT"); 
	}
	void step() override {
		SchemeModuleWidget::step();
		if (module) {
			unsigned int count = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			if (module->params[MZ_999::PARAM_ON].getValue() > 1.5f)
				count *= 2;
			count %= 1000;
			count /= 10;
			count -= 50;
			count = abs(count);
			float lerp = count / 50.0f;
			onButton->color = nvgLerpRGBA(nvgRGB(0,0,0), SUBLIGHTBLUE, lerp);
		}
	}
};

Model *modelMZ999 = createModel<MZ_999, MZ999>("MZ-999");
