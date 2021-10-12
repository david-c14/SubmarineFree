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
	light = new LightButtonLight();
	light->box.pos = Vec(0,0);
	light->box.size = box.size;
	light->button = this;
	addChild(light);
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

	nvgRestore(args.vg);
	Widget::draw(args);
}

void LightButtonLight::draw(const DrawArgs &args) {
	float value = 0.0f;
	if (button->getParamQuantity()) {
		value = button->getParamQuantity()->getValue();
	}

	Rect lightbox = Rect(Vec(box.size.x / 4.0f, box.size.y / 4.0f), Vec(box.size.x / 2.0f, box.size.y / 4.0f));	
	NVGcolor lcol = (value > 0.5f)?button->color:nvgRGB(0x4a,0x4a,0x4a);

	nvgSave(args.vg);
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

	// Halo
	if (!gScheme.isFlat) {
		float lradius = mm2px(0.544);
		float oradius = lradius + 15.0;
		nvgBeginPath(args.vg);
		nvgRect(args.vg, box.size.x / 2.0 - oradius, box.size.y * 0.375f - oradius, 2 * oradius, 2 * oradius);
		NVGpaint paint;
		NVGcolor icol = color::mult(lcol, 0.08);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(args.vg, box.size.x / 2.0, box.size.y * 0.375f, lradius, oradius, icol, ocol);
		nvgFillPaint(args.vg, paint);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgFill(args.vg);	
	}
	nvgRestore(args.vg);

}
