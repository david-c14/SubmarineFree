#include "SubmarineFree.hpp"

template <int x>
struct BP1x : ModuleWidget {
	BP1x(Module *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(pluginInstance, "BP-1", x));
	}
};

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
		drawText(vg, 3, 377, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 12, gScheme.alternative, "submarine");
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
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.alternative, "submarine");
		nvgRestore(vg);
	}
};

struct BP1_2 : BP1<2> {
	BP1_2(Module *module) : BP1<2>(module) {}
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
		drawText(vg, 150, 377, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 12, gScheme.alternative, "submarine");
	}
};

Model *modelBP101 = Model::create<Module, BP1_1>("BP-101");
Model *modelBP102 = Model::create<Module, BP1_2>("BP-102");
Model *modelBP104 = Model::create<Module, BP1_4>("BP-104");
Model *modelBP108 = Model::create<Module, BP1<8>>("BP-108");
Model *modelBP110 = Model::create<Module, BP1<10>>("BP-110");
Model *modelBP112 = Model::create<Module, BP1<12>>("BP-112");
Model *modelBP116 = Model::create<Module, BP1<16>>("BP-116");
Model *modelBP120 = Model::create<Module, BP1_20>("BP-120");
Model *modelBP124 = Model::create<Module, BP1<24>>("BP-124");
Model *modelBP132 = Model::create<Module, BP1<32>>("BP-132");
