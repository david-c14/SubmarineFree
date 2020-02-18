//SubTag W20

#include <string.h>
#include "SubmarineFree.hpp"
#include "settings.hpp"

struct BulkParamWidget : widget::OpaqueWidget {
	float *value = NULL;
	float minValue = .0f;
	float maxValue = 1.0f;
	ui::Tooltip* tooltip = NULL;
	std::string description;

	void onButton(const event::Button& e) override;
	void onDoubleClick(const event::DoubleClick& e) override;
	void onEnter(const event::Enter& e) override;
	void onLeave(const event::Leave& e) override;

	/** For legacy patch loading */
	void fromJson(json_t* rootJ);
	void createContextMenu();
	void resetAction();
	virtual void reset() {}
	virtual void randomize() {}

	std::string getString() { return std::string("hello"); }
};
/*	
struct ParamField : ui::TextField {
	ParamWidget* paramWidget;

	void step() override {
		// Keep selected
		APP->event->setSelected(this);
		TextField::step();
	}

	void setParamWidget(ParamWidget* paramWidget) {
		this->paramWidget = paramWidget;
		if (paramWidget->paramQuantity)
			text = paramWidget->paramQuantity->getDisplayValueString();
		selectAll();
	}

	void onSelectKey(const event::SelectKey& e) override {
		if (e.action == GLFW_PRESS && (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)) {
			float oldValue = paramWidget->paramQuantity->getValue();
			if (paramWidget->paramQuantity)
				paramWidget->paramQuantity->setDisplayValueString(text);
			float newValue = paramWidget->paramQuantity->getValue();

			if (oldValue != newValue) {
				// Push ParamChange history action
				history::ParamChange* h = new history::ParamChange;
				h->moduleId = paramWidget->paramQuantity->module->id;
				h->paramId = paramWidget->paramQuantity->paramId;
				h->oldValue = oldValue;
				h->newValue = newValue;
				APP->history->push(h);
			}

			ui::MenuOverlay* overlay = getAncestorOfType<ui::MenuOverlay>();
			overlay->requestDelete();
			e.consume(this);
		}

		if (!e.getTarget())
			TextField::onSelectKey(e);
	}
};*/


struct BulkParamTooltip : ui::Tooltip {
	BulkParamWidget* bulkParamWidget;

	void step() override {
		if (bulkParamWidget->value) {
			// Quantity string
			text = bulkParamWidget->getString();
			// Param description
			std::string description = bulkParamWidget->description;
			if (!description.empty())
				text += "\n" + description;
		}
		Tooltip::step();
		// Position at bottom-right of parameter
		box.pos = bulkParamWidget->getAbsoluteOffset(bulkParamWidget->box.size).round();
		// Fit inside parent (copied from Tooltip.cpp)
		assert(parent);
		box = box.nudge(parent->box.zeroPos());
	}
};

/*
struct ParamResetItem : ui::MenuItem {
	ParamWidget* paramWidget;
	void onAction(const event::Action& e) override {
		paramWidget->resetAction();
	}
};

*/

void BulkParamWidget::onButton(const event::Button& e) {
	OpaqueWidget::onButton(e);

	// Touch parameter
	if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT && (e.mods & RACK_MOD_MASK) == 0) {
		e.consume(this);
	}

	// Right click to open context menu
	if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_RIGHT && (e.mods & RACK_MOD_MASK) == 0) {
		createContextMenu();
		e.consume(this);
	}
}

void BulkParamWidget::onDoubleClick(const event::DoubleClick& e) {
	resetAction();
}

void BulkParamWidget::onEnter(const event::Enter& e) {
	if (settings::paramTooltip && !tooltip && value) {
		BulkParamTooltip* bulkParamTooltip = new BulkParamTooltip;
		bulkParamTooltip->bulkParamWidget = this;
		APP->scene->addChild(bulkParamTooltip);
		tooltip = bulkParamTooltip;
	}
}

void BulkParamWidget::onLeave(const event::Leave& e) {
	if (tooltip) {
		APP->scene->removeChild(tooltip);
		delete tooltip;
		tooltip = NULL;
	}
}

void BulkParamWidget::fromJson(json_t* rootJ) {
	json_t* valueJ = json_object_get(rootJ, "value");
	if (valueJ) {
		if (value)
			*value = json_number_value(valueJ);
	}
}

