#include "SubmarineFree.hpp"

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
	int out
) {
	float fade = clamp((inputs[cv].active?params[polar].value + inputs[cv].value:params[fader].value)/10.0f, 0.0f, 1.0f);
	if (params[mode].value > 0.5f) {
		outputs[out].value = inputs[a].value * powf(1.0f - fade, 0.5f) + inputs[b].value * powf(fade, 0.5f);
		lights[light1].value = 0.0f;
	}
	else {
		outputs[out].value = inputs[a].value * (1.0f - fade) + inputs[b].value * fade;
		lights[light1].value = 1.0f;
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
		NUM_LIGHTS
	};
	LightKnob *fader1;

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
	}
};

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelXF101 = Model::create<XF_101, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
