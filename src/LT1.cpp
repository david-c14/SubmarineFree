//SubTag W20

#include <string.h>
#include "SubmarineFree.hpp"

struct LT_116 : Module {
	enum ParamIds {
		PARAM_COEFF_1,
		PARAM_OUTPUT_CHANNELS = PARAM_COEFF_1 + 256,
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

	int numberOfInputs = 1;
	int numberOfOutputs = 16;
	
	LT_116() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				configParam(PARAM_COEFF_1 + i * 16 + j, -INFINITY, INFINITY, 0.0f, string::f("Coefficient [%d,%d]", i, j));
			}
		}
		configParam(PARAM_OUTPUT_CHANNELS, 1.0f, 16.0f, 16.0f, "Number of channels in output");
	}

	void process(const ProcessArgs &args) override {
		numberOfInputs = inputs[INPUT_1].getChannels();
		numberOfOutputs = params[PARAM_OUTPUT_CHANNELS].getValue();
	}
};
	
struct LT116 : SchemeModuleWidget {
	LightKnob *knobs[256];
	LT116(LT_116 *module) {
		setModule(module);
		this->box.size = Vec(300, 380);
		addChild(new SchemePanel(this->box.size));

		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				knobs[i * 16 + j] = createParamCentered<TinyKnob<LightKnob>>(Vec(15 + i * 18, 30 + j * 18), module, LT_116::PARAM_COEFF_1 + i * 16 + j);
				addParam(knobs[i * 16 + j]);
			}
		}
		addInput(createInputCentered<SilverPort>(Vec(35, 330), module, LT_116::INPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(265, 330), module, LT_116::OUTPUT_1));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(200, 330), module, LT_116::PARAM_OUTPUT_CHANNELS));
	}
	void appendContextMenu(Menu *menu) override {
		if (!module)
			return;
		menu->addChild(new MenuSeparator);
	/*
		EventWidgetMenuItem *vmi = createMenuItem<EventWidgetMenuItem>("Match Cable Colors");
		vmi->stepHandler = [=]() {
			vmi->rightText = CHECKMARK(module->params[EO_102::PARAM_COLORS].getValue());
		};
		vmi->clickHandler = [=]() {
			bool val = module->params[EO_102::PARAM_COLORS].getValue();
			module->params[EO_102::PARAM_COLORS].setValue(!val);
		};
		menu->addChild(vmi);
*/
	}
	void step() override {
		if (!module)
			return;
		LT_116 *ltModule = dynamic_cast<LT_116 *>(module);
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				knobs[i * 16 + j]->setEnabled((i < ltModule->numberOfInputs) && (j < ltModule->numberOfOutputs));
			}
		}
		SchemeModuleWidget::step();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LT-116");

		// Text
		drawText(vg, 35, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 265, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUTPUT");
		drawText(vg, 200, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CHANNELS");
	}
};

Model *modelLT116 = createModel<LT_116, LT116>("LT-116");
