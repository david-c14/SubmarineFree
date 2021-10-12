/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "color.hpp"

void LightKnob::setEnabled(int val) {
	enabled = val;
}

void LightKnob::setRadius(int r) {
	Widget *w = dynamic_cast<Widget *>(this);
	radius = r;
	w->box.size.x = r * 2;
	w->box.size.y = r * 2;
	light->box.size.x = r * 2;
	light->box.size.y = r * 2;
}

void LightKnob::doDraw(const rack::widget::Widget::DrawArgs &args) {
	nvgSave(args.vg);

	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, radius, radius * 1.2, radius);
		nvgFillColor(args.vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(args.vg);
	}
		
	// Circle
	{
		if (!gScheme.isFlat) {
			nvgSave(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, radius, radius, radius);
			nvgTranslate(args.vg, radius, radius);
			nvgRotate(args.vg, M_PI / -15);
			nvgScale(args.vg, 40, 1);
			NVGpaint paint;
			paint = nvgRadialGradient(args.vg, 0, 0, 0, radius * 0.2, nvgRGB(0x7a,0x7a,0x7a), nvgRGB(10,10,10));
			nvgFillPaint(args.vg, paint);
			nvgFill(args.vg);	
			nvgRestore(args.vg);
		}
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, radius, radius, radius * 0.9);
		nvgFillColor(args.vg, nvgRGB(10,10,10));
		nvgFill(args.vg);
	}
	nvgRestore(args.vg);	
}

void LightKnobLight::draw(const DrawArgs &args) {
	NVGcolor lcol = knob->enabled?knob->color:nvgRGB(0x4a,0x4a,0x4a);
	float value = knob->getParamQuantity()->getValue();
	float minValue = knob->getParamQuantity()->getMinValue();
	float maxValue = knob->getParamQuantity()->getMaxValue();
	float angle;
	if (std::isfinite(minValue) && std::isfinite(maxValue)) {
		angle = rescale(value, minValue, maxValue, knob->minAngle, knob->maxAngle);
	}
	else {
		angle = rescale(value, -1.0, 1.0, knob->minAngle, knob->maxAngle);
		angle = fmodf(angle, 2*M_PI);
	}
	float cx = (1.0f + sinf(angle) * 0.7f) * knob->radius;
	float cy = (1.0f - cosf(angle) * 0.7f) * knob->radius;
	float lradius = mm2px(0.544);
	float oradius = lradius + 15.0;
	
	// Light
	{
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgTranslate(args.vg, knob->radius, knob->radius);
		nvgRotate(args.vg, angle);
		nvgRect(args.vg, knob->radius * -0.05, knob->radius * -0.9, knob->radius * 0.1, knob->radius * 0.4);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, lcol);
		}
		else {
			NVGpaint paint;
			NVGcolor ocol = color::mult(lcol, 0.1);
			paint = nvgRadialGradient(args.vg, 0, knob->radius * -0.7, knob->radius * 0.05, knob->radius * 0.2, lcol, ocol);
			nvgFillPaint(args.vg, paint);
		}
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
	
	// Halo
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, cx - oradius, cy - oradius, 2 * oradius, 2 * oradius);
		NVGpaint paint;
		NVGcolor icol = color::mult(lcol, 0.08);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(args.vg, cx, cy, lradius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgFill(args.vg);	
	}
}
