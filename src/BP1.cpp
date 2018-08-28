#include "SubmarineFree.hpp"

template <int x>
struct BP1 : ModuleWidget {
	BP1(Module *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(plugin, "BP-1", x));
	}
};

Model *modelBP101 = Model::create<Module, BP1<1>>("Submarine (Free)", "BP-101", "BP-101 Blanking Plate", BLANK_TAG);
Model *modelBP102 = Model::create<Module, BP1<2>>("Submarine (Free)", "BP-102", "BP-102 Blanking Plate", BLANK_TAG);
Model *modelBP104 = Model::create<Module, BP1<4>>("Submarine (Free)", "BP-104", "BP-104 Blanking Plate", BLANK_TAG);
Model *modelBP108 = Model::create<Module, BP1<8>>("Submarine (Free)", "BP-108", "BP-108 Blanking Plate", BLANK_TAG);
Model *modelBP110 = Model::create<Module, BP1<10>>("Submarine (Free)", "BP-110", "BP-110 Blanking Plate", BLANK_TAG);
Model *modelBP112 = Model::create<Module, BP1<12>>("Submarine (Free)", "BP-112", "BP-112 Blanking Plate", BLANK_TAG);
Model *modelBP116 = Model::create<Module, BP1<16>>("Submarine (Free)", "BP-116", "BP-116 Blanking Plate", BLANK_TAG);
Model *modelBP120 = Model::create<Module, BP1<20>>("Submarine (Free)", "BP-120", "BP-120 Blanking Plate", BLANK_TAG);
Model *modelBP124 = Model::create<Module, BP1<24>>("Submarine (Free)", "BP-124", "BP-124 Blanking Plate", BLANK_TAG);
Model *modelBP132 = Model::create<Module, BP1<32>>("Submarine (Free)", "BP-132", "BP-132 Blanking Plate", BLANK_TAG);
