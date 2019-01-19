#include "../SubmarineFree.hpp"
#include "util/color.hpp"

void SubSwitch2::draw(NVGcontext *vg) {
	float cx = 7.0f + (box.size.x - 14.0f) * value / (maxValue - minValue);
	float cy = 7.0f + (box.size.y - 14.0f) * value / (maxValue - minValue);

	// Shadow
	{
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 0, cy - 7.0f, 14.0f, 17.0f, 7.0f);
		nvgFillColor(vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(vg);
	}

	// Slot
	{
		nvgSave(vg);
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 7.0f);
		nvgTranslate(vg, box.size.x / 2.0f, box.size.y / 2.0f);
		nvgScale(vg, box.size.x / 14.0f, box.size.y / 14.0f);
		NVGpaint paint;
		paint = nvgRadialGradient(vg, 0, 0, 0, 7.0f, nvgRGB(0,0,0), nvgRGB(0x22,0x22,0x22));
		nvgFillPaint(vg, paint);
		nvgFill(vg);	
		nvgRestore(vg);
	}
	
	// Slider
	{
		nvgBeginPath(vg);
		nvgCircle(vg, cx, cy, 5.0f);
		nvgFillPaint(vg, nvgLinearGradient(vg, cx, cy - 7.0f, cx, cy + 7.0f, nvgRGB(0x33, 0x33, 0x33), nvgRGB(0x11, 0x11, 0x11)));
		nvgFill(vg);
		nvgStrokePaint(vg, nvgLinearGradient(vg, cx, cy - 7.0f, cx, cy + 7.0f, nvgRGB(0xee, 0xee, 0xee), nvgRGB(0xcc, 0xcc, 0xcc)));
		nvgStrokeWidth(vg, 2.0f);
		nvgStroke(vg);
	}
}
