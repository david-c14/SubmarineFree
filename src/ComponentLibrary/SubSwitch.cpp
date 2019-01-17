#include "../SubmarineFree.hpp"
#include "util/color.hpp"

void SubSwitch::draw(NVGcontext *vg) {
	float cx = 7.0f + (box.size.x - 14.0f) * value / (maxValue - minValue);
	float cy = 7.0f + (box.size.y - 14.0f) * value / (maxValue - minValue);
	nvgBeginPath(vg);
	nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 7.0f);
	nvgFillColor(vg, nvgRGB(0, 0, 0));
	nvgFill(vg);
	
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, 5.0f);
	nvgFillColor(vg, nvgRGB(0x33, 0x33, 0x33));
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGB(0xee, 0xee, 0xee));
	nvgStrokeWidth(vg, 2.0f);
	nvgStroke(vg);
/*
	// Shadow
	{
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius * 1.2, radius);
		nvgFillColor(vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(vg);
	}
		
	// Circle
	{
//		float ctm[6];
//		nvgCurrentTransform(vg, ctm);
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
//		nvgResetTransform(vg);
//		nvgTransform(vg, ctm[0], ctm[1], ctm[2], ctm[3], ctm[4], ctm[5]);
		nvgRestore(vg);
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius, radius * 0.9);
		nvgFillColor(vg, nvgRGB(10,10,10));
		nvgFill(vg);
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
//		float ctm[6];
//		nvgCurrentTransform(vg, ctm);
		nvgSave(vg);
		nvgBeginPath(vg);
		nvgTranslate(vg, radius, radius);
		nvgRotate(vg, angle);
		nvgRect(vg, radius * -0.05, radius * -0.9, radius * 0.1, radius * 0.4);
		NVGpaint paint;
		NVGcolor ocol = colorMult(lcol, 0.1);
		paint = nvgRadialGradient(vg, 0, radius * -0.7, radius * 0.05, radius * 0.2, lcol, ocol);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
//		nvgResetTransform(vg);
//		nvgTransform(vg, ctm[0], ctm[1], ctm[2], ctm[3], ctm[4], ctm[5]);
		nvgRestore(vg);
	}
	
	// Halo
	{
		nvgBeginPath(vg);
		nvgRect(vg, cx - oradius, cy - oradius, 2 * oradius, 2 * oradius);
		NVGpaint paint;
		NVGcolor icol = colorMult(lcol, 0.08);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(vg, cx, cy, lradius, oradius, icol, ocol);
		nvgFillPaint(vg, paint);
		nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
		nvgFill(vg);	
	}
*/	
}
