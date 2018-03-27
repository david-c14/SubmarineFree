#include "SubmarineFree.hpp"

struct XF : Module {
	enum ParamIds {
		CV_PARAM_1,
		MODE_PARAM_1,
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
		NUM_LIGHTS
	};

	XF() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void XF::step() {
}

struct XF101 : ModuleWidget {
	XF101(XF *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/XF-104.svg")));

		addInput(Port::create<sub_port>(Vec(27.5,18), Port::INPUT, module, XF::INPUT_A_1));
		addInput(Port::create<sub_port>(Vec(127.5,18), Port::INPUT, module, XF::INPUT_B_1));
		addInput(Port::create<sub_port>(Vec(27.5,74), Port::INPUT, module, XF::INPUT_CV_1));

		addOutput(Port::create<sub_port>(Vec(127.5,74), Port::OUTPUT, module, XF::OUTPUT_1));

		addParam(ParamWidget::create<sub_sw_2>(Vec(41, 46), module, XF::CV_PARAM_1, 0.0f, 1.0f, 0.0f));
		addParam(ParamWidget::create<sub_sw_3>(Vec(125, 43.5), module, XF::MODE_PARAM_1, 0.0f, 2.0f, 0.0f));
	}
};

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelXF101 = Model::create<XF, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
