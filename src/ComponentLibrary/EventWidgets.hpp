#include <functional>

struct EventAction : history::Action {
	std::function<void ()> undoHandler;
	std::function<void ()> redoHandler;
	void undo() override;
	void redo() override;
	EventAction(std::string name, std::function<void()> uHandler, std::function<void()> rHandler);
};

struct EventButton : OpaqueWidget {
	std::function<void ()> clickHandler;
	std::function<void ()> rightClickHandler;
	void onButton(const event::Button &e) override;
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
	void draw(const DrawArgs &args) override;
	void onDragStart(const event::DragStart &e) override;
	void onDragMove(const event::DragMove &e) override;
	void onDragEnd(const event::DragEnd &e) override;
	void onDoubleClick(const event::DoubleClick &e) override;
	void onEnter(const event::Enter &e) override;
	void onLeave(const event::Leave &e) override;
};

struct CheckBox : EventButton {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	CheckBox();
	void draw (const DrawArgs &args) override;
};

struct EventRadioButton : EventButton {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventRadioButton();
	void draw (const DrawArgs &args) override;
};

struct EventLabel : Widget {
	std::string label;
	void draw(const DrawArgs &args) override;
};

struct RectButton : OpaqueWidget {
	std::string label;
	std::function<void ()> clickHandler;
	void draw(const DrawArgs &args) override;
	void onButton(const event::Button &e) override;
};

struct EventMenuItem : MenuItem {
	std::function<void()> clickHandler;
	void onAction(const event::Action &e) override;
};
