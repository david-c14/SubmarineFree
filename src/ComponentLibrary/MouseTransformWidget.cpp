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

void MouseTransformWidget::onMouseDown(EventMouseDown &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onMouseDown(e);
	e.pos = pos;
};

void MouseTransformWidget::onMouseUp(EventMouseUp &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onMouseUp(e);
	e.pos = pos;
};

void MouseTransformWidget::onMouseMove(EventMouseMove &e) {
	Vec pos = e.pos;
	Vec mouseRel = e.mouseRel;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		nvgTransformPoint(&e.mouseRel.x, &e.mouseRel.y, invLinear, e.mouseRel.x, e.mouseRel.y);
	}
	Widget::onMouseMove(e);
	e.pos = pos;
	e.mouseRel = mouseRel;
};

void MouseTransformWidget::onHoverKey(EventHoverKey &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onHoverKey(e);
	e.pos = pos;
};

void MouseTransformWidget::onScroll(EventScroll &e) {
	Vec pos = e.pos;
	Vec scrollRel = e.scrollRel;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		nvgTransformPoint(&e.scrollRel.x, &e.scrollRel.y, invLinear, e.scrollRel.x, e.scrollRel.y);
	}
	Widget::onScroll(e);
	e.pos = pos;
	e.scrollRel = scrollRel;
};

void MouseTransformWidget::onPathDrop(EventPathDrop &e) {
	Vec pos = e.pos;
	if (hasInverse) {
		nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
	}
	Widget::onPathDrop(e);
	e.pos = pos;
};
