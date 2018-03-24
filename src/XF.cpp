#include "SubmarineFree.hpp"

struct XF : Module {
	enum ParamIds {
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

		addInput(Port::create<PJ301MPort>(Vec(27.5,18), Port::INPUT, module, XF::INPUT_A_1));
		addInput(Port::create<PJ301MPort>(Vec(127.5,18), Port::INPUT, module, XF::INPUT_B_1));
		addInput(Port::create<PJ301MPort>(Vec(27.5,74), Port::INPUT, module, XF::INPUT_CV_1));

		addOutput(Port::create<PJ301MPort>(Vec(127.5,74), Port::OUTPUT, module, XF::OUTPUT_1));
	}
};

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelXF101 = Model::create<XF, XF101>("SubmarineFree", "XF-101", "XF-101 1-Channel Cross Fader", BLANK_TAG);
