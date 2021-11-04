//SubTag DS TM TW W8

#include "shared/DS.hpp"

namespace {

	NVGcolor colors[26];
	char labels[27] = "-1234+ABCDEFGHIJKLMNOPQRST";
	std::vector<std::string> connectorLabels {
		"-ve Rail",
		"Device Input 1",
		"Device Input 2",
		"Device Input 3",
		"Device Input 4",
		"+ve Rail",
		"Gate A Output",
		"Gate B Output",
		"Gate C Output",
		"Gate D Output",
		"Gate E Output",
		"Gate F Output",
		"Gate G Output",
		"Gate H Output",
		"Gate I Output",
		"Gate J Output",
		"Gate K Output",
		"Gate L Output",
		"Gate M Output",
		"Gate N Output",
		"Gate O Output",
		"Gate P Output",
		"Gate Q Output",
		"Gate R Output",
		"Gate S Output",
		"Gate T Output"
	};

	std::vector<std::string> gateLabels;
	unsigned int copyBuffer[107] = { 0 };	// 20 gates, 20 * 4 connectors, 4 output connectors, 3 additional bytes of size information
	unsigned int pasteBuffer[107];   	// Working space if the copy needs to be compacted before pasting

// Based on - Set of 20 Simple, Distinct Colors
// Thanks to Sacha Trubetskoy
// https://sashat.me/2017/01/11/list-of-20-simple-distinct-colors/	

	json_t *saveJson() {
		json_t *settings = json_object();
		json_t *arr = json_array();
		json_array_append_new(arr, json_string("#201e24"));
		json_array_append_new(arr, json_string("#c9b70e"));
		json_array_append_new(arr, json_string("#0c8e15"));
		json_array_append_new(arr, json_string("#c91847"));
		json_array_append_new(arr, json_string("#0986ad"));
		json_array_append_new(arr, json_string("#ffffff"));
		json_array_append_new(arr, json_string("#911eb4"));
		json_array_append_new(arr, json_string("#f032e6"));
		json_array_append_new(arr, json_string("#a9a9a9"));
		json_array_append_new(arr, json_string("#800000"));
		json_array_append_new(arr, json_string("#9A6324"));
		json_array_append_new(arr, json_string("#808000"));
		json_array_append_new(arr, json_string("#469990"));
		json_array_append_new(arr, json_string("#5050C5"));
		json_array_append_new(arr, json_string("#FF4173"));
		json_array_append_new(arr, json_string("#f58231"));
		json_array_append_new(arr, json_string("#FFFF28"));
		json_array_append_new(arr, json_string("#bfef45"));
		json_array_append_new(arr, json_string("#64DC73"));
		json_array_append_new(arr, json_string("#42d4f4"));
		json_array_append_new(arr, json_string("#2828FF"));
		json_array_append_new(arr, json_string("#fabebe"));
		json_array_append_new(arr, json_string("#ffd8b1"));
		json_array_append_new(arr, json_string("#fffac8"));
		json_array_append_new(arr, json_string("#aaffc3"));
		json_array_append_new(arr, json_string("#e6beff"));
		json_object_set_new(settings, "colors", arr);
		system::createDirectory(asset::user("SubmarineFree"));
		std::string settingsFilename = asset::user("SubmarineFree/DO-1xx.json");
		FILE *file = fopen(settingsFilename.c_str(), "w");
		if (file) {
			json_dumpf(settings, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
			fclose(file);
		}
		return settings;
	}

	void loadJson() {
		json_t *settings;
		json_error_t error;
		FILE *file = fopen(asset::user("SubmarineFree/DO-1xx.json").c_str(), "r");
		if (file) {
			settings = json_loadf(file, 0, &error);
			fclose(file);
		}
		else {
			settings = saveJson();
		}
		if (!settings) {
			WARN("Submarine Free DO-1xx: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
			return;
		}
		json_t *arr = json_object_get(settings, "colors");
		if (arr) {
			int size = json_array_size(arr);
			if (size > 26)
				size = 26;
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					colors[i] = color::fromHexString(json_string_value(j1));
				}
			}
		}
		json_decref(settings);
	}


	typedef uint16_t status_t;

	void drawConnector(NVGcontext *vg, float x, float y, NVGcolor color, char label) {
		static char lbl[2] = {0,0};
		lbl[0] = label;
		nvgFillColor(vg, color);
		nvgBeginPath(vg);
		nvgCircle(vg, x, y, 4);
		nvgFill(vg);
		nvgFontFaceId(vg, gScheme.font()->handle);
		nvgFontSize(vg, 7 * 90 / SVG_DPI);
		float bright = color.r * 0.212655 + color.g * 0.715158 + color.b * 0.072187;
		nvgFillColor(vg, (bright > 0.5f)?nvgRGB(0,0,0):nvgRGB(255,255,255));
		nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(vg, x, y, lbl, NULL);
	}

	inline void drawOutput(NVGcontext *vg, float leftPos) {
		nvgMoveTo(vg, leftPos, 30);
		nvgLineTo(vg, 81, 30);
	}

