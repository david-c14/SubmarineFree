/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "color.hpp"

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
		//nvgArc(vg, radius, radius, radius - 1, M_PI + 0.2076, -0.2076, NVG_CW); 
		//nvgArc(vg, 24.5, 12.5, 2.5, 1.467 - M_PI, M_PI - 1.467, NVG_CCW);
		//nvgArc(vg, radius, radius, radius - 1, 0.2076, M_PI - 0.2076, NVG_CW);
		//nvgArc(vg, 0.5, 12.5, 2.5, 1.467, -1.467, NVG_CCW);


		nvgMoveTo(vg, 0.665568, 9.993499);
		nvgBezierTo(vg, 1.849660, 4.402603, 6.784906, 0.403117, 12.499817, 0.403117);
		nvgBezierTo(vg, 18.214727, 0.403117, 23.149975, 4.402601, 24.334068, 9.993498);
		nvgBezierTo(vg, 23.438292, 9.993016, 22.610352, 10.470624, 22.162313, 11.246303);
		nvgBezierTo(vg, 21.714275, 12.021981, 21.714256, 12.977802, 22.162262, 13.753500);
		nvgBezierTo(vg, 22.610268, 14.529198, 23.438189, 15.006838, 24.333965, 15.006392);
		nvgBezierTo(vg, 23.149975, 20.597290, 18.214729, 24.596775, 12.499819, 24.596775);
		nvgBezierTo(vg, 6.784908, 24.596775, 1.849660, 20.597288, 0.665567, 15.006392);
		nvgBezierTo(vg, 2.049333, 15.005648, 3.170693, 13.883659, 3.170665, 12.499894);
		nvgBezierTo(vg, 3.170636, 11.116129, 2.049230, 9.994187, 0.665464, 9.993499);
		nvgBezierTo(vg, 0.665499, 9.993499, 0.665533, 9.993499, 0.665568, 9.993499);




		nvgPathWinding(vg, NVG_SOLID);
		nvgClosePath(vg);
		nvgCircle(vg, radius, radius, 8.0);
		nvgPathWinding(vg, NVG_HOLE);
		nvgStrokeWidth(vg, 0.80645);
		if (type == app::PortWidget::OUTPUT) {
			if (gScheme.isFlat) {
				nvgFillColor(vg, color::mult(col, nvgRGB(0x80, 0x80, 0x80)));
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
