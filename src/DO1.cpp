//SubTag TM TW W8

#include "SubmarineFree.hpp"

namespace {

	struct Functor {
		std::string name;
		std::function<void (const Widget::DrawArgs &)> draw;
	};

	std::vector<Functor> functions {
		{ // Short Circuit
			"Short Circuit",
			[](const Widget::DrawArgs &args) {
				nvgFontSize(args.vg, 16);
				nvgFontFaceId(args.vg, gScheme.font()->handle);
				nvgFillColor(args.vg, SUBLIGHTBLUE);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
				nvgText(args.vg, 30, 30, "0", NULL);
			}
		},
		{ // NOT Gate
			"NOT Gate",
			[](const Widget::DrawArgs &args) {
				nvgFontSize(args.vg, 16);
				nvgFontFaceId(args.vg, gScheme.font()->handle);
				nvgFillColor(args.vg, SUBLIGHTBLUE);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
				nvgText(args.vg, 30, 30, "1", NULL);
			}
		},
	};

	struct PLGateKnob : Knob {
		Module *module;
		int index;
		PLGateKnob() {
			box.size.x = 60;
			box.size.y = 60;
			snap = true;
			smooth = false;
			speed = 0.5f;
		}
		void draw(const DrawArgs &args) override {
			if (module) {
				unsigned int val = (unsigned int)APP->engine->getParam(module, index);
				if (val >= functions.size()) {
					val = functions.size() - 1;
				}
				functions[val].draw(args);
			}
		}
	};
}

template <unsigned int x, unsigned int y>
struct DO1 : Module {
	enum ParamIds {
		PARAM_GATE_1,
		PARAM_CONNECTOR_1 = y,
		PARAM_CONNECTOR_2,
		PARAM_CONNECTOR_3,
		PARAM_CONNECTOR_4,
		PARAM_CONNECTOR_OUT_1 = y + 4 * y,
		NUM_PARAMS = y + x + 4 * y
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = x
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = x
	};
	enum LightIds {
		NUM_LIGHTS
	};

	DO1() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int ix = 0; ix < x; ix++) {
			configParam(PARAM_CONNECTOR_OUT_1 + ix, 0.0f, x + 4.0f * y, 0.0f, "Connection" );
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			configParam(PARAM_GATE_1 + iy, 0.0f, functions.size() - 1.0f, 0.0f, "Gate" );
			configParam(PARAM_CONNECTOR_1 + 4 * iy, 0.0f, 1 + x + 4 * iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_2 + 4 * iy, 0.0f, 1 + x + 4 * iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_3 + 4 * iy, 0.0f, 1 + x + 4 * iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_4 + 4 * iy, 0.0f, 1 + x + 4 * iy, 0.0f, "Connection");
		}
	}
	void process(const ProcessArgs &args) override {
	}
};

template <unsigned int x, unsigned int y>
struct DOWidget : SchemeModuleWidget {
	ScrollWidget *collectionScrollWidget;
	DOWidget(DO1<x,y> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(x * 30, 380);
		addChild(new SchemePanel(this->box.size));
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(createInputCentered<BluePort>(Vec(15 + ix * 30, 30), module, DO1<x,y>::INPUT_1 + ix));
			addOutput(createOutputCentered<BluePort>(Vec(15 + ix * 30, 350), module, DO1<x,y>::OUTPUT_1 + ix));
		}
		collectionScrollWidget = new ScrollWidget();
		collectionScrollWidget->box.pos = Vec(10,45);
		collectionScrollWidget->box.size = Vec(box.size.x - 20, box.size.y - 90);
		addChild(collectionScrollWidget);
		for (unsigned int iy = 0; iy < y; iy++) {
			PLGateKnob *knob = createParamCentered<PLGateKnob>(Vec(60, 80 * iy), module, DO1<x,y>::PARAM_GATE_1 + iy);
			knob->module = module;
			knob->index = DO1<x,y>::PARAM_GATE_1 + iy;
			collectionScrollWidget->container->addChild(knob);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		char workingSpace[10];
		snprintf(workingSpace, 10, "DO-1%02d", y);
		drawBase(vg, workingSpace);
	}
};

Model *modelDO105 = createModel<DO1<4,5>, DOWidget<4,5>>("DO-105");
