/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"

ResizeHandle::ResizeHandle() {
	box.size = Vec(8, 10);
}

void ResizeHandle::onDragStart(const event::DragStart& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	dragPos = APP->scene->rack->getMousePos();
	ModuleWidget* mw = getAncestorOfType<ModuleWidget>();
	assert(mw);
	originalBox = mw->box;
}

void ResizeHandle::onDragMove(const event::DragMove& e) {
	SchemePanel* sp = getAncestorOfType<SchemePanel>();
	ModuleWidget *mw = sp->getAncestorOfType<ModuleWidget>();
	assert(sp);

	Vec newDragPos = APP->scene->rack->getMousePos();
	float deltaX = newDragPos.x - dragPos.x;

	Rect newBox = originalBox;
	Rect oldBox = mw->box;
	if (right) {
		newBox.size.x += deltaX;
		newBox.size.x = clamp(newBox.size.x, sp->minWidth, sp->maxWidth);
		newBox.size.x = std::round(newBox.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
	}
	else {
		newBox.size.x -= deltaX;
		newBox.size.x = clamp(newBox.size.x, sp->minWidth, sp->maxWidth);
		newBox.size.x = std::round(newBox.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
		newBox.pos.x = originalBox.pos.x + originalBox.size.x - newBox.size.x;
	}

	// Set box and test whether it's valid
	sp->resize(newBox, oldBox);
}

void ResizeHandle::draw(const DrawArgs& args) {
	ModuleWidget *mw = getAncestorOfType<ModuleWidget>();
	nvgStrokeWidth(args.vg, 1.0);
	for (float x = 1.0; x <= 8.0; x += 2.0) {
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x - 0.5, 0);
		nvgLineTo(args.vg, x - 0.5, box.size.y);
		nvgStrokeColor(args.vg, gScheme.getHighlight(mw->module));
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x + 0.5, 0);
		nvgLineTo(args.vg, x + 0.5, box.size.y);
		nvgStrokeColor(args.vg, gScheme.getShadow(mw->module));
		nvgStroke(args.vg);
	}
}
