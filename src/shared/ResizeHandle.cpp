/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"

ResizeHandle::ResizeHandle() {
	box.size = Vec(4, 20);
}

void ResizeHandle::onDragStart(const event::DragStart& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	dragPos = APP->scene->rack->mousePos;
	ModuleWidget* mw = getAncestorOfType<ModuleWidget>();
	assert(mw);
	originalBox = mw->box;
}

void ResizeHandle::onDragMove(const event::DragMove& e) {
	SchemePanel* sp = getAncestorOfType<SchemePanel>();
	assert(sp);

	Vec newDragPos = APP->scene->rack->mousePos;
	float deltaX = newDragPos.x - dragPos.x;

	Rect newBox = originalBox;
	Rect oldBox = sp->box;
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
	ModuleWidget *mw = sp->getAncestorOfType<ModuleWidget>();
	mw->box = newBox;
	if (!APP->scene->rack->requestModulePos(mw, newBox.pos)) {
		mw->box = oldBox;
	}
	else {
		sp->box = newBox;
		mw->setSize(newBox.size);
	}
	
}

void ResizeHandle::draw(const DrawArgs& args) {
	for (float x = 5.0; x <= 10.0; x += 5.0) {
		nvgBeginPath(args.vg);
		const float margin = 5.0;
		nvgMoveTo(args.vg, x + 0.5, margin + 0.5);
		nvgLineTo(args.vg, x + 0.5, box.size.y - margin + 0.5);
		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, nvgRGBAf(0.5, 0.5, 0.5, 0.5));
		nvgStroke(args.vg);
	}
}
