#include "SubmarineFree.hpp"

namespace SubmarineAO {

	float None(float x, float y, float c) { return 0.0f; }
	float X(float x, float y, float c) { return x; }
	float Y(float x, float y, float c) { return y; }
	float C(float x, float y, float c) { return c; }
	float XAddY(float x, float y, float c) { return x + y; }
	float XAddC(float x, float y, float c) { return x + c; }
	float YAddC(float x, float y, float c) { return y + c; }
	float XAddYAddC(float x, float y, float c) { return x + y + c; }
	float XSubY(float x, float y, float c) { return x - y; }
	float YSubX(float x, float y, float c) { return y - x; }
	float XSubC(float x, float y, float c) { return x - c; }
	float CSubX(float x, float y, float c) { return c - x; }
	float YSubC(float x, float y, float c) { return y - c; }
	float CSubY(float x, float y, float c) { return c - y; }
	float XMulY(float x, float y, float c) { return x * y; }
	float XMulC(float x, float y, float c) { return x * c; }
	float YMulC(float x, float y, float c) { return y * c; }
	float XMulYMulC(float x, float y, float c) { return x * y * c; }

	typedef float (*func_t)(float, float, float);

	struct Functor {
		std::string name;
		func_t func;
	};

#define SUB_MULT_SYM "\xc3\x97"
#define SUB_DIV_SYM "\xc3\xb7"

	std::vector<Functor> functions {
		{ "", *None },
		{ "x", *X },
		{ "y", *Y },
		{ "c", *C },
		{ "x + y", *XAddY },
		{ "x + c", *XAddC },
		{ "y + c", *YAddC },
		{ "x + y + c", *XAddYAddC },
		{ "x - y", *XSubY },
		{ "y - x", *YSubX },
		{ "x - c", *XSubC },
		{ "c - x", *CSubX },
		{ "y - c", *YSubC },
		{ "c - y", *CSubY },
		{ "x " SUB_MULT_SYM " y", *XMulY },
		{ "x " SUB_MULT_SYM " c", *XMulC },
		{ "y " SUB_MULT_SYM " c", *YMulC },
		{ "x " SUB_MULT_SYM " y " SUB_MULT_SYM " c", *XMulYMulC }
	};	

}

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
			result = SubmarineAO::functions[params[PARAM_FUNC_1].value].func(inputs[INPUT_X_1].value, inputs[INPUT_Y_1].value, params[PARAM_CONST_1].value);
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
		addParam(ParamWidget::create<sub_knob_small_snap>(Vec(33, 40), module, AO1<x,y>::PARAM_FUNC_1, 0.0f, SubmarineAO::functions.size() - 1.0f, 0.0f ));
		addParam(ParamWidget::create<sub_knob_small>(Vec(33, 70), module, AO1<x,y>::PARAM_CONST_1, 0.0f, 100.0f, 0.0f));
	}
};

Model *modelAO105 = Model::create<AO1<1,5>, AOWidget<1,5>>("SubmarineFree", "A0-105", "A0-105 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
Model *modelAO110 = Model::create<AO1<2,5>, AOWidget<2,5>>("SubmarineFree", "A0-110", "A0-110 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
