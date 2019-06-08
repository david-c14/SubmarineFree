#include "../SubmarineFree.hpp"
#include <cstring>

MouseTransformWidget::MouseTransformWidget() {
	identity();
};

Rect MouseTransformWidget::getChildrenBoundingBox() {
	Rect bound = Widget::getChildrenBoundingBox();
	Vec topLeft = bound.pos;
	Vec bottomRight = bound.getBottomRight();
	nvgTransformPoint(&topLeft.x, &topLeft.y, transform, topLeft.x, topLeft.y);
	nvgTransformPoint(&bottomRight.x, &bottomRight.y, transform, bottomRight.x, bottomRight.y);
	return Rect(topLeft, bottomRight.minus(topLeft));
};

void MouseTransformWidget::identity() {
	nvgTransformIdentity(transform);
	nvgTransformIdentity(inverse);
	std::memcpy(invLinear, inverse, 4 * sizeof(float));
	invLinear[4] = invLinear[5] = 0.0;
	hasInverse = true;
};

void MouseTransformWidget::translate(Vec delta) {
	float t[6];
	nvgTransformTranslate(t, delta.x, delta.y);
	nvgTransformPremultiply(transform, t);
	hasInverse = nvgTransformInverse(inverse, transform);
	std::memcpy(invLinear, inverse, 4 * sizeof(float));
};

void MouseTransformWidget::rotate(float angle) {
	float t[6];
	nvgTransformRotate(t, angle);
	nvgTransformPremultiply(transform, t);
	hasInverse = nvgTransformInverse(inverse, transform);
	std::memcpy(invLinear, inverse, 4 * sizeof(float));
};

void MouseTransformWidget::scale(Vec s) {
	float t[6];
	nvgTransformScale(t, s.x, s.y);
	nvgTransformPremultiply(transform, t);
	hasInverse = nvgTransformInverse(inverse, transform);
	std::memcpy(invLinear, inverse, 4 * sizeof(float));
};

void MouseTransformWidget::draw(NVGcontext *vg) {
	// No need to save the state because that is done in the parent
	nvgTransform(vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
	Widget::draw(vg);
};

void MouseTransformWidget::onButton(const event::Button &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onButton(e);
	e.pos = pos;
};

void MouseTransformWidget::onHover(const event::Hover &e) {
	event::Hover e2;
	e2.pos.x = e.pos.x;
	e2.pos.y = e.pos.y;
	e2.mouseDelta.x = e.mouseDelta.x;
	e2.mouseDelta.y = e.mouseDelta.y;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
		nvgTransformPoint(&e2.mouseDelta.x, &e2.mouseDelta.y, invLinear, e2.mouseDelta.x, e2.mouseDelta.y);
	}
	Widget::onHover(e2);
};

void MouseTransformWidget::onDragHover(const event::DragHover &e) {
	Vec pos = e.pos;
	Vec mouseDelta = e.mouseDelta;
	if (hasInverse) {
		Vec newPos;
		nvgTransformPoint(&newPos.x, &newPos.y, inverse, e.pos.x, e.pos.y); 
		e.pos = newPos;
		Vec newDelta;
		nvgTransformPoint(&newDelta.x, &newDelta.y, invLinear, e.mouseDelta.x, e.mouseDelta.y);
		e.mouseDelta = newDelta;
	}
	Widget::onDragHover(e);
	e.pos = pos;
	e.mouseDelta = mouseDelta;
};

void MouseTransformWidget::onHoverKey(const event::HoverKey &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onHoverKey(e);
	e.pos = pos;
};

void MouseTransformWidget::onHoverText(const event::HoverText &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onHoverText(e);
	e.pos = pos;
};

void MouseTransformWidget::onHoverScroll(const event::HoverScroll &e) {
	Vec pos = e.pos;
	Vec scrollDelta = e.scrollDelta;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		nvgTransformPoint(&e.scrollDelta.x, &e.scrollDelta.y, invLinear, e.scrollDelta.x, e.scrollDelta.y);
	}
	Widget::onHoverScroll(e);
	e.pos = pos;
	e.scrollDelta = scrollDelta;
};

void MouseTransformWidget::onPathDrop(const event::PathDrop &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onPathDrop(e);
	e.pos = pos;
};
