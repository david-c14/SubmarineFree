#include "../SubmarineFree.hpp"

LightKnob::LightKnob() {
	enabled = 0;
}

void LightKnob::addFrame(std::shared_ptr<SVG> svg) {
	frames.push_back(svg);
	if (!sw->svg) {
		sw->setSVG(svg);
		box.size = sw->box.size;
	}
}

void LightKnob::setEnabled(char val) {
	if (val == enabled)
		return;
	enabled = val;
	debug("Enabled %f", sw->box.size.x);
	assert(frames.size() > 1);
	sw->setSVG(frames[enabled]);
	dirty = true;
}

