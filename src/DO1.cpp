//SubTag TM TW W8

#include "SubmarineFree.hpp"

namespace {

	void drawConnector(NVGcontext *vg, float x, float y, NVGcolor color) {
		nvgFillColor(vg, color);
		nvgBeginPath(vg);
		nvgCircle(vg, x, y, 4);
		nvgFill(vg);
	}

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
				drawConnector(args.vg, 50, 30, nvgRGB(0xff, 0xff, 0xff));
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
				drawConnector(args.vg, 50, 30, nvgRGB(0xff, 0xff, 0xff));
			}
		},
	};

	struct PLConnectorRenderer : TransparentWidget {
		std::function<void (const Widget::DrawArgs &)> drawLambda;
		void draw(const DrawArgs &args) override {
			drawLambda(args);
			Widget::draw(args);
		}
	};

	struct PLGateKnob : Knob {
		Module *module;
		int index;
		PLGateKnob() {
			box.size.x = 60;
			box.size.y = 60;
			snap = true;
			smooth = false;
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

	struct PLConnectorKnob : Knob {
		Module *module;
		ScrollWidget *scrollWidget;
		int index;
		PLConnectorKnob() {
			box.size.x = 8;
			box.size.y = 8;
			snap = true;
			smooth = false;
		}
	};

	template <unsigned int x, unsigned int y>
	struct PLFixedConnectorKnob : PLConnectorKnob {
		void draw(const DrawArgs &args) override {
			if (module) {
				drawConnector(args.vg, box.size.x / 2.0f, box.size.y / 2.0f, nvgRGB(0xff, 0xff, 0xff));
	/*
				unsigned int val = (unsigned int)APP->engine->getParam(module, index);
				if (val > (x + y + 1)) {
					val = (x + y + 1);
				}
				float destX = 0;
				float destY = 0;
				if (val < (x + 2)) {
					destX = (scrollWidget->box.size.x / (x * 2 + 4.0f)) * (val * 2 + 1) - box.pos.x;
					destY = box.pos.y - scrollWidget->box.size.y - parent->box.size.y + 4;
				}
				else {
					destX = 50.f - box.pos.x;
					destY = -4 - scrollWidget->box.size.y + (val - x - 2) * 80 + scrollWidget->container->box.pos.y;
				}
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 4, 4);
				nvgLineTo(args.vg, destX, destY);
				nvgStrokeColor(args.vg, nvgRGBAf(0.2f, 0.2f, 0.2f, 0.5f));
				nvgStrokeWidth(args.vg, 3);
				nvgStroke(args.vg);
				nvgStrokeColor(args.vg, nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f));
				nvgStrokeWidth(args.vg, 2);
				nvgStroke(args.vg);
*/
			}
		}
	};
	
	template<unsigned int x, unsigned int y>
	struct PLBackground : OpaqueWidget {
		void draw(const DrawArgs &args) override {
			nvgFillColor(args.vg, nvgRGB(0,0,0));
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFill(args.vg);
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 1, 5, nvgRGB(0x22, 0x22, 0x22));
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 3, 5, nvgRGB(0xff, 0x00, 0x00));
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 5, 5, nvgRGB(0xff, 0xff, 0x00));
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 7, 5, nvgRGB(0x00, 0x00, 0xff));
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 9, 5, nvgRGB(0x00, 0xff, 0xff));
			drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * 11, 5, nvgRGB(0xff, 0xff, 0xff));
			Widget::draw(args);
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
			configParam(PARAM_CONNECTOR_OUT_1 + ix, 0.0f, x + y + 1, 0.0f, "Connection" );
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			configParam(PARAM_GATE_1 + iy, 0.0f, functions.size() - 1.0f, 0.0f, "Gate" );
			configParam(PARAM_CONNECTOR_1 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_2 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_3 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
			configParam(PARAM_CONNECTOR_4 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, "Connection");
		}
	}
	void process(const ProcessArgs &args) override {
	}
};

