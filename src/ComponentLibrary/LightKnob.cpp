#include "../SubmarineFree.hpp"
#include "util/color.hpp"

LightKnob::LightKnob() {
	enabled = 0;
	minAngle = -0.83*M_PI;
	maxAngle = 0.83*M_PI;
}

void LightKnob::setSVG(std::shared_ptr<SVG> svg1, std::shared_ptr<SVG> svg2) {
	frames.push_back(svg1);
	SVGKnob::setSVG(svg1);
	frames.push_back(svg2);
}

void LightKnob::setEnabled(int val) {
	if (enabled == val)
		return;
	enabled = val;
	assert(frames.size() > 1);
	sw->setSVG(frames[enabled]);
	dirty = true;
}

void sub_knob_med::step() {
	
}

void sub_knob_med::draw(NVGcontext *vg) {
	// Circle
	nvgBeginPath(vg);
	nvgCircle(vg, radius, radius, radius);
	nvgFillColor(vg, nvgRGB(0,0,0));
	nvgFill(vg);

	float angle;
	if (isfinite(minValue) && isfinite(maxValue)) {
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
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy ,lradius);
	nvgFillColor(vg, nvgRGB(0x29,0xb2,0xef));
	nvgFill(vg);
	nvgStrokeWidth(vg, 0.5);
	nvgStrokeColor(vg, nvgRGBA(0,0,0,0.6));
	nvgStroke(vg);
	
	// Halo
	nvgBeginPath(vg);
	nvgRect(vg, cx - oradius, cy - oradius, 2 * oradius, 2 * oradius);
	NVGpaint paint;
	NVGcolor icol = colorMult(nvgRGB(0x29, 0xb2, 0xef), 0.08);
	NVGcolor ocol = nvgRGB(0, 0, 0);
	paint = nvgRadialGradient(vg, cx, cy, lradius, oradius, icol, ocol);
	nvgFillPaint(vg, paint);
	nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
	nvgFill(vg);	
	
}

void sub_knob_med::onChange(EventChange &e) {
	Knob::onChange(e);
}
