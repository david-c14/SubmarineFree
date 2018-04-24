#include "DS.hpp"

float DS_Module::midpoint() {
	return (voltage0 * 0.5f + voltage1 * 0.5f);
}

json_t *DS_Module::toJson() {
	json_t *rootJ = json_object();
	json_object_set_new(rootJ, "voltage0", json_real(voltage0));
	json_object_set_new(rootJ, "voltage1", json_real(voltage1));
	return rootJ;
}

void DS_Module::fromJson(json_t *rootJ) {
	json_t *j0 = json_object_get(rootJ, "voltage0");
	if (j0)
		voltage0 = json_number_value(j0);
	json_t *j1 = json_object_get(rootJ, "voltage1");
	if (j1)
		voltage1 = json_number_value(j1);
}

void DS_Module::onReset() {
	voltage0 = 0.0f;
	voltage1 = 5.0f;
}

float DS_Module::output(int state) {
	return state?voltage1:voltage0;
} 

void DS_Module::appendContextMenu(Menu *menu) {
	menu->addChild(MenuEntry::create());
	DS_MenuItemH *mh = MenuItem::create<DS_MenuItemH>("1V");
	mh->module = this;
	mh->v = 1.0f;
	menu->addChild(mh);
	mh = MenuItem::create<DS_MenuItemH>("5V");
	mh->module = this;
	mh->v = 5.0f;
	menu->addChild(mh);
	mh = MenuItem::create<DS_MenuItemH>("10V");
	mh->module = this;
	mh->v = 10.0f;
	menu->addChild(mh);
	menu->addChild(MenuEntry::create());
	DS_MenuItemL *ml = MenuItem::create<DS_MenuItemL>("0V");
	ml->module = this;
	ml->v = 0.0f;
	menu->addChild(ml);
	ml = MenuItem::create<DS_MenuItemL>("-5V");
	ml->module = this;
	ml->v = -5.0f;
	menu->addChild(ml);
	ml = MenuItem::create<DS_MenuItemL>("-10V");
	ml->module = this;
	ml->v = -10.0f;
	menu->addChild(ml);
}

void DS_MenuItemH::onAction(EventAction &e) {
	module->voltage1 = v;
}

void DS_MenuItemH::step() {
	rightText = CHECKMARK(module->voltage1 == v);
}

void DS_MenuItemL::onAction(EventAction &e) {
	module->voltage0 = v;
}

void DS_MenuItemL::step() {
	rightText = CHECKMARK(module->voltage0 == v);
}


float DS_Schmitt::high(float v0, float v1) {
	return (v0 * 0.4f + v1 * 0.6f);
}

float DS_Schmitt::low(float v0, float v1) {
	return (v0 * 0.6f + v1 * 0.4f);
}

void DS_Schmitt::reset() {
	_state = 0;
}

void DS_Schmitt::set() {
	_state = 1;
}

int DS_Schmitt::state(float vl, float vh, float v) {
	if (_state) {
		if (v < vl)
			_state = 0;
	}
	else {
		if (v > vh)
			_state = 1;
	}
	return _state;
}

int DS_Schmitt::state(DS_Module *module, float v) {
	return state(low(module->voltage0, module->voltage1), high(module->voltage0, module->voltage1), v);
}

int DS_Schmitt::edge(float vl, float vh, float v) {
	int old = _state;
	return (state(vl, vh, v) != old);
}
	
int DS_Schmitt::edge(DS_Module *module, float v) {
	int old = _state;
	return (state(module, v) != old);
}

int DS_Schmitt::redge(float vl, float vh, float v) {
	int old = _state;
	return (state(vl, vh, v) && !old);
}

int DS_Schmitt::redge(DS_Module *module, float v) {
	int old = _state;
	return (state(module, v) && !old);
}

int DS_Schmitt::fedge(float vl, float vh, float v) {
	int old = _state;
	return (!state(vl, vh, v) && old);
}

int DS_Schmitt::fedge(DS_Module *module, float v) {
	int old = _state;
	return (!state(module, v) && old);
}
