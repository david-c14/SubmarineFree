#include <functional>

struct EventWidgetAction : history::Action {
	std::function<void ()> undoHandler;
	std::function<void ()> redoHandler;
	void undo() override;
	void redo() override;
	EventWidgetAction(std::string name, std::function<void()> uHandler, std::function<void()> rHandler);
};

struct EventWidgetButtonBase : OpaqueWidget {
	std::function<void ()> clickHandler;
	std::function<void ()> rightClickHandler;
	void onButton(const event::Button &e) override;
};

struct EventWidgetSlider;

struct EventWidgetSliderTooltip : ui::Tooltip {
	EventWidgetSlider *slider;
	void step() override; 
};

struct EventWidgetSlider : OpaqueWidget {
	float startingValue = 0.5f;
	int transparent = false;
	float value = 0.5f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float defaultValue = 0.5f;
	std::string label;
	EventWidgetSliderTooltip *tooltip = NULL;
	std::function<void(float, float)> changedHandler;
	std::function<void(float, float)> changingHandler;
	std::function<std::string(float)> textHandler;
	void draw(const DrawArgs &args) override;
	void onDragStart(const event::DragStart &e) override;
	void onDragMove(const event::DragMove &e) override;
	void onDragEnd(const event::DragEnd &e) override;
	void onDoubleClick(const event::DoubleClick &e) override;
	void onEnter(const event::Enter &e) override;
	void onLeave(const event::Leave &e) override;
};

struct EventWidgetCheckBox : EventWidgetButtonBase {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventWidgetCheckBox();
	void draw (const DrawArgs &args) override;
};

struct EventWidgetRadioButton : EventWidgetButtonBase {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventWidgetRadioButton();
	void draw (const DrawArgs &args) override;
};

struct EventWidgetLabel : Widget {
	std::string label;
	void draw(const DrawArgs &args) override;
};

struct EventWidgetButton : OpaqueWidget {
	std::string label;
	std::function<void ()> clickHandler;
	void draw(const DrawArgs &args) override;
	void onButton(const event::Button &e) override;
};

struct EventWidgetMenuItem : MenuItem {
	std::function<void()> clickHandler;
	void onAction(const event::Action &e) override;
};
