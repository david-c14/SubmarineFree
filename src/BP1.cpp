#include "SubmarineFree.hpp"

template <int x>
struct BP : ModuleWidget {
	BP(Module *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(plugin, "BP-1", x));
	}
};

Model *modelBP101 = Model::create<Module, BP<1>>("SubmarineFree", "BP-101", "BP-101 Blanking Plate", BLANK_TAG);
Model *modelBP102 = Model::create<Module, BP<2>>("SubmarineFree", "BP-102", "BP-102 Blanking Plate", BLANK_TAG);
Model *modelBP104 = Model::create<Module, BP<4>>("SubmarineFree", "BP-104", "BP-104 Blanking Plate", BLANK_TAG);
Model *modelBP108 = Model::create<Module, BP<8>>("SubmarineFree", "BP-108", "BP-108 Blanking Plate", BLANK_TAG);
Model *modelBP110 = Model::create<Module, BP<10>>("SubmarineFree", "BP-110", "BP-110 Blanking Plate", BLANK_TAG);
Model *modelBP112 = Model::create<Module, BP<12>>("SubmarineFree", "BP-112", "BP-112 Blanking Plate", BLANK_TAG);
Model *modelBP116 = Model::create<Module, BP<16>>("SubmarineFree", "BP-116", "BP-116 Blanking Plate", BLANK_TAG);
Model *modelBP120 = Model::create<Module, BP<20>>("SubmarineFree", "BP-120", "BP-120 Blanking Plate", BLANK_TAG);
Model *modelBP124 = Model::create<Module, BP<24>>("SubmarineFree", "BP-124", "BP-124 Blanking Plate", BLANK_TAG);
Model *modelBP132 = Model::create<Module, BP<32>>("SubmarineFree", "BP-132", "BP-132 Blanking Plate", BLANK_TAG);