void BulkParamWidget::createContextMenu() {
	ui::Menu* menu = createMenu();

	MenuLabel* menuLabel = new MenuLabel;
	menuLabel->text = getString();
	menu->addChild(menuLabel);

	EventParamField *paramField = new EventParamField;
	paramField->box.size.x = 100;
	menu->addChild(paramField);


	EventWidgetMenuItem *resetItem = new EventWidgetMenuItem;
	resetItem->text = "Initialize";
	resetItem->rightText = "Double-click";
	resetItem->clickHandler = [=]() {
		this->resetAction();
	};
	menu->addChild(resetItem);
}

void BulkParamWidget::resetAction() {
	if (value) {
		float oldValue = *value;
		reset();
		float newValue = *value;
		if (oldValue != newValue) {
			APP->history->push(new EventWidgetAction(
				"reset parameter",
				[this,oldValue]() {
					if (this->value) *value = oldValue;
				},
				[this,newValue]() {
					if (this->value) *value = newValue;
				}
			));
		}
	}
}

/** Implements vertical dragging behavior for BulkParamWidgets */
struct BulkKnob : BulkParamWidget {
	/** Multiplier for mouse movement to adjust knob value */
	float speed = 1.0;
	float oldValue = .0f;
	bool smooth = true;
	/** Enable snapping at integer values */
	bool snap = false;
	float snapValue = NAN;
	/** Drag horizontally instead of vertically */
	bool horizontal = false;

	void onHover(const event::Hover& e) override;
	void onButton(const event::Button& e) override;
	void onDragStart(const event::DragStart& e) override;
	void onDragEnd(const event::DragEnd& e) override;
	void onDragMove(const event::DragMove& e) override;
	void reset() override;
	void randomize() override;
};

void BulkKnob::onHover(const event::Hover& e) {
	math::Vec c = box.size.div(2);
	float dist = e.pos.minus(c).norm();
	if (dist <= c.x) {
		BulkParamWidget::onHover(e);
	}
}

void BulkKnob::onButton(const event::Button& e) {
	math::Vec c = box.size.div(2);
	float dist = e.pos.minus(c).norm();
	if (dist <= c.x) {
		BulkParamWidget::onButton(e);
	}
}

void BulkKnob::onDragStart(const event::DragStart& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	if (value) {
		oldValue = *value;
		if (snap) {
			snapValue = *value;
		}
	}

	APP->window->cursorLock();
}

void BulkKnob::onDragEnd(const event::DragEnd& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	APP->window->cursorUnlock();

	if (value) {
		float newValue = *value;
		if (oldValue != newValue) {
			APP->history->push(new EventWidgetAction(
				"move knob",
				[this]() {
					if (this->value) *value = this->oldValue;
				},
				[this,newValue]() {
					if (this->value) *value = newValue;
				}
			));
		}
	}
}

void BulkKnob::onDragMove(const event::DragMove& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	if (value) {
		float range = 2.0f;
		if (std::isfinite(minValue) && std::isfinite(maxValue)) {
			range = maxValue - minValue;
		}
		float delta = (horizontal ? e.mouseDelta.x : -e.mouseDelta.y);
		delta *= 0.0015f;
		delta *= speed;
		delta *= range;

		// Drag slower if mod is held
		int mods = APP->window->getMods();
		if ((mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
			delta /= 16.f;
		}
		// Drag even slower if mod+shift is held
		if ((mods & RACK_MOD_MASK) == (RACK_MOD_CTRL | GLFW_MOD_SHIFT)) {
			delta /= 256.f;
		}

		if (snap) {
			snapValue += delta;
			snapValue = math::clamp(snapValue, minValue, maxValue);
			*value = std::round(snapValue);
		}
		else {
			*value += delta;
		}
	}

	BulkParamWidget::onDragMove(e);
}

void BulkKnob::reset() {
	if (value)
		oldValue = snapValue = *value = .0f;
}

void BulkKnob::randomize() {
	if (value && std::isfinite(minValue) && std::isfinite(maxValue)) {
		*value = math::rescale(random::uniform(), 0.f, 1.f, minValue, maxValue);
		if (snap)
			*value = std::round(*value);
		oldValue = snapValue = *value;
	}
}

struct BulkLightKnob : BaseLightKnob, BulkKnob {
	BulkLightKnob() {smooth = false;}
	float getBLKValue() override {
		if (value)
			return *value;
		return BaseLightKnob::getBLKValue();
	}
	float getBLKMinValue() override {
		return minValue;
	}
	float getBLKMaxValue() override {
		return maxValue;
	}
	void draw(const DrawArgs &args) override {
		doDraw(args);
	}
};

	float clipboard[256];
	bool clipboardUsed = false;

struct LT_116 : Module {
	enum ParamIds {
		PARAM_OUTPUT_CHANNELS,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float bulkParams[256];

	int numberOfInputs = 1;
	int numberOfOutputs = 16;
	
	LT_116() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		//		configParam<BulkQuantity>(PARAM_COEFF_1 + i * 16 + j, -INFINITY, INFINITY, 0.0f, string::f("Coefficient [%d,%d]", i + 1, j + 1));
		configParam(PARAM_OUTPUT_CHANNELS, 1.0f, 16.0f, 16.0f, "Number of channels in output");
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		json_t *arr = json_array();
		for (unsigned int i = 0; i < 256; i++) {
			json_array_append_new(arr, json_real(bulkParams[i]));
		}
		json_object_set_new(rootJ, "coefficients", arr);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		json_t *arr = json_object_get(rootJ, "coefficients");
		if (arr) {
			int size = json_array_size(arr);
			if (size > 256)
				size = 256;
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					bulkParams[i] = json_real_value(j1);
				}
			}
		}
	}

	void process(const ProcessArgs &args) override {
		numberOfInputs = inputs[INPUT_1].getChannels();
		numberOfOutputs = params[PARAM_OUTPUT_CHANNELS].getValue();
		for (int i = 0; i < numberOfOutputs; i += 4) {
			float working[4] = {0};
			for (int j = 0; j < numberOfInputs; j++) {
				float input = inputs[INPUT_1].getVoltage(j);
				for (int n = 0; n < 4; n++) {
					working[n] += input * bulkParams[(i + n) * 16 + j];
				}
			}
			for (unsigned int n = 0; n < 4; n++) {
				outputs[OUTPUT_1].setVoltage(working[n], i + n);
			}
		}
		outputs[OUTPUT_1].setChannels(numberOfOutputs);
	}
};