	inline void drawInput1of1(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 20, 8);
		nvgLineTo(vg, 20, 30);
		nvgLineTo(vg, rightPos, 30);
	}

	inline void drawInput1of2(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 26);
		nvgLineTo(vg, rightPos, 26);
	}
	
	inline void drawInput2of2(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 34);
		nvgLineTo(vg, rightPos, 34);
	}

	inline void drawInput1of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 24);
		nvgLineTo(vg, rightPos, 24);
	}

	inline void drawInput2of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 30);
		nvgLineTo(vg, rightPos, 30);
	}

	inline void drawInput3of3(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 37);
		nvgLineTo(vg, 17, 37);
		nvgLineTo(vg, 17, 36);
		nvgLineTo(vg, rightPos, 36);
	}

	inline void drawInput1of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 8);
		nvgLineTo(vg, 23, 8);
		nvgLineTo(vg, 23, 21);
		nvgLineTo(vg, rightPos, 21);
	}
	
	inline void drawInput2of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 23);
		nvgLineTo(vg, 17, 23);
		nvgLineTo(vg, 17, 27);
		nvgLineTo(vg, rightPos, 27);
	}
	
	inline void drawInput3of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 37);
		nvgLineTo(vg, 17, 37);
		nvgLineTo(vg, 17, 33);
		nvgLineTo(vg, rightPos, 33);
	}
	
	inline void drawInput4of4(NVGcontext *vg, float rightPos) {
		nvgMoveTo(vg, -5, 51);
		nvgLineTo(vg, 23, 51);
		nvgLineTo(vg, 23, 39);
		nvgLineTo(vg, rightPos, 39);
	}
	
	inline void drawAnd(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 18);
		nvgBezierTo(vg, 61, 18, 61, 42, 33, 42);
		nvgLineTo(vg, 33, 18);
	}

	inline void drawOr(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 18);
		nvgQuadTo(vg, 45, 18, 55, 30);
		nvgQuadTo(vg, 45, 42, 33, 42);
		nvgQuadTo(vg, 36, 30, 33, 18);
	}

	inline void drawXor(NVGcontext *vg) {
		nvgMoveTo(vg, 33, 17);
		nvgQuadTo(vg, 36, 30, 33, 43);
		nvgMoveTo(vg, 37, 18);
		nvgQuadTo(vg, 49, 18, 59, 30);
		nvgQuadTo(vg, 49, 42, 37, 42);
		nvgQuadTo(vg, 40, 30, 37, 18);
	}

	inline void drawNot(NVGcontext *vg, float xPos) {
		nvgEllipse(vg, xPos, 30, 4, 4);
	}

	inline void drawBox(NVGcontext *vg) {
		nvgRect(vg, 30, 10, 30, 40);
	}

	struct Functor {
		std::string name;
		std::string truthTable;
		std::function<void (const Widget::DrawArgs &, Vec size)> draw;
		std::function<status_t (status_t a,
					status_t b,
					status_t c,
					status_t d,
					status_t &a0,
					status_t &b0,
					status_t &c0,
					status_t &d0)> process;
		

	};

#define LAMBDA_HEADER [] (status_t a, status_t b, status_t c, status_t d, status_t &a0, status_t &b0, status_t &c0, status_t &d0) -> status_t

#define A "A,"
#define A0 "A\xE2\x82\x80,"
#define A1 "A\xE2\x82\x81,"
#define B "B,"
#define C "C,"
#define C0 "C\xE2\x82\x80,"
#define C1 "C\xE2\x82\x81,"
#define D "D,"
#define D0 "D\xE2\x82\x80,"
#define D1 "D\xE2\x82\x81,"
#define E "E,"
#define E0 "E\xE2\x82\x80,"
#define E1 "E\xE2\x82\x81,"
#define S "S,"
#define R "R,"
#define Q "Q,"
#define Q0 "Q\xE2\x82\x80,"
#define Q1 "Q\xE2\x82\x81,"
#define NT "^"
#define ON "1,"
#define ZE "0,"
#define X "\xC3\x97,"
#define UP "\xE2\x86\x91,"
#define DN "\xE2\x86\x93,"

	std::vector<Functor> functions {
#include "gates/NC"
,
#include "gates/NOT"
,
#include "gates/2-AND"
,
#include "gates/3-AND"
,
#include "gates/4-AND"
,
#include "gates/OR"
,
#include "gates/XOR"
,
#include "gates/2-NAND"
,
#include "gates/3-NAND"
,
#include "gates/4-NAND"
,
#include "gates/NOR"
,
#include "gates/XNOR"
,
#include "gates/XOR"
,
#include "gates/D-TYPE-FLIPFLOP"
,
#include "gates/SR-FLIPFLOP"
,
#include "gates/D-TYPE-LATCH"
,
#include "gates/SR-LATCH"
,
#include "gates/LATCH"
,
#include "gates/DELAY"
,
#include "gates/CLK-DELAY"
	};

