#include "SubmarineFree.hpp"


struct XF_Correlator {
	static const int frameSize = 1024;
	float samples_a[frameSize];
	float samples_b[frameSize];
	int n = 0;
	int sp = 0;
	float covariance = 0;
	float sigma_a = 0;
	float sigma_b = 0;
	float sigma_a2 = 0;
	float sigma_b2 = 0;
	int schmitt = 0;
	float correlation = 0;

	int correlate(float, float);
	XF_Correlator() {};
};

struct XF_Controls {
	int a;
	int ar;
	int b;
	int br;
	int fader;
	int cv;
	int out;
	int outr;
	int polar;
	int mode;
	int light1;
	int light2;
	int light3;
	XF_Correlator *correlator;
}; 

int XF_Correlator::correlate(float a, float b) {
	//Remove old samples
	if (n == frameSize) {
		covariance -= (samples_a[sp] * samples_b[sp]);
		sigma_a -= samples_a[sp];
		sigma_b -= samples_b[sp];
		sigma_a2 -= (samples_a[sp] * samples_a[sp]);
		sigma_b2 -= (samples_b[sp] * samples_b[sp]);
	}
	else {
		n++;
	}

	//Add new samples
	covariance += (a * b);
	sigma_a += samples_a[sp] = a;
	sigma_b += samples_b[sp] = b;
	sigma_a2 += (a * a);
	sigma_b2 += (b * b);
	sp++;
	if (sp > frameSize - 1) {
		sp -= frameSize;
	}
	float stdev_a = powf(sigma_a2 - (sigma_a * sigma_a / n), 0.5f);
	float stdev_b = powf(sigma_b2 - (sigma_b * sigma_b / n), 0.5f);
	if (stdev_a * stdev_b == 0.0f)
		correlation = (stdev_a == stdev_b);
	else
		correlation = covariance / (stdev_a * stdev_b);
	if (schmitt) {
		if (fabs(correlation) < 0.4)
			schmitt = 0;
	}
	else {
		if (fabs(correlation) > 0.6)
			schmitt = 1;
	}
	return schmitt;
}

struct XF : Module {
	XF(int p, int i, int o, int l) : Module(p, i, o, l) {}
	void crossFade(XF_Controls *controls);
};

void XF::crossFade(XF_Controls *controls) {
	float fade = clamp((inputs[controls->cv].active?params[controls->polar].value * 5.0f + inputs[controls->cv].value:params[controls->fader].value)/10.0f, 0.0f, 1.0f);
	int mode = 0;
	if (params[controls->mode].value > 1.5f) {
		mode = controls->correlator->correlate(inputs[controls->a].value, inputs[controls->b].value);
		if (controls->correlator->correlation < -0.1f) {
			lights[controls->light3].value = 0.0f;
			lights[controls->light3 + 1].value = 1.0f;
		}
		else {
			lights[controls->light3].value = 1.0f;
			lights[controls->light3 + 1].value = 0.0f;
		}
	}
	else if (params[controls->mode].value > 0.5f) {
		mode = 1;
		lights[controls->light3].value = 0.0f;
		lights[controls->light3 + 1].value = 0.0f;
	}
	else {
		mode = 0;
		lights[controls->light3].value = 0.0f;
		lights[controls->light3 + 1].value = 0.0f;
	}
	if (mode == 1) {
		outputs[controls->out].value = inputs[controls->a].value * powf(1.0f - fade, 0.5f) + inputs[controls->b].value * powf(fade, 0.5f);
		if (controls->outr)
			outputs[controls->outr].value = inputs[controls->ar].value * powf(1.0f - fade, 0.5f) + inputs[controls->br].value * powf(fade, 0.5f);
		lights[controls->light1].value = 0.0f;
		lights[controls->light2].value = 1.0f;
	}
	else {
		outputs[controls->out].value = inputs[controls->a].value * (1.0f - fade) + inputs[controls->b].value * fade;
		if (controls->outr)
			outputs[controls->outr].value = inputs[controls->ar].value * (1.0f - fade) + inputs[controls->br].value * fade;
		lights[controls->light1].value = 1.0f;
		lights[controls->light2].value = 0.0f;
	}
}

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

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelXF101 = Model::create<XF_101, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
Model *modelXF102 = Model::create<XF_102, XF102>("SubmarineFree", "XF-102", "XF-102 2-Channel Cross Fader", BLANK_TAG);

