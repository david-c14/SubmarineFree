#include "SubmarineFree.hpp"

struct SizeableModuleWidget : SchemeModuleWidget {
	bool stabilized = false;
	float fullSize = 210.0f;
	SchemePanel *panel;
	SizeableModuleWidget(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(210, 380);
		panel = new SchemePanel(this->box.size);
		addChild(panel);
	}
	void Resize() {
		panel->box.size = this->box.size;
		panel->dirty = true;
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
	MinButton() {
		this->box.size = Vec(10, 20);
	}
	void onDragEnd(const event::DragEnd &e) override {
		mw->Minimize(mw->box.size.x > 16.0f);
		e.consume(this);
	}
	void draw(const DrawArgs &args) override {
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
};

struct BackPanel : Widget {
	void draw(const DrawArgs &args) override {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGB(0 ,0 ,0));
		nvgFill(args.vg);
		Widget::draw(args);
	}
};

struct WM101 : SizeableModuleWidget {
	MinButton *minButton;
	BackPanel *backPanel;
	WM101(Module *module) : SizeableModuleWidget(module) {
		minButton = new MinButton();
		minButton->box.pos = Vec(0,180);
		minButton->mw = this;
		addChild(minButton); 
		backPanel = new BackPanel();
		backPanel->box.pos = Vec(10, 15);
		backPanel->box.size = Vec(box.size.x - 20, box.size.y - 30);
		addChild(backPanel);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		if (this->box.size.x > 16.0f) {
			drawBase(vg, "WM-101");
		}
		else {
			drawBackground(vg);
			drawLogo(vg, 0, 380, 1, -M_PI / 2.0f);
			nvgSave(vg);
			nvgTranslate(vg, 0, 377);
			nvgRotate(vg, -M_PI / 2.0f);
			drawText(vg, 20, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine");
			drawText(vg, 220, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "WM-101 Wire Manager");
			nvgRestore(vg);
		}
	}
	void step() override {
		if (!stabilized) {
			stabilized = true;
		}
		SizeableModuleWidget::step();
	}
	void onResize() override {
		bool small = this->box.size.x < 16.0f;
		minButton->box.pos.x = small?2.5f:(this->box.size.x - 10.0f);
		backPanel->visible = !small;
		SizeableModuleWidget::onResize();	
	}
};

Model *modelWM101 = createModel<Module, WM101>("WM-101");
