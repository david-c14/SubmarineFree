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
	radius = r;
	box.size.x = r * 2;
	box.size.y = r * 2;
}

void LightKnob::draw(NVGcontext *vg) {
	nvgSave(vg);
	NVGcolor lcol = enabled?color:nvgRGB(0x4a,0x4a,0x4a);

	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius * 1.2, radius);
		nvgFillColor(vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(vg);
	}
		
	// Circle
	{
		if (!gScheme.isFlat) {
			nvgSave(vg);
			nvgBeginPath(vg);
			nvgCircle(vg, radius, radius, radius);
			nvgTranslate(vg, radius, radius);
			nvgRotate(vg, M_PI / -15);
			nvgScale(vg, 40, 1);
			NVGpaint paint;
			paint = nvgRadialGradient(vg, 0, 0, 0, radius * 0.2, nvgRGB(0x7a,0x7a,0x7a), nvgRGB(10,10,10));
			nvgFillPaint(vg, paint);
			nvgFill(vg);	
			nvgRestore(vg);
		}
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius, radius * 0.9);
		nvgFillColor(vg, nvgRGB(10,10,10));
		nvgFill(vg);
	}

	float angle;
	if (std::isfinite(paramQuantity->getMinValue()) && std::isfinite(paramQuantity->getMaxValue())) {
		angle = rescale(paramQuantity->getValue(), paramQuantity->getMinValue(), paramQuantity->getMaxValue(), minAngle, maxAngle);
	}
	else {
		angle = rescale(paramQuantity->getValue(), -1.0, 1.0, minAngle, maxAngle);
		angle = fmodf(angle, 2*M_PI);
	}
	float cx = (1.0f + sinf(angle) * 0.7f) * radius;
	float cy = (1.0f - cosf(angle) * 0.7f) * radius;
	float lradius = mm2px(0.544);
	float oradius = lradius + 15.0;
	
	// Light
	{
		nvgSave(vg);
		nvgBeginPath(vg);
		nvgTranslate(vg, radius, radius);
		nvgRotate(vg, angle);
		nvgRect(vg, radius * -0.05, radius * -0.9, radius * 0.1, radius * 0.4);
		if (gScheme.isFlat) {
			nvgFillColor(vg, lcol);
		}
		else {
			NVGpaint paint;
			NVGcolor ocol = color::mult(lcol, 0.1);
			paint = nvgRadialGradient(vg, 0, radius * -0.7, radius * 0.05, radius * 0.2, lcol, ocol);
			nvgFillPaint(vg, paint);
		}
		nvgFill(vg);
		nvgRestore(vg);
	}
	
	// Halo
	if (!gScheme.isFlat) {
		nvgBeginPath(vg);
		nvgRect(vg, cx - oradius, cy - oradius, 2 * oradius, 2 * oradius);
		NVGpaint paint;
		NVGcolor icol = color::mult(lcol, 0.08);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(vg, cx, cy, lradius, oradius, icol, ocol);
		nvgFillPaint(vg, paint);
		nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
		nvgFill(vg);	
	}
	nvgRestore(vg);	
}
