/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"
#include "color.hpp"

void SilverPort::draw(const DrawArgs &args) {
	nvgSave(args.vg);
	float radius = box.size.x / 2.0f;

	// Shadow
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, radius, radius * 1.2, radius);
		nvgFillColor(args.vg, nvgRGBAf(0, 0, 0, 0.15));
		nvgFill(args.vg);
	}

	// Switch
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 6, 6, 13, 13);
	nvgFillColor(args.vg, nvgRGB(0,0,0));
	nvgFill(args.vg);
	if (!gScheme.isFlat) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 10, 6, 5, 13);
		nvgFillPaint(args.vg, nvgLinearGradient(args.vg, radius, 19, radius, radius, nvgRGB(0x60, 0x60, 0x60), nvgRGB(0,0,0)));
		nvgFill(args.vg);
	}

	nvgLineJoin(args.vg, NVG_BEVEL);
	// Port body
	{
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, radius, radius, 7);
		nvgStrokeWidth(args.vg, 4);
		if (gScheme.isFlat) {
			nvgStrokeColor(args.vg, col);
		}
		else {
			nvgStrokePaint(args.vg, nvgRadialGradient(args.vg, radius + 0.3, radius + 1, 0, 9, nvgRGB(0x20, 0x20, 0x20), col));
		}
		nvgStroke(args.vg);
	}
		
	// Outer rim
	{
		nvgBeginPath(args.vg);
//		nvgMoveTo(args.vg, 0.66556777, 9.9934984);
		//nvgArc(args.vg, radius, radius, radius - 1, M_PI + 0.2076, -0.2076, NVG_CW); 
		//nvgArc(args.vg, 24.5, 12.5, 2.5, 1.467 - M_PI, M_PI - 1.467, NVG_CCW);
		//nvgArc(args.vg, radius, radius, radius - 1, 0.2076, M_PI - 0.2076, NVG_CW);
		//nvgArc(args.vg, 0.5, 12.5, 2.5, 1.467, -1.467, NVG_CCW);


		nvgMoveTo(args.vg, 0.665568, 9.993499);
		nvgBezierTo(args.vg, 1.849660, 4.402603, 6.784906, 0.403117, 12.499817, 0.403117);
		nvgBezierTo(args.vg, 18.214727, 0.403117, 23.149975, 4.402601, 24.334068, 9.993498);
		nvgBezierTo(args.vg, 23.438292, 9.993016, 22.610352, 10.470624, 22.162313, 11.246303);
		nvgBezierTo(args.vg, 21.714275, 12.021981, 21.714256, 12.977802, 22.162262, 13.753500);
		nvgBezierTo(args.vg, 22.610268, 14.529198, 23.438189, 15.006838, 24.333965, 15.006392);
		nvgBezierTo(args.vg, 23.149975, 20.597290, 18.214729, 24.596775, 12.499819, 24.596775);
		nvgBezierTo(args.vg, 6.784908, 24.596775, 1.849660, 20.597288, 0.665567, 15.006392);
		nvgBezierTo(args.vg, 2.049333, 15.005648, 3.170693, 13.883659, 3.170665, 12.499894);
		nvgBezierTo(args.vg, 3.170636, 11.116129, 2.049230, 9.994187, 0.665464, 9.993499);
		nvgBezierTo(args.vg, 0.665499, 9.993499, 0.665533, 9.993499, 0.665568, 9.993499);




		nvgPathWinding(args.vg, NVG_SOLID);
		nvgClosePath(args.vg);
		nvgCircle(args.vg, radius, radius, 8.0);
		nvgPathWinding(args.vg, NVG_HOLE);
		nvgStrokeWidth(args.vg, 0.80645);
		if (type == engine::Port::OUTPUT) {
			if (gScheme.isFlat) {
				nvgFillColor(args.vg, color::mult(col, nvgRGB(0x80, 0x80, 0x80)));
			}
			else {
				nvgFillPaint(args.vg, nvgRadialGradient(args.vg, radius + 0.3, radius + 1, 1, 12, col, nvgRGB(0x3f, 0x3f, 0x3f)));
			}
			nvgStrokeColor(args.vg, nvgRGB(0x36,0x36,0x36));
		}
		else {
			if (gScheme.isFlat) {
				nvgFillColor(args.vg, col);
			}
			else {
				nvgFillPaint(args.vg, nvgRadialGradient(args.vg, radius + 0.3, radius + 1, 1, 12, col, nvgRGB(0xff, 0xff, 0xff)));
			}
			nvgStrokeColor(args.vg, nvgRGB(0x66,0x66,0x66));
		}
		nvgFill(args.vg);
		nvgStroke(args.vg); 
	}
	nvgRestore(args.vg);
}
