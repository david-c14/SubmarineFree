#include "SubmarineFree.hpp"

struct SizeableModuleWidget : SchemeModuleWidget {
	bool stabilized = false;
	float fullSize = 210.0f;
	SizeableModuleWidget(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(210, 380);
		addChild(new SchemePanel(this->box.size));
	}
	void Resize() {
		onResize();
	}
	void Minimize(bool minimize) {
		float oldSize = box.size.x;
		box.size.x = minimize?15:fullSize;
		ShiftOthers(box.size.x - oldSize);
		Resize();
	}
	void ShiftOthers(float delta) {
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
	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();
		json_object_set_new (rootJ, "width", json_real(box.size.x));
		return rootJ;
	}
	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);
		json_t *widthJ = json_object_get(rootJ, "width");
		if (widthJ)
			box.size.x = json_number_value(widthJ);
		Minimize(box.size.x < 16.0f);
	}
	virtual void onResize() {
	}
};

struct MinButton : OpaqueWidget {
	SizeableModuleWidget *mw;
	void onDragEnd(const event::DragEnd &e) override {
		mw->Minimize(mw->box.size.x > 16.0f);
		e.consume(this);
	}
};

struct WM101 : SizeableModuleWidget {
	WM101(Module *module) : SizeableModuleWidget(module) {
		MinButton *minButton = new MinButton();
		minButton->box.pos = Vec(0,0);
		minButton->box.size = Vec(15,30);
		minButton->mw = this;
		addChild(minButton); 
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "WM-101");
	}
	void step() override {
		if (!stabilized) {
			stabilized = true;
		}
		SizeableModuleWidget::step();
	}
};

Model *modelWM101 = createModel<Module, WM101>("WM-101");