#undef A
#undef A0
#undef A1
#undef B
#undef C
#undef C0
#undef C1
#undef D
#undef D0
#undef D1
#undef E
#undef E0
#undef E1
#undef S
#undef R
#undef Q
#undef Q0
#undef Q1
#undef NT
#undef ON
#undef ZE
#undef X
#undef UP
#undef DN
#undef LAMBDA_HEADER

	struct PLConnectorRenderer : TransparentWidget {
		std::function<void (const Widget::DrawArgs &)> drawLambda;
		void draw(const DrawArgs &args) override {
			drawLambda(args);
			Widget::draw(args);
		}
	};

	struct PLTruthTable : MenuEntry {
		std::string table;
		unsigned int inputs = 0;
		unsigned int others = 0;
		unsigned int rows = 0;
		std::vector<std::string> entries;
		PLTruthTable(std::string tableValue) {
			table = tableValue;
			std::stringstream ss(table);
			std::string tableSize;
			std::getline(ss, tableSize, ',');
			
			inputs = tableSize[0] - '0';
			others = tableSize[1] - '0';
			rows = tableSize[2] - '0';
			if (rows > 9) 
				rows -= 7;
			box.size.x = 40 * (inputs + others) + 7;
			box.size.y = 25 * rows + 7;
			while (ss.good()) {
				std::string substr;
				std::getline(ss, substr, ',');
				entries.push_back(substr);
			}
			entries.pop_back();
			//assert(entries.size() == (inputs + others) * rows);
			if (entries.size() != (inputs + others) * rows) {
				WARN("Invalid Truth Table: %s", tableValue.c_str());
			}
		}
		void draw(const DrawArgs &args) override {
			nvgBeginPath(args.vg);
			nvgStrokeColor(args.vg, color::alpha(bndGetTheme()->menuTheme.textColor, 0.25));
			nvgStrokeWidth(args.vg, 1.0f);
			nvgMoveTo(args.vg, 0, 28.5);
			nvgLineTo(args.vg, box.size.x, 28.5);
			nvgMoveTo(args.vg, inputs * 40 + 3.5, 0);
			nvgLineTo(args.vg, inputs * 40 + 3.5, box.size.y);
			nvgStroke(args.vg);
			NVGcolor rightColor = bndGetTheme()->regularTheme.textColor;
			nvgStrokeColor(args.vg, rightColor);
			unsigned int x = 0;
			unsigned int y = 0;
			for (std::string item : entries) {
				if (item[0] == '^') {
					bndIconLabelValue(args.vg, x, y + 3, 40, 25, -1, rightColor, BND_CENTER, 13, item.c_str() + 1, NULL);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, x + 13, y + 5.5);
					nvgLineTo(args.vg, x + 27, y + 5.5);
					nvgStroke(args.vg);
				}
				else {
					bndIconLabelValue(args.vg, x, y + 3, 40, 25, -1, rightColor, BND_CENTER, 13, item.c_str(), NULL);
				}
				x += 40;
				if (x >= inputs * 40) 
					x += 7;
				if (x > (inputs + others) * 40) {
					y += 25;
					x = 0;
				}
				if (y == 25)
					y += 7;
			}			

			Widget::draw(args);
		}
	};

	struct PLGateKnob : Knob {
		Module *module;
		int index;
		std::function<void(int index, unsigned int val)> rightClickHandler;
		PLGateKnob() {
			box.size.x = 86;
			box.size.y = 60;
			snap = true;
			smooth = false;
		}
		void draw(const DrawArgs &args) override {
			if (module) {
				unsigned int val = (unsigned int)(getParamQuantity()->getValue());
				if (val >= functions.size()) {
					val = functions.size() - 1;
				}
				functions[val].draw(args, box.size);
				unsigned int i = box.pos.y / 80;
				i += 6;
				drawConnector(args.vg, box.size.x - 5, box.size.y / 2.0f, colors[i], labels[i]);
			}
		}
		void onButton(const event::Button &e) override {
			if (module) {
				if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
					e.consume(this);
					unsigned int val = (unsigned int)(getParamQuantity()->getValue());
					if (val >= functions.size()) {
						val = functions.size() - 1;
					}
					if (rightClickHandler) {
						rightClickHandler(index, val);
					}
					return;
				}
			}
			Knob::onButton(e);
		}
	};

	struct PLConnectorKnob : Knob {
		Module *module;
		float fade = 0.1f;
		PLConnectorKnob() {
			box.size.x = 10;
			box.size.y = 10;
			snap = true;
			smooth = false;
		}
		void step() override {
			if (fade > 0.1f)
				fade -= 0.02f;
		}
		void onDragStart(const event::DragStart &e) override {
			Knob::onDragStart(e);
			fade = 0.9f;
		}
		void onDragMove(const event::DragMove &e) override {
			Knob::onDragMove(e);
			fade = 0.9f;
		}
		void onDragEnd(const event::DragEnd &e) override {
			Knob::onDragEnd(e);
			fade = 0.9f;
		}
	};

	template<unsigned int x, unsigned int y>
	struct PLBackground : OpaqueWidget {
		void draw(const DrawArgs &args) override {
			nvgFillColor(args.vg, nvgRGB(0,0,0));
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFill(args.vg);
			nvgStrokeWidth(args.vg, 2);
			for (unsigned int ix = 0; ix < x; ix++) {
				nvgStrokeColor(args.vg, colors[ix + 1]);
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, box.size.x / (x * 2 + 4.0f) * (ix * 2 + 3), 5);
				nvgLineTo(args.vg, 15 + ix * 30 - box.pos.x, 30 - box.pos.y);
				nvgStroke(args.vg);
			}
			for (unsigned int ix = 0; ix < x + 2; ix++) {
				drawConnector(args.vg, box.size.x / (x * 2 + 4.0f) * (ix * 2 + 1), 5, colors[ix], labels[ix]);
			}
			Widget::draw(args);
		}
	};

	std::vector<std::string> getGateLabels() {
		if (gateLabels.size() == 0) {
			for (unsigned int i = 0; i < functions.size(); i++ ) {
				gateLabels.push_back(functions[i].name);
			}
		}
		return gateLabels;
	}
}

template <unsigned int x, unsigned int y>
struct DO1 : DS_Module {
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

