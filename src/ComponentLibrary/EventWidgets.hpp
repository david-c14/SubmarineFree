#include <functional>

struct EventAction : history::Action {
	std::function<void ()> undoHandler;
	std::function<void ()> redoHandler;
	void undo() override {
		if (undoHandler)
			undoHandler();
	}
	void redo() override {
		if (redoHandler)
			redoHandler();
	}
	EventAction(std::string name, std::function<void()> uHandler, std::function<void()> rHandler) {
		this->name = name;
		undoHandler = uHandler;
		redoHandler = rHandler;
	}
};

struct EventButton : OpaqueWidget {
	std::function<void ()> clickHandler;
	std::function<void ()> rightClickHandler;
	void onButton(const event::Button &e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			if (clickHandler) {
				clickHandler();
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
};

struct EventSlider;

struct EventSliderTooltip : ui::Tooltip {
	EventSlider *slider;
	void step() override; 
};

struct EventSlider : OpaqueWidget {
	float startingValue = 0.5f;
	int transparent = false;
	float value = 0.5f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float defaultValue = 0.5f;
	std::string label;
	EventSliderTooltip *tooltip = NULL;
	std::function<void(float, float)> changedHandler;
	std::function<void(float, float)> changingHandler;
	std::function<std::string(float)> textHandler;
	void draw(const DrawArgs &args) override {
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
	void onDragStart(const event::DragStart &e) override {
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) return;
		startingValue = value;
		APP->window->cursorLock();
	}
	void onDragMove(const event::DragMove &e) override {
		float oldValue = value;
		value += e.mouseDelta.x * (maxValue - minValue) * 0.001f;
		value = clamp(value, minValue, maxValue);
		if (oldValue != value) {
			if (changingHandler) {
				changingHandler(value, startingValue);
			}
		}
	}
	void onDragEnd(const event::DragEnd &e) override {
		APP->window->cursorUnlock();
		if (changedHandler) {
			changedHandler(value, startingValue);
		}
	}
	void onDoubleClick(const event::DoubleClick &e) override {
		startingValue = value;
		value = defaultValue;
		if (changedHandler) {
			changedHandler(value, startingValue);
		}
	}
	void onEnter(const event::Enter &e) override {
		if (settings::paramTooltip && !tooltip) {
			tooltip = new EventSliderTooltip();
			tooltip->slider = this;
			APP->scene->addChild(tooltip);
		}	
	}
	
	void onLeave(const event::Leave &e) override {
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = NULL;
		}
	}
};

void EventSliderTooltip::step() {
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

struct CheckBox : EventButton {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	CheckBox() {
		clickHandler = [=]() { 
			selected = !selected;
			if (changeHandler) {
				changeHandler();
			}
		};
	}
	void draw (const DrawArgs &args) override {
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
};

struct EventRadioButton : EventButton {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventRadioButton() {
		clickHandler = [=]() {
			if (selected)
				return;
			selected = true;
			if (changeHandler) {
				changeHandler();
			}
		};
	}
	void draw (const DrawArgs &args) override {
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
		EventButton::draw(args);
	}
};

struct EventLabel : Widget {
	std::string label;
	void draw(const DrawArgs &args) override {
		nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		if (!label.empty()) {
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 13);
			nvgTextAlign(args.vg, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
			nvgText(args.vg, box.size.x / 2, box.size.y / 2, label.c_str(), NULL);
		}
		Widget::draw(args);
	}
};

struct RectButton : OpaqueWidget {
	std::string label;
	std::function<void ()> clickHandler;
	void draw(const DrawArgs &args) override {
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
	void onButton(const event::Button &e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			e.consume(this);
			if (clickHandler) {
				clickHandler();
			}
			return;
		}
		OpaqueWidget::onButton(e);
	}
};

struct EventMenuItem : MenuItem {
	std::function<void()> clickHandler;
	void onAction(const event::Action &e) override {
		e.consume(this);
		if (clickHandler) {
			clickHandler();
		}
	}	
};
