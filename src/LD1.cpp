//SubTag TM DS W2 W6 AM

#include "shared/DS.hpp"

template <int x>
struct LD_1 : DS_Module {
	enum ParamIds {
		PARAM_CUTOFF_1,
		PARAM_WIDTH_1 = x,
		NUM_PARAMS = x + x
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

	DS_Schmitt schmittState[x];

	LD_1() : DS_Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < x; i++) {
			configParam(PARAM_CUTOFF_1 + i, -10.0f, 10.0f, 5.0f, "Signal " + std::to_string(i + 1) + " Cutoff Centre", " V");
			configParam(PARAM_WIDTH_1 + i, 0.0f, 5.0f, 1.0f, "Signal " + std::to_string(i + 1) + " Hysteresis", " V");
			configInput(INPUT_1 + i, "Signal " + std::to_string(i + 1));
			configOutput(OUTPUT_1 + i, "Signal " + std::to_string(i + 1));
		}
	}
	void process(const ProcessArgs &args) override {
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].setVoltage(output(schmittState[i].state(params[PARAM_CUTOFF_1 + i].getValue() - params[PARAM_WIDTH_1 + i].getValue(), params[PARAM_CUTOFF_1 + i].getValue() + params[PARAM_WIDTH_1 + i].getValue(), inputs[INPUT_1 + i].getVoltage())));
		}
	}
};

namespace {

	struct LDMenuItem: MenuItem {
		Module *module;
		float cutoff;
		float width;
		unsigned int deviceCount;
		ParamWidget** cutoffWidgets;
		ParamWidget** widthWidgets;
		void onAction(const event::Action &e) override {
			for (unsigned int i = 0; i < deviceCount; i++) {

				cutoffWidgets[i]->getParamQuantity()->setValue(cutoff);
				widthWidgets[i]->getParamQuantity()->setValue(width);
			}
		}
	};

	struct LDParentMenuItem : MenuItem {
		Module *module;
		unsigned int deviceCount;
		int cutoffParamIndex;
		int widthParamIndex;
		ParamWidget **cutoffWidgets;
		ParamWidget **widthWidgets;
		Menu *createChildMenu() override {
			Menu *menu = new Menu();
			LDMenuItem *menuItem = createMenuItem<LDMenuItem>("Cutoff 5V");
			menuItem->module = module;
			menuItem->cutoff = 5.0f;
			menuItem->width = 1.0f;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menu->addChild(menuItem);
			menuItem = createMenuItem<LDMenuItem>("Cutoff 0V");
			menuItem->module = module;
			menuItem->cutoff = 0.0f;
			menuItem->width = 0.0f;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menu->addChild(menuItem);
			menuItem = createMenuItem<LDMenuItem>("Cutoff 2.5V");
			menuItem->module = module;
			menuItem->cutoff = 2.5f;
			menuItem->width = 0.5f;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menu->addChild(menuItem);
			menuItem = createMenuItem<LDMenuItem>("TTL Levels");
			menuItem->module = module;
			menuItem->cutoff = 1.4f;
			menuItem->width = 0.6f;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menu->addChild(menuItem);
			
			return menu;
		}
	};

} // end namespace

struct LD103 : SchemeModuleWidget {
	static const int deviceCount = 3;	
	ParamWidget* cutoffWidgets[deviceCount];
	ParamWidget* widthWidgets[deviceCount];
	LD103(LD_1<deviceCount> *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < deviceCount; i++) {
			int offset = 116 * i;
			addInput(createInputCentered<SilverPort>(Vec(15,31.5 + offset), module, LD_1<3>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,115.5 + offset), module, LD_1<3>::OUTPUT_1 + i));
			cutoffWidgets[i] = createParamCentered<TinyKnob<LightKnob>>(Vec(15, 57.5 + offset), module, LD_1<3>::PARAM_CUTOFF_1 + i);
			widthWidgets[i] = createParamCentered<TinyKnob<LightKnob>>(Vec(15, 89.5 + offset), module, LD_1<3>::PARAM_WIDTH_1 + i);
			addParam(cutoffWidgets[i]);
			addParam(widthWidgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		if (module) {
			LDParentMenuItem *menuItem = createMenuItem<LDParentMenuItem>("Input Range");
			menuItem->module = module;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menuItem->rightText = SUBMENU;
			menu->addChild(menuItem);
		}
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LD-103");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 2);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (int i = 0; i < 3; i++) {
			nvgMoveTo(vg, 15, 31.5 + i * 116);
			nvgLineTo(vg, 15, 115.5 + i * 116);
		}
		nvgStroke(vg);
		for (int i = 0; i < 3; i++) {
			drawText(vg, 22, 72 + i * 116, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "C");
			drawText(vg, 3, 80 + i * 116, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "W");
		}
	}
};

