#include <settings.hpp>
#include "../SubmarineFree.hpp"

void EventWidgetAction::undo() {
	if (undoHandler)
		undoHandler();
}

void EventWidgetAction::redo() {
	if (redoHandler)
		redoHandler();
}

EventWidgetAction::EventWidgetAction(std::string name, std::function<void()> uHandler, std::function<void()> rHandler) {
	this->name = name;
	undoHandler = uHandler;
	redoHandler = rHandler;
}

void EventWidgetButtonBase::onButton(const event::Button &e) {
	if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
		if (clickHandler) {
			clickHandler();
			e.consume(this);
			return;
		}
		OpaqueWidget::onButton(e);
		if (e.isConsumed())
			return;
		if (doubleClickHandler) {
			e.consume(this);
			return;
		}
	}
	if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
		if (rightClickHandler) {
			rightClickHandler();
			e.consume(this);
			return;
		}
	}
	OpaqueWidget::onButton(e);
}

void EventWidgetButtonBase::onDoubleClick(const event::DoubleClick &e) {
	if (doubleClickHandler) {
		doubleClickHandler();
		e.consume(this);
		return;
	}
	OpaqueWidget::onDoubleClick(e);
}

void EventWidgetSlider::draw(const DrawArgs &args) {
	Vec minHandlePos;
	Vec maxHandlePos;
	float width;
	float height;
	if (box.size.x < box.size.y) {
		width = box.size.x;
		height = 10;
		minHandlePos = Vec(box.size.x / 2, height / 2);
		maxHandlePos = Vec(box.size.x / 2, box.size.y - height / 2);
	}
	else {
		width = 10;
		height = box.size.y;
		minHandlePos = Vec(width / 2, box.size.y / 2);
		maxHandlePos = Vec(box.size.x - width / 2, box.size.y / 2);
	}
	Vec pos = Vec(rescale(value, minValue, maxValue, minHandlePos.x, maxHandlePos.x), rescale(value, minValue, maxValue, minHandlePos.y, maxHandlePos.y));
	nvgFillColor(args.vg, nvgRGB(0, 0, 0));
	nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	nvgStrokeWidth(args.vg, 1);
	if (!transparent) {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, minHandlePos.x, minHandlePos.y);
		nvgLineTo(args.vg, maxHandlePos.x, maxHandlePos.y);
		nvgStroke(args.vg);
	}
	nvgBeginPath(args.vg);
	nvgRect(args.vg, pos.x - width / 2 + 0.5, pos.y - height / 2 + 0.5, width - 1, height - 1);
	if (!transparent) nvgFill(args.vg);
	nvgStroke(args.vg);
}
void EventWidgetSlider::onDragStart(const event::DragStart &e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
	startingValue = value;
	APP->window->cursorLock();
}

void EventWidgetSlider::onDragMove(const event::DragMove &e) {
	float oldValue = value;
	value += e.mouseDelta.x * (maxValue - minValue) * 0.001f;
	value = clamp(value, minValue, maxValue);
	if (oldValue != value) {
		if (changingHandler) {
			changingHandler(value, startingValue);
		}
	}
}

void EventWidgetSlider::onDragEnd(const event::DragEnd &e) {
	APP->window->cursorUnlock();
	if (changedHandler) {
		changedHandler(value, startingValue);
	}
}

void EventWidgetSlider::onDoubleClick(const event::DoubleClick &e) {
	startingValue = value;
	value = defaultValue;
	if (changedHandler) {
		changedHandler(value, startingValue);
	}
}

void EventWidgetSlider::onEnter(const event::Enter &e) {
	if (settings::paramTooltip && !tooltip) {
		tooltip = new EventWidgetSliderTooltip();
		tooltip->slider = this;
		APP->scene->addChild(tooltip);
	}	
}

void EventWidgetSlider::onLeave(const event::Leave &e) {
	if (tooltip) {
		APP->scene->removeChild(tooltip);
		delete tooltip;
		tooltip = NULL;
	}
}

