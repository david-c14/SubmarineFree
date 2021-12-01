/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "color.hpp"

void SubSwitch2::draw(const DrawArgs &args) {
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	if (getParamQuantity()) {
		value = getParamQuantity()->getValue();
		minValue = getParamQuantity()->getMinValue();
		maxValue = getParamQuantity()->getMaxValue();
	}

	float cx = 7.0f + (box.size.x - 14.0f) * value / (maxValue - minValue);
	float cy = 7.0f + (box.size.y - 14.0f) * value / (maxValue - minValue);

	nvgSave(args.vg);
	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0, cy - 7.0f, 14.0f, 17.0f, 7.0f);
		nvgFillColor(args.vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(args.vg);
	}

	// Slot
	{
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 7.0f);
		nvgTranslate(args.vg, box.size.x / 2.0f, box.size.y / 2.0f);
		nvgScale(args.vg, box.size.x / 14.0f, box.size.y / 14.0f);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, nvgRGB(0x10, 0x10, 0x10));
		}
		else {
			NVGpaint paint;
			paint = nvgRadialGradient(args.vg, 0, 0, 0, 7.0f, nvgRGB(0,0,0), nvgRGB(0x22,0x22,0x22));
			nvgFillPaint(args.vg, paint);
		}
		nvgFill(args.vg);	
		nvgRestore(args.vg);
	}
	
	// Slider
	{
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cx, cy, 5.0f);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, nvgRGB(0x20, 0x20, 0x20));
			nvgStrokeColor(args.vg, nvgRGB(0xe0, 0xe0, 0xe0));
		}
		else {
			nvgFillPaint(args.vg, nvgLinearGradient(args.vg, cx, cy - 7.0f, cx, cy + 7.0f, nvgRGB(0x33, 0x33, 0x33), nvgRGB(0x11, 0x11, 0x11)));
			nvgStrokePaint(args.vg, nvgLinearGradient(args.vg, cx, cy - 7.0f, cx, cy + 7.0f, nvgRGB(0xee, 0xee, 0xee), nvgRGB(0xcc, 0xcc, 0xcc)));
		}
		nvgFill(args.vg);
		nvgStrokeWidth(args.vg, 2.0f);
		nvgStroke(args.vg);
	}
	nvgRestore(args.vg);
}
