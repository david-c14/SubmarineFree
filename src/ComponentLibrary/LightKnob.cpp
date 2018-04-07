#include "../SubmarineFree.hpp"

LightKnob::LightKnob() {
	enabled = 0;
}

void LightKnob::addFrame(std::shared_ptr<SVG> svg) {
	frames.push_back(svg);
	if (!sw->svg) {
		setSVG(svg);
	}
}

void LightKnob::setEnabled(char val) {
	if (val == enabled)
		return;
	enabled = val;
	assert(frames.size() > 1);
	sw->setSVG(frames[enabled]);
	dirty = true;
}

