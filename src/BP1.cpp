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
	}
};

template <int x>
struct BP1_2 : BP1<x> {
	BP1_2(Module *module) : BP1<x>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		SchemeModuleWidget::drawBackground(vg);
		SchemeModuleWidget::drawLogo(vg, (x * 15 - 20) / 2.0f, 365, 1, 0);
	}
};

Model *modelBP101 = Model::create<Module, BP1x<1>>("Submarine (Free)", "BP-101", "BP-101 Blanking Plate", BLANK_TAG);
Model *modelBP102 = Model::create<Module, BP1_2<2>>("Submarine (Free)", "BP-102", "BP-102 Blanking Plate", BLANK_TAG);
Model *modelBP104 = Model::create<Module, BP1_2<4>>("Submarine (Free)", "BP-104", "BP-104 Blanking Plate", BLANK_TAG);
Model *modelBP108 = Model::create<Module, BP1<8>>("Submarine (Free)", "BP-108", "BP-108 Blanking Plate", BLANK_TAG);
Model *modelBP110 = Model::create<Module, BP1x<10>>("Submarine (Free)", "BP-110", "BP-110 Blanking Plate", BLANK_TAG);
Model *modelBP112 = Model::create<Module, BP1x<12>>("Submarine (Free)", "BP-112", "BP-112 Blanking Plate", BLANK_TAG);
Model *modelBP116 = Model::create<Module, BP1x<16>>("Submarine (Free)", "BP-116", "BP-116 Blanking Plate", BLANK_TAG);
Model *modelBP120 = Model::create<Module, BP1x<20>>("Submarine (Free)", "BP-120", "BP-120 Blanking Plate", BLANK_TAG);
Model *modelBP124 = Model::create<Module, BP1x<24>>("Submarine (Free)", "BP-124", "BP-124 Blanking Plate", BLANK_TAG);
Model *modelBP132 = Model::create<Module, BP1x<32>>("Submarine (Free)", "BP-132", "BP-132 Blanking Plate", BLANK_TAG);
