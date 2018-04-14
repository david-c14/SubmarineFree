#include "XF.hpp"

struct XF_102 : XF {
	enum ParamIds {
		PARAM_CV_1, PARAM_CV_2,
		PARAM_MODE_1, PARAM_MODE_2,
		PARAM_FADE_1, PARAM_FADE_2,
		PARAM_LINK_1,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1, INPUT_A_2,
		INPUT_B_1, INPUT_B_2,
		INPUT_CV_1, INPUT_CV_2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1, OUTPUT_2,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1, LIGHT_LIN_2,
		LIGHT_LOG_1, LIGHT_LOG_2,
		LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2,
		NUM_LIGHTS
	};
	char faderKnob_enabled[2];
	XF_Correlator correlators[2];
	XF_Controls controls[3] = {
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
		},
		{
			INPUT_A_2,
			0,
			INPUT_B_2,
			0,
			PARAM_FADE_2,
			INPUT_CV_2,
			OUTPUT_2,
			0,
			PARAM_CV_2,
			PARAM_MODE_2,
			LIGHT_LIN_2,
			LIGHT_LOG_2,
			LIGHT_AUTO_2,
			&correlators[0]
		},	
		{
			INPUT_A_1,
			INPUT_A_2,
			INPUT_B_1,
			INPUT_B_2,
			PARAM_FADE_1,
			INPUT_CV_1,
			OUTPUT_1,
			OUTPUT_2,
			PARAM_CV_1,
			PARAM_MODE_1,
			LIGHT_LIN_1,
			LIGHT_LOG_1,
			LIGHT_AUTO_1,
			&correlators[0]
		}
	};

	XF_102() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void XF_102::step() {
	faderKnob_enabled[0] = !inputs[INPUT_CV_1].active;
	if (params[PARAM_LINK_1].value > 0.5f) {
		crossFade(&controls[2]);
		faderKnob_enabled[1] = 0;
	}
	else {
		faderKnob_enabled[1] = !inputs[INPUT_CV_2].active;
		crossFade(&controls[0]);
		crossFade(&controls[1]);
	}
}

struct XF102 : ModuleWidget {
	XF102(XF_102 *module) : ModuleWidget(module) {
		LightKnob *fader;
		setPanel(SVG::load(assetPlugin(plugin, "res/XF-104.svg")));

		addInput(Port::create<PJ301MPort>(Vec(27.5,18), Port::INPUT, module, XF_102::INPUT_A_1));
		addInput(Port::create<sub_port>(Vec(127.5,18), Port::INPUT, module, XF_102::INPUT_B_1));
		addInput(Port::create<sub_port>(Vec(27.5,74), Port::INPUT, module, XF_102::INPUT_CV_1));

		addOutput(Port::create<sub_port>(Vec(127.5,74), Port::OUTPUT, module, XF_102::OUTPUT_1));

		addParam(ParamWidget::create<sub_sw_2>(Vec(41, 46), module, XF_102::PARAM_CV_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_3>(Vec(125, 43.5), module, XF_102::PARAM_MODE_1, 0.0f, 2.0f, 0.0f));
		addParam(ParamWidget::create<sub_btn>(Vec(90, 94.5), module, XF_102::PARAM_LINK_1, 0.0f, 1.0f, 0.0f));
		fader = ParamWidget::create<sub_knob_large>(Vec(63, 31), module, XF_102::PARAM_FADE_1, 0.0f, 10.0f, 5.0f);
		fader->moduleFlag = &module->faderKnob_enabled[0];
		addParam(fader);

		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 47), module, XF_102::LIGHT_LIN_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 57), module, XF_102::LIGHT_LOG_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueRedLight>>(Vec(141, 67), module, XF_102::LIGHT_AUTO_1));

		addInput(Port::create<PJ301MPort>(Vec(27.5,106), Port::INPUT, module, XF_102::INPUT_A_2));
		addInput(Port::create<sub_port>(Vec(127.5,106), Port::INPUT, module, XF_102::INPUT_B_2));
		addInput(Port::create<sub_port>(Vec(27.5,162), Port::INPUT, module, XF_102::INPUT_CV_2));

		addOutput(Port::create<sub_port>(Vec(127.5,162), Port::OUTPUT, module, XF_102::OUTPUT_2));

		addParam(ParamWidget::create<sub_sw_2>(Vec(41, 134), module, XF_102::PARAM_CV_2, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_3>(Vec(125, 131.5), module, XF_102::PARAM_MODE_2, 0.0f, 2.0f, 0.0f));
		fader = ParamWidget::create<sub_knob_large>(Vec(63, 119), module, XF_102::PARAM_FADE_2, 0.0f, 10.0f, 5.0f);
		fader->moduleFlag = &module->faderKnob_enabled[1];
		addParam(fader);

		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 135), module, XF_102::LIGHT_LIN_2));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 145), module, XF_102::LIGHT_LOG_2));
		addChild(ModuleLightWidget::create<TinyLight<BlueRedLight>>(Vec(141, 155), module, XF_102::LIGHT_AUTO_2));
	}
};

Model *modelXF102 = Model::create<XF_102, XF102>("SubmarineFree", "XF-102", "XF-102 Dual Mono Cross Fader", MIXER_TAG, DUAL_TAG);
