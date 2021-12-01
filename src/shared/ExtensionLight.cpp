/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"

ExtensionLight::ExtensionLight() {
	box.size.x = 4;
	box.size.y = 4;
}

void ExtensionLight::drawBackground(const widget::Widget::DrawArgs& args) {
	getShape(args);
	if (bgColor.a > 0.0) {
		nvgFillColor(args.vg, bgColor);
		nvgFill(args.vg);
	}

	if (borderColor.a > 0.0) {
		nvgStrokeWidth(args.vg, 0.5);
		nvgStrokeColor(args.vg, borderColor);
		nvgStroke(args.vg);
	}
}

void ExtensionLight::drawLight(const widget::Widget::DrawArgs& args) {
	if (color.a > 0.0) {
		getShape(args);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	}
}

void LeftLight::getShape(const widget::Widget::DrawArgs &args) {
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, 0, box.size.y / 2);
	nvgLineTo(args.vg, box.size.x, box.size.y);
	nvgLineTo(args.vg, box.size.x, 0);
	nvgClosePath(args.vg);
}

void RightLight::getShape(const widget::Widget::DrawArgs &args) {
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, box.size.x, box.size.y / 2);
	nvgLineTo(args.vg, 0, box.size.y);
	nvgLineTo(args.vg, 0, 0);
	nvgClosePath(args.vg);
}
