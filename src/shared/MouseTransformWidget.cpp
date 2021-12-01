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

void MouseTransformWidget::draw(const DrawArgs &args) {
	DrawArgs txCtx = args;
	if (hasInverse) {
		nvgTransformPoint(&txCtx.clipBox.pos.x, &txCtx.clipBox.pos.y, inverse, txCtx.clipBox.pos.x, txCtx.clipBox.pos.y);
	}
	
	// No need to save the state because that is done in the parent
	nvgTransform(txCtx.vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
	Widget::draw(txCtx);
};

void MouseTransformWidget::drawLayer(const DrawArgs &args, int layer) {
	DrawArgs txCtx = args;
	if (hasInverse) {
		nvgTransformPoint(&txCtx.clipBox.pos.x, &txCtx.clipBox.pos.y, inverse, txCtx.clipBox.pos.x, txCtx.clipBox.pos.y);
	}
	
	// No need to save the state because that is done in the parent
	nvgTransform(txCtx.vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
	Widget::drawLayer(txCtx, layer);
};

void MouseTransformWidget::onButton(const event::Button &e) {
	event::Button e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
	}
	Widget::onButton(e2);
};

void MouseTransformWidget::onHover(const event::Hover &e) {
	event::Hover e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
		nvgTransformPoint(&e2.mouseDelta.x, &e2.mouseDelta.y, invLinear, e2.mouseDelta.x, e2.mouseDelta.y);
	}
	Widget::onHover(e2);
};

void MouseTransformWidget::onDragHover(const event::DragHover &e) {
	event::DragHover e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
		nvgTransformPoint(&e2.mouseDelta.x, &e2.mouseDelta.y, invLinear, e2.mouseDelta.x, e2.mouseDelta.y);
	}
	Widget::onDragHover(e2);
};

void MouseTransformWidget::onHoverKey(const event::HoverKey &e) {
	event::HoverKey e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
	}
	Widget::onHoverKey(e2);
};

void MouseTransformWidget::onHoverText(const event::HoverText &e) {
	event::HoverText e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
	}
	Widget::onHoverText(e2);
};

void MouseTransformWidget::onHoverScroll(const event::HoverScroll &e) {
	event::HoverScroll e2 = e;
	if (hasInverse) {
		nvgTransformPoint(&e2.pos.x, &e2.pos.y, inverse, e2.pos.x, e2.pos.y); 
		nvgTransformPoint(&e2.scrollDelta.x, &e2.scrollDelta.y, invLinear, e2.scrollDelta.x, e2.scrollDelta.y);
	}
	Widget::onHoverScroll(e2);
};
