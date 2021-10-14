/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "color.hpp"

LightButton::LightButton() {
	box.size.x = 16.0f;
	box.size.y = 16.0f;
}

void LightButton::draw(const DrawArgs &args) {
	nvgSave(args.vg);

	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.0f, 0.0f, box.size.x, box.size.y * 1.2f, 3.0f);
		nvgFillColor(args.vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(args.vg);
	}

	// Body
	{
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.5f, 0.5f, box.size.x - 1.0f, box.size.y - 1.0f, 2.5f);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, nvgRGB(0xcc, 0xcc, 0xcc));
		}
		else {
			NVGpaint paint;
			paint = nvgLinearGradient(args.vg, 0, 0, 0, box.size.y, nvgRGB(0xdd, 0xdd, 0xdd), nvgRGB(0xbb, 0xbb, 0xbb));
			nvgFillPaint(args.vg, paint);
		}
		nvgStrokeColor(args.vg, nvgRGB(0x55, 0x77, 0x77));
		nvgFill(args.vg);
		nvgStroke(args.vg);
	}
	float value = 0.0f;
	if (getParamQuantity()) {
		value = getParamQuantity()->getValue();
	}
	if (value < 0.5f) {
		drawLight(args, false);
	}

	nvgRestore(args.vg);
	Widget::draw(args);
}

void LightButton::drawLayer(const DrawArgs &args, int layer) {
	if (layer == 1) {
		float value = 0.0f;
		if (getParamQuantity()) {
			value = getParamQuantity()->getValue();
		}
		if (value > 0.5f) {
			drawLight(args, true);
			drawHalo(args);
		}

	}
	Widget::drawLayer(args, layer);
}

void LightButton::drawLight(const DrawArgs &args, bool enabled) {
	Rect lightbox = Rect(Vec(box.size.x / 4.0f, box.size.y / 4.0f), Vec(box.size.x / 2.0f, box.size.y / 4.0f));	
	NVGcolor lcol = (enabled)?color:nvgRGB(0x4a,0x4a,0x4a);

	// Light
	{
		nvgBeginPath(args.vg);
		nvgRect(args.vg, lightbox.pos.x, lightbox.pos.y, lightbox.size.x, lightbox.size.y);
		if (gScheme.isFlat) {
			nvgFillColor(args.vg, lcol);
		}
		else {
			NVGpaint paint;
			NVGcolor ocol = color::mult(lcol, 0.1);
			paint = nvgRadialGradient(args.vg, box.size.x / 2.0f, box.size.y * 0.375f, 1.0f, 4.0f, lcol, ocol);
			nvgFillPaint(args.vg, paint);
		}
		nvgFill(args.vg);
	}
}

void LightButton::drawHalo(const DrawArgs &args) {
	// Halo
	// Don't draw halo if rendering in a framebuffer, e.g. screenshots or Module Browser
	if (args.fb)
		return;

	const float halo = settings::haloBrightness;
	if (halo == 0.f)
		return;

	// If light is off, rendering the halo gives no effect.
	if (color.r == 0.f && color.g == 0.f && color.b == 0.f)
		return;

	nvgSave(args.vg);
	float lradius = mm2px(0.544);
	float oradius = lradius + 15.0;
	nvgBeginPath(args.vg);
	nvgRect(args.vg, box.size.x / 2.0 - oradius, box.size.y * 0.375f - oradius, 2 * oradius, 2 * oradius);
	NVGpaint paint;
	NVGcolor icol = color::mult(color, halo);
	NVGcolor ocol = nvgRGB(0, 0, 0);
	paint = nvgRadialGradient(args.vg, box.size.x / 2.0, box.size.y * 0.375f, lradius, oradius, icol, ocol);
	nvgFillPaint(args.vg, paint);
	nvgGlobalCompositeBlendFunc(args.vg, NVG_ONE_MINUS_DST_COLOR, NVG_ONE);
	nvgFill(args.vg);	
	nvgRestore(args.vg);
}
