//SubTag W10
#include <settings.hpp>
#include "SubmarineFree.hpp"
#include "ComponentLibrary/EventWidgets.hpp"

Widget *masterWireManager = NULL;

struct SizeableModuleWidget : SchemeModuleWidget {
	bool stabilized = false;
	float fullSize = 0;
	SchemePanel *panel;
	SizeableModuleWidget(Module *module, float size) : SchemeModuleWidget(module) {
		fullSize = size;
		this->box.size = Vec(fullSize, 380);
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
		APP->scene->rack->requestModulePos(this, box.pos);
	}
	virtual void onResize() {
	}
};

struct MinButton : EventWidgetButtonBase {
	SizeableModuleWidget *mw;
	MinButton() {
		this->box.size = Vec(10, 20);
		this->clickHandler = [=]() {
			mw->Minimize(mw->box.size.x > 16.0f);
		};
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

std::string percentageTextHandler(float value) {
	return string::f("%.3g%c", value * 100.0, '%');
}

struct AddButton : EventWidgetButtonBase {
	void draw (const DrawArgs &args) override {
		nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		nvgStrokeWidth(args.vg, 2);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, box.size.x / 2, 0);
		nvgLineTo(args.vg, box.size.x / 2, box.size.y);
		nvgMoveTo(args.vg, 0, box.size.y / 2);
		nvgLineTo(args.vg, box.size.x, box.size.y / 2);

		nvgStroke(args.vg);
		OpaqueWidget::draw(args);
	}
};

struct MenuButton : EventWidgetButtonBase {
	void draw (const DrawArgs &args) override {
		nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		nvgStrokeWidth(args.vg, 2);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0, box.size.y / 2 - 4);
		nvgLineTo(args.vg, box.size.x, box.size.y / 2 - 4);
		nvgMoveTo(args.vg, 0, box.size.y / 2);
		nvgLineTo(args.vg, box.size.x, box.size.y / 2);
		nvgMoveTo(args.vg, 0, box.size.y / 2 + 4);
		nvgLineTo(args.vg, box.size.x, box.size.y / 2 + 4);

		nvgStroke(args.vg);
		OpaqueWidget::draw(args);
	}
};

struct EditPanel : BackPanel {
	std::function<void(NVGcolor)> completeHandler;
	std::function<void()> cancelHandler;
	EventWidgetSlider *r;
	EventWidgetSlider *g;
	EventWidgetSlider *b;
	EditPanel() {
		r = new EventWidgetSlider();
		r->box.pos = Vec(10, 105);
		r->box.size = Vec(110, 19);
		r->textHandler = percentageTextHandler;
		r->label = "Red";
		addChild(r);
		g = new EventWidgetSlider();
		g->box.pos = Vec(10, 145);
		g->box.size = Vec(110, 19);
		g->textHandler = percentageTextHandler;
		g->label = "Green";
		addChild(g);
		b = new EventWidgetSlider();
		b->box.pos = Vec(10, 185);
		b->box.size = Vec(110, 19);
		b->textHandler = percentageTextHandler;
		b->label = "Blue";
		addChild(b);
		EventWidgetButton *saveButton = new EventWidgetButton();
		saveButton->box.pos = Vec(5, 250);
		saveButton->box.size = Vec(55, 19);
		saveButton->label = "Save";
		saveButton->clickHandler = [=](){
			if (this->completeHandler) {
				this->completeHandler(nvgRGBf(r->value, g->value, b->value));
			}
		};
		addChild(saveButton);
		
		EventWidgetButton *cancelButton = new EventWidgetButton();
		cancelButton->box.pos = Vec(70, 250);
		cancelButton->box.size = Vec(55, 19);
		cancelButton->label = "Cancel";
		cancelButton->clickHandler = [=](){
			if (this->cancelHandler) {
				this->cancelHandler();
			}
		};
		addChild(cancelButton);
	}
	void drawBackground(NVGcontext *vg, float r1, float r2, float g1, float g2, float b1, float b2, float y) {
		NVGpaint grad = nvgLinearGradient(vg, r->box.pos.x + 5, 100, r->box.size.x - 10, 100, nvgRGBf(r1, g1, b1), nvgRGBf(r2, g2, b2));
		nvgBeginPath(vg);
		nvgFillPaint(vg, grad);
		nvgRect(vg, r->box.pos.x, y, r->box.size.x, 10);
		nvgFill(vg);
	}
	void draw(const DrawArgs &args) override {
		BackPanel::draw(args);
		NVGcolor color = nvgRGBf(r->value, g->value, b->value);
		NVGcolor colorOutline = nvgLerpRGBA(color, nvgRGBf(0.0, 0.0, 0.0), 0.5);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 12, 12);
		nvgQuadTo(args.vg, box.size.x / 2, 150, box.size.x - 12, 12);
		nvgStrokeColor(args.vg, colorOutline);
		nvgStrokeWidth(args.vg, 5);
		nvgStroke(args.vg);

		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, 3);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 12, 12, 9);
		nvgCircle(args.vg, box.size.x - 12, 12, 9);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);

		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, colorOutline);
		nvgStroke(args.vg);
	
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 12, 12, 5);
		nvgCircle(args.vg, box.size.x - 12, 12, 5);
		nvgFillColor(args.vg, nvgRGB(0, 0, 0));
		nvgFill(args.vg);

		drawBackground(args.vg, 0, 1, g->value, g->value, b->value, b->value, 109.5f);	
		drawBackground(args.vg, r->value, r->value, 0, 1, b->value, b->value, 149.5f);
		drawBackground(args.vg, r->value, r->value, g->value, g->value, 0, 1, 189.5f);
	}
};

