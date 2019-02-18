//SubTag TM DS W2 W6 AM

#include "DS.hpp"

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

	LD_1() : DS_Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override {
		for (int i = 0; i < x; i++) {
			outputs[OUTPUT_1 + i].value = output(schmittState[i].state(params[PARAM_CUTOFF_1 + i].value - params[PARAM_WIDTH_1 + i].value, params[PARAM_CUTOFF_1 + i].value + params[PARAM_WIDTH_1 + i].value, inputs[INPUT_1 + i].value));
		}
	}
};

struct LD103 : SchemeModuleWidget {
	static const int deviceCount = 3;	
	ParamWidget *cutoffWidgets[deviceCount];
	ParamWidget *widthWidgets[deviceCount];
	LD103(LD_1<deviceCount> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < deviceCount; i++) {
			int offset = 116 * i;
			addInput(createInputCentered<SilverPort>(Vec(15,31.5 + offset), module, LD_1<3>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(15,115.5 + offset), module, LD_1<3>::OUTPUT_1 + i));

			cutoffWidgets[i] = createParamCentered<TinyKnob<LightKnob>>(Vec(15, 57.5 + offset), module, LD_1<3>::PARAM_CUTOFF_1 + i, -10.0f, 10.0f, 5.0f);
			addParam(cutoffWidgets[i]);
			widthWidgets[i] = createParamCentered<TinyKnob<LightKnob>>(Vec(15, 89.5 + offset), module, LD_1<3>::PARAM_WIDTH_1 + i, 0.0f, 5.0f, 1.0f);
			addParam(widthWidgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override;
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LD-103");
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 2);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (int i = 0; i < 3; i++) {
			nvgMoveTo(vg, 15, 31.5 + i * 116);
			nvgLineTo(vg, 15, 115.5 + i * 116);
		}
		nvgStroke(vg);
		for (int i = 0; i < 3; i++) {
			drawText(vg, 22, 72 + i * 116, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "C");
			drawText(vg, 3, 80 + i * 116, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "W");
		}
	}
};

struct LDMenuItem3: MenuItem {
	LD103 *ld103;
	float cutoff;
	float width;
	void onAction(EventAction &e) override {
		for (int i = 0; i < LD103::deviceCount; i++) {
			ld103->cutoffWidgets[i]->setValue(cutoff);
			ld103->widthWidgets[i]->setValue(width);
		}
	}
};

void LD103::appendContextMenu(Menu *menu) {
	SchemeModuleWidget::appendContextMenu(menu);
	menu->addChild(MenuEntry::create());
	LD103 *ld103 = dynamic_cast<LD103*>(this);
	assert(ld103);
	LDMenuItem3 *menuItem = MenuItem::create<LDMenuItem3>("Cutoff 5V");
	menuItem->ld103 = ld103;
	menuItem->cutoff = 5.0f;
	menuItem->width = 1.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem3>("Cutoff 0V");
	menuItem->ld103 = ld103;
	menuItem->cutoff = 0.0f;
	menuItem->width = 0.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem3>("Cutoff 2.5V");
	menuItem->ld103 = ld103;
	menuItem->cutoff = 2.5f;
	menuItem->width = 0.5f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem3>("TTL Levels");
	menuItem->ld103 = ld103;
	menuItem->cutoff = 1.4f;
	menuItem->width = 0.6f;
	menu->addChild(menuItem);
	DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
	if (dsMod) {
		dsMod->appendContextMenu(menu);
	}
}

struct LD106 : SchemeModuleWidget {
	static const int deviceCount = 6;	
	ParamWidget *cutoffWidgets[deviceCount];
	ParamWidget *widthWidgets[deviceCount];
	LD106(LD_1<deviceCount> *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		for (int i = 0; i < deviceCount; i++) {
			int offset = 58 * i;
			addInput(createInputCentered<SilverPort>(Vec(16.5,31.5 + offset), module, LD_1<6>::INPUT_1 + i));

			addOutput(createOutputCentered<BluePort>(Vec(74.5,31.5 + offset), module, LD_1<6>::OUTPUT_1 + i));

			cutoffWidgets[i] = createParamCentered<SmallKnob<LightKnob>>(Vec(16, 59 + offset), module, LD_1<6>::PARAM_CUTOFF_1 + i, -10.0f, 10.0f, 5.0f);
			addParam(cutoffWidgets[i]);
			widthWidgets[i] = createParamCentered<SmallKnob<LightKnob>>(Vec(74, 59 + offset), module, LD_1<6>::PARAM_WIDTH_1 + i, 0.0f, 5.0f, 1.0f);
			addParam(widthWidgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override;
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LD-106");
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 1);
		nvgLineJoin(vg, NVG_ROUND);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (int i = 0; i < 6; i++) {
			nvgMoveTo(vg, 3, 73.5 + i * 58);
			nvgLineTo(vg, 87, 73.5 + i * 58);
			nvgMoveTo(vg, 16, 89.5 + i * 58);
			nvgLineTo(vg, 39, 89.5 + i * 58);
			nvgMoveTo(vg, 55, 89.5 + i * 58);
			nvgLineTo(vg, 74, 89.5 + i * 58);
			nvgMoveTo(vg, 39.5, 78 + i * 58);
			nvgLineTo(vg, 55.5, 89.5 + i * 58);
			nvgLineTo(vg, 39.5, 101 + i * 58);
			nvgClosePath(vg);
		}
		nvgStroke(vg);
		for (int i = 0; i < 6; i++) {
			drawText(vg, 28, 58 + i * 58, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "CUTOFF");
			drawText(vg, 62, 65 + i * 58, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "WIDTH");
		}
	}
};

struct LDMenuItem: MenuItem {
	LD106 *ld106;
	float cutoff;
	float width;
	void onAction(EventAction &e) override {
		for (int i = 0; i < LD106::deviceCount; i++) {
			ld106->cutoffWidgets[i]->setValue(cutoff);
			ld106->widthWidgets[i]->setValue(width);
		}
	}
};

void LD106::appendContextMenu(Menu *menu) {
	SchemeModuleWidget::appendContextMenu(menu);
	menu->addChild(MenuEntry::create());
	LD106 *ld106 = dynamic_cast<LD106*>(this);
	assert(ld106);
	LDMenuItem *menuItem = MenuItem::create<LDMenuItem>("Cutoff 5V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 5.0f;
	menuItem->width = 1.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("Cutoff 0V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 0.0f;
	menuItem->width = 0.0f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("Cutoff 2.5V");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 2.5f;
	menuItem->width = 0.5f;
	menu->addChild(menuItem);
	menuItem = MenuItem::create<LDMenuItem>("TTL Levels");
	menuItem->ld106 = ld106;
	menuItem->cutoff = 1.4f;
	menuItem->width = 0.6f;
	menu->addChild(menuItem);
	DS_Module *dsMod = dynamic_cast<DS_Module *>(module);
	if (dsMod) {
		dsMod->appendContextMenu(menu);
	}
}

Model *modelLD103 = Model::create<LD_1<3>, LD103>("Submarine (Free)", "LD-103", "LD-103 Schmitt Trigger Line Drivers", LOGIC_TAG, MULTIPLE_TAG);
Model *modelLD106 = Model::create<LD_1<6>, LD106>("Submarine (Free)", "LD-106", "LD-106 Schmitt Trigger Line Drivers", LOGIC_TAG, MULTIPLE_TAG);
