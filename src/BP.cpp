#include "SubmarineFree.hpp"

struct BP : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	BP() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void BP::step() {
}

struct BP110 : ModuleWidget {
	BP110(BP *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/BP-110.svg")));
	}
};

// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelBP110 = Model::create<BP, BP110>("SubmarineFree", "BP-110", "BP-110 Blanking Plate", BLANK_TAG);
