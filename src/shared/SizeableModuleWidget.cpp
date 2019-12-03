#include <settings.hpp>
#include "../SubmarineFree.hpp"

SizeableModuleWidget::SizeableModuleWidget(float size) {
	fullSize = size;
	this->box.size = Vec(fullSize, 380);
	panel = new SchemePanel(this->box.size);
	addChild(panel);
}

void SizeableModuleWidget::Resize() {
	panel->box.size = this->box.size;
	panel->dirty = true;
	onResize();
}

void SizeableModuleWidget::Minimize(bool minimize) {
	float oldSize = box.size.x;
	box.size.x = minimize?15:fullSize;
	ShiftOthers(box.size.x - oldSize);
	Resize();
	unsigned int id = module->id;
	float fs = fullSize; 
	if (!stabilized)
		return;
	APP->history->push(new EventWidgetAction(
		"Resize Wire Manager",
		[id, minimize, fs]() {
			SizeableModuleWidget *mw = dynamic_cast<SizeableModuleWidget *>(APP->scene->rack->getModule(id));
			if (mw) {
				mw->box.size.x = minimize?fs:15;
				mw->ShiftOthers(minimize?(fs-15):(15-fs));
				mw->Resize();
			}
		},
		[id, minimize, fs]() {
			SizeableModuleWidget *mw = dynamic_cast<SizeableModuleWidget *>(APP->scene->rack->getModule(id));
			if (mw) {
				mw->box.size.x = minimize?15:fs;
				mw->ShiftOthers(minimize?(15-fs):(fs-15));
				mw->Resize();
			}
		}
	));
}

void SizeableModuleWidget::ShiftOthers(float delta) {
	if (!stabilized)
		return;
	if (delta == 0.0f)
		return;
	for (Widget *w : APP->scene->rack->moduleContainer->children) {
		if (this == w)
			continue;
		if (this->box.pos.x > w->box.pos.x) 
			continue;
		if (this->box.pos.y != w->box.pos.y)
			continue;
		w->box.pos.x += delta;
	}
}

json_t *SizeableModuleWidget::toJson() {
	json_t *rootJ = ModuleWidget::toJson();
	json_object_set_new (rootJ, "width", json_real(box.size.x));
	return rootJ;
}

void SizeableModuleWidget::fromJson(json_t *rootJ) {
	ModuleWidget::fromJson(rootJ);
	json_t *widthJ = json_object_get(rootJ, "width");
	if (widthJ)
		box.size.x = json_number_value(widthJ);
	Minimize(box.size.x < 16.0f);
	APP->scene->rack->requestModulePos(this, box.pos);
}

void SizeableModuleWidget::onResize() {
}

MinButton::MinButton() {
	this->box.size = Vec(10, 20);
	this->clickHandler = [=]() {
		mw->Minimize(mw->box.size.x > 16.0f);
	};
}

void MinButton::draw(const DrawArgs &args) {
	nvgBeginPath(args.vg);
	if (this->box.pos.x < 15.0f) {
		nvgMoveTo(args.vg, 0, 0);
		nvgLineTo(args.vg, 10, 10);
		nvgLineTo(args.vg, 0, 20);
		nvgClosePath(args.vg);
	}
	else {
		nvgMoveTo(args.vg, 10, 0);
		nvgLineTo(args.vg, 0, 10);
		nvgLineTo(args.vg, 10, 20);
		nvgClosePath(args.vg);
	}
	nvgFillColor(args.vg, gScheme.getAlternative(mw->module));
	nvgFill(args.vg);
	Widget::draw(args);
}
