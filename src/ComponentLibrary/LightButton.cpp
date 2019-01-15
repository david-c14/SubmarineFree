#include "../SubmarineFree.hpp"
#include "util/color.hpp"

void LightButton::draw(NVGcontext *vg) {

	NVGcolor lcol = (value > 0.5f)?color:nvgRGB(0x4a,0x4a,0x4a);

	// Shadow
	{
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 0.0f, 0.0f, box.size.x, box.size.y * 1.2f, 3.0f);
		nvgFillColor(vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(vg);
	}

	// Body
	{
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 0.5f, 0.5f, box.size.x - 1.0f, box.size.y - 1.0f, 2.5f);
		NVGpaint paint;
		paint = nvgLinearGradient(vg, 0, 0, 0, box.size.y, nvgRGB(0xdd, 0xdd, 0xdd), nvgRGB(0xbb, 0xbb, 0xbb));
		nvgFillPaint(vg, paint);
		nvgStrokeColor(vg, nvgRGB(0x55, 0x77, 0x77));
		nvgFill(vg);
		nvgStroke(vg);
		nvgBeginPath(vg);
	}

	Rect lightbox = Rect(Vec(box.size.x / 4.0f, box.size.y / 4.0f), Vec(box.size.x / 2.0f, box.size.y / 4.0f));	

	// Light
	{
		nvgBeginPath(vg);
		nvgRect(vg, lightbox.pos.x, lightbox.pos.y, lightbox.size.x, lightbox.size.y);
		NVGpaint paint;
		NVGcolor ocol = colorMult(lcol, 0.1);
		paint = nvgRadialGradient(vg, box.size.x / 2.0f, box.size.y * 0.375f, 1.0f, 4.0f, lcol, ocol);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}

	// Halo
	{
		float lradius = mm2px(0.544);
		float oradius = lradius + 15.0;
		nvgBeginPath(vg);
		nvgRect(vg, box.size.x / 2.0 - oradius, box.size.y * 0.375f - oradius, 2 * oradius, 2 * oradius);
		NVGpaint paint;
		NVGcolor icol = colorMult(lcol, 0.08);
		NVGcolor ocol = nvgRGB(0, 0, 0);
		paint = nvgRadialGradient(vg, box.size.x / 2.0, box.size.y * 0.375f, lradius, oradius, icol, ocol);
		nvgFillPaint(vg, paint);
		nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
		nvgFill(vg);	
	}
}
