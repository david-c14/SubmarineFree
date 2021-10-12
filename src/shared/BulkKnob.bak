#include "../SubmarineFree.hpp"

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
			int thisModuleId = module->id;
			int thisParamId = paramId;
			int thisOldValue = oldValue;
			APP->history->push(new EventWidgetAction(
				"move knob",
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, thisOldValue);
				},
				[=]() {
					setBulkParamValue(thisModuleId, thisParamId, newValue);
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
		oldValue = snapValue = *value = defaultValue;
}

void BulkKnob::randomize() {
	if (value && std::isfinite(minValue) && std::isfinite(maxValue)) {
		*value = math::rescale(random::uniform(), 0.f, 1.f, minValue, maxValue);
		if (snap)
			*value = std::round(*value);
		oldValue = snapValue = *value;
	}
}
