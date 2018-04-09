#include "SubmarineFree.hpp"

const int frameSize = 1024;

struct XF_Correlator {
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
	void crossFadeMono(
		int a, 
		int b, 
		int fader, 
		int cv, 
		int polar, 
		int mode,
		int light1,
		int light2,
		int light3,
		XF_Correlator *correlator,
		int out
	); 
};

void XF::crossFadeMono(
	int a, 
	int b, 
	int fader, 
	int cv, 
	int polar, 
	int mode,
	int light1,
	int light2,
	int light3,
	XF_Correlator *correlator,
	int out
) {
	float fade = clamp((inputs[cv].active?params[polar].value + inputs[cv].value:params[fader].value)/10.0f, 0.0f, 1.0f);
	int mode = 0;
	if (params[mode].value > 1.5f) {
		mode = correlator->correlate(inputs[a].value, inputs[b].value)
		if (correlator->correlation < -0.1f) {
			lights[light3].value = 0.0f;
			lights[light3 + 1].value = 1.0f;
		}
		else {
			lights[light3].value = 1.0f;
			lights[light3 + 1].value = 0.0f;
		}
	}
	else if (params[mode].value > 0.5f) {
		mode = 0;
	}
	else {
		mode = 1;
	}
	if (mode == 1) {
		outputs[out].value = inputs[a].value * powf(1.0f - fade, 0.5f) + inputs[b].value * powf(fade, 0.5f);
		lights[light1].value = 0.0f;
		lights[light2].value = 1.0f;
	}
	else {
		outputs[out].value = inputs[a].value * (1.0f - fade) + inputs[b].value * fade;
		lights[light1].value = 1.0f;
		lights[light2].value = 0.0f;
	}
}

struct XF_101 : XF {
	enum ParamIds {
		PARAM_CV_1,
		PARAM_MODE_1,
		PARAM_LINK_1,
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
	LightKnob *fader1;
	XF_Correlator correlators[1];

	XF_101() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void XF_101::step() {
	fader1->setEnabled(!inputs[INPUT_CV_1].active);
	crossFadeMono(
		INPUT_A_1, 
		INPUT_B_1, 
		PARAM_FADE_1, 
		INPUT_CV_1, 
		PARAM_CV_1, 
		PARAM_MODE_1,
		LIGHT_LIN_1,
		LIGHT_LOG_1,
		LIGHT_AUTO_1,
		&correlators[0],
		OUTPUT_1
	);
}

struct XF101 : ModuleWidget {
	XF101(XF_101 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/XF-104.svg")));

		addInput(Port::create<sub_port>(Vec(27.5,18), Port::INPUT, module, XF_101::INPUT_A_1));
		addInput(Port::create<sub_port>(Vec(127.5,18), Port::INPUT, module, XF_101::INPUT_B_1));
		addInput(Port::create<sub_port>(Vec(27.5,74), Port::INPUT, module, XF_101::INPUT_CV_1));

		addOutput(Port::create<sub_port>(Vec(127.5,74), Port::OUTPUT, module, XF_101::OUTPUT_1));

		addParam(ParamWidget::create<sub_sw_2>(Vec(41, 46), module, XF_101::PARAM_CV_1, 5.0f, 0.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_3>(Vec(125, 43.5), module, XF_101::PARAM_MODE_1, 0.0f, 2.0f, 0.0f));
		addParam(ParamWidget::create<sub_btn>(Vec(90, 94.5), module, XF_101::PARAM_LINK_1, 0.0f, 1.0f, 0.0f));
		module->fader1 = ParamWidget::create<sub_knob_large>(Vec(63, 31), module, XF_101::PARAM_FADE_1, 0.0f, 10.0f, 5.0f);
		addParam(module->fader1);
		module->fader1->setEnabled(1);

		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 47), module, XF_101::LIGHT_LIN_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueLight>>(Vec(141, 57), module, XF_101::LIGHT_LOG_1));
		addChild(ModuleLightWidget::create<TinyLight<BlueRedLight>>(Vec(141, 67), module, XF_101::LIGHT_AUTO_1));
	}
};

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelXF101 = Model::create<XF_101, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
