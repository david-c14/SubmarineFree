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

struct SS_212 : Module {
	static constexpr int deviceCount = 12;
	SS_212() : Module(0, 0, deviceCount, 0) {}
};

struct SS212 : ModuleWidget {
	int v = 0;
	SS212(SS_212 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-212.svg")));
		for (int i = 0; i < SS_212::deviceCount; i++) {
			addOutput(Port::create<sub_port>(Vec(2.5,19 + i * 29), Port::OUTPUT, module, i));
			module->outputs[i].value = 1.0f * i / 12.0f;
		}
	}

	void appendContextMenu(Menu *menu) override;

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();
		json_object_set_new(rootJ, "octave", json_integer(v));
		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);
		json_t *intJ = json_object_get(rootJ, "octave");
		if (intJ)
			v = json_integer_value(intJ);
	}
};

struct SSMenuItem : MenuItem {
	SS212 *ss212;
	int v;
	void onAction(EventAction &e) override {
		ss212->v = v;
		for (int i = 0; i < SS_212::deviceCount; i++) {
			ss212->module->outputs[i].value = v + 1.0f * i / 12.0f;
		}
	}
	void step() override {
		rightText = CHECKMARK(ss212->v == v);
	}
};

void SS212::appendContextMenu(Menu *menu) {
	char label[20];
	menu->addChild(MenuEntry::create());
	SS212 *ss212 = dynamic_cast<SS212*>(this);
        assert(ss212);
	for (int i = -5; i < 5; i++) {
		sprintf(label, "Octave %d", i);
		SSMenuItem *menuItem = MenuItem::create<SSMenuItem>(label);
		menuItem->ss212 = ss212;
		menuItem->v = i;
		menu->addChild(menuItem);
	}
}

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

struct SS_220 : Module {
	static constexpr int deviceCount = 12;
	static constexpr int deviceSetCount = 10;
	SS_220() : Module(0, 0, deviceCount * deviceSetCount, 0) {}
};

struct SS220 : ModuleWidget {
	SS220(SS_220 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/SS-220.svg")));
		for (int j = 0; j < SS_220::deviceSetCount; j++) {
			for (int i = 0; i < SS_220::deviceCount; i++) {
				addOutput(Port::create<sub_port>(Vec(2.5 + 30 * j, 19 + i * 29), Port::OUTPUT, module, j * SS_220::deviceCount + i));
				module->outputs[j * SS_220::deviceCount + i].value = (j - 5.0f) + 1.0f * i / 12.0f;
			}
		}
	}
};

Model *modelSS112 = Model::create<SS_112, SS112>("SubmarineFree", "SS-112", "SS-112 12 Input Sinks", UTILITY_TAG);
Model *modelSS207 = Model::create<SS_207, SS207>("SubmarineFree", "SS-207", "SS-207 7 Irrational Output Voltage Sources", UTILITY_TAG);
Model *modelSS212 = Model::create<SS_212, SS212>("SubmarineFree", "SS-212", "SS-212 12 Chromatic Output Voltage Sources", UTILITY_TAG);
Model *modelSS220 = Model::create<SS_220, SS220>("SubmarineFree", "SS-220", "SS-220 120 Chromatic Output Voltage Sources", UTILITY_TAG);
Model *modelSS221 = Model::create<SS_221, SS221>("SubmarineFree", "SS-221", "SS-221 21 Output Voltage Sources", UTILITY_TAG);
