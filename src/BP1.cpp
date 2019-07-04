#include "SubmarineFree.hpp"

template <int x>
struct BP1 : SchemeModuleWidget {
	BP1(Module *module) : SchemeModuleWidget(module) {
		SchemePanel *panel = new SchemePanel(Vec(x * 15, 380));
		addChild(panel);
		this->box.size = Vec(x * 15, 380);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, box.size.x - 20, 365, 1, 0);
		drawText(vg, 3, 377, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 12, gScheme.getAlternative(module), "submarine");
	}
};

struct BP1_1 : BP1<1> {
	BP1_1(Module *module) : BP1<1>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 20, 1, -M_PI / 2.0f);
		nvgSave(vg);
		nvgTranslate(vg, 1, 377);
		nvgRotate(vg, -M_PI / 2.0f);
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine");
		nvgRestore(vg);
	}
};

struct BP1_2 : BP1<2> {
	SubText *textField;
	BP1_2(Module *module) : BP1<2>(module) {
		MouseTransformWidget *tw = createWidget<MouseTransformWidget>(Vec(2, 15));
		tw->rotate(M_PI / 2.0f);
		addChild(tw);

		textField = createWidget<SubText>(Vec(0, -25));
		textField->box.size = Vec(350, 25);
		tw->addChild(textField);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 5, 365, 1, 0);
	}
};

struct BP1_4 : BP1<4> {
	BP1_4(Module *module) : BP1<4>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 20, 365, 1, 0);
	}
};

struct BP1_20 : BP1<20> {
	BP1_20(Module *module) : BP1<20>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 70, 15, 0);
		drawText(vg, 150, 377, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 12, gScheme.getAlternative(module), "submarine");
	}
};

Model *modelBP101 = createModel<Module, BP1_1>("BP-101");
Model *modelBP102 = createModel<Module, BP1_2>("BP-102");
Model *modelBP104 = createModel<Module, BP1_4>("BP-104");
Model *modelBP108 = createModel<Module, BP1<8>>("BP-108");
Model *modelBP110 = createModel<Module, BP1<10>>("BP-110");
Model *modelBP112 = createModel<Module, BP1<12>>("BP-112");
Model *modelBP116 = createModel<Module, BP1<16>>("BP-116");
Model *modelBP120 = createModel<Module, BP1_20>("BP-120");
Model *modelBP124 = createModel<Module, BP1<24>>("BP-124");
Model *modelBP132 = createModel<Module, BP1<32>>("BP-132");
