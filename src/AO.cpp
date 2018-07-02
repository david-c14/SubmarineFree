#include "SubmarineFree.hpp"

template <unsigned int x, unsigned int y>
struct AO : Module {
	enum ParamIds {
		PARAM_FUNC_1,
		PARAM_CONST_1 = x * y,
		NUM_PARAMS = 2 * x * y
	};
	enum InputIds {
		INPUT_X_1,
		INPUT_Y_1 = x,
		NUM_INPUTS = x + y
	};
	enum OutputIds {
		OUTPUT_X_1,
		OUTPUT_Y_1 = x,
		NUM_OUTPUTS = x + y
	};
	enum LightIds {
		NUM_LIGHTS
	};

	AO<x, y>() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

template <unsigned int x, unsigned int y>
void AO<x, y>::step() {
}

template <unsigned int x, unsigned int y>
struct AOWidget : ModuleWidget {
	AOWidget(AO<x,y> *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/AG-106.svg")));
		for (unsigned int i = 0; i < y; i++) {
			addInput(Port::create<sub_port>(Vec(4, 65 + i * 58), Port::INPUT, module, AO<x,y>::INPUT_Y_1 + i));
		}
		for (unsigned int i = 0; i < x; i++) {
			addInput(Port::create<sub_port>(Vec(39 + 25 * i, 19), Port::INPUT, module, AO<x,y>::INPUT_X_1 + i));
		}
	}
};

Model *modelAO101 = Model::create<AO<1,5>, AOWidget<1,5>>("SubmarineFree", "A0-101", "A0-101 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
Model *modelAO102 = Model::create<AO<2,5>, AOWidget<2,5>>("SubmarineFree", "A0-102", "A0-102 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
