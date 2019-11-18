#include <settings.hpp>
#include "../SubmarineFree.hpp"

void SubTooltip::step() {
	if (stepLambda != NULL)
		stepLambda();
	//Tooltip::step();
}

void TooltipKnob::onEnter(const event::Enter &e) {
	if (settings::paramTooltip && !tooltip) {
		SubTooltip *stt = new SubTooltip();
		tooltip = stt;
		stt->stepLambda = [=]() {
			tooltip->text = "";
			if (this->getText != NULL)
				tooltip->text = this->getText();
			stt->Tooltip::step();
			tooltip->box.pos = this->getAbsoluteOffset(this->box.size).round();
		};
		APP->scene->addChild(tooltip);
	}	
}

void TooltipKnob::onLeave(const event::Leave &e) {
	if (tooltip) {
		APP->scene->removeChild(tooltip);
		delete tooltip;
		tooltip = NULL;
	}
}