	enum StatusIds {
		STATUS_ALL_ZEROES,
		STATUS_A,
		STATUS_ALL_ONES = STATUS_A + x,
		STATUS_OUT,
		STATUS_A0 = STATUS_OUT + y,
		STATUS_B0 = STATUS_A0 + y,
		STATUS_C0 = STATUS_B0 + y,
		STATUS_D0 = STATUS_C0 + y,
		NUM_STATUS
	};

	status_t statuses[NUM_STATUS] = { 0 };

	DO1() {
		std::vector<std::string> gLabels = getGateLabels();
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int ix = 0; ix < x; ix++) {
			configSwitch(PARAM_CONNECTOR_OUT_1 + ix, 0.0f, x + y + 1, 0.0f, string::f("Device Output %d", ix + 1), connectorLabels );
			configInput(INPUT_1 + ix, string::f("Signal %d", ix + 1));
			configOutput(OUTPUT_1 + ix, string::f("Signal %d", ix + 1));
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			configSwitch(PARAM_GATE_1 + iy, 0.0f, functions.size() - 1.0f, 0.0f, "Gate", gLabels );
			configSwitch(PARAM_CONNECTOR_1 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, string::f("Gate %c Input 1", labels[iy + 6]), connectorLabels);
			configSwitch(PARAM_CONNECTOR_2 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, string::f("Gate %c Input 2", labels[iy + 6]), connectorLabels);
			configSwitch(PARAM_CONNECTOR_3 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, string::f("Gate %c Input 3", labels[iy + 6]), connectorLabels);
			configSwitch(PARAM_CONNECTOR_4 + 4 * iy, 0.0f, 1 + x + iy, 0.0f, string::f("Gate %c Input 4", labels[iy + 6]), connectorLabels);
		}
		statuses[STATUS_ALL_ZEROES] = 0;
		statuses[STATUS_ALL_ONES] = ~statuses[STATUS_ALL_ZEROES];
		loadJson();
	}
	void process(const ProcessArgs &args) override {
		unsigned int maxPoly = 0;
		for (unsigned int ix = 0; ix < x; ix++) {
			unsigned int channels = inputs[INPUT_1 + ix].getChannels();
			maxPoly = (maxPoly > channels)?maxPoly:channels;
			statuses[STATUS_A + ix] = 0;
			for (unsigned int iy = 0; iy < 16; iy++) {
				statuses[STATUS_A + ix] <<= 1;
				statuses[STATUS_A + ix] += (inputs[INPUT_1 + ix].getPolyVoltage(iy) > midpoint());
			}
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			unsigned int gate = params[PARAM_GATE_1 + iy].getValue();
			if (gate >= functions.size()) {
				gate = functions.size() - 1;
			}
			status_t in[4];
			for (unsigned int ix = 0; ix < 4; ix++) {
				unsigned int val = params[PARAM_CONNECTOR_1 + ix + 4 * iy].getValue();
				if (val > x + iy + 1)
					val = x + iy + 1;
				in[ix] = statuses[STATUS_ALL_ZEROES + val];
			}
			statuses[STATUS_OUT + iy] = functions[gate].process(in[0], in[1], in[2], in[3], statuses[STATUS_A0 + iy], statuses[STATUS_B0 + iy], statuses[STATUS_C0 + iy], statuses[STATUS_D0 + iy]);
		}
		for (unsigned int ix = 0; ix < x; ix++) {
			outputs[OUTPUT_1 + ix].setChannels(maxPoly);
			unsigned int val = params[PARAM_CONNECTOR_OUT_1 + ix].getValue();
			if (val > 1 + x + y)
				val = 1 + x + y;
			for (unsigned int iy = 0; iy < 16; iy++) {
				outputs[OUTPUT_1 + ix].setVoltage(((statuses[STATUS_ALL_ZEROES + val] >> (15-iy)) & 0x1)?voltage1:voltage0, iy);
			}
		}
	}
};