struct WireParamField : ui::TextField {
	std::function<void(std::string)> changeHandler;
	void step() override {
		// Keep selected
		APP->event->setSelected(this);
		TextField::step();
	}

	void setText(std::string text) {
		this->text = text;
		selectAll();
	}

	void onSelectKey(const event::SelectKey &e) override {
		if (e.action == GLFW_PRESS && (e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_KP_ENTER)) {
			if (changeHandler) {
				changeHandler(text);
			}
			ui::MenuOverlay *overlay = getAncestorOfType<ui::MenuOverlay>();
			overlay->requestDelete();
			e.consume(this);
		}
		if (!e.getTarget())
			TextField::onSelectKey(e);
	}
};

struct WireButton : EventWidgetButtonBase {
	NVGcolor color;
	EventWidgetCheckBox *checkBox;
	WireButton() {
		checkBox = new EventWidgetCheckBox();
		checkBox->box.pos = Vec(1,1);
		checkBox->box.size = Vec(19, 19);
		addChild(checkBox);
	}
	bool testChar(char x) {
		if (x >= '0' && x <= '9')
			return true;
		if (x >= 'A' && x <= 'F')
			return true;
		if (x >= 'a' && x <= 'f')
			return true;
		return false;
	}
	bool setColor(std::string text) {
		if (text[0] != '#')
			return false;
		for (unsigned int i = 1; i < 7; i++) {
			if (text.length() <= i)
				return false;
			if (!testChar(text[i]))
				return false;
		}
		color = color::fromHexString(text);
		return true;
	}
	void draw(const DrawArgs &args) override {
		NVGcolor colorOutline = nvgLerpRGBA(color, nvgRGBf(0.0, 0.0, 0.0), 0.5);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 32, box.size.y / 2);
		nvgLineTo(args.vg, box.size.x, box.size.y / 2);
		nvgStrokeColor(args.vg, colorOutline);
		nvgStrokeWidth(args.vg, 5);
		nvgStroke(args.vg);
		
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, 3);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 32, box.size.y / 2, 9);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
		
		nvgStrokeWidth(args.vg, 1.0);
		nvgStrokeColor(args.vg, colorOutline);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 32, box.size.y / 2, 5);
		nvgFillColor(args.vg, nvgRGBf(0.0, 0.0, 0.0));
		nvgFill(args.vg);

		OpaqueWidget::draw(args);
	}
	unsigned int index() {
		unsigned int i = 0;
		for (Widget *w : parent->children) {
			if (w == this)
				return i;
			i++;
		}
		return 0;
	}
};

struct WM101 : SizeableModuleWidget {

	enum {
		HIGHLIGHT_OFF,
		HIGHLIGHT_LOW,
		HIGHLIGHT_ON
	};
	int highlight;
	ModuleWidget *lastHover = NULL;
	bool highlightIsDirty = true;

	int cableCount = 0;
	Widget *lastCable = NULL;
	unsigned int newColorIndex = 0;

	MinButton *minButton;
	BackPanel *backPanel;
	EventWidgetCheckBox *checkBoxAll;
	BackPanel *deleteConfirmPanel;
	EventWidgetLabel *deleteLabel;
	EventWidgetButton *deleteCancelButton;
	EventWidgetButton *deleteOkButton;
	EditPanel *editPanel;
	BackPanel *settingsPanel;
	EventWidgetRadioButton *highlightOff;
	EventWidgetRadioButton *highlightLow;
	EventWidgetRadioButton *highlightOn;
	EventWidgetSlider *highlightSlider;
	EventWidgetCheckBox *varyCheck;
	EventWidgetSlider *varyH;
	EventWidgetSlider *varyS;
	EventWidgetSlider *varyL;
	BackPanel *blockingPanel;
	
