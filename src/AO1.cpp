#include "SubmarineFree.hpp"

namespace SubmarineAO {

	typedef float (*func_t)(float, float, float);

	struct Functor {
		std::string name;
		func_t func;
	};

#define LAMBDA(e) [](float x, float y, float c)->float { return e ; }
#define X "X"			// X
#define Y "Y"			// Y
#define C "C"			// C
#define A "+"			// Addition symbol
#define S "-"			// Subtraction symbol
#define OP "("			// Open Parenthesis
#define CP ")"			// Close Parenthesis
#define M "\xc3\x97"		// Multiplication symbol
#define D "\xc3\xb7"		// Division symbol
#define R "\xe2\x88\x9a"	// Root symbol
#define S2 "\xc2\xb2"		// Superscript 2
#define S3 "\xc2\xb3"		// Superscript 3
#define s0 "\xe2\x82\x80"	// Subscript 0
#define s1 "\xe2\x82\x81"	// Subscript 1
#define s2 "\xe2\x82\x82"	// Subscript 2
#define E "\xe2\x84\xaf"	// e
#define SX "\xcb\xa3"		// Superscript x
#define SY "\xca\xb8"		// Superscript y
#define SC "\xe1\xb6\x9c"	// Superscript c
#define SIN "sin"		// sine function
#define COS "cos"		// cosine function
#define TAN "tan"		// tangent function
#define ASIN "asin"		// arcsine function
#define ACOS "acos"		// arcosine function
#define ATAN "atan"		// arctangent function
#define LOG "log"		// log function
#define LOG2 LOG s2		// base-2 log function
#define LOG10 LOG s1 s0		// base-10 log function


	std::vector<Functor> functions {
		{ "",                      LAMBDA(0) },
		{ X A C,                   LAMBDA(x + c) },
		{ Y A C,                   LAMBDA(y + c) },
		{ C,                       LAMBDA(c) },
		{ X A Y X C,               LAMBDA(x + y + c) },
		{ C S X,                   LAMBDA(c - x) },
		{ C S Y,                   LAMBDA(c - y) },
		{ X S OP Y A C CP,         LAMBDA(x - ( y + c )) },
		{ OP X A C CP S Y,         LAMBDA(( x + c ) - y) },
		{ Y S OP X A C CP,         LAMBDA(y - ( x + c )) },
		{ OP Y A C CP S X,         LAMBDA(( y + c ) - x) },
		{ OP X M Y CP A C,         LAMBDA(( x * y ) + c) },
		{ OP X A C CP M Y,         LAMBDA(( x + c ) * y) },
		{ X M OP Y A C CP,        LAMBDA(x * ( y + c )) },
		{ X M C,                   LAMBDA(x * c) },
		{ Y M C,                   LAMBDA(y * c) },
		{ X M Y M C,               LAMBDA(x * y * c) }
	};	

#undef X
#undef Y
#undef C
#undef A
#undef S
#undef OP
#undef CP
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
#undef SIN
#undef COS
#undef TAN
#undef ASIN
#undef ACOS
#undef ATAN
#undef LOG
#undef LOG2
#undef LOG10

} // end namespace SubmarineA0

struct AOFuncDisplay : Knob {
	std::shared_ptr<Font> font;
	AOFuncDisplay() {
		box.size.x = 80;
		box.size.y = 15;
		snap = true;
		smooth = false;
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}
	void draw(NVGcontext *vg) override {
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 40, 11, SubmarineAO::functions[value].name.c_str(), NULL);
	}
};

struct AOConstDisplay : Knob {
	std::shared_ptr<Font> font;
	AOConstDisplay() {
		box.size.x = 80;
		box.size.y = 15;
		snap = true;
		speed = 0.005;
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}
	void draw(NVGcontext *vg) override {
		char mtext[41];
		sprintf(mtext, "C=%4.2f", ((int)value)/100.0f);
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 40, 11, mtext, NULL);
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
		float vx[x];
		for (unsigned int ix = 0; ix < x; ix++) {
			vx[ix] = inputs[INPUT_X_1 + ix].value;
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			float vy = inputs[INPUT_Y_1 + iy].value;
			for (unsigned int ix = 0; ix < x; ix++) {
				unsigned int f = params[PARAM_FUNC_1 + ix + iy * x].value;
				if (f >= SubmarineAO::functions.size())
					f = SubmarineAO::functions.size() - 1;
				if (f > 0)
					vy = vx[ix] = SubmarineAO::functions[f].func(vx[ix], vy, ((int)params[PARAM_CONST_1 + ix + iy * x].value)/100.0f);
			}
			outputs[OUTPUT_Y_1 + iy].value = isfinite(vy)?vy:0.0f;
		}
		for (unsigned int ix = 0; ix < x; ix++) {
			outputs[OUTPUT_X_1 + ix].value = isfinite(vx[ix])?vx[ix]:0.0f;
		}
	}
};

template <unsigned int x, unsigned int y>
struct AOWidget : ModuleWidget {
	AOWidget(AO1<x,y> *module) : ModuleWidget(module) {
		setPanel(SubHelper::LoadPanel(plugin, "AO-1", x*y));
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(Port::create<sub_port>(Vec(4, 61 + ix * 46), Port::INPUT, module, AO1<x,y>::INPUT_X_1 + ix));
			addOutput(Port::create<sub_port>(Vec(46 + y * 90, 61 + ix * 46), Port::OUTPUT, module, AO1<x,y>::OUTPUT_X_1 + ix));
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			addInput(Port::create<sub_port>(Vec(70 + 90 * iy, 19), Port::INPUT, module, AO1<x,y>::INPUT_Y_1 + iy));
			addOutput(Port::create<sub_port>(Vec(70 + 90 * iy, 335), Port::OUTPUT, module, AO1<x,y>::OUTPUT_Y_1 + iy));
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			for (unsigned int ix = 0; ix < x; ix++) {
				addParam(ParamWidget::create<AOFuncDisplay>(Vec(42.5 + 90 * iy, 60 + 46 * ix), module, AO1<x,y>::PARAM_FUNC_1 + ix + iy * x, 0.0f, SubmarineAO::functions.size() - 1.0f, 0.0f ));
				addParam(ParamWidget::create<AOConstDisplay>(Vec(42.5 + 90 * iy, 76 + 46 * ix), module, AO1<x,y>::PARAM_CONST_1 + ix + iy * x, -10000.0f, 10000.0f, 0.0f));
			}
		}
	}
};

Model *modelAO106 = Model::create<AO1<6,1>, AOWidget<6,1>>("SubmarineFree", "A0-106", "A0-106 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
Model *modelAO112 = Model::create<AO1<6,2>, AOWidget<6,2>>("SubmarineFree", "A0-112", "A0-112 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
