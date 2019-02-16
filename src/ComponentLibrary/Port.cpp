/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "util/color.hpp"

void SilverPort::draw(NVGcontext *vg) {
	nvgSave(vg);
	float radius = box.size.x / 2.0f;

	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius * 1.2, radius);
		nvgFillColor(vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(vg);
	}

	// Switch
	nvgBeginPath(vg);
	nvgRect(vg, 6, 6, 13, 13);
	nvgFillColor(vg, nvgRGB(0,0,0));
	nvgFill(vg);
	if (!gScheme.isFlat) {
		nvgBeginPath(vg);
		nvgRect(vg, 10, 6, 5, 13);
		nvgFillPaint(vg, nvgLinearGradient(vg, radius, 19, radius, radius, nvgRGB(0x60, 0x60, 0x60), nvgRGB(0,0,0)));
		nvgFill(vg);
	}

	nvgLineJoin(vg, NVG_BEVEL);
	// Port body
	{
		nvgBeginPath(vg);
		nvgCircle(vg, radius, radius, 7);
		nvgStrokeWidth(vg, 4);
		if (gScheme.isFlat) {
			nvgStrokeColor(vg, col);
		}
		else {
			nvgStrokePaint(vg, nvgRadialGradient(vg, radius + 0.3, radius + 1, 0, 9, nvgRGB(0x20, 0x20, 0x20), col));
		}
		nvgStroke(vg);
	}
		
	// Outer rim
	{
		nvgBeginPath(vg);
//		nvgMoveTo(vg, 0.66556777, 9.9934984);
		nvgArc(vg, radius, radius, radius - 1, M_PI + 0.2076, -0.2076, NVG_CW); 
		nvgArc(vg, 24.5, 12.5, 2.5, 1.467 - M_PI, M_PI - 1.467, NVG_CCW);
		nvgArc(vg, radius, radius, radius - 1, 0.2076, M_PI - 0.2076, NVG_CW);
		nvgArc(vg, 0.5, 12.5, 2.5, 1.467, -1.467, NVG_CCW);
		nvgPathWinding(vg, NVG_HOLE);
		nvgClosePath(vg);
		nvgCircle(vg, radius, radius, 8.0);
		nvgPathWinding(vg, NVG_SOLID);
		nvgStrokeWidth(vg, 0.80645);
		if (type == Port::OUTPUT) {
			if (gScheme.isFlat) {
				nvgFillColor(vg, colorMult(col, nvgRGB(0x80, 0x80, 0x80)));
			}
			else {
				nvgFillPaint(vg, nvgRadialGradient(vg, radius + 0.3, radius + 1, 1, 12, col, nvgRGB(0x3f, 0x3f, 0x3f)));
			}
			nvgStrokeColor(vg, nvgRGB(0x36,0x36,0x36));
		}
		else {
			if (gScheme.isFlat) {
				nvgFillColor(vg, col);
			}
			else {
				nvgFillPaint(vg, nvgRadialGradient(vg, radius + 0.3, radius + 1, 1, 12, col, nvgRGB(0xff, 0xff, 0xff)));
			}
			nvgStrokeColor(vg, nvgRGB(0x66,0x66,0x66));
		}
		nvgFill(vg);
		nvgStroke(vg); 
	}
	nvgRestore(vg);
}