/*
debug("101");

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
 enum LightIds {;
  LIGHT_LIN_1,
  LIGHT_LOG_1,
  LIGHT_AUTO_1, LIGHT_INV_1,
  NUM_LIGHTS
 };

debug("102");

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
 enum LightIds {;
  LIGHT_LIN_1, LIGHT_LIN_2,
  LIGHT_LOG_1, LIGHT_LOG_2,
  LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2,
  NUM_LIGHTS
 };

debug("104");

 enum ParamIds {
  PARAM_CV_1, PARAM_CV_2, PARAM_CV_3, PARAM_CV_4,
  PARAM_MODE_1, PARAM_MODE_2, PARAM_MODE_3, PARAM_MODE_4,
  PARAM_FADE_1, PARAM_FADE_2, PARAM_FADE_3, PARAM_FADE_4,
  PARAM_LINK_1, PARAM_LINK_2,
  NUM_PARAMS
 };
 enum InputIds {
  INPUT_A_1, INPUT_A_2, INPUT_A_3, INPUT_A_4,
  INPUT_B_1, INPUT_B_2, INPUT_B_3, INPUT_B_4,
  INPUT_CV_1, INPUT_CV_2, INPUT_CV_3, INPUT_CV_4,
  NUM_INPUTS
 };
 enum OutputIds {
  OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4,
  NUM_OUTPUTS
 };
 enum LightIds {;
  LIGHT_LIN_1, LIGHT_LIN_2, LIGHT_LIN_3, LIGHT_LIN_4,
  LIGHT_LOG_1, LIGHT_LOG_2, LIGHT_LOG_3, LIGHT_LOG_4,
  LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_INV_2, LIGHT_INV_2, LIGHT_AUTO_3, LIGHT_INV_3, LIGHT_AUTO_4, LIGHT_INV_4
  NUM_LIGHTS
 };

debug("201");

 enum ParamIds {
  PARAM_CV_1,
  PARAM_MODE_1,
  PARAM_FADE_1,
  NUM_PARAMS
 };
 enum InputIds {
  INPUT_A_1, INPUT_AR_1,
  INPUT_B_1, INPUT_BR_1,
  INPUT_CV_1,
  NUM_INPUTS
 };
 enum OutputIds {
  OUTPUT_1, OUTPUTR_1,
  NUM_OUTPUTS
 };
 enum LightIds {;
  LIGHT_LIN_1,
  LIGHT_LOG_1,
  LIGHT_AUTO_1, LIGHT_INV_1,
  NUM_LIGHTS
 };

debug("202");

 enum ParamIds {
  PARAM_CV_1, PARAM_CV_2,
  PARAM_MODE_1, PARAM_MODE_2,
  PARAM_FADE_1, PARAM_FADE_2,
  NUM_PARAMS
 };
 enum InputIds {
  INPUT_A_1, INPUT_A_2, INPUT_AR_1, INPUT_AR_2,
  INPUT_B_1, INPUT_B_2, INPUT_BR_1, INPUT_BR_2,
  INPUT_CV_1, INPUT_CV_2,
  NUM_INPUTS
 };
 enum OutputIds {
  OUTPUT_1, OUTPUT_2, OUTPUTR_1, OUTPUTR_2,
  NUM_OUTPUTS
 };
 enum LightIds {;
  LIGHT_LIN_1, LIGHT_LIN_2,
  LIGHT_LOG_1, LIGHT_LOG_2,
  LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2,
  NUM_LIGHTS
 };

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
,
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
   &correlators[1]
  }
,
  {
   INPUT_A_3,
   0,
   INPUT_B_3,
   0,
   PARAM_FADE_3,
   INPUT_CV_3,
   OUTPUT_3,
   0,
   PARAM_CV_3,
   PARAM_MODE_3,
   LIGHT_LIN_3,
   LIGHT_LOG_3,
   LIGHT_AUTO_3,
   &correlators[2]
  }
,
  {
   INPUT_A_4,
   0,
   INPUT_B_4,
   0,
   PARAM_FADE_4,
   INPUT_CV_4,
   OUTPUT_4,
   0,
   PARAM_CV_4,
   PARAM_MODE_4,
   LIGHT_LIN_4,
   LIGHT_LOG_4,
   LIGHT_AUTO_4,
   &correlators[3]
  }
,
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
,
  {
   INPUT_A_3,
   INPUT_A_4,
   INPUT_B_3,
   INPUT_B_4,
   PARAM_FADE_3,
   INPUT_CV_3,
   OUTPUT_3,
   OUTPUT_4,
   PARAM_CV_3,
   PARAM_MODE_3,
   LIGHT_LIN_3,
   LIGHT_LOG_3,
   LIGHT_AUTO_3,
   &correlators[2]
  }

  {
   INPUT_A_1,
   INPUT_AR_1,
   INPUT_B_1,
   INPUT_BR_1,
   PARAM_FADE_1,
   INPUT_CV_1,
   OUTPUT_1,
   OUTPUTR_1,
   PARAM_CV_1,
   PARAM_MODE_1,
   LIGHT_LIN_1,
   LIGHT_LOG_1,
   LIGHT_AUTO_1,
   &correlators[0]
  }
,
  {
   INPUT_A_2,
   INPUT_AR_2,
   INPUT_B_2,
   INPUT_BR_2,
   PARAM_FADE_2,
   INPUT_CV_2,
   OUTPUT_2,
   OUTPUTR_2,
   PARAM_CV_2,
   PARAM_MODE_2,
   LIGHT_LIN_2,
   LIGHT_LOG_2,
   LIGHT_AUTO_2,
   &correlators[1]
  }
*/
