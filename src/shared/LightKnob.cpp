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

void LightKnob::draw(const DrawArgs &args) {
	nvgSave(args.vg);
	NVGcolor lcol = enabled?color:nvgRGB(0x4a,0x4a,0x4a);
	float value = 0.0f;
	float minValue = -1.0f;
	float maxValue = 1.0f;
	if (paramQuantity) {
		value = paramQuantity->getValue();
		minValue = paramQuantity->getMinValue();
		maxValue = paramQuantity->getMaxValue();
	}

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

	float angle;
	if (std::isfinite(minValue) && std::isfinite(maxValue)) {
		angle = rescale(value, minValue, maxValue, minAngle, maxAngle);
	}
	else {
		angle = rescale(value, -1.0, 1.0, minAngle, maxAngle);
		angle = fmodf(angle, 2*M_PI);
	}
	float cx = (1.0f + sinf(angle) * 0.7f) * radius;
	float cy = (1.0f - cosf(angle) * 0.7f) * radius;
	float lradius = mm2px(0.544);
	float oradius = lradius + 15.0;
	
	// Light
	{
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgTranslate(args.vg, radius, radius);
		nvgRotate(args.vg, angle);
		nvgRect(args.vg, radius * -0.05, radius * -0.9, radius * 0.1, radius * 0.4);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, lcol);
		}
		else {
			NVGpaint paint;
			NVGcolor ocol = color::mult(lcol, 0.1);
			paint = nvgRadialGradient(args.vg, 0, radius * -0.7, radius * 0.05, radius * 0.2, lcol, ocol);
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
	nvgRestore(args.vg);	
}
