#include "../SubmarineFree.hpp"
#include "settings.hpp"

void BulkParamTooltip::step() {
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

std::string BulkParamWidget::getString() {
	std::string s;
	if (!label.empty())
		s += label + ": ";
	s += getDisplayValueString() + unit;
	return s;
}

float BulkParamWidget::getDisplayValue() {
	if (!value)
		return .0f;
	float v = *value;
	if (displayBase == 0.f) {
		// Linear
		// v is unchanged
	}
	else if (displayBase < 0.f) {
		// Logarithmic
		v = std::log(v) / std::log(-displayBase);
	}
	else {
		// Exponential
		v = std::pow(displayBase, v);
	}
	return v * displayMultiplier + displayOffset;
}

void BulkParamWidget::setDisplayValue(float displayValue) {
	if (!value)
		return;
	float v = displayValue - displayOffset;
	if (displayMultiplier == 0.f)
		v = 0.f;
	else
		v /= displayMultiplier;
	if (displayBase == 0.f) {
		// Linear
			// v is unchanged
	}
	else if (displayBase < 0.f) {
		// Logarithmic
		v = std::pow(-displayBase, v);
	}
	else {
		// Exponential
		v = std::log(v) / std::log(displayBase);
	}
	*value = v;
}

std::string BulkParamWidget::getDisplayValueString() {
	return string::f("%.*g", 5, math::normalizeZero(getDisplayValue()));
}

void BulkParamWidget::setDisplayValueString(std::string s) {
	float v = 0.f;
	char suffix[2];
	int n = std::sscanf(s.c_str(), "%f%1s", &v, suffix);
	if (n >= 2) {
		// Parse SI prefixes
		switch (suffix[0]) {
			case 'n': v *= 1e-9f; break;
			case 'u': v *= 1e-6f; break;
			case 'm': v *= 1e-3f; break;
			case 'k': v *= 1e3f; break;
			case 'M': v *= 1e6f; break;
			case 'G': v *= 1e9f; break;
			default: break;
		}
	}
	if (n >= 1)
		setDisplayValue(v);
}

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
	paramField->text = getDisplayValueString();
	paramField->selectAll();
	paramField->changeHandler = [=](std::string text) {
		float oldValue = *value;
		setDisplayValueString(text);
		float newValue = *value;

		if (oldValue != newValue) {
			int thisModuleId = module->id;
			int thisParamId = paramId;
			APP->history->push(new EventWidgetAction(
				"change parameter",
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, oldValue);
				},
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, newValue);
				}
			));	
		}
	};
	menu->addChild(paramField);


	EventWidgetMenuItem *resetItem = new EventWidgetMenuItem;
	resetItem->text = "Initialize";
	resetItem->rightText = "Double-click";
	resetItem->clickHandler = [=]() {
		this->resetAction();
	};
	menu->addChild(resetItem);
	if (contextMenuCallback) {
		contextMenuCallback(menu);
	}
}

void BulkParamWidget::resetAction() {
	if (value) {
		float oldValue = *value;
		reset();
		float newValue = *value;
		if (oldValue != newValue) {
			int thisModuleId = module->id;
			int thisParamId = paramId;
			APP->history->push(new EventWidgetAction(
				"reset parameter",
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, oldValue);
				},
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, newValue);
				}
			));
		}
	}
}

void BulkParamWidget::setBulkParamValue(int thisModuleId, int thisParamId, float thisValue) {
	SchemeModuleWidget *smw = dynamic_cast<SchemeModuleWidget *>(APP->scene->rack->getModule(thisModuleId));
	if (smw) {
		float *value = smw->getBulkParam(thisParamId);
		if (value) *value = thisValue;
	}
}