template <unsigned int x, unsigned int y>
struct DOWidget : SchemeModuleWidget {
	ScrollWidget *collectionScrollWidget;
	PLBackground<x,y> *background;
	DOWidget(DO1<x,y> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(x * 30, 380);
		addChild(new SchemePanel(this->box.size));
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(createInputCentered<BluePort>(Vec(15 + ix * 30, 30), module, DO1<x,y>::INPUT_1 + ix));
			addOutput(createOutputCentered<BluePort>(Vec(15 + ix * 30, 350), module, DO1<x,y>::OUTPUT_1 + ix));
		}
		background = new PLBackground<x,y>();
		background->box.pos = Vec(5, 45);
		background->box.size = Vec(box.size.x - 10, box.size.y - 90);
		addChild(background);
		collectionScrollWidget = new ScrollWidget();
		float posDiff = background->box.size.x / x;
		float pos = posDiff / 2;
		for (unsigned int ix = 0; ix < x; ix++) {
			PLFixedConnectorKnob<x,y> *knob = createParamCentered<PLFixedConnectorKnob<x,y>>(Vec(pos, background->box.size.y - 5), module, DO1<x, y>::PARAM_CONNECTOR_OUT_1 + ix);
			knob->module = module;
			knob->scrollWidget = collectionScrollWidget;
			knob->index = DO1<x,y>::PARAM_CONNECTOR_OUT_1 + ix;
			background->addChild(knob);
			pos = pos + posDiff;
		}
		collectionScrollWidget->box.pos = Vec(5,55);
		collectionScrollWidget->box.size = Vec(box.size.x - 10, box.size.y - 110);
		addChild(collectionScrollWidget);
		for (unsigned int iy = 0; iy < y; iy++) {
			PLGateKnob *knob = createParamCentered<PLGateKnob>(Vec(60, 80 * iy), module, DO1<x,y>::PARAM_GATE_1 + iy);
			knob->module = module;
			knob->index = DO1<x,y>::PARAM_GATE_1 + iy;
			collectionScrollWidget->container->addChild(knob);
		}
		PLConnectorRenderer *renderer = new PLConnectorRenderer();
		renderer->box.pos = background->box.pos;
		renderer->box.size = background->box.size;
		renderer->drawLambda = [this](const DrawArgs &args) {
			this->drawConnectors(args);
		};
		addChild(renderer);
	}

	void drawWire(const DrawArgs &args, float sx, float sy, float dx, float dy) {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, sx, sy);
		nvgLineTo(args.vg, dx, dy);
		nvgStrokeColor(args.vg, nvgRGBAf(0.2f, 0.2f, 0.2f, 0.5f));
		nvgStrokeWidth(args.vg, 3);
		nvgStroke(args.vg);
		nvgStrokeColor(args.vg, nvgRGBAf(1.0f, 1.0f, 1.0f, 0.5f));
		nvgStrokeWidth(args.vg, 2);
		nvgStroke(args.vg);
	}
	void drawConnectors(const DrawArgs &args) {
		// outputConnectors
		float posDiff = background->box.size.x / x;
		float pos = posDiff / 2;
		for (unsigned int i = 0; i < x; i++) {
			float startX = pos;
			float startY = background->box.size.y - 5;
			unsigned int val = (unsigned int)APP->engine->getParam(module, DO1<x,y>::PARAM_CONNECTOR_OUT_1 + i);
			if (val > (x + y + 1)) {
				val = (x + y + 1);
			}
			float destX = 0;
			float destY = 0;
			if (val < x + 2) {
				destX = background->box.size.x / (x + 2);
				destX /= 2;
				destX *= (val * 2 + 1);
				destY = 5;
			}
			drawWire(args, startX, startY, destX, destY);
			pos += posDiff;
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		char workingSpace[10];
		snprintf(workingSpace, 10, "DO-1%02d", y);
		drawBase(vg, workingSpace);
	}
};

Model *modelDO105 = createModel<DO1<4,5>, DOWidget<4,5>>("DO-105");
