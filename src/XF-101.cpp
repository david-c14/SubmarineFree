#include "XF.hpp"

struct XF_101 : XF {
	enum ParamIds {
		PARAM_CV_1,
		PARAM_MODE_1,
		PARAM_FADE_1,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1,
		INPUT_B_1,
		INPUT_CV_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1,
		LIGHT_LOG_1,
		LIGHT_AUTO_1,
		LIGHT_INV_1,
		NUM_LIGHTS
	};
	char faderKnob_enabled[1];
	XF_Correlator correlators[1];
	XF_Controls controls[1] = {
		{
			INPUT_A_1,
			0,
			INPUT_B_1,
			0,
			PARAM_FADE_1,
			INPUT_CV_1,
			OUTPUT_1,
			0,
			PARAM_CV_1,
			PARAM_MODE_1,
			LIGHT_LIN_1,
			LIGHT_LOG_1,
			LIGHT_AUTO_1,
			&correlators[0]
		}
	};

	XF_101() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void XF_101::step() {
	faderKnob_enabled[0] = !inputs[INPUT_CV_1].active;
	crossFade(&controls[0]);
}

struct XF101 : ModuleWidget {
	XF101(XF_101 *module) : ModuleWidget(module) {
		LightKnob *fader;
		setPanel(SVG::load(assetPlugin(plugin, "res/XF-104.svg")));

		addInput(Port::create<PJ301MPort>(Vec(27.5,18), Port::INPUT, module, XF_101::INPUT_A_1));
		addInput(Port::create<sub_port>(Vec(127.5,18), Port::INPUT, module, XF_101::INPUT_B_1));
		addInput(Port::create<sub_port>(Vec(27.5,74), Port::INPUT, module, XF_101::INPUT_CV_1));

		addOutput(Port::create<sub_port>(Vec(127.5,74), Port::OUTPUT, module, XF_101::OUTPUT_1));

		addParam(ParamWidget::create<sub_sw_2>(Vec(41, 46), module, XF_101::PARAM_CV_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_3>(Vec(125, 43.5), module, XF_101::PARAM_MODE_1, 0.0f, 2.0f, 0.0f));
		fader = ParamWidget::create<sub_knob_large>(Vec(63, 31), module, XF_101::PARAM_FADE_1, 0.0f, 10.0f, 5.0f);
		fader->moduleFlag = &module->faderKnob_enabled[0];
		addParam(fader);

		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 47), module, XF_101::LIGHT_LIN_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 57), module, XF_101::LIGHT_LOG_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueRedLight>>(Vec(141, 67), module, XF_101::LIGHT_AUTO_1));
	}
};

Model *modelXF101 = Model::create<XF_101, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
