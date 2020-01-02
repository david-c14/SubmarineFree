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