struct LD106 : SchemeModuleWidget {
	static const int deviceCount = 6;	
	ParamWidget* cutoffWidgets[deviceCount];
	ParamWidget* widthWidgets[deviceCount];
	LD106(LD_1<deviceCount> *module) {
		setModule(module);
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < deviceCount; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<SilverPort>(Vec(16.5,31.5 + offset), module, LD_1<6>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,31.5 + offset), module, LD_1<6>::OUTPUT_1 + i));

			cutoffWidgets[i] = createParamCentered<SmallKnob<LightKnob>>(Vec(16, 59 + offset), module, LD_1<6>::PARAM_CUTOFF_1 + i);
			widthWidgets[i] = createParamCentered<SmallKnob<LightKnob>>(Vec(74, 59 + offset), module, LD_1<6>::PARAM_WIDTH_1 + i);
			addParam(cutoffWidgets[i]);
			addParam(widthWidgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override{
		SchemeModuleWidget::appendContextMenu(menu);
		if (module) {
			LDParentMenuItem *menuItem = createMenuItem<LDParentMenuItem>("Input Range");
			menuItem->module = module;
			menuItem->rightText = SUBMENU;
			menuItem->deviceCount = deviceCount;
			menuItem->cutoffWidgets = cutoffWidgets;
			menuItem->widthWidgets = widthWidgets;
			menu->addChild(menuItem);
		}
		DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
		if (dsMod) {
			dsMod->appendContextMenu(menu);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LD-106");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineJoin(vg, NVG_ROUND);
		nvgLineCap(vg, NVG_ROUND);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgBeginPath(vg);
		for (int i = 0; i < 5; i++) {
			nvgMoveTo(vg, 3, 73.5 + i * 58);
			nvgLineTo(vg, 87, 73.5 + i * 58);
		}
		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgBeginPath(vg);
		for (int i = 0; i < 6; i++) {
			nvgMoveTo(vg, 16, 31.5 + i * 58);
			nvgLineTo(vg, 39, 31.5 + i * 58);
			nvgMoveTo(vg, 55, 31.5 + i * 58);
			nvgLineTo(vg, 74, 31.5 + i * 58);
			nvgMoveTo(vg, 39.5, 20 + i * 58);
			nvgLineTo(vg, 55.5, 31.5 + i * 58);
			nvgLineTo(vg, 39.5, 43 + i * 58);
			nvgClosePath(vg);
			nvgMoveTo(vg, 41.5, 34.5 + i * 58);
			nvgLineTo(vg, 46.5, 34.5 + i * 58);
			nvgLineTo(vg, 46.5, 29.5 + i * 58);
			nvgMoveTo(vg, 48.5, 29.5 + i * 58);
			nvgLineTo(vg, 43.5, 29.5 + i * 58);
			nvgLineTo(vg, 43.5, 34.5 + i * 58);
		}
		nvgStroke(vg);
		for (int i = 0; i < 6; i++) {
			drawText(vg, 28, 58 + i * 58, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 6, gScheme.getContrast(module), "CUTOFF");
			drawText(vg, 62, 65 + i * 58, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 6, gScheme.getContrast(module), "WIDTH");
		}
	}
};

Model *modelLD103 = createModel<LD_1<3>, LD103>("LD-103");
Model *modelLD106 = createModel<LD_1<6>, LD106>("LD-106");
