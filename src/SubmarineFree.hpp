#include "rack0.hpp"

using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *pluginInstance;

// Forward-declare each Model, defined in each module source file
extern Model *modelAG104;
extern Model *modelAG106;

extern Model *modelAO106;
extern Model *modelAO112;
extern Model *modelAO118;
extern Model *modelAO124;
extern Model *modelAO136;

extern Model *modelBB120;

extern Model *modelBP101;
extern Model *modelBP102;
extern Model *modelBP104;
extern Model *modelBP108;
extern Model *modelBP110;
extern Model *modelBP112;
extern Model *modelBP116;
extern Model *modelBP120;
extern Model *modelBP124;
extern Model *modelBP132;

extern Model *modelDN112;

extern Model *modelEO102;

extern Model *modelFF110;
extern Model *modelFF120;
extern Model *modelFF206;
extern Model *modelFF212;

extern Model *modelLA108;

extern Model *modelLD103;
extern Model *modelLD106;

extern Model *modelNG106;
extern Model *modelNG112;

extern Model *modelOG104;
extern Model *modelOG106;

extern Model *modelPG104;
extern Model *modelPG112;

extern Model *modelPO101;
extern Model *modelPO102;
extern Model *modelPO204;

extern Model *modelSS112;
extern Model *modelSS208;
extern Model *modelSS212;
extern Model *modelSS220;
extern Model *modelSS221;

extern Model *modelTD116;
extern Model *modelTD202;
extern Model *modelTF101;
extern Model *modelTM105;

extern Model *modelWK101;
extern Model *modelWK205;

extern Model *modelXF101;
extern Model *modelXF102;
extern Model *modelXF104;
extern Model *modelXF201;
extern Model *modelXF202;
extern Model *modelXF301;

extern Model *modelXG104;
extern Model *modelXG106;

#include "ComponentLibrary/components.hpp"

struct SubHelper {
	static std::shared_ptr<Svg> LoadPanel(Plugin *pluginInstance, const char *str, int num) {
		char workingSpace[100];
		snprintf(workingSpace, 100, "res/%s%02d.svg", str, num);
		return APP->window->loadSvg(asset::plugin(pluginInstance, workingSpace));
	}
};

void setWidgetValue(ParamWidget *widget, float value) {
	if (widget->paramQuantity) {
		if (widget->paramQuantity->module) {
			APP->engine->setParam(widget->paramQuantity->module, widget->paramQuantity->paramId, value);
		}
	}
}

#define SUBMENU "\xe2\x96\xb6"