template <class K>
K* createBulkParamCentered(math::Vec pos, float minValue, float maxValue) {
	K* widget = new K();
	widget->box.pos = pos.minus(widget->box.size.div(2));
	widget->minValue = minValue;
	widget->maxValue = maxValue;
	return widget;
}
	
struct LT116 : SchemeModuleWidget {
	BulkLightKnob *knobs[256];
	float *bulkParams;
	LT116(LT_116 *module) {
		setModule(module);
		if (module)
			bulkParams = module->bulkParams;
		this->box.size = Vec(300, 380);
		addChild(new SchemePanel(this->box.size));

		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				knobs[i * 16 + j] = createBulkParamCentered<TinyKnob<BulkLightKnob>>(Vec(15 + i * 18, 30 + j * 18), -INFINITY, +INFINITY);
				if (module)
					knobs[i * 16 + j]->value = &(bulkParams[i * 16 + j]);
				/*knobs[i * 16 + j]->rightClickHandler = [=](RotaryKnob *knob) {
					addKnobMenu(knob);
				};*/
				addChild(knobs[i * 16 + j]);
			}
		}
		addInput(createInputCentered<SilverPort>(Vec(35, 330), module, LT_116::INPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(265, 330), module, LT_116::OUTPUT_1));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(200, 330), module, LT_116::PARAM_OUTPUT_CHANNELS));
	}
	void appendContextMenu(Menu *menu) override {
		if (!module)
			return;
		menu->addChild(new MenuSeparator);

		EventWidgetMenuItem *cmi = createMenuItem<EventWidgetMenuItem>("Copy");
		cmi->clickHandler = [=]() {
			copy();
		};
		menu->addChild(cmi);
		
		if (clipboardUsed) {
			EventWidgetMenuItem *pmi = createMenuItem<EventWidgetMenuItem>("Paste");
			pmi->clickHandler = [=]() {
				paste();
			};
			menu->addChild(pmi);
		}
	}
	void step() override {
		if (!module)
			return;
		LT_116 *ltModule = dynamic_cast<LT_116 *>(module);
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				knobs[i * 16 + j]->setEnabled((j < ltModule->numberOfInputs) && (i < ltModule->numberOfOutputs));
			}
		}
		SchemeModuleWidget::step();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LT-116");

		// Text
		drawText(vg, 35, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 265, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUTPUT");
		drawText(vg, 200, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CHANNELS");
		drawText(vg, 50, 330, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x93"); 
		drawText(vg, 240, 330, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x92"); 
	}
	void addKnobMenu() {
	//	appendContextMenu(menu);
	}
	void copy() {
		clipboardUsed = true;
		for (int i = 0; i < 256; i++) {
			clipboard[i] = bulkParams[i];
		}
	}
	void paste() {
		for (int i = 0; i < 256; i++) {
			bulkParams[i] = clipboard[i];
		}
	}
};

Model *modelLT116 = createModel<LT_116, LT116>("LT-116");
