#include <settings.hpp>
#include "../SubmarineFree.hpp"

void SubTooltip::step() {
	if (stepLambda != NULL)
		stepLambda();
	//Tooltip::step();
}