template <unsigned int x, unsigned int y>
struct DOWidget : SchemeModuleWidget {
	ScrollWidget *collectionScrollWidget;
	PLConnectorKnob *knobs[x + 4 * y];
	PLGateKnob *gateKnobs[y];
	PLBackground<x,y> *background;
	DOWidget(DO1<x,y> *module) {
		setModule(module);
		this->box.size = Vec(x * 30, 380);
		addChild(new SchemePanel(this->box.size));
		background = new PLBackground<x,y>();
		background->box.pos = Vec(5, 45);
		background->box.size = Vec(box.size.x - 10, box.size.y - 90);
		addChild(background);
		collectionScrollWidget = new ScrollWidget();
		float posDiff = background->box.size.x / x;
		float pos = posDiff / 2;
		for (unsigned int ix = 0; ix < x; ix++) {
			knobs[ix + 4 * y] = createParamCentered<PLConnectorKnob>(Vec(pos, background->box.size.y - 5), module, DO1<x, y>::PARAM_CONNECTOR_OUT_1 + ix);
			knobs[ix + 4 * y]->module = module;
			knobs[ix + 4 * y]->speed = 20.0f / (2 + x + 4 * y);
			background->addChild(knobs[ix + 4 * y]);
			pos = pos + posDiff;
		}
		collectionScrollWidget->box.pos = Vec(5,55);
		collectionScrollWidget->box.size = Vec(box.size.x - 10, box.size.y - 110);
		addChild(collectionScrollWidget);
		for (unsigned int iy = 0; iy < y; iy++) {
			gateKnobs[iy] = createParamCentered<PLGateKnob>(Vec(53, 80 * (iy + 1)), module, DO1<x,y>::PARAM_GATE_1 + iy);
			gateKnobs[iy]->module = module;
			gateKnobs[iy]->index = DO1<x,y>::PARAM_GATE_1 + iy;
			gateKnobs[iy]->rightClickHandler = [=](int index, unsigned int val) {	
				this->appendGateRightClickMenu(index, val);
			};
			collectionScrollWidget->container->addChild(gateKnobs[iy]);
		}
		for (unsigned int iy = 0; iy < y; iy++) {
			for (unsigned int ix = 0; ix < 4; ix++) {
				knobs[4 * iy + ix] = createParamCentered<PLConnectorKnob>(Vec(5, (iy + 1) * 80.0f + ix * 14.0f - 21.0f), module, DO1<x, y>::PARAM_CONNECTOR_1 + iy * 4 + ix);
				knobs[4 * iy + ix]->module = module;
				knobs[4 * iy + ix]->speed = 20.0f / (4 * iy + x + 2);
				collectionScrollWidget->container->addChild(knobs[4 * iy + ix]);	
			}
		}
		PLConnectorRenderer *renderer = new PLConnectorRenderer();
		renderer->box.pos = background->box.pos;
		renderer->box.size = background->box.size;
		renderer->drawLambda = [this](const DrawArgs &args) {
			this->drawConnectors(args);
		};
		addChild(renderer);
		for (unsigned int ix = 0; ix < x; ix++) {
			addInput(createInputCentered<BluePort>(Vec(15 + ix * 30, 30), module, DO1<x,y>::INPUT_1 + ix));
			addOutput(createOutputCentered<BluePort>(Vec(15 + ix * 30, 350), module, DO1<x,y>::OUTPUT_1 + ix));
		}
	}

	void appendGateRightClickMenu(unsigned int index, unsigned int val) {
		Menu *menu = createMenu();
		MenuLabel *menuTitle = new MenuLabel();
		menuTitle->text = string::f("Gate %c", labels[index + 6]);
		menu->addChild(menuTitle);

		MenuLabel *menuLabel = new MenuLabel();
		menuLabel->text = functions[val].name;
		menu->addChild(menuLabel);
		menu->addChild(new MenuSeparator());

		EventWidgetMenuItem *io = createMenuItem<EventWidgetMenuItem>("Inputs / Outputs");
		io->rightText = SUBMENU;
		io->childMenuHandler = [=]() {
			return this->appendGateIOMenu(index);
		};
		menu->addChild(io);

		EventWidgetMenuItem *tt = createMenuItem<EventWidgetMenuItem>("Truth Table");
		tt->rightText = SUBMENU;
		tt->childMenuHandler = [=]() {
			Menu *menu = new Menu();
			menu->addChild(new PLTruthTable(functions[val].truthTable));
			return menu;
		};
		menu->addChild(tt);

		menu->addChild(new MenuSeparator());
		
		if (val == 0) {
			EventWidgetMenuItem *moveUp = createMenuItem<EventWidgetMenuItem>("Delete and Shuffle Up");
			moveUp->clickHandler = [=]() {
				this->MoveItemsUp(index);
			};
			menu->addChild(moveUp);
		}
		else {
			EventWidgetMenuItem *moveDown = createMenuItem<EventWidgetMenuItem>("Shuffle Down");
			moveDown->clickHandler = [=]() {
				this->MoveItemsDown(index);
			};
			menu->addChild(moveDown);
		}
	}

