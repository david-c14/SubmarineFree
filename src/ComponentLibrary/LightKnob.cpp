#include "../SubmarineFree.hpp"

LightKnob::LightKnob() {
	enabled = 0;
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
