//SubTag W1

#include "SubmarineFree.hpp"

struct XX219 : SchemeModuleWidget {

	XX219(Module *module) {
		setModule(module);
		this->box.size = Vec(15, 380);
		addChild(new SchemePanel(this->box.size));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 20, 1, -M_PI / 2.0f);
		nvgSave(vg);
		nvgTranslate(vg, 1, 377);
		nvgRotate(vg, -M_PI / 2.0f);
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine     OBSOLETE");
		nvgRestore(vg);	
  	}
	void addMenuItem(Menu *menu, const char *label) {
		MenuLabel *ml = new MenuLabel();
		ml->text = label;
		menu->addChild(ml);
	}
	void appendContextMenu(Menu *menu) override {
		ModuleWidget::appendContextMenu(menu);
		addMenuItem(menu, "");
		addMenuItem(menu, "This Module does Nothing");
	}
};

Model *modelXX219 = createModel<Module, XX219>("XX-219");

struct TM_105 : Module  {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		INPUT_2,
		INPUT_3,
		INPUT_4,
		INPUT_5,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_TOR,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	TM_105() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < 5; i++) {
			configInput(i, "Deprecated");
		}
		configOutput(OUTPUT_TOR, "Deprecated");
	}
};

struct TM105 : SchemeModuleWidget {
	TM105(TM_105 *module) {
		setModule(module);
		this->box.size = Vec(15, 380);
		addChild(new SchemePanel(this->box.size));
		for (unsigned int i = 0; i < 5; i++) {
			addInput(createInputCentered<DeprecatedPort>(Vec(7.5,41.5 + 32 * i), module, TM_105::INPUT_1 + i));
		}
		addOutput(createOutputCentered<DeprecatedPort>(Vec(7.5,244.5), module, TM_105::OUTPUT_TOR));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 20, 1, -M_PI / 2.0f);
		nvgSave(vg);
		nvgTranslate(vg, 1, 377);
		nvgRotate(vg, -M_PI / 2.0f);
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine     OBSOLETE");
		nvgRestore(vg);	
	}
	void addMenuItem(Menu *menu, const char *label) {
		MenuLabel *ml = new MenuLabel();
		ml->text = label;
		menu->addChild(ml);
	}
	void appendContextMenu(Menu *menu) override {
		ModuleWidget::appendContextMenu(menu);
		addMenuItem(menu, "");
		addMenuItem(menu, "This Module does Nothing");
	}
};

Model *modelTM105 = createModel<TM_105, TM105>("TM-105");