	rack::ui::Menu *appendGateIOMenu(unsigned int index) {
		Menu *menu = new Menu();
		unsigned int cval;
		bool foundOutput = false;

		cval = knobs[(index - DO1<x,y>::PARAM_GATE_1) * 4]->getParamQuantity()->getValue();
		if (cval > ((unsigned int)index + 5)) {
			cval = index + 5;
		}							
		MenuLabel *menu1 = new MenuLabel();
		menu1->text = string::f("Input 1 from %s", connectorLabels[cval].c_str());
		menu->addChild(menu1);

		cval = knobs[(index - DO1<x,y>::PARAM_GATE_1) * 4 + 1]->getParamQuantity()->getValue();
		if (cval > ((unsigned int)index + 5)) {
			cval = index + 5;
		}							
		MenuLabel *menu2 = new MenuLabel();
		menu2->text = string::f("Input 2 from %s", connectorLabels[cval].c_str());
		menu->addChild(menu2);
					
		cval = knobs[(index - DO1<x,y>::PARAM_GATE_1) * 4 + 2]->getParamQuantity()->getValue();
		if (cval > ((unsigned int)index + 5)) {
			cval = index + 5;
		}							
		MenuLabel *menu3 = new MenuLabel();
		menu3->text = string::f("Input 3 from %s", connectorLabels[cval].c_str());
		menu->addChild(menu3);
					
		cval = knobs[(index - DO1<x,y>::PARAM_GATE_1) * 4 + 3]->getParamQuantity()->getValue();
		if (cval > ((unsigned int)index + 5)) {
			cval = index + 5;
		}							
		MenuLabel *menu4 = new MenuLabel();
		menu4->text = string::f("Input 4 from %s", connectorLabels[cval].c_str());
		menu->addChild(menu4);

		for (unsigned int i = index * 4 + 1; i < y * 4; i++) {
			cval = knobs[i]->getParamQuantity()->getValue();
			if (cval == (unsigned int)index + 6) {
				if (!foundOutput)
					menu->addChild(new MenuSeparator());
				MenuLabel *menuOut = new MenuLabel();
				menuOut->text = string::f("Output to Gate %c Input %d", labels[6 + (i / 4)], (i % 4) + 1);
				menu->addChild(menuOut);
				foundOutput = true;
			}
		}
					
		for (unsigned int i = 0; i < 4; i++) {
			cval = knobs[y * 4 + i]->getParamQuantity()->getValue();
			if (cval == (unsigned int)index + 6) {
				if (!foundOutput)
					menu->addChild(new MenuSeparator());
				MenuLabel *menuOut = new MenuLabel();
				menuOut->text = string::f("Output to Device Output %d", i + 1);
				menu->addChild(menuOut);
				foundOutput = true;
			}
		}

		return menu;
	}
	void MoveItemsUp(unsigned int index) {
		history::ComplexAction *complex = new history::ComplexAction();
		complex->name = "Shuffle Up";
		MoveItemUp(index, complex);
		APP->history->push(complex);
	}
	void MoveItemUp(unsigned int index, history::ComplexAction *complex) {
		if (index == y - 1) {
			complex->push(shuffleChange(gateKnobs[index]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 0]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 1]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 2]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 3]->getParamQuantity(), 0));
			return;
		}
		complex->push(shuffleChange(gateKnobs[index]->getParamQuantity(), gateKnobs[index + 1]->getParamQuantity()->getValue()));
		complex->push(shuffleChange(knobs[index * 4 + 0]->getParamQuantity(), knobs[index * 4 + 4]->getParamQuantity()->getValue()));
		complex->push(shuffleChange(knobs[index * 4 + 1]->getParamQuantity(), knobs[index * 4 + 5]->getParamQuantity()->getValue()));
		complex->push(shuffleChange(knobs[index * 4 + 2]->getParamQuantity(), knobs[index * 4 + 6]->getParamQuantity()->getValue()));
		complex->push(shuffleChange(knobs[index * 4 + 3]->getParamQuantity(), knobs[index * 4 + 7]->getParamQuantity()->getValue()));
		for (unsigned int i = index * 4 + 4; i < (y * 4 + 4); i++) {
			ParamQuantity *pq = knobs[i]->getParamQuantity();
			if (pq->getValue() == index + 7) {
				complex->push(shuffleChange(pq, index + 6));	
			}
		}
		MoveItemUp(index + 1, complex);
	}
	void MoveItemsDown(unsigned int index) {
		history::ComplexAction *complex = new history::ComplexAction();
		complex->name = "Shuffle Down";
		if (MoveItemDown(index, complex)) {
			complex->push(shuffleChange(gateKnobs[index]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 0]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 1]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 2]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[index * 4 + 3]->getParamQuantity(), 0));
			APP->history->push(complex);
		}

	}
	bool MoveItemDown(unsigned int index, history::ComplexAction *complex) {
		if (index >= y)
			return false;
		unsigned int val = gateKnobs[index]->getParamQuantity()->getValue();
		if (val == 0) {
			return true;
		}
		if (!MoveItemDown(index + 1, complex)) {
			return false;
		}
		complex->push(shuffleChange(gateKnobs[index + 1]->getParamQuantity(), val));
		complex->push(shuffleChange(knobs[index * 4 + 4]->getParamQuantity(), knobs[index * 4 + 0]->getParamQuantity()->getValue())); 
		complex->push(shuffleChange(knobs[index * 4 + 5]->getParamQuantity(), knobs[index * 4 + 1]->getParamQuantity()->getValue())); 
		complex->push(shuffleChange(knobs[index * 4 + 6]->getParamQuantity(), knobs[index * 4 + 2]->getParamQuantity()->getValue())); 
		complex->push(shuffleChange(knobs[index * 4 + 7]->getParamQuantity(), knobs[index * 4 + 3]->getParamQuantity()->getValue())); 

		for (unsigned int i = index * 4; i < (y * 4 + 4); i++) {
			ParamQuantity *pq = knobs[i]->getParamQuantity();
			if (pq->getValue() == index + 6) {
				complex->push(shuffleChange(pq, index + 7));
			}
		}
		return true;
	}

	history::ParamChange *shuffleChange(ParamQuantity *pq, float newValue) {
		float oldValue = pq->getValue();
		pq->setValue(newValue);
		newValue = pq->getValue();
		history::ParamChange *h = new history::ParamChange();
		h->name = "change parameter";
		h->moduleId = module->id;
		h->paramId = pq->paramId;
		h->oldValue = oldValue;
		h->newValue = newValue;
		return h;
	}

	void copyToBuffer() {
		unsigned int gateCount = 0; // Number of gates actually used.
		unsigned int usedCount = 0; // Highest numbered gate in use.
		copyBuffer[0] = y;
		for (unsigned int i = 0; i < y; i++) {
			copyBuffer[i * 5 + 3] = gateKnobs[i]->getParamQuantity()->getValue();
			copyBuffer[i * 5 + 4] = knobs[i * 4 + 0]->getParamQuantity()->getValue();
			copyBuffer[i * 5 + 5] = knobs[i * 4 + 1]->getParamQuantity()->getValue();
			copyBuffer[i * 5 + 6] = knobs[i * 4 + 2]->getParamQuantity()->getValue();
			copyBuffer[i * 5 + 7] = knobs[i * 4 + 3]->getParamQuantity()->getValue();
			if (copyBuffer[i * 5 + 3] != 0) {
				gateCount++;
				usedCount = i + 1;
			}
		}
		copyBuffer[103] = knobs[y * 4 + 0]->getParamQuantity()->getValue();
		copyBuffer[104] = knobs[y * 4 + 1]->getParamQuantity()->getValue();
		copyBuffer[105] = knobs[y * 4 + 2]->getParamQuantity()->getValue();
		copyBuffer[106] = knobs[y * 4 + 3]->getParamQuantity()->getValue();
		copyBuffer[1] = usedCount;
		copyBuffer[2] = gateCount;
	}

	void pasteFromBuffer() {
		unsigned int deviceSize = copyBuffer[0];
		unsigned int usedCount = copyBuffer[1];
		unsigned int gateCount = copyBuffer[2];
		if (!deviceSize) 
			return;
		if (gateCount > y)
			return;
		if (usedCount > y) {
			compactBuffer();
			pasteFromBuffer(pasteBuffer);
		}
		else {
			pasteFromBuffer(copyBuffer);
		}
	}

	void compactBuffer() {
		for (unsigned int i = 0; i < 107; i++) {
			pasteBuffer[i] = copyBuffer[i];
		}
		int shuffleDistance = 0;
		for (unsigned int i = 0; i < 20; i++) {
			if (pasteBuffer[i * 5 + 3] == 0) {
				shuffleDistance++;
				continue;
			}
			if (shuffleDistance == 0) {
				continue;
			}
			unsigned int o = i - shuffleDistance;
			pasteBuffer[o * 5 + 3] = pasteBuffer[i * 5 + 3];
			pasteBuffer[o * 5 + 4] = pasteBuffer[i * 5 + 4];
			pasteBuffer[o * 5 + 5] = pasteBuffer[i * 5 + 5];
			pasteBuffer[o * 5 + 6] = pasteBuffer[i * 5 + 6];
			pasteBuffer[o * 5 + 7] = pasteBuffer[i * 5 + 7];
			for(unsigned int j = 0; j < 20; j++) {
				if (pasteBuffer[j * 5 + 4] == (i + 6))
					pasteBuffer[j * 5 + 4] -= shuffleDistance;
				if (pasteBuffer[j * 5 + 5] == (i + 6))
					pasteBuffer[j * 5 + 5] -= shuffleDistance;
				if (pasteBuffer[j * 5 + 6] == (i + 6))
					pasteBuffer[j * 5 + 6] -= shuffleDistance;
				if (pasteBuffer[j * 5 + 7] == (i + 6))
					pasteBuffer[j * 5 + 7] -= shuffleDistance;
			}
			if (pasteBuffer[103] == (i + 6))
				pasteBuffer[103] -= shuffleDistance;
			if (pasteBuffer[104] == (i + 6))
				pasteBuffer[104] -= shuffleDistance;
			if (pasteBuffer[105] == (i + 6))
				pasteBuffer[105] -= shuffleDistance;
			if (pasteBuffer[106] == (i + 6))
				pasteBuffer[106] -= shuffleDistance;
			pasteBuffer[i * 5 + 3] = 0;
			pasteBuffer[i * 5 + 4] = 0;
			pasteBuffer[i * 5 + 5] = 0;
			pasteBuffer[i * 5 + 6] = 0;
			pasteBuffer[i * 5 + 7] = 0;
		}
		pasteBuffer[1] = pasteBuffer[2];
	}

	void pasteFromBuffer(unsigned int * buffer) {
		unsigned int usedCount = buffer[1];
		if (usedCount > y)
			return;
		history::ComplexAction *complex = new history::ComplexAction();
		complex->name = "Paste";
		for (unsigned int i = 0; i < usedCount; i++) {
			complex->push(shuffleChange(gateKnobs[i]->getParamQuantity(), buffer[i * 5 + 3]));
			complex->push(shuffleChange(knobs[i * 4 + 0]->getParamQuantity(), buffer[i * 5 + 4]));
			complex->push(shuffleChange(knobs[i * 4 + 1]->getParamQuantity(), buffer[i * 5 + 5]));
			complex->push(shuffleChange(knobs[i * 4 + 2]->getParamQuantity(), buffer[i * 5 + 6]));
			complex->push(shuffleChange(knobs[i * 4 + 3]->getParamQuantity(), buffer[i * 5 + 7]));
		}
		for (unsigned int i = usedCount; i < y; i++) {
			complex->push(shuffleChange(gateKnobs[i]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[i * 4 + 0]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[i * 4 + 1]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[i * 4 + 2]->getParamQuantity(), 0));
			complex->push(shuffleChange(knobs[i * 4 + 3]->getParamQuantity(), 0));
		}
 		complex->push(shuffleChange(knobs[y * 4 + 0]->getParamQuantity(), buffer[103]));
		complex->push(shuffleChange(knobs[y * 4 + 1]->getParamQuantity(), buffer[104]));
		complex->push(shuffleChange(knobs[y * 4 + 2]->getParamQuantity(), buffer[105]));
		complex->push(shuffleChange(knobs[y * 4 + 3]->getParamQuantity(), buffer[106]));
		APP->history->push(complex);
	}

	std::string getGateName(unsigned int index) {
		unsigned int val = (unsigned int)(gateKnobs[index]->getParamQuantity()->getValue());
		if (val >= functions.size()) {
			val = functions.size() - 1;
		}
		return functions[val].name;
	}

	std::string getGateText(unsigned int index) {
		if (!module)
			return std::string("Browser");
		return string::f("Gate %d: ", index + 1) + getGateName(index);
	}

	std::string getConnectorNameText(unsigned int index) {
		unsigned int gate = index / 4;
		unsigned connector = index % 4;
		if (gate < y) {
			return string::f("Connector %d.%d \xe2\x86\xa3 ", gate + 1, connector + 1);
		}
		index -= (y * 4);
		return string::f("Output connector %c \xe2\x86\xa3 ", 'A' + index);
	}

	std::string getConnectorText(unsigned int index) {
		if (!module)
			return std::string("Browser");
		std::string connectorName = getConnectorNameText(index);
		unsigned int val = (unsigned int)(knobs[index]->getParamQuantity()->getValue());
		if (val > x + y + 1)
			val = x + y + 1;
		if (val == 0)
			return connectorName + std::string("Low");
		if (val == x + 1)
			return connectorName + std::string("High");
		if (val < x + 1)
			return connectorName + string::f("Input connector %c", '@' + val);
		return connectorName + string::f("Gate %d: ", val - x -1) + getGateName(val - x - 2);
	}

	void drawWire(const DrawArgs &args, float sx, float sy, float dx, float dy, NVGcolor color, float fade, char label) {
		color.a = fade;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, sx, sy);
		nvgLineTo(args.vg, dx, dy);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStrokeColor(args.vg, nvgRGBAf(color.r / 2.0f, color.g / 2.0f, color.b / 2.0f, fade));
		nvgStrokeWidth(args.vg, 3);
		nvgStroke(args.vg);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, 2);
		nvgStroke(args.vg);
		color.a = 1;
		drawConnector(args.vg, sx, sy, color, label);
		drawConnector(args.vg, dx, dy, color, label);
	}
	void drawConnectors(const DrawArgs &args) {
		if (!module)
			return;
		for (unsigned int i = 0; i < x; i++) {
			nvgBeginPath(args.vg);
			nvgStrokeWidth(args.vg, 2);
			nvgStrokeColor(args.vg, nvgRGB(255, 255, 255));
			nvgMoveTo(args.vg, (background->box.size.x / (x * 2)) * (i * 2 + 1), background->box.size.y - 5);
			nvgLineTo(args.vg, 15 + i * 30 - background->box.pos.x, 350 - background->box.pos.y);
			nvgStroke(args.vg);
		}
		for (unsigned int i = 0; i < x + 4 * y; i++) {
			float startX = 0;
			float startY = 0;
			float destX = 0;
			float destY = 0;
			float scissorTop = args.clipBox.pos.y;
			float scissorBottom = args.clipBox.size.y;
			if (i < 4 * y) {
				startX = 5;
				startY = (i / 4) * 80.0f + collectionScrollWidget->container->box.pos.y;
				startY += (i % 4) * 14.0f + 69.0f;
				scissorBottom -= 10;
			}
			else {
				startX = (background->box.size.x / (x * 2)) * ((i - 4 * y) * 2 + 1);
				startY = background->box.size.y - 5;
			}
			unsigned int val = (unsigned int)(knobs[i]->getParamQuantity()->getValue());
			if (val > (x + y + 1)) {
				val = (x + y + 1);
			}
			if (val < x + 2) {
				destX = background->box.size.x / (x + 2);
				destX /= 2;
				destX *= (val * 2 + 1);
				destY = 5;
			}
			else {
				destX = 91.0f; 
				destY = 10 + 80 * (val - x - 1) + collectionScrollWidget->container->box.pos.y;
				scissorTop += 10;
				scissorBottom -= 10;
			}
			nvgScissor(args.vg, args.clipBox.pos.x, scissorTop, args.clipBox.size.x, scissorBottom);
			NVGcolor color = colors[val];
			float fade = val?knobs[i]->fade:0.0f;
			drawWire(args, startX, startY, destX, destY, color, fade, labels[val]);
			nvgResetScissor(args.vg);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		char workingSpace[10];
		snprintf(workingSpace, 10, "DO-1%02d", y);
		drawBase(vg, workingSpace);
	}
	void appendContextMenu(Menu *menu) override {
		menu->addChild(new MenuSeparator());
		EventWidgetMenuItem *copy = createMenuItem<EventWidgetMenuItem>("Copy");
		copy->clickHandler = [=]() {
			this->copyToBuffer();
		};
		menu->addChild(copy);
		if (copyBuffer[0] != 0) {
			if (copyBuffer[2] <= y) {
				EventWidgetMenuItem *paste = createMenuItem<EventWidgetMenuItem>("Paste");
				paste->clickHandler = [=]() {
					this->pasteFromBuffer();
				};
				menu->addChild(paste);
			} 
			else {
				MenuLabel *paste = new MenuLabel();
				paste->text = "Paste (device is too small)";
				menu->addChild(paste);
			}
		}
		SchemeModuleWidget::appendContextMenu(menu);
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
};

Model *modelDO105 = createModel<DO1<4,5>, DOWidget<4,5>>("DO-105");
Model *modelDO110 = createModel<DO1<4,10>, DOWidget<4,10>>("DO-110");
Model *modelDO115 = createModel<DO1<4,15>, DOWidget<4,15>>("DO-115");
Model *modelDO120 = createModel<DO1<4,20>, DOWidget<4,20>>("DO-120");
