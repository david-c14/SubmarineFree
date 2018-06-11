#include "SubmarineFree.hpp"

struct SS_112 : Module {
	static constexpr int deviceCount = 12;
	SS_112() : Module(0, 1, 0, 0) {}
};

struct SS112 : ModuleWidget {
	SS112(SS_112 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-112.svg")));
		for (int i = 0; i < SS_112::deviceCount; i++) {
			addInput(Port::create<sub_port>(Vec(2.5,19 + i * 29), Port::INPUT, module, i));
		}
	}
};

Model *modelSS112 = Model::create<SS_112, SS112>("SubmarineFree", "SS-112", "SS-112 Sinks and Sources - 12 Input Sinks", UTILITY_TAG);