void EventWidgetSliderTooltip::step() {
	if (slider->textHandler) {
		text = slider->textHandler(slider->value);
	}
	else {
		text = string::f("%.3g", slider->value);
	}
	std::string label = slider->label;
	if (!label.empty())
		text = label + ": " + text;
	Tooltip::step();
	box.pos = slider->getAbsoluteOffset(slider->box.size).round();
}

EventWidgetCheckBox::EventWidgetCheckBox() {
	clickHandler = [=]() { 
		selected = !selected;
		if (changeHandler) {
			changeHandler();
		}
	};
}

void EventWidgetCheckBox::draw (const DrawArgs &args) {
	nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	if (!label.empty()) {
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 13);	
		nvgTextAlign(args.vg, NVG_ALIGN_MIDDLE);
		nvgText(args.vg, 21, box.size.y / 2, label.c_str(), NULL);
	}
	nvgStrokeWidth(args.vg, 1);
	nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	if (selected) {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.y / 2 - 4, box.size.y / 2 - 5);
		nvgLineTo(args.vg, box.size.y / 2 + 6, box.size.y / 2 + 5);
		nvgMoveTo(args.vg, box.size.y / 2 - 4, box.size.y / 2 + 5);
		nvgLineTo(args.vg, box.size.y / 2 + 6, box.size.y / 2 - 5);
		nvgStroke(args.vg);
	}
	nvgBeginPath(args.vg);
	nvgRect(args.vg, box.size.y / 2 - 7, box.size.y / 2 - 8, 16, 16);
	nvgStroke(args.vg);
	OpaqueWidget::draw(args);
}

EventWidgetRadioButton::EventWidgetRadioButton() {
	clickHandler = [=]() {
		if (selected)
			return;
		selected = true;
		if (changeHandler) {
			changeHandler();
		}
	};
}

void EventWidgetRadioButton::draw (const DrawArgs &args) {
	nvgStrokeWidth(args.vg, 1);
	nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	if (!label.empty()) {
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 13);
		nvgTextAlign(args.vg, NVG_ALIGN_MIDDLE);
		nvgText(args.vg, 21, box.size.y / 2, label.c_str(), NULL);
	}
	if (selected) {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, box.size.y / 2 + 1, box.size.y / 2, 5);
		nvgFill(args.vg);
	}
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, box.size.y / 2 + 1, box.size.y / 2, 8);
	nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	nvgStroke(args.vg);
	EventWidgetButtonBase::draw(args);
}

void EventWidgetLabel::draw(const DrawArgs &args) {
	nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	if (!label.empty()) {
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 13);
		nvgTextAlign(args.vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		nvgText(args.vg, box.size.x / 2, box.size.y / 2, label.c_str(), NULL);
	}
	Widget::draw(args);
}

void EventWidgetButton::draw(const DrawArgs &args) {
	nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	if (!label.empty()) {
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 13);
		nvgTextAlign(args.vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
		nvgText(args.vg, box.size.x / 2, box.size.y / 2, label.c_str(), NULL);
	}
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.5, 0.5, box.size.x - 1, box.size.y - 1);
	nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
	nvgStrokeWidth(args.vg, 1);
	nvgStroke(args.vg);
	OpaqueWidget::draw(args);
}

void EventWidgetMenuItem::onAction(const event::Action &e) {
	e.consume(this);
	if (clickHandler) {
		clickHandler();
	}
}	

Menu *EventWidgetMenuItem::createChildMenu() {
	if (childMenuHandler) {
		return childMenuHandler();
	}
	return MenuItem::createChildMenu();
}

void EventWidgetMenuItem::step() {
	if (stepHandler) {
		stepHandler();
	}
	MenuItem::step();
}

void EventParamField::step() {
	// Keep selected
	APP->event->setSelected(this);
	TextField::step();
}

void EventParamField::setText(std::string text) {
	this->text = text;
	selectAll();
}

void EventParamField::onSelectKey(const event::SelectKey &e) {
	if (e.action == GLFW_PRESS && (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)) {
		if (changeHandler) {
			changeHandler(text);
		}
		ui::MenuOverlay *overlay = getAncestorOfType<ui::MenuOverlay>();
		overlay->requestDelete();
		e.consume(this);
	}
	if (!e.getTarget())
		TextField::onSelectKey(e);
}