	ScrollWidget *scrollWidget;
	WM101(Module *module) : SizeableModuleWidget(module, 150) {
		minButton = new MinButton();
		minButton->box.pos = Vec(140,180);
		minButton->mw = this;
		addChild(minButton); 
		backPanel = new BackPanel();
		backPanel->box.pos = Vec(10, 15);
		backPanel->box.size = Vec(box.size.x - 20, box.size.y - 30);
		addChild(backPanel);
		scrollWidget = new ScrollWidget();
		scrollWidget->box.pos = Vec(0, 21);
		scrollWidget->box.size = Vec(backPanel->box.size.x, backPanel->box.size.y - 21);
		backPanel->addChild(scrollWidget);
		checkBoxAll = new EventWidgetCheckBox();
		checkBoxAll->box.pos = Vec(1,1);
		checkBoxAll->box.size = Vec(19, 19);
		checkBoxAll->changeHandler = [=](){
			this->checkAll(checkBoxAll->selected);
		};
		backPanel->addChild(checkBoxAll);
		AddButton *addButton = new AddButton();
		addButton->box.pos = Vec(57, 2);
		addButton->box.size = Vec(16, 16);
		addButton->clickHandler = [=]() {
			this->editDialog(NULL);
		};
		backPanel->addChild(addButton);
		MenuButton *menuButton = new MenuButton();
		menuButton->box.pos = Vec(112, 2);
		menuButton->box.size = Vec(16, 16);
		menuButton->clickHandler = [=]() {
			this->addMenu();
		};
		menuButton->rightClickHandler = menuButton->clickHandler;
		backPanel->addChild(menuButton);

		deleteConfirmPanel = new BackPanel();
		deleteConfirmPanel->box.pos = backPanel->box.pos;
		deleteConfirmPanel->box.size = backPanel->box.size;
		deleteConfirmPanel->visible = false;
		addChild(deleteConfirmPanel);

		deleteLabel = new EventWidgetLabel();
		deleteLabel->box.pos = Vec(15, 195);
		deleteLabel->box.size = Vec(box.size.x - 40, 19);
		deleteConfirmPanel->addChild(deleteLabel);
		
		deleteOkButton = new EventWidgetButton();
		deleteOkButton->box.pos = Vec(5, 250);
		deleteOkButton->box.size = Vec(55, 19);
		deleteOkButton->label = "OK";
		deleteConfirmPanel->addChild(deleteOkButton);
		
		deleteCancelButton = new EventWidgetButton();
		deleteCancelButton->box.pos = Vec(70, 250);
		deleteCancelButton->box.size = Vec(55, 19);
		deleteCancelButton->label = "Cancel";
		deleteCancelButton->clickHandler = [=]() { this->cancel(); };
		deleteConfirmPanel->addChild(deleteCancelButton);

		editPanel = new EditPanel();
		editPanel->box.pos = backPanel->box.pos;
		editPanel->box.size = backPanel->box.size;
		editPanel->visible = false;
		editPanel->cancelHandler = [=]() {
			this->cancel();
		};
		addChild(editPanel);
		
		settingsPanel = new BackPanel();
		settingsPanel->box.pos = backPanel->box.pos;
		settingsPanel->box.size = backPanel->box.size;
		settingsPanel->visible = false;
		addChild(settingsPanel);

		varyCheck = new EventWidgetCheckBox();
		varyCheck->label = "Variation";
		varyCheck->box.pos = Vec(10, 5);
		varyCheck->box.size = Vec(box.size.x - 40, 19);
		varyCheck->changeHandler = [=]() { this->varyCheckChanged(); };
		settingsPanel->addChild(varyCheck);

		EventWidgetLabel *hLabel = new EventWidgetLabel();
		hLabel->label = "H";
		hLabel->box.pos = Vec(10, 25);
		hLabel->box.size = Vec(10, 19);
		settingsPanel->addChild(hLabel);
	
		EventWidgetLabel *sLabel = new EventWidgetLabel();
		sLabel->label = "S";
		sLabel->box.pos = Vec(10, 45);
		sLabel->box.size = Vec(10, 19);
		settingsPanel->addChild(sLabel);
	
		EventWidgetLabel *lLabel = new EventWidgetLabel();
		lLabel->label = "L";
		lLabel->box.pos = Vec(10, 65);
		lLabel->box.size = Vec(10, 19);
		settingsPanel->addChild(lLabel);

		varyH = new EventWidgetSlider();
		varyH->box.pos = Vec(20, 25);
		varyH->box.size = Vec(box.size.x - 50, 19);
		varyH->value = 0.1f;
		varyH->textHandler = [](float value) -> std::string {
			return string::f("%.4g%s", value * 360.0, "\xC2\xB0");
		};
		varyH->label = "Hue";
		varyH->changedHandler = [=](float value, float oldValue) { 
			this->saveSettings(); 
			APP->history->push(new EventWidgetAction(
				"Hue Variation Change",
				[oldValue]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyH->value = oldValue;
						wm->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyH->value = value;
						wm->saveSettings();
					}
				}
			));
		};
		settingsPanel->addChild(varyH);
	
		varyS = new EventWidgetSlider();
		varyS->box.pos = Vec(20, 45);
		varyS->box.size = Vec(box.size.x - 50, 19);
		varyS->value = 0.1f;
		varyS->textHandler = percentageTextHandler;
		varyS->label = "Saturation";
		varyS->changedHandler = [=](float value, float oldValue) { 
			this->saveSettings(); 
			APP->history->push(new EventWidgetAction(
				"Saturation Variation Change",
				[oldValue]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyS->value = oldValue;
						wm->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyS->value = value;
						wm->saveSettings();
					}
				}
			));
		};
		settingsPanel->addChild(varyS);
	
		varyL = new EventWidgetSlider();
		varyL->box.pos = Vec(20, 65);
		varyL->box.size = Vec(box.size.x - 50, 19);
		varyL->value = 0.1f;
		varyL->textHandler = percentageTextHandler;
		varyL->label = "Lightness";
		varyL->changedHandler = [=](float value, float oldValue) { 
			this->saveSettings(); 
			APP->history->push(new EventWidgetAction(
				"Lightness Variation Change",
				[oldValue]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyL->value = oldValue;
						wm->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->varyL->value = value;
						wm->saveSettings();
					}
				}
			));
		};
		settingsPanel->addChild(varyL);
	
		EventWidgetLabel *highlightLabel = new EventWidgetLabel();
		highlightLabel->label = "Highlighting";
		highlightLabel->box.pos = Vec(10, 105);
		highlightLabel->box.size = Vec(box.size.x - 40, 19);
		settingsPanel->addChild(highlightLabel);

		highlightOff = new EventWidgetRadioButton();
		highlightOff->label = "Off";
		highlightOff->box.pos = Vec(10, 125);
		highlightOff->box.size = Vec(box.size.x - 40, 19);
		highlightOff->selected = true;
		highlightOff->changeHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_OFF);
			this->saveSettings();
		};
		settingsPanel->addChild(highlightOff);

		highlightLow = new EventWidgetRadioButton();
		highlightLow->label = "When hovering";
		highlightLow->box.pos = Vec(10, 145);
		highlightLow->box.size = Vec(box.size.x - 40, 19);
		highlightLow->changeHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_LOW);
			this->saveSettings();
		};
		settingsPanel->addChild(highlightLow);

		highlightOn = new EventWidgetRadioButton();
		highlightOn->label = "Always On";
		highlightOn->box.pos = Vec(10, 165);
		highlightOn->box.size = Vec(box.size.x - 40, 19);
		highlightOn->changeHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_ON);
			this->saveSettings();
		};
		settingsPanel->addChild(highlightOn);

		highlightSlider = new EventWidgetSlider();
		highlightSlider->box.pos = Vec(10, 185);
		highlightSlider->box.size = Vec(box.size.x - 40, 21);
		highlightSlider->value = 0.1f;
		highlightSlider->textHandler = percentageTextHandler;
		highlightSlider->label = "Opacity";
		highlightSlider->changedHandler = [=](float value, float oldValue) { 
			this->saveSettings(); 
			APP->history->push(new EventWidgetAction(
				"Highlight Opacity Change",
				[oldValue]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->highlightSlider->value = oldValue;
						wm->saveSettings();
						wm->highlightIsDirty = true;
					}
				},
				[value]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						wm->highlightSlider->value = value;
						wm->saveSettings();
						wm->highlightIsDirty = true;
					}
				}
			));
		};
		settingsPanel->addChild(highlightSlider);

		EventWidgetButton *settingsButton = new EventWidgetButton();
		settingsButton->box.pos = Vec(35, 250);
		settingsButton->box.size = Vec(60, 19);
		settingsButton->label = "Close";
		settingsButton->clickHandler = [=]() {
			this->cancel();
		};
		settingsPanel->addChild(settingsButton);

		blockingPanel = new BackPanel();
		blockingPanel->box.pos = backPanel->box.pos;
		blockingPanel->box.size = backPanel->box.size;
		blockingPanel->visible = false;
		addChild(blockingPanel);

		EventWidgetLabel *blockingLabel = new EventWidgetLabel();
		blockingLabel->label = "Another WM-101";
		blockingLabel->box.pos = Vec(5, 100);
		blockingLabel->box.size = Vec(120, 21);
		blockingPanel->addChild(blockingLabel);

		EventWidgetLabel *blockingLabel2 = new EventWidgetLabel();
		blockingLabel2->label = "is Already Open";
		blockingLabel2->box.pos = Vec(5, 113);
		blockingLabel2->box.size = Vec(120, 21);
		blockingPanel->addChild(blockingLabel2);

		EventWidgetButton *blockingButton = new EventWidgetButton();
		blockingButton->label = "Take Over";
		blockingButton->box.pos = Vec(30, 150);
		blockingButton->box.size = Vec(70, 19);
		blockingButton->clickHandler = [=]() { this->takeMasterSlot(); };
		blockingPanel->addChild(blockingButton);

		loadSettings();
	}
	~WM101() {
		this->_delete();
	}
	void varyCheckChanged() {
		bool selected = varyCheck->selected;
		saveSettings();
		APP->history->push(new EventWidgetAction(
			selected?"Select Variations":"Deselect Variations",
			[selected]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->varyCheck->selected = !selected;
					wm->saveSettings();
				}
			},
			[selected]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->varyCheck->selected = selected;
					wm->saveSettings();
				}
			}
		));
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
	void takeMasterSlot() {
		blockingPanel->visible = false;
		backPanel->visible = (box.size.x > 16.0f);
		masterWireManager = this;
		scrollWidget->container->clearChildren();
		loadSettings();
	}
	void step() override {
		if (!module) {
			return;
		}
		if (masterWireManager != this) {
			if (masterWireManager) {
				blockingPanel->visible = (box.size.x > 16.0f);
				backPanel->visible = false;	
				editPanel->visible = false;
				settingsPanel->visible = false;
				SizeableModuleWidget::step();
				return;
			}
			takeMasterSlot();
		}
		if (!stabilized) {
			stabilized = true;
			cableCount = APP->scene->rack->cableContainer->children.size();
		}
		int newSize = APP->scene->rack->cableContainer->children.size();
		if (newSize < cableCount) {
			cableCount = newSize;
			if (cableCount) 
				lastCable = APP->scene->rack->cableContainer->children.back();
			else
				lastCable = NULL;
		}
		else if (newSize > cableCount) {
			history::ComplexAction* complex = NULL;
			if (cableCount == -1) {
				complex = new history::ComplexAction();
				complex->name = "Recolor All Wires";
				APP->history->push(complex);
				cableCount = 0;
			}
			std::list<Widget *>::reverse_iterator iterator = APP->scene->rack->cableContainer->children.rbegin();
			for (int i = 0; i < newSize - cableCount; i++) {
				colorCable(*iterator, complex);
				++iterator;
			}
			cableCount = newSize;
			if (cableCount)
				lastCable = APP->scene->rack->cableContainer->children.back();
			else
				lastCable = NULL;
			highlightIsDirty = true;		
		}
		highlightWires();
		SizeableModuleWidget::step();
	}
	void highlightWires() {
		ModuleWidget *focusedModuleWidget = nullptr;
		if (highlight) {
			if (APP->event && APP->event->hoveredWidget) {
				focusedModuleWidget = dynamic_cast<ModuleWidget *>(APP->event->hoveredWidget);
				if (!focusedModuleWidget)
					focusedModuleWidget = APP->event->hoveredWidget->getAncestorOfType<ModuleWidget>();
			}
		}
		if (focusedModuleWidget != lastHover) {
			lastHover = focusedModuleWidget;
			highlightIsDirty = true;
		}
		if (highlightIsDirty) {
			highlightIsDirty = false;
			for (Widget *widget : APP->scene->rack->cableContainer->children) {
				CableWidget *cable = dynamic_cast<CableWidget *>(widget);
				if (focusedModuleWidget) {
					if (!cable->outputPort || !cable->inputPort) {
						cable->color = nvgTransRGBA(cable->color, 0xFF);
					}
					else if (cable->outputPort->getAncestorOfType<ModuleWidget>() == focusedModuleWidget) {
						cable->color = nvgTransRGBA(cable->color, 0xFF);
					}
					else if (cable->inputPort->getAncestorOfType<ModuleWidget>() == focusedModuleWidget) {
						cable->color = nvgTransRGBA(cable->color, 0xFF);
					}
					else {
						cable->color = nvgTransRGBAf(cable->color, highlightSlider->value);
					}
				}
				else {
					if (highlight == 2)
						cable->color = nvgTransRGBAf(cable->color, highlightSlider->value);
					else
						cable->color = nvgTransRGBA(cable->color, 0xFF);
				}

			}
		}
	}
	void colorCable(Widget *widget, history::ComplexAction *complex) {
		CableWidget *cable = dynamic_cast<CableWidget *>(widget);
		NVGcolor oldColor = cable->color;
		cable->color = findColor(cable->color);
		if (varyCheck->selected) {
			cable->color = varyColor(cable->color);
		}
		if (!complex)
			return;
		NVGcolor newColor = cable->color;
		int id = cable->cable->id;
		complex->push(
			new EventWidgetAction("Color Cable",
				[id, oldColor](){
					CableWidget *c = APP->scene->rack->getCable(id);
					if (c)
						c->color = oldColor;
				},
				[id, newColor]() {
					CableWidget *c = APP->scene->rack->getCable(id);
					if (c)
						c->color = newColor;
				}
			)
		); 
	}
	NVGcolor findColor(NVGcolor color) {
		auto vi = scrollWidget->container->children.begin();
		std::advance(vi, newColorIndex);
		for (int i = 0; i < 2; i++) {
			while(newColorIndex < scrollWidget->container->children.size()) {
				newColorIndex++;
				WireButton *wb = dynamic_cast<WireButton *>(*vi);
				if (wb->checkBox->selected) {
					return wb->color;
				}
				std::advance(vi, 1);
			}
			newColorIndex = 0;
			vi = scrollWidget->container->children.begin();
		}
		return color;
	}
	NVGcolor varyColor(NVGcolor color) {
		float r = color.r;
		float g = color.g;
		float b = color.b;
		float a = color.a;

	// convert to hsl
		
		float Cmax = std::max(r, std::max(g,b));
		float Cmin = std::min(r, std::min(g,b));
		float delta = Cmax - Cmin;

		float h = 0;
		float s = 0;
		float l = (Cmax + Cmin) / 2;

		if (delta > 0) {
			s = delta / (1 - std::abs(l * 2 - 1));
			if (Cmax == r) {
				h = std::fmod(6 + (g-b)/delta, 6);
			}
			else if (Cmax == g) {
				h = (b-r)/delta + 2;
			}
			else {
				h = (r-g)/delta + 4;
			}
		}
		
	// Modify color

		h = std::fmod(1 + h / 6 + (random::uniform() - 0.5f) * varyH->value, 1.0f);
		s = rescale(random::uniform(), 0.0f, 1.0f, std::max(s - varyS->value, 0.0f), std::min(s + varyS->value, 1.0f));
		l = rescale(random::uniform(), 0.0f, 1.0f, std::max(l - varyL->value, 0.0f), std::min(l + varyL->value, 1.0f));
		return nvgHSLA(h, s, l, a * 255);
	}
	void onResize() override {
		bool small = this->box.size.x < 16.0f;
		minButton->box.pos.x = small?2.5f:(this->box.size.x - 10.0f);
		backPanel->visible = !small;
		editPanel->visible = false;
		blockingPanel->visible = false;
		deleteConfirmPanel->visible = false;
		settingsPanel->visible = false;
		SizeableModuleWidget::onResize();	
	}
	EventWidgetAction *checkBoxAction(unsigned int index, bool selected) {
		return new EventWidgetAction(
				selected?"Select Color":"Deselect Color",	
				[index, selected]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						WireButton *wb = wm->findWireButton(index);
						if (wb) {
							wb->checkBox->selected = !selected;
							wm->saveSettings();
						}
					}
				},
				[index, selected]() {
					if (masterWireManager) {
						WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
						WireButton *wb = wm->findWireButton(index);
						if (wb) {
							wb->checkBox->selected = selected;
							wm->saveSettings();
						}
					}
				}
			)
		;
	}
	void updateWireButtonCheckBox(WireButton *wb) {
		this->saveSettings();
		APP->history->push(checkBoxAction(wb->index(), wb->checkBox->selected));
	}
	void addColor(NVGcolor color, bool selected) {
		float y = scrollWidget->container->children.size() * 21;
		WireButton *wb = new WireButton();
		wb->box.pos = Vec(0, y);
		wb->box.size = Vec(scrollWidget->box.size.x, 21);
		wb->color = color;
		wb->checkBox->selected = selected;
		wb->checkBox->changeHandler = [=]() {
			this->updateWireButtonCheckBox(wb);
		};
		wb->rightClickHandler = [=]() {
			this->addWireMenu(wb);
		};
		scrollWidget->container->addChild(wb);
	}
	void insertColor(NVGcolor color, bool selected, unsigned int index) {
		addColor(color, selected);
		Widget *w = scrollWidget->container->children.back();
		scrollWidget->container->children.pop_back();
		auto v = scrollWidget->container->children.begin();
		std::advance(v, index);
		scrollWidget->container->children.insert(v, w);
		this->reflow();
	}
	void setDefaults() {
		for (NVGcolor color : settings::cableColors) {
			addColor(color, true);
		}
		
		addColor(nvgRGB(0xff, 0xae, 0xc9), false);
		addColor(nvgRGB(0xb7, 0x00, 0xb5), false);
		addColor(nvgRGB(0x80, 0x80, 0x80), false);
		addColor(nvgRGB(0xff, 0xff, 0xff), false);
		addColor(nvgRGB(0x10, 0x0f, 0x12), false);
		addColor(nvgRGB(0xff, 0x99, 0x41), false);
		addColor(nvgRGB(0x80, 0x36, 0x10), false);
	}
	void loadSettings() {
		json_error_t error;
		FILE *file = fopen(asset::user("SubmarineFree/WM-101.json").c_str(), "r");
		if (!file) {
			file = fopen(asset::user("SubmarineUtility/WireManager.json").c_str(), "r");
		}
		if (!file) {
			setDefaults();
			return;
		}
		json_t *rootJ = json_loadf(file, 0, &error);
		fclose(file);
		if (!rootJ) {
			WARN("Submarine Free WM-101: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
			return;
		}
		json_t *arr = json_object_get(rootJ, "colors");
		if (arr) {
			int size = json_array_size(arr);
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					json_t *c1 = json_object_get(j1, "color");
					if (c1) {
						int selected = false;
						json_t *s1 = json_object_get(j1, "selected");
						if (s1) {
							selected = clamp((int)json_number_value(s1), 0, 1);
						}
						addColor(color::fromHexString(json_string_value(c1)), selected);
					}
				}
			}
		}
		json_t *h1 = json_object_get(rootJ, "highlight");
		if (h1) {
			highlight = clamp((int)json_number_value(h1), 0, 2);
			highlightChangedCore(highlight);
		}
		json_t *t1 = json_object_get(rootJ, "highlight_trans");
		if (t1) {
			highlightSlider->value = clamp(json_number_value(t1), 0.0f, 1.0f);
		}
		json_t *v1 = json_object_get(rootJ, "variation");
		if (v1) {
			varyCheck->selected = clamp((int)json_number_value(v1), 0, 1);
		}
		v1 = json_object_get(rootJ, "variationH");
		if (v1) {
			varyH->value = clamp(json_number_value(v1), 0.0f, 1.0f);
		}
		v1 = json_object_get(rootJ, "variationS");
		if (v1) {
			varyS->value = clamp(json_number_value(v1), 0.0f, 1.0f);
		}
		v1 = json_object_get(rootJ, "variationL");
		if (v1) {
			varyL->value = clamp(json_number_value(v1), 0.0f, 1.0f);
		}
		json_decref(rootJ);
	}
	void saveSettings() {
		json_t *settings = json_object();
		json_t *arr = json_array();
		for (Widget *w : scrollWidget->container->children) {
			WireButton *wb = dynamic_cast<WireButton *>(w);
			json_t *color = json_object();
			std::string s = color::toHexString(wb->color);
			json_object_set_new(color, "color", json_string(s.c_str()));
			json_object_set_new(color,"selected", json_real(wb->checkBox->selected));
			json_array_append_new(arr, color);
		}
		json_object_set_new(settings, "colors", arr);
		json_object_set_new(settings, "highlight", json_real(highlight));
		json_object_set_new(settings, "highlight_trans", json_real(highlightSlider->value));
		json_object_set_new(settings, "variation", json_real(varyCheck->selected));
		json_object_set_new(settings, "variationH", json_real(varyH->value));
		json_object_set_new(settings, "variationS", json_real(varyS->value));
		json_object_set_new(settings, "variationL", json_real(varyL->value));
		system::createDirectory(asset::user("SubmarineFree"));
		std::string settingsFilename = asset::user("SubmarineFree/WM-101.json");
		FILE *file = fopen(settingsFilename.c_str(), "w");
		if (file) {
			json_dumpf(settings, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
			fclose(file);
		}
		json_decref(settings);
	}
	WireButton *findWireButton(unsigned int index) {
		if (index >= scrollWidget->container->children.size())
			return NULL;
		auto vi = scrollWidget->container->children.begin();
		std::advance(vi, index);
		return dynamic_cast<WireButton *>(*vi);
	}
	void checkAll(bool selected) {
		history::ComplexAction *complex = new history::ComplexAction();
		complex->name = selected?"Select All Colors":"Deselect All Colors"; 
		APP->history->push(complex);
		EventWidgetAction *allAction = new EventWidgetAction(
			complex->name,
			[selected]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->checkBoxAll->selected = !selected;
				}
			},
			[selected]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->checkBoxAll->selected = selected;
				}
			}
		);
		complex->push(allAction);
		
		signed int index = -1;
		for (Widget *widget : scrollWidget->container->children) {
			WireButton *wb = dynamic_cast<WireButton *>(widget);
			index++;
			if (wb->checkBox->selected == selected)
				continue;
			wb->checkBox->selected = selected;
			complex->push(checkBoxAction(index, selected));
		}
		saveSettings();
	}
	bool isFirst(WireButton *wb) {
		return scrollWidget->container->children.front() == wb;
	}
	bool isLast(WireButton *wb) {
		return scrollWidget->container->children.back() == wb;
	}
	void addMenu() {
		Menu *menu = createMenu();

		menu->addChild(new MenuLabel());

		EventWidgetMenuItem *cAll = new EventWidgetMenuItem();
		cAll->text = "Select All Colors";
		cAll->clickHandler = [=]() {
			checkBoxAll->selected = true;
			checkAll(true);
		};
		menu->addChild(cAll);

		EventWidgetMenuItem *uAll = new EventWidgetMenuItem();
		uAll->text = "Unselect All Colors";
		uAll->clickHandler = [=]() {
			checkBoxAll->selected = false;
			checkAll(false);
		};
		menu->addChild(uAll);

		menu->addChild(new MenuLabel());

		EventWidgetMenuItem *rAll = new EventWidgetMenuItem();
		rAll->text = "Recolor All Wires...";
		rAll->clickHandler = [=]() {
			this->recolorAllDialog();
		};
		menu->addChild(rAll);

		menu->addChild(new MenuLabel());

		EventWidgetMenuItem *add = new EventWidgetMenuItem();
		add->text = "Add New Color ...";
		add->clickHandler = [=]() {
			this->editDialog(NULL);	
		};
		menu->addChild(add);
		
		menu->addChild(new MenuLabel());

		EventWidgetMenuItem *settings = new EventWidgetMenuItem();
		settings->text = "Settings...";
		settings->clickHandler = [=]() {
			this->settingsDialog();
		};
		menu->addChild(settings);
	
		menu->addChild(new MenuLabel());

		EventWidgetMenuItem *var = new EventWidgetMenuItem();
		var->text = "Variations";
		var->rightText = CHECKMARK(varyCheck->selected);
		var->clickHandler = [=]() {
			varyCheck->selected = !(varyCheck->selected);
			varyCheckChanged();
		};
		menu->addChild(var);

		EventWidgetMenuItem *hOff = new EventWidgetMenuItem();
		hOff->text = "Highlight Off";
		hOff->rightText = CHECKMARK(highlight == HIGHLIGHT_OFF);
		hOff->clickHandler = [=]() {
			highlightChanged(HIGHLIGHT_OFF);
			saveSettings();
		};
		menu->addChild(hOff);

		EventWidgetMenuItem *hLow = new EventWidgetMenuItem();
		hLow->text = "Highlight When Hovering";
		hLow->rightText = CHECKMARK(highlight == HIGHLIGHT_LOW);
		hLow->clickHandler = [=]() {
			highlightChanged(HIGHLIGHT_LOW);
			saveSettings();
		};
		menu->addChild(hLow);

		EventWidgetMenuItem *hOn = new EventWidgetMenuItem();
		hOn->text = "Highlight Always";
		hOn->rightText = CHECKMARK(highlight == HIGHLIGHT_ON);
		hOn->clickHandler = [=]() {
			highlightChanged(HIGHLIGHT_ON);
			saveSettings();
		};
		menu->addChild(hOn);
	}
	void addWireMenu(WireButton *wb) {
		Menu *menu = createMenu();
		MenuLabel *label = new MenuLabel();
		label->text = "Color: " + color::toHexString(wb->color);
		menu->addChild(label);
		WireParamField *paramField = new WireParamField();
		paramField->box.size.x = 100;
		paramField->setText(color::toHexString(wb->color));
		paramField->changeHandler = [=](std::string text) {
			if (wb->setColor(text)) {
				this->saveSettings();
			};
		};
		menu->addChild(paramField);
		menu->addChild(new MenuLabel());
		
		EventWidgetMenuItem *ed = new EventWidgetMenuItem();
		ed->text = "Edit...";
		ed->clickHandler = [=]() {
			this->editDialog(wb);
		};
		menu->addChild(ed);
		if (!isFirst(wb)) {
			EventWidgetMenuItem *mu = new EventWidgetMenuItem();
			mu->text = "Move Up";
			mu->clickHandler = [=]() {
				this->swap(wb->box.pos.y / 21 - 1);
			};
			menu->addChild(mu);
		}
		if (!isLast(wb)) {
			EventWidgetMenuItem *md = new EventWidgetMenuItem();
			md->text = "Move Down";
			md->clickHandler = [=]() {
				this->swap(wb->box.pos.y / 21);
			};
			menu->addChild(md);
		}
		EventWidgetMenuItem *dm = new EventWidgetMenuItem();
		dm->text = "Delete Wire...";
		dm->clickHandler = [=]() {
			this->deleteDialog(wb);
		};
		menu->addChild(dm);
	}
	void deleteWire(WireButton *wb) {
		NVGcolor color = wb->color;
		bool selected = wb->checkBox->selected;
		unsigned int index = wb->index();
		scrollWidget->container->removeChild(wb);
		delete wb;
		reflow();
		saveSettings();
		APP->history->push(new EventWidgetAction(
			"Delete Color",
			[color, selected, index]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					if (wm) {
						wm->insertColor(color, selected, index);
						wm->saveSettings();
					}
				}
			},
			[index] {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					if (wm) {
						WireButton *wb = wm->findWireButton(index);
						if (wb) {
							wm->scrollWidget->container->removeChild(wb);
							delete wb;
							wm->reflow();
							wm->saveSettings();
						}
					}
				}
			}
		));
	}
	void reflow() {
		unsigned int y = 0;
		for (Widget *widget : scrollWidget->container->children) {
			WireButton *wb = dynamic_cast<WireButton *>(widget);
			wb->box.pos.y = y;
			y += 21;
		}
		
	}
	void swap(int i) {
		if (i < 0) return;
		if (i > (int)(scrollWidget->container->children.size()) - 2) return;
		auto vi = std::begin(scrollWidget->container->children);
		std::advance(vi, i);
		WireButton *wb1 = dynamic_cast<WireButton *>(* vi++);
		WireButton *wb2 = dynamic_cast<WireButton *>(* vi);
		NVGcolor col = wb1->color;
		wb1->color = wb2->color;
		wb2->color = col;
		bool sel = wb1->checkBox->selected;
		wb1->checkBox->selected = wb2->checkBox->selected;
		wb2->checkBox->selected = sel;
		saveSettings();
	}
	void deleteDialog(WireButton *wb) {
		deleteLabel->label = "Delete Color?";
		deleteOkButton->clickHandler = [=]() {
			this->deleteOk(wb);
		};
		backPanel->visible = false;
		deleteConfirmPanel->visible = true;
	}
	void recolorAllDialog() {
		deleteLabel->label = "Recolor All Wires?";
		deleteOkButton->clickHandler = [=]() {
			this->cableCount = -1;
			this->cancel();
		};
		backPanel->visible = false;
		deleteConfirmPanel->visible = true;
	}
	void cancel() {
		backPanel->visible = true;
		editPanel->visible = false;
		settingsPanel->visible = false;
		deleteConfirmPanel->visible = false;
	}
	void deleteOk(WireButton *wb) {
		this->deleteWire(wb);
		cancel();
	}
	void settingsDialog() {
		varyH->defaultValue = varyH->value;
		varyS->defaultValue = varyS->value;
		varyL->defaultValue = varyL->value;
		highlightSlider->defaultValue = highlightSlider->value;
		backPanel->visible = false;
		settingsPanel->visible = true;
	}
	void editAdd(NVGcolor col) {
		addColor(col, false);
		unsigned int index = scrollWidget->container->children.size() - 1;
		APP->history->push(new EventWidgetAction(
			"Add Color",
			[index]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					WireButton *wb = wm->findWireButton(index);
					if (wb) {
						wm->scrollWidget->container->removeChild(wb);
						delete wb;
						wm->saveSettings();
					}
				}
			},
			[index, col]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->insertColor(col, false, index);
					wm->saveSettings();
				}
			}
		));
	}
	void editEdit(WireButton *wb, NVGcolor col) {
		NVGcolor oldCol = wb->color;
		unsigned int index = wb->index();
		wb->color = col;
		APP->history->push(new EventWidgetAction(
			"Edit Color",
			[index, oldCol]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					WireButton *wb = wm->findWireButton(index);
					if (wb) {
						wb->color = oldCol;
						wm->saveSettings();
					}
				}
			},
			[index, col]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					WireButton *wb = wm->findWireButton(index);
					if (wb) {
						wb->color = col;
						wm->saveSettings();
					}
				}
			}
		));
	}
	void editDialog(WireButton *wb) {
		backPanel->visible = false;
		editPanel->completeHandler = [=](NVGcolor col) {
			if (wb) {
				editEdit(wb, col);
			}
			else {
				editAdd(col);
			}
			saveSettings();
			cancel();
		};
		if (wb) {
			editPanel->r->value = wb->color.r;
			editPanel->g->value = wb->color.g;
			editPanel->b->value = wb->color.b;
		}
		else {
			editPanel->r->value = 0.5f;
			editPanel->g->value = 0.5f;
			editPanel->b->value = 0.5f;
		}
		editPanel->visible = true;
	}
	void highlightChangedCore(int value) {
		highlight = value;
		highlightOff->selected = false;
		highlightLow->selected = false;
		highlightOn->selected = false;
		switch (value) {
			case HIGHLIGHT_OFF:
			highlightOff->selected = true;
			break;
			
			case HIGHLIGHT_LOW:
			highlightLow->selected = true;
			break;
		
			case HIGHLIGHT_ON:
			highlightOn->selected = true;
			break;
		}
		highlightIsDirty = true;
	}
	void highlightChanged(int value) {
		int oldValue = highlight;
		highlightChangedCore(value);
		APP->history->push(new EventWidgetAction(
			"Highlight Mode Changed",
			[oldValue]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->highlightChangedCore(oldValue);
					wm->saveSettings();
				}
			},
			[value]() {
				if (masterWireManager) {
					WM101 *wm = dynamic_cast<WM101 *>(masterWireManager);
					wm->highlightChangedCore(value);
					wm->saveSettings();
				}
			}
		));
	}
	void _delete() {
		if (masterWireManager != this)
			return;
		masterWireManager = NULL;
		if (!stabilized)
			return;
		if (highlight) {
			highlight = HIGHLIGHT_OFF;
			highlightIsDirty = true;
			highlightWires();
		}
	}
};

Model *modelWM101 = createModel<Module, WM101>("WM-101");
