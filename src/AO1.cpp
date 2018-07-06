#include "SubmarineFree.hpp"

namespace SubmarineAO {

	float None(float x, float y, float c) { return 0.0f; }
	float XAddC(float x, float y, float c) { return x + c; }
	float YAddC(float x, float y, float c) { return y + c; }
	float C(float x, float y, float c) { return c; }
	float XAddYAddC(float x, float y, float c) { return x + y + c; }
	
	float XSubC(float x, float y, float c) { return x - c; }
	float CSubX(float x, float y, float c) { return c - x; }
	float YSubC(float x, float y, float c) { return y - c; }
	float CSubY(float x, float y, float c) { return c - y; }
	float XSubYAddC(float x, float y, float c) { return x - y - c; }
	float XAddCSubY(float x, float y, float c) { return x + c - y; }
	float YSubXAddC(float x, float y, float c) { return y - x - c; }
	float YAddCSubX(float x, float y, float c) { return y + c - x; }

	float XMulYAddC(float x, float y, float c) { return x * y + c; }
	float XAddCMulY(float x, float y, float c) { return (x + c) * y; }
	float XMulCAddY(float x, float y, float c) { return x * (y + c); }
	float XMulC(float x, float y, float c) { return x * c; }
	float YMulC(float x, float y, float c) { return y * c; }
	float XMulYMulC(float x, float y, float c) { return x * y * c; }

	typedef float (*func_t)(float, float, float);

	struct Functor {
		std::string name;
		func_t func;
	};

#define SUB_M "\xc3\x97"
#define SUB_D "\xc3\xb7"

	std::vector<Functor> functions {
		{ "", *None },
		{ "X+C", *XAddC },
		{ "Y+C", *YAddC },
		{ "C", *C },
		{ "X+Y+C", *XAddYAddC },
		{ "X-C", *XSubC },
		{ "C-X", *CSubX },
		{ "Y-C", *YSubC },
		{ "C-Y", *CSubY },
		{ "X-(Y+C)", *XSubYAddC },
		{ "(X+C)-Y", *XAddCSubY },
		{ "Y-(X+C)", *YSubXAddC },
		{ "(Y+C)-X", *YAddCSubX },
		{ "(X" SUB_M "Y)+C", *XMulYAddC },
		{ "(X+C)" SUB_M "Y", *XAddCMulY },
		{ "X" SUB_M "(Y+C)", *XMulCAddY },
		{ "X" SUB_M "C", *XMulC },
		{ "Y" SUB_M "C", *YMulC },
		{ "X" SUB_M "Y" SUB_M "C", *XMulYMulC }
	};	

}

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
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}
	void draw(NVGcontext *vg) override {
		char mtext[41];
		sprintf(mtext, "%6.2f", floor(value * 100.0f + 0.5f)/100.0f);
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
			result = SubmarineAO::functions[params[PARAM_FUNC_1].value].func(inputs[INPUT_X_1].value, inputs[INPUT_Y_1].value, floor(params[PARAM_CONST_1].value * 100.0f + 0.5f)/100.0f);
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
		addParam(ParamWidget::create<AOConstDisplay>(Vec(33, 70), module, AO1<x,y>::PARAM_CONST_1, -INFINITY, INFINITY, 0.0f));
	}
};

Model *modelAO105 = Model::create<AO1<1,5>, AOWidget<1,5>>("SubmarineFree", "A0-105", "A0-105 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
Model *modelAO110 = Model::create<AO1<2,5>, AOWidget<2,5>>("SubmarineFree", "A0-110", "A0-110 Arithmetic Operators", UTILITY_TAG, MULTIPLE_TAG);
