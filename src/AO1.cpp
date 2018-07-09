#include "SubmarineFree.hpp"

namespace SubmarineAO {

	typedef float (*func_t)(float, float, float);

	struct Functor {
		std::string name;
		func_t func;
	};

#define LAMBDA(e) [](float x, float y, float c)->float { return e ; }
#define M "\xc3\x97"
#define D "\xc3\xb7"
#define R "\xe2\x88\x9a"
#define S2 "\xc2\xb2"
#define S3 "\xc2\xb3"
#define s0 "\xe2\x82\x80"
#define s1 "\xe2\x82\x81"
#define s2 "\xe2\x82\x82"
#define E "\xe2\x84\xaf"
#define SX "\xcb\xa3"
#define SY "\xca\xb8"
#define SC "\xe1\xb6\x9c"

	std::vector<Functor> functions {
		{ "",                      LAMBDA(0) },
		{ "X+C",                   LAMBDA(x + c) },
		{ "Y+C",                   LAMBDA(y + c) },
		{ "C",                     LAMBDA(c) },
		{ "X+Y+C",                 LAMBDA(x + y + c) },
		{ "C-X",                   LAMBDA(c - x) },
		{ "C-Y",                   LAMBDA(c - y) },
		{ "X-(Y+C)",               LAMBDA(x - ( y + c )) },
		{ "(X+C)-Y",               LAMBDA(( x + c ) - y) },
		{ "Y-(X+C)",               LAMBDA(y - ( x + c )) },
		{ "(Y+C)-X",               LAMBDA(( y + c ) - x) },
		{ "(X" M "Y)+C",       LAMBDA(( x * y ) + c) },
		{ "(X+C)" M "Y",       LAMBDA(( x + c ) * y) },
		{ "X" M "(Y+C)",       LAMBDA(x * ( y + c )) },
		{ "X" M "C",           LAMBDA(x * c) },
		{ "Y" M "C",           LAMBDA(y * c) },
		{ "X" M "Y" M "C", LAMBDA(x * y * c) },
		{ D R S2 S3 s0 s1 s2 E SX SY SC, LAMBDA(0) }
	};	

}

#undef M
#undef D
#undef R
#undef S2
#undef S3
#undef s0
#undef s1
#undef s2
#undef E
#undef SX
#undef SY
#undef SC

struct AOFuncDisplay : Knob {
	std::shared_ptr<Font> font;
	AOFuncDisplay() {
		box.size.x = 50;
		box.size.y = 16;
		snap = true;
		smooth = false;
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}
	void draw(NVGcontext *vg) override {
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 25, 12, SubmarineAO::functions[value].name.c_str(), NULL);
	}
};

struct AOConstDisplay : Knob {
	std::shared_ptr<Font> font;
	AOConstDisplay() {
		box.size.x = 50;
		box.size.y = 16;
		snap = true;
		speed = 0.005;
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}
	void draw(NVGcontext *vg) override {
		char mtext[41];
		sprintf(mtext, "c=%6.2f", ((int)value)/100.0f);
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 25, 12, mtext, NULL);
	}
};

template <unsigned int x, unsigned int y>
struct AO1 : Module {
	enum ParamIds {
		PARAM_FUNC_1,
		PARAM_CONST_1 = x * y,
		NUM_PARAMS = 2 * x * y
	};
	enum InputIds {
		INPUT_X_1,
		INPUT_Y_1 = x,
		NUM_INPUTS = x + y
	};
	enum OutputIds {
		OUTPUT_X_1,
		OUTPUT_Y_1 = x,
		NUM_OUTPUTS = x + y
	};
	enum LightIds {
		NUM_LIGHTS
	};

	AO1() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		float result = 0;
		if (params[PARAM_FUNC_1].value) {
			result = SubmarineAO::functions[params[PARAM_FUNC_1].value].func(inputs[INPUT_X_1].value, inputs[INPUT_Y_1].value, ((int)params[PARAM_CONST_1].value)/100.0f);
		}
		outputs[OUTPUT_X_1].value = result; 
		outputs[OUTPUT_Y_1].value = result;
	}
};

template <unsigned int x, unsigned int y>
struct AOWidget : ModuleWidget {
	AOWidget(AO1<x,y> *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(plugin, "AO-1", x*y));
		for (unsigned int i = 0; i < y; i++) {
			addInput(Port::create<sub_port>(Vec(4, 65 + i * 58), Port::INPUT, module, AO1<x,y>::INPUT_Y_1 + i));
			addOutput(Port::create<sub_port>(Vec(31 + x * 120, 65 + i * 58), Port::OUTPUT, module, AO1<x,y>::OUTPUT_Y_1 + i));
		}
		for (unsigned int i = 0; i < x; i++) {
			addInput(Port::create<sub_port>(Vec(77.5 + 120 * i, 19), Port::INPUT, module, AO1<x,y>::INPUT_X_1 + i));
			addOutput(Port::create<sub_port>(Vec(77.5 + 120 * i, 330), Port::OUTPUT, module, AO1<x,y>::OUTPUT_X_1 + i));
		}
		addParam(ParamWidget::create<AOFuncDisplay>(Vec(33, 40), module, AO1<x,y>::PARAM_FUNC_1, 0.0f, SubmarineAO::functions.size() - 1.0f, 0.0f ));
		addParam(ParamWidget::create<AOConstDisplay>(Vec(33, 70), module, AO1<x,y>::PARAM_CONST_1, -10000.0f, 10000.0f, 0.0f));
	}
};

Model *modelAO105 = Model::create<AO1<1,5>, AOWidget<1,5>>("SubmarineFree", "A0-105", "A0-105 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
Model *modelAO110 = Model::create<AO1<2,5>, AOWidget<2,5>>("SubmarineFree", "A0-110", "A0-110 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
