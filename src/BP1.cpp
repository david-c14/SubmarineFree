#include "SubmarineFree.hpp"

template <int x>
struct BP1x : ModuleWidget {
	BP1x(Module *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(plugin, "BP-1", x));
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

Model *modelBP101 = Model::create<Module, BP1_1>("Submarine (Free)", "BP-101", "BP-101 Blanking Plate", BLANK_TAG);
Model *modelBP102 = Model::create<Module, BP1_2>("Submarine (Free)", "BP-102", "BP-102 Blanking Plate", BLANK_TAG);
Model *modelBP104 = Model::create<Module, BP1_4>("Submarine (Free)", "BP-104", "BP-104 Blanking Plate", BLANK_TAG);
Model *modelBP108 = Model::create<Module, BP1<8>>("Submarine (Free)", "BP-108", "BP-108 Blanking Plate", BLANK_TAG);
Model *modelBP110 = Model::create<Module, BP1<10>>("Submarine (Free)", "BP-110", "BP-110 Blanking Plate", BLANK_TAG);
Model *modelBP112 = Model::create<Module, BP1<12>>("Submarine (Free)", "BP-112", "BP-112 Blanking Plate", BLANK_TAG);
Model *modelBP116 = Model::create<Module, BP1<16>>("Submarine (Free)", "BP-116", "BP-116 Blanking Plate", BLANK_TAG);
Model *modelBP120 = Model::create<Module, BP1_20>("Submarine (Free)", "BP-120", "BP-120 Blanking Plate", BLANK_TAG);
Model *modelBP124 = Model::create<Module, BP1<24>>("Submarine (Free)", "BP-124", "BP-124 Blanking Plate", BLANK_TAG);
Model *modelBP132 = Model::create<Module, BP1<32>>("Submarine (Free)", "BP-132", "BP-132 Blanking Plate", BLANK_TAG);
