#include "SubmarineFree.hpp"

struct SS_112 : Module {
	static constexpr int deviceCount = 12;
	SS_112() : Module(0, deviceCount, 0, 0) {}
};

struct SS112 : ModuleWidget {
	SS112(SS_112 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-112.svg")));
		for (int i = 0; i < SS_112::deviceCount; i++) {
			addInput(Port::create<sub_port>(Vec(2.5,19 + i * 29), Port::INPUT, module, i));
		}
	}
};

struct SS_207 : Module {
	static constexpr int deviceCount = 8;
	SS_207() : Module(0, 0, deviceCount, 0) {}
};

struct SS207 : ModuleWidget {
	SS207(SS_207 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-207.svg")));
		for (int i = 0; i < SS_207::deviceCount; i++) {
			addOutput(Port::create<sub_port>(Vec(2.5,19 + 43 * i), Port::OUTPUT, module, i));
		}
		module->outputs[0].value = M_PI;
		module->outputs[1].value = 2 * M_PI;
		module->outputs[2].value = M_E;
		module->outputs[3].value = M_SQRT1_2;
		module->outputs[4].value = M_SQRT2;
		module->outputs[5].value = powf(3.0f, 0.5f);
		module->outputs[6].value = powf(5.0f, 0.5f);
		module->outputs[7].value = powf(7.0f, 0.5f);
	}
};

struct SS_221 : Module {
	static constexpr int deviceCount = 21;
	SS_221() : Module(0, 0, deviceCount, 0) {}
};

struct SS221 : ModuleWidget {
	SS221(SS_221 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-221.svg")));
		for (int i = 0; i < SS_221::deviceCount; i++) {
			addOutput(Port::create<sub_port>(Vec(2.5 + 45 * (i % 2),19 + i * 16), Port::OUTPUT, module, i));
			module->outputs[i].value = 10.0f - i;
		}
	}
};

Model *modelSS112 = Model::create<SS_112, SS112>("SubmarineFree", "SS-112", "SS-112 12 Input Sinks", UTILITY_TAG);
Model *modelSS207 = Model::create<SS_207, SS207>("SubmarineFree", "SS-207", "SS-207 7 Irrational Output Voltage Sources", UTILITY_TAG);
Model *modelSS221 = Model::create<SS_221, SS221>("SubmarineFree", "SS-221", "SS-221 21 Output Voltage Sources", UTILITY_TAG);
