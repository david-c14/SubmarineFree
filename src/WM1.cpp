//SubTag W10
#include <settings.hpp>
#include <system.hpp>
#include <osdialog.h>
#include "SubmarineFree.hpp"

struct BackPanel : Widget {
	Widget *cancelTargetWidget;
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

struct CollectionButton : EventWidgetButtonBase {
	void draw (const DrawArgs &args) override {
		nvgStrokeColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
		nvgStrokeWidth(args.vg, 2);
		nvgFillColor(args.vg, nvgRGB(0,0,0));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 1, 1, box.size.x / 2 - 1, box.size.y / 2 - 1);
		nvgFill(args.vg);
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, box.size.x * 0.25 + 1 , box.size.y * 0.25 + 1, box.size.x / 2 - 1, box.size.y / 2 - 1);
		nvgFill(args.vg);
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, box.size.x * 0.5 + 1, box.size.y * 0.5 + 1, box.size.x / 2 - 1, box.size.y / 2 - 1);
		nvgFill(args.vg);
		nvgStroke(args.vg);

		OpaqueWidget::draw(args);
	}
};

struct WirePanel : BackPanel {
	NVGcolor color;
	std::function<void()> cancelHandler;
	WirePanel() {	
		EventWidgetLabel *label = new EventWidgetLabel();
		label->label = "Color one wire";
		label->box.pos = Vec(15, 200);
		label->box.size = Vec(100, 19);
		addChild(label);
		EventWidgetButton *cancelButton = new EventWidgetButton();
		cancelButton->box.pos = Vec(35, 250);
		cancelButton->box.size = Vec(60, 19);
		cancelButton->label = "Cancel";
		cancelButton->clickHandler = [=](){
			if (this->cancelHandler) {
				this->cancelHandler();
			}
		};
		addChild(cancelButton);
	}
	void draw(const DrawArgs &args) override {
		BackPanel::draw(args);
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
	}
};

struct EditPanel : BackPanel {
	std::function<void(NVGcolor, std::string)> completeHandler;
	std::function<void()> cancelHandler;
	EventWidgetSlider *r;
	EventWidgetSlider *g;
	EventWidgetSlider *b;
	Label *labelLabel;
	EventParamField *labelField;
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
		labelLabel = new Label();
		labelLabel->box.pos = Vec(0, 223);
		labelLabel->text = "Color label";
		addChild(labelLabel);
		labelField = new EventParamField();
		labelField->box.pos = Vec(7, 243);
		labelField->box.size.x = 116;
		// postpone call to addChild, or this will capture all keystrokes!

		EventWidgetButton *saveButton = new EventWidgetButton();
		saveButton->box.pos = Vec(5, 275);
		saveButton->box.size = Vec(55, 19);
		saveButton->label = "Save";
		saveButton->clickHandler = [=](){
			if (this->completeHandler) {
				this->completeHandler(nvgRGBf(r->value, g->value, b->value), labelField->text);
			}
		};
		addChild(saveButton);
		
		EventWidgetButton *cancelButton = new EventWidgetButton();
		cancelButton->box.pos = Vec(70, 275);
		cancelButton->box.size = Vec(55, 19);
		cancelButton->label = "Cancel";
		cancelButton->clickHandler = [=](){
			if (this->cancelHandler) {
				this->cancelHandler();
			}
		};
		addChild(cancelButton);
	}
	~EditPanel() {
		if (labelField) {
			if (!visible) {
				labelField->clearChildren();
				labelField->parent = NULL;
				delete labelField;
			}
		}
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

struct BillboardPanel : BackPanel {
	std::function<void(const DrawArgs)> drawHandler;

	BillboardPanel() {
		// warning if there are no current colors
		EventWidgetLabel *label = new EventWidgetLabel();
		label->label = "No Wire Colors Defined";
		label->box.pos = Vec(25, 175);
		label->box.size = Vec(100, 19);
		addChild(label);
	}
	void draw(const DrawArgs &args) override {
		BackPanel::draw(args);
		if (drawHandler) {
			drawHandler(args);
		}
	}
};

struct ViewToggleButton : EventWidgetButtonBase {
	// special click behavior, drawing options mimic MinButton
	SizeableModuleWidget *mw;
	ViewToggleButton() {
		this->box.size = Vec(8, 14);
	}
	void draw (const DrawArgs &args) override {
		nvgBeginPath(args.vg);
		if (this->box.pos.x == 1) {
			nvgMoveTo(args.vg, 8, 0);
			nvgLineTo(args.vg, 0, 7);
			nvgLineTo(args.vg, 8, 14);
			nvgClosePath(args.vg);
		} else {
			nvgMoveTo(args.vg, 0, 0);
			nvgLineTo(args.vg, 8, 7);
			nvgLineTo(args.vg, 0, 14);
			nvgClosePath(args.vg);
		}
		nvgFillColor(args.vg, gScheme.getAlternative(mw->module));
		nvgFill(args.vg);
		EventWidgetButtonBase::draw(args);
	};
};

struct WireButton : EventWidgetButtonBase {
	NVGcolor color;
	std::string label;
	EventWidgetCheckBox *checkBox;
	// add a dedicated tooltip to show color descriptions on rollover
	ui::Tooltip* tooltip = NULL;

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
	bool setLabel(std::string text) {
		label = text;
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
	void onEnter(const event::Enter &e) override {
		if (!tooltip && (this->label != "")) {
			SubTooltip *stt = new SubTooltip();
			tooltip = stt;
			APP->scene->addChild(tooltip);
			stt->stepLambda = [=]() {
				tooltip->text = std::string(this->label);
				stt->Tooltip::step();
				tooltip->box.pos = this->getAbsoluteOffset(this->box.size).minus(this->box.size).plus(Vec(100, 0)).round();
			};
		}
		EventWidgetButtonBase::onEnter(e);
		e.consume(this);
	}
	void onLeave(const event::Leave &e) override {
		if (tooltip) {
			if (tooltip->parent == APP->scene) {
				APP->scene->removeChild(tooltip);
			}
			delete tooltip;
			tooltip = NULL;
		}
		EventWidgetButtonBase::onLeave(e);
		e.consume(this);
	}
};

struct ColorCollectionButton : EventWidgetButtonBase {
	std::string name;
	std::vector<NVGcolor> colors;
	std::vector<std::string> labels;
	// add a dedicated tooltip to show color descriptions on rollover
	ui::Tooltip* tooltip;
	float tooltipX = 0;
	std::string foundLabel = "";
	
	void draw(const DrawArgs &args) override {
		if (!name.empty()) {
			nvgFillColor(args.vg, nvgRGBf(1.0f, 1.0f, 1.0f));
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 13);	
			nvgTextAlign(args.vg, NVG_ALIGN_BOTTOM);
			nvgText(args.vg, 2, 15, name.c_str(), NULL);
		}
		float width = box.size.x - 25.0f;
		width = width / colors.size();
		float left = 0.0f;
		for (NVGcolor color : colors) {
			NVGcolor col = color;
			col.a = 1.0f;
			nvgBeginPath(args.vg);
			nvgRect(args.vg, left, 15, width, 8);
			nvgFillColor(args.vg, col);
			nvgFill(args.vg);
			left += width;
		}
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

	void onEnter(const event::Enter &e) override {
		OpaqueWidget::onEnter(e);
		e.consume(this);
	}
	void onHover(const event::Hover &e) override {
		float mouseX = e.pos.x;  // relative to this collection button
		if (!tooltip) {
			SubTooltip *stt = new SubTooltip();
			tooltip = stt;
			APP->scene->addChild(tooltip);
			stt->stepLambda = [=]() {
				tooltip->text = foundLabel;
				stt->Tooltip::step();
				// place the tooltip just above the target color's block
				tooltip->box.pos = this->getAbsoluteOffset(this->box.size).minus(this->box.size).plus(Vec(tooltipX, -6)).round();
			};
		}
		// adapt logic above to show the description for the current target color chip
		float width = box.size.x - 25.0f;
		width = width / colors.size();
		int colorIndex = floor(mouseX / width);
		if (colorIndex < int(labels.size())) {
			foundLabel = std::string(labels[colorIndex]);
			tooltipX = (width * colorIndex);
		} else {
			foundLabel = "";
		}
		tooltip->visible = !(foundLabel == "");
		OpaqueWidget::onHover(e);
		e.consume(this);
	}
	void onLeave(const event::Leave &e) override {
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = NULL;
		}
		OpaqueWidget::onLeave(e);
		e.consume(this);
	}
};

struct WM_Base {
	virtual void loadCollectionFromDisk(std::string path) {
	}
	virtual ColorCollectionButton *addCollection(std::string name, std::vector<NVGcolor> colors, std::vector<std::string> labels) {
		return NULL;
	}
	void loadCollectionDialog() {
		std::string dir = asset::user("SubmarineFree");
		system::createDirectory(dir);
		std::string filename = "";
		
		osdialog_filters* filters = osdialog_filters_parse("Submarine Wire Manager Collection(.wmCollection):wmCollection");
		DEFER({
			osdialog_filters_free(filters);
		});

		char* pathC = osdialog_file(OSDIALOG_OPEN, dir.c_str(), filename.c_str(), filters);
		if (!pathC) {
			// Fail silently
			return;
		}
		DEFER({
			std::free(pathC);
		});

		loadCollectionFromDisk(pathC);
	}
	ColorCollectionButton *loadCollectionFromJson(json_t *json) {
		json_t *n1 = json_object_get(json, "name");	
		json_t *a1 = json_object_get(json, "colors");
		json_t *a2 = json_object_get(json, "labels");
		ColorCollectionButton *cb = NULL;
		int asize, j;
		std::vector<NVGcolor> colors;
		std::vector<std::string> labels;
		if (a1) {
			asize = json_array_size(a1);
			for (j = 0; j < asize; j++) {
				json_t *c1 = json_array_get(a1, j);
				if (c1) {
					colors.push_back(color::fromHexString(json_string_value(c1)));
				}
			}
		}	
		if (a2) {
			asize = json_array_size(a2);
			for (j = 0; j < asize; j++) {
				json_t *l1 = json_array_get(a2, j);
				if (l1) {
					labels.push_back(json_string_value(l1));
				} else {
					labels.push_back("");
				}
			}
		}
		cb = addCollection(n1?json_string_value(n1):"[Unnamed]", colors, labels);
		return cb;
	}
	void drawBillboardBase(NVGcontext *vg, rack::math::Rect box, std::vector<NVGcolor> currentColors, std::vector<std::string> currentLabels, bool draw3d) {
		float blockHeight = box.size.y / currentColors.size();
		float stop1 = blockHeight * 0.10f;
		float stop2 = blockHeight * 0.28f;
		float stop3 = blockHeight * 0.46f;
		float currentBlockTop = 0.0f;
		float shadowGray = 0.0f;
		for(unsigned int i = 0; i < currentColors.size(); i++) {
			// draw a block of the n-th wire color
			NVGcolor col = currentColors[i];
			col.a = 1.0f;  // make sure it's opaque here!
			NVGcolor shadowCol = nvgLerpRGBA(col, nvgRGBf(0.0f, 0.0f, 0.0f), 0.9f);
			NVGcolor highlightCol = nvgLerpRGBA(col, nvgRGBf(1.0f, 1.0f, 1.0f), 0.55f);
			if (draw3d) {
				float stopPos1 = currentBlockTop + stop1;
				float stopPos2 = currentBlockTop + stop2;
				float stopPos3 = currentBlockTop + stop3;
				float end = currentBlockTop + blockHeight;

				nvgBeginPath(vg);
				nvgRect(vg, 0.0f, currentBlockTop, box.size.x, blockHeight);
				NVGpaint grad = nvgLinearGradient(vg, 0, currentBlockTop, 0, stopPos1, shadowCol, col);
				nvgFillPaint(vg, grad);
				nvgFill(vg);

				nvgBeginPath(vg);
				nvgRect(vg, 0.0f, stopPos1, box.size.x, blockHeight - stop1);
				grad = nvgLinearGradient(vg, 0, stopPos1, 0, stopPos2, col, highlightCol);
				nvgFillPaint(vg, grad);
				nvgFill(vg);

				nvgBeginPath(vg);
				nvgRect(vg, 0.0f, stopPos2, box.size.x, blockHeight - stop2);
				grad = nvgLinearGradient(vg, 0, stopPos2, 0, stopPos3, highlightCol, col);
				nvgFillPaint(vg, grad);
				nvgFill(vg);

				nvgBeginPath(vg);
				nvgRect(vg, 0.0f, stopPos3, box.size.x, blockHeight - stop3);
				grad = nvgLinearGradient(vg, 0, stopPos3, 0, end, col, shadowCol);
				nvgFillPaint(vg, grad);
				nvgFill(vg);
			}
			else {
				nvgBeginPath(vg);
				nvgRect(vg, 0.0f, currentBlockTop, box.size.x, blockHeight);
				nvgFillColor(vg, col);
				nvgFill(vg);
			}
			currentBlockTop += blockHeight;
		}
		currentBlockTop = 0.0f;
		for(unsigned int i = 0; i < currentColors.size(); i++) {
			// add a big high-contrast label for this block (if labeled)
			std::string label = "";
			if (i < currentLabels.size()) {
				 label = currentLabels[i];
			}
			if (!label.empty()) {
				float labelY = currentBlockTop + (blockHeight/2.0f);
				nvgFontFaceId(vg, APP->window->uiFont->handle);
				nvgFontSize(vg, 24);
				nvgTextAlign(vg, NVG_ALIGN_MIDDLE);
				// add a drop shadow to work on all backgrounds
				nvgFillColor(vg, nvgRGBf(shadowGray, shadowGray, shadowGray));
				nvgFontBlur(vg, 1.15f);
				nvgText(vg, 5, labelY, label.c_str(), NULL);
				// add white text over this
				nvgFontBlur(vg, 0.0f);
				nvgFillColor(vg, nvgRGBf(1.0f, 1.0f, 1.0f));
				nvgText(vg, 5, labelY, label.c_str(), NULL);
			}
			currentBlockTop += blockHeight;
		}
	}
};

struct WM101;

WM101 *masterWireManager = NULL;

struct WM101 : SizeableModuleWidget, WM_Base {

	enum {
		HIGHLIGHT_OFF,
		HIGHLIGHT_LOW,
		HIGHLIGHT_ON
	};
	int highlight = HIGHLIGHT_OFF;
	ModuleWidget *lastHover = NULL;
	bool highlightIsDirty = true;

	int cableCount = 0;
	Widget *lastCable = NULL;
	unsigned int newColorIndex = 0;

	MinButton *minButton;
	ViewToggleButton *viewToggle;
	BackPanel *backPanel;
	EventWidgetCheckBox *checkBoxAll;
	BackPanel *deleteConfirmPanel;
	EventWidgetLabel *deleteLabel;
	EventWidgetButton *deleteCancelButton;
	EventWidgetButton *deleteOkButton;
	EditPanel *editPanel;
	WirePanel *wirePanel;
	BillboardPanel *billboardPanel;
	BackPanel *settingsPanel;
	EventWidgetRadioButton *highlightOff;
	EventWidgetRadioButton *highlightLow;
	EventWidgetRadioButton *highlightOn;
	EventWidgetSlider *highlightSlider;
	EventWidgetCheckBox *varyCheck;
	EventWidgetSlider *varyH;
	EventWidgetSlider *varyS;
	EventWidgetSlider *varyL;
	EventWidgetCheckBox *redoCheck;
	EventWidgetCheckBox *billboard3d;
	BackPanel *blockingPanel;
	BackPanel *collectionPanel;
	ScrollWidget *collectionScrollWidget;
	
	ScrollWidget *scrollWidget;
	WM101(Module *module) : SizeableModuleWidget(150) {
		setModule(module);
		minButton = new MinButton();
		minButton->box.pos = Vec(140,160);
		minButton->mw = this;
		addChild(minButton); 

		viewToggle = new ViewToggleButton();
		viewToggle->box.pos = Vec(141, 1);
		viewToggle->mw = this;
		viewToggle->clickHandler = [=]() {
			toggleBillboard();
		};
		addChild(viewToggle); 

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
		addButton->box.pos = Vec(38, 2);
		addButton->box.size = Vec(16, 16);
		addButton->clickHandler = [=]() {
			this->editDialog(NULL);
		};
		backPanel->addChild(addButton);
		CollectionButton *collectionButton = new CollectionButton();
		collectionButton->box.pos = Vec(75, 2);
		collectionButton->box.size = Vec(16, 16);
		collectionButton->clickHandler = [=]() {
			this->collectionsDialog();
		};
		backPanel->addChild(collectionButton);
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

		wirePanel = new WirePanel();
		wirePanel->box.pos = backPanel->box.pos;
		wirePanel->box.size = backPanel->box.size;
		wirePanel->visible = false;
		wirePanel->cancelHandler = [=]() {
			this->cancel();
		};
		addChild(wirePanel);

		billboardPanel = new BillboardPanel();
		billboardPanel->box.pos = Vec(0, 15);
		billboardPanel->box.size = Vec(box.size.x, box.size.y - 30);
		billboardPanel->visible = false;
		billboardPanel->drawHandler = [=](const DrawArgs &args) { 
			this->drawBillboard(args);
		};
		addChild(billboardPanel);

		collectionPanel = new BackPanel();
		collectionPanel->box.pos = backPanel->box.pos;
		collectionPanel->box.size = backPanel->box.size;
		collectionPanel->visible = false;
		addChild(collectionPanel);

		collectionScrollWidget = new ScrollWidget();
		collectionScrollWidget->box.pos = Vec(0,0);
		collectionScrollWidget->box.size = Vec(backPanel->box.size.x, backPanel->box.size.y - 25);
		collectionPanel->addChild(collectionScrollWidget);
	
		EventWidgetButton *collectionCloseButton = new EventWidgetButton();
		collectionCloseButton->box.pos = Vec(35, 325);
		collectionCloseButton->box.size = Vec(60, 19);
		collectionCloseButton->label = "Close";
		collectionCloseButton->clickHandler = [=]() { this->cancel(); };
		collectionPanel->addChild(collectionCloseButton);
		
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
						masterWireManager->varyH->value = oldValue;
						masterWireManager->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						masterWireManager->varyH->value = value;
						masterWireManager->saveSettings();
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
						masterWireManager->varyS->value = oldValue;
						masterWireManager->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						masterWireManager->varyS->value = value;
						masterWireManager->saveSettings();
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
						masterWireManager->varyL->value = oldValue;
						masterWireManager->saveSettings();
					}
				},
				[value]() {
					if (masterWireManager) {
						masterWireManager->varyL->value = value;
						masterWireManager->saveSettings();
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
						masterWireManager->highlightSlider->value = oldValue;
						masterWireManager->saveSettings();
						masterWireManager->highlightIsDirty = true;
					}
				},
				[value]() {
					if (masterWireManager) {
						masterWireManager->highlightSlider->value = value;
						masterWireManager->saveSettings();
						masterWireManager->highlightIsDirty = true;
					}
				}
			));
		};
		settingsPanel->addChild(highlightSlider);

		redoCheck = new EventWidgetCheckBox();
		redoCheck->label = "Redo colors?";
		redoCheck->box.pos = Vec(10, 220);
		redoCheck->box.size = Vec(box.size.x - 40, 19);
		redoCheck->changeHandler = [=]() { this->redoCheckChanged(); };
		settingsPanel->addChild(redoCheck);

		billboard3d = new EventWidgetCheckBox();
		billboard3d->label = "3D billboard?";
		billboard3d->box.pos = Vec(10, 255);
		billboard3d->box.size = Vec(box.size.x - 40, 19);
		billboard3d->selected = 1;
		billboard3d->changeHandler = [=]() { this->billboard3dChanged(); };
		settingsPanel->addChild(billboard3d);

		EventWidgetButton *settingsButton = new EventWidgetButton();
		settingsButton->box.pos = Vec(35, 290);
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

		if (!module)
			loadSettings();
	}
	~WM101() {
		this->_delete();
	}
	void onHoverKey(const event::HoverKey &e) override {
		ModuleWidget::onHoverKey(e);
		if (e.isConsumed())
			return;
		if (e.action == GLFW_PRESS) {
			if ((e.key >= GLFW_KEY_F1) && (e.key <= GLFW_KEY_F25)) {
				if ((e.mods & RACK_MOD_MASK) == RACK_MOD_CTRL) {
					ColorCollectionButton *collection = findCollectionButton(e.key - GLFW_KEY_F1);	
					if (collection) {
						applyCollection(collection);
					}
					e.consume(this);
				}
				if ((e.mods & RACK_MOD_MASK) == GLFW_MOD_SHIFT) {
					WireButton *button = findWireButton(e.key - GLFW_KEY_F1);
					if (button) {
						button->checkBox->selected = !button->checkBox->selected;
						updateWireButtonCheckBox(button);
					}
					e.consume(this);
				}
			}
		}
		e.consume(this);
	}
	void varyCheckChanged() {
		bool selected = varyCheck->selected;
		saveSettings();
		APP->history->push(new EventWidgetAction(
			selected?"Select Variations":"Deselect Variations",
			[selected]() {
				if (masterWireManager) {
					masterWireManager->varyCheck->selected = !selected;
					masterWireManager->saveSettings();
				}
			},
			[selected]() {
				if (masterWireManager) {
					masterWireManager->varyCheck->selected = selected;
					masterWireManager->saveSettings();
				}
			}
		));
	}
	void redoCheckChanged() {
		bool selected = redoCheck->selected;
		saveSettings();
		APP->history->push(new EventWidgetAction(
			selected?"Select Redo colors":"Deselect Redo colors",
			[selected]() {
				if (masterWireManager) {
					masterWireManager->redoCheck->selected = !selected;
					masterWireManager->saveSettings();
				}
			},
			[selected]() {
				if (masterWireManager) {
					masterWireManager->redoCheck->selected = selected;
					masterWireManager->saveSettings();
				}
			}
		));
	}
	void billboard3dChanged() {
		bool selected = billboard3d->selected;
		saveSettings();
		APP->history->push(new EventWidgetAction(
			selected?"Turn on 3D billboard":"Turn off 3D billboard",
			[selected]() {
				if (masterWireManager) {
					masterWireManager->billboard3d->selected = !selected;
					masterWireManager->saveSettings();
				}
			},
			[selected]() {
				if (masterWireManager) {
					masterWireManager->billboard3d->selected = selected;
					masterWireManager->saveSettings();
				}
			}
		));
	}

	void drawBillboard(const DrawArgs &args) {
		std::vector<NVGcolor> currentColors = currentCollectionColors();
		std::vector<std::string> currentLabels = currentCollectionLabels();
		drawBillboardBase(args.vg, billboardPanel->box, currentCollectionColors(), currentCollectionLabels(), billboard3d->selected);
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
		viewToggle->visible = true;
		masterWireManager = this;
		scrollWidget->container->clearChildren();
		collectionScrollWidget->container->clearChildren();
		loadSettings();
	}
	void step() override {
		if (!module) {
			return;
		}
		if (!stabilized) {
			stabilized = true;
			cableCount = APP->scene->rack->cableContainer->children.size();
		}
		if (masterWireManager != this) {
			if (masterWireManager) {
				hidePanels();
				blockingPanel->visible = (box.size.x > 16.0f);
				viewToggle->visible = false;
				SizeableModuleWidget::step();
				return;
			}
			takeMasterSlot();
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
		if (wirePanel->visible && APP->scene->rack->incompleteCable) {
			colorCable(APP->scene->rack->incompleteCable,NULL);
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
		if (cable->cable->id > -1 && !complex && redoCheck->selected && !wirePanel->visible)
			return;
		NVGcolor oldColor = cable->color;
		if (wirePanel->visible) {
			cable->color = wirePanel->color;
			cancel();
		}
		else {
			cable->color = findColor(cable->color);
		}
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
		minButton->box.pos.y = small?180:160;
		hidePanels();
		backPanel->visible = !small;
		viewToggle->visible = !small;
		SizeableModuleWidget::onResize();	
	}
	void toggleBillboard() {
		// assumes we're not already minimized (toggle not available)
		bool alreadyShowingBillboard = billboardPanel->visible;
		if (alreadyShowingBillboard) {
			hidePanels();
			backPanel->visible = true;
		} else {
			showBillboard();
		}
	}
	EventWidgetAction *checkBoxAction(unsigned int index, bool selected) {
		return new EventWidgetAction(
				selected?"Select Color":"Deselect Color",	
				[index, selected]() {
					if (masterWireManager) {
						WireButton *wb = masterWireManager->findWireButton(index);
						if (wb) {
							wb->checkBox->selected = !selected;
							masterWireManager->saveSettings();
						}
					}
				},
				[index, selected]() {
					if (masterWireManager) {
						WireButton *wb = masterWireManager->findWireButton(index);
						if (wb) {
							wb->checkBox->selected = selected;
							masterWireManager->saveSettings();
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
	void selectWirePanel(NVGcolor color) {
		backPanel->visible = false;
		wirePanel->visible = true;
		wirePanel->color = color;
	}
	WireButton *addColor(NVGcolor color, std::string label, bool selected) {
		float y = scrollWidget->container->children.size() * 21;
		WireButton *wb = new WireButton();
		wb->box.pos = Vec(0, y);
		wb->box.size = Vec(scrollWidget->box.size.x, 21);
		wb->color = color;
		wb->label = label;
		wb->checkBox->selected = selected;
		wb->checkBox->changeHandler = [=]() {
			this->updateWireButtonCheckBox(wb);
		};
		wb->rightClickHandler = [=]() {
			this->addWireMenu(wb);
		};
		wb->doubleClickHandler = [=]() {
			this->selectWirePanel(wb->color);
		};
		scrollWidget->container->addChild(wb);
		return wb;
	}
	ColorCollectionButton *addCollection(std::string name, std::vector<NVGcolor> colors, std::vector<std::string> labels) override {
		float y = collectionScrollWidget->container->children.size() * 24;
		ColorCollectionButton *btn = new ColorCollectionButton();
		btn->box.pos = Vec(0, y);
		btn->box.size = Vec(collectionScrollWidget->box.size.x, 24);
		btn->name = name;
		btn->colors = colors;
		btn->labels = labels;
		btn->rightClickHandler = [=]() {
			this->addCollectionMenu(btn);
		};
		btn->doubleClickHandler = [=]() {
			this->applyCollection(btn);
		};
		collectionScrollWidget->container->addChild(btn);
		return btn;
	}
	std::vector<NVGcolor> currentCollectionColors() {
		std::vector<NVGcolor> colors;
		for (Widget *w : scrollWidget->container->children) {
			WireButton *wb = dynamic_cast<WireButton *>(w);
			NVGcolor col = wb->color;
			col.a = wb->checkBox->selected?1.0f:0.0f;
			colors.push_back(col);
		}
		return colors;
	}
	std::vector<std::string> currentCollectionLabels() {
		std::vector<std::string> labels;
		for (Widget *w : scrollWidget->container->children) {
			WireButton *wb = dynamic_cast<WireButton *>(w);
			std::string lab = wb->label;
			labels.push_back(lab);
		}
		return labels;
	}
	void applyCollection(std::vector<NVGcolor> colors, std::vector<std::string> labels) {
		scrollWidget->container->clearChildren();
		for(unsigned int i = 0; i < colors.size(); i++) {
			NVGcolor color = colors[i];
			std::string label = "";
			if (i < labels.size()) {
				 label = labels[i];
			}
			bool selected = (color.a > 0.5f);
			color.a = 1.0f;
			addColor(color, label, selected);
		}
	}
	void applyCollection(ColorCollectionButton *btn) {
		std::vector<NVGcolor> oldColors = currentCollectionColors();
		std::vector<std::string> oldLabels = currentCollectionLabels();
		std::vector<NVGcolor> newColors = btn->colors;
		std::vector<std::string> newLabels = btn->labels;
		applyCollection(newColors, newLabels);
		saveSettings();
		this->cancel();
		APP->history->push(new EventWidgetAction(
			"Use Collection",
			[oldColors, oldLabels]() {
				if (masterWireManager) {
					masterWireManager->applyCollection(oldColors, oldLabels);
					masterWireManager->saveSettings();
				}
			},
			[newColors, newLabels]() {
				if (masterWireManager) {
					masterWireManager->applyCollection(newColors, newLabels);
					masterWireManager->saveSettings();
				}
			}
		));
	}
	void insertCollection(std::string name, std::vector<NVGcolor> colors, std::vector<std::string> labels, unsigned int index) {
		ColorCollectionButton *w = addCollection(name, colors, labels);
		collectionScrollWidget->container->children.pop_back();
		auto v = collectionScrollWidget->container->children.begin();
		std::advance(v, index);
		collectionScrollWidget->container->children.insert(v, w);
		this->reflowCollections();
	}
	void insertColor(NVGcolor color, std::string label, bool selected, unsigned int index) {
		WireButton *w = addColor(color, label, selected);
		scrollWidget->container->children.pop_back();
		auto v = scrollWidget->container->children.begin();
		std::advance(v, index);
		scrollWidget->container->children.insert(v, w);
		this->reflow();
	}
	void setDefaults() {
		// add some useful color conventions

		// Omri Cohen's colors as shown here  <https://github.com/david-c14/ModularFungi/blob/master/res/Colors.png>
		scrollWidget->container->clearChildren();
		addColor(color::fromHexString("#c91847"), "Audio", 		true);  // audio
		addColor(color::fromHexString("#0986ad"), "Clk/Trig/Gate", 	false);  // clock/trigger/gate
		addColor(color::fromHexString("#c9b70e"), "Volt/Oct", 		false);  // volt/octave
		addColor(color::fromHexString("#0c8e15"), "Modulation", 	false);  // modulation
		addCollection(std::string("Modular Fungi"), currentCollectionColors(), currentCollectionLabels());

		// jack color conventions used in NYSTHI modules  <https://github.com/patman023/nysthimanual/blob/master/pages/basics/basics.md>
		scrollWidget->container->clearChildren();
		addColor(color::fromHexString("#dddddd"), "Audio",		true);  // audio
		addColor(color::fromHexString("#3c82dc"), "CV",			false);  // control voltage
		addColor(color::fromHexString("#fad12d"), "Gate",		false);  // gate
		addColor(color::fromHexString("#dc7814"), "Pulse/Trigger",	false);  // pulse/trigger
		addColor(color::fromHexString("#800080"), "Sync",		false);  // sync
		addCollection(std::string("NYSTHI"), currentCollectionColors(), currentCollectionLabels());

		// jack color conventions used in TheXOR modules  <https://github.com/The-XOR/RackPlugins>
		scrollWidget->container->clearChildren();
		addColor(color::fromHexString("#ff0000"), "Clock",		true);  // clock
		addColor(color::fromHexString("#fffc0d"), "Reset",		false);  // reset
		addColor(color::fromHexString("#008000"), "CV",			false);  // control voltage
		addColor(color::fromHexString("#f1f1f1"), "Gate",		false);  // gate
		addColor(color::fromHexString("#000000"), "Modulation",		false);  // modulation
		addColor(color::fromHexString("#0000ff"), "Trigger",		false);  // trigger
		addColor(color::fromHexString("#ff5555"), "Expansion",		false);  // expansion
		addCollection(std::string("TheXOR"), currentCollectionColors(), currentCollectionLabels());

		// add default colors (will remain active)
		scrollWidget->container->clearChildren();
		for (NVGcolor color : settings::cableColors) {
			addColor(color, "", true);	// assume no labels for these colors
		}
		
		addColor(nvgRGB(0xff, 0xae, 0xc9), "", false);
		addColor(nvgRGB(0xb7, 0x00, 0xb5), "", false);
		addColor(nvgRGB(0x80, 0x80, 0x80), "", false);
		addColor(nvgRGB(0xff, 0xff, 0xff), "", false);
		addColor(nvgRGB(0x10, 0x0f, 0x12), "", false);
		addColor(nvgRGB(0xff, 0x99, 0x41), "", false);
		addColor(nvgRGB(0x80, 0x36, 0x10), "", false);
		addCollection(std::string("Default"), currentCollectionColors(), currentCollectionLabels());
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
					json_t *l1 = json_object_get(j1, "label");
					if (c1) {
						int selected = false;
						json_t *s1 = json_object_get(j1, "selected");
						if (s1) {
							selected = clamp((int)json_number_value(s1), 0, 1);
						}
						std::string label;
						label = (l1) ? json_string_value(l1) : "";
						addColor(color::fromHexString(json_string_value(c1)), label, selected);
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
		v1 = json_object_get(rootJ, "redo");
		if (v1) {
			redoCheck->selected = clamp((int)json_number_value(v1), 0, 1);
		}
		v1 = json_object_get(rootJ, "billboard");
		if (v1) {
			billboard3d->selected = clamp((int)json_number_value(v1), 0, 1);
		}
		arr = json_object_get(rootJ, "collections");
		if (arr) {
			collectionScrollWidget->container->clearChildren();
			int size = json_array_size(arr);
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					loadCollectionFromJson(j1);
				}
			}
			reflowCollections();
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
			json_object_set_new(color, "label", json_string(wb->label.c_str()));
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
		json_object_set_new(settings, "redo", json_real(redoCheck->selected));
		json_object_set_new(settings, "billboard", json_real(billboard3d->selected));
		arr = json_array();
		for (Widget *w : collectionScrollWidget->container->children) {
			ColorCollectionButton *cb = dynamic_cast<ColorCollectionButton *>(w);
			json_t *c1 = json_object();
			saveCollectionToJson(cb, c1);
			json_array_append_new(arr, c1);
		} 
		json_object_set_new(settings, "collections", arr);
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
	ColorCollectionButton *findCollectionButton(unsigned int index) {
		if (index >= collectionScrollWidget->container->children.size())
			return NULL;
		auto vi = collectionScrollWidget->container->children.begin();
		std::advance(vi, index);
		return dynamic_cast<ColorCollectionButton *>(*vi);
	}
	void checkAll(bool selected) {
		history::ComplexAction *complex = new history::ComplexAction();
		complex->name = selected?"Select All Colors":"Deselect All Colors"; 
		APP->history->push(complex);
		EventWidgetAction *allAction = new EventWidgetAction(
			complex->name,
			[selected]() {
				if (masterWireManager) {
					masterWireManager->checkBoxAll->selected = !selected;
				}
			},
			[selected]() {
				if (masterWireManager) {
					masterWireManager->checkBoxAll->selected = selected;
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

		EventWidgetMenuItem *rAll = new EventWidgetMenuItem();
		rAll->text = "Recolor All Wires...";
		rAll->clickHandler = [=]() {
			this->recolorAllDialog();
		};
		menu->addChild(rAll);

		EventWidgetMenuItem *add = new EventWidgetMenuItem();
		add->text = "Add New Color ...";
		add->clickHandler = [=]() {
			this->editDialog(NULL);	
		};
		menu->addChild(add);
		
		menu->addChild(new MenuSeparator);
	
		EventWidgetMenuItem *collections = new EventWidgetMenuItem();
		collections->text = "Collections...";
		collections->clickHandler = [=]() {
			this->collectionsDialog();
		};
		menu->addChild(collections);

		EventWidgetMenuItem *addColl = new EventWidgetMenuItem();
		addColl->text = "Save as Collection";
		addColl->clickHandler = [=]() {
			this->saveAsCollection();
		};
		menu->addChild(addColl);

		EventWidgetMenuItem *loadColl = new EventWidgetMenuItem();
		loadColl->text = "Load collection...";
		loadColl->clickHandler = [=]() {
			this->loadCollectionDialog();
		};
		menu->addChild(loadColl);

		menu->addChild(new MenuSeparator);
	
		EventWidgetMenuItem *settings = new EventWidgetMenuItem();
		settings->text = "Settings...";
		settings->clickHandler = [=]() {
			this->settingsDialog();
		};
		menu->addChild(settings);
	
		menu->addChild(new MenuSeparator);

		EventWidgetMenuItem *var = new EventWidgetMenuItem();
		var->text = "Variations";
		var->rightText = CHECKMARK(varyCheck->selected);
		var->clickHandler = [=]() {
			this->varyCheck->selected = !(this->varyCheck->selected);
			this->varyCheckChanged();
		};
		menu->addChild(var);

		EventWidgetMenuItem *hOff = new EventWidgetMenuItem();
		hOff->text = "Highlight Off";
		hOff->rightText = CHECKMARK(highlight == HIGHLIGHT_OFF);
		hOff->clickHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_OFF);
			saveSettings();
		};
		menu->addChild(hOff);

		EventWidgetMenuItem *hLow = new EventWidgetMenuItem();
		hLow->text = "Highlight When Hovering";
		hLow->rightText = CHECKMARK(highlight == HIGHLIGHT_LOW);
		hLow->clickHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_LOW);
			saveSettings();
		};
		menu->addChild(hLow);

		EventWidgetMenuItem *hOn = new EventWidgetMenuItem();
		hOn->text = "Highlight Always";
		hOn->rightText = CHECKMARK(highlight == HIGHLIGHT_ON);
		hOn->clickHandler = [=]() {
			this->highlightChanged(HIGHLIGHT_ON);
			saveSettings();
		};
		menu->addChild(hOn);

		menu->addChild(new MenuSeparator);

		EventWidgetMenuItem *redo = new EventWidgetMenuItem();
		redo->text = "Keep colors on redo";
		redo->rightText = CHECKMARK(redoCheck->selected);
		redo->clickHandler = [=]() {
			this->redoCheck->selected = !(this->redoCheck->selected);
			this->redoCheckChanged();
		};
		menu->addChild(redo);

		EventWidgetMenuItem *billboard = new EventWidgetMenuItem();
		billboard->text = "3D billboard";
		billboard->rightText = CHECKMARK(billboard3d->selected);
		billboard->clickHandler = [=]() {
			this->billboard3d->selected = !(this->billboard3d->selected);
			this->billboard3dChanged();
		};
		menu->addChild(billboard);
	}
	void addCollectionMenu(ColorCollectionButton *cb) {
		// by convention, place at the current mouse location
		addCollectionMenu(cb, false);
	}
	void addCollectionMenu(ColorCollectionButton *cb, bool forcePosition) {
		Menu *menu = createMenu();
		if (forcePosition) {
			// put the name field under the mouse
			menu->box.pos = APP->window->mousePos.minus(Vec(100, 12));
		}
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(cb->name);
		paramField->changeHandler = [=](std::string text) {
			this->changeCollectionName(cb, text);
		};
		menu->addChild(paramField);

		EventWidgetMenuItem *am = new EventWidgetMenuItem();
		am->text = "Use Collection";
		am->clickHandler = [=]() {
			this->applyCollection(cb);
		};
		unsigned int index = cb->index();
		if (index < 25) {
			char str[40];
			snprintf(str, 40, RACK_MOD_CTRL_NAME "-F%d", cb->index() + 1);
			am->rightText = str;
		}
		menu->addChild(am);
		
		if (cb->index() > 0) {
			EventWidgetMenuItem *um = new EventWidgetMenuItem();
			um->text = "Move Up";
			um->clickHandler = [=]() {
				this->swapCollection(cb, true);
			};
			menu->addChild(um);
		}

		if (cb->index() < collectionScrollWidget->container->children.size() - 1) {
			EventWidgetMenuItem *um = new EventWidgetMenuItem();
			um->text = "Move Down";
			um->clickHandler = [=]() {
				this->swapCollection(cb, false);
			};
			menu->addChild(um);
		}

		EventWidgetMenuItem *dm = new EventWidgetMenuItem();
		dm->text = "Delete Collection...";
		dm->clickHandler = [=]() {
			this->deleteCollectionDialog(cb);
		};
		menu->addChild(dm);

		EventWidgetMenuItem *sm = new EventWidgetMenuItem();
		sm->text = "Save collection...";
		sm->clickHandler = [=]() {
			this->saveCollectionDialog(cb);
		};
		menu->addChild(sm);
	}
	unsigned int swapCollectionCore(ColorCollectionButton *cb, bool up) {
		unsigned int index = cb->index();
		ColorCollectionButton *cb2 = findCollectionButton(index + (up?-1:+1));
		if (cb2) {
			std::string name = cb->name;
			cb->name = cb2->name;
			cb2->name = name;
			std::vector<NVGcolor> colors = cb->colors;
			cb->colors = cb2->colors;
			cb2->colors = colors;	
			std::vector<std::string> labels = cb->labels;
			cb->labels = cb2->labels;
			cb2->labels = labels;
		}
		saveSettings();
		return index;
	}
	void swapCollection(ColorCollectionButton *cb, bool up) {
		unsigned int index = swapCollectionCore(cb, up);
		APP->history->push(new EventWidgetAction(
			up?"Move Collection Up":"Move Collection Down",
			[index, up]() {
				if (masterWireManager) {
					ColorCollectionButton *btn = masterWireManager->findCollectionButton(index + (up?-1:+1));
					if (btn) {
						masterWireManager->swapCollectionCore(btn, !up);
					}
				}
			},
			[index, up]() {
				if (masterWireManager) {
					ColorCollectionButton *btn = masterWireManager->findCollectionButton(index);
					if (btn) {
						masterWireManager->swapCollectionCore(btn, up);
					}
				}
			}
		));
	}

	void loadCollectionFromDisk(std::string pathC) override {
		json_error_t error;
		FILE *file = fopen(pathC.c_str(), "r");
		if (!file) {
			return;
		}
		json_t *rootJ = json_loadf(file, 0, &error);
		fclose(file);
		if (!rootJ) {
			WARN("Submarine Free WM-101: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
			return;
		}
		ColorCollectionButton *cb = loadCollectionFromJson(rootJ);
		json_decref(rootJ);
		if (!cb)
			return;
		
		reflowCollections();
		saveSettings();
		std::vector<NVGcolor> colors = cb->colors; 
		std::vector<std::string> labels = cb->labels;
		std::string name = cb->name;
		unsigned int index = cb->index();
		APP->history->push(new EventWidgetAction(
			"Load Collection",
			[index]() {
				if (masterWireManager) {
					ColorCollectionButton *cb = masterWireManager->findCollectionButton(index);
					if (cb) {
						masterWireManager->collectionScrollWidget->container->removeChild(cb);
						delete cb;
						masterWireManager->reflowCollections();
						masterWireManager->saveSettings();
					}
				}
			},
			[name, colors, labels]() {
				if (masterWireManager) {
					masterWireManager->addCollection(name, colors, labels);
					masterWireManager->saveSettings();
				}
			}
		));
	}
	void saveCollectionDialog(ColorCollectionButton *cb) {
		std::string dir = asset::user("SubmarineFree");
		system::createDirectory(dir);
		std::string filename = cb->name;

		osdialog_filters* filters = osdialog_filters_parse("Submarine Wire Manager Collection(.wmCollection):wmCollection");
		DEFER({
			osdialog_filters_free(filters);
		});

		char* pathC = osdialog_file(OSDIALOG_SAVE, dir.c_str(), filename.c_str(), filters);
		if (!pathC) {
			// Fail silently
			return;
		}
		DEFER({
			std::free(pathC);
		});

		// Append .vcv extension if no extension was given.
		std::string pathStr = pathC;
		if (string::filenameExtension(string::filename(pathStr)) == "") {
			pathStr += ".wmCollection";
		}

		saveCollectionToDisk(cb, pathStr);
	}
	void saveCollectionToDisk(ColorCollectionButton *cb, std::string pathStr) {
		json_t *settings = json_object();
		saveCollectionToJson(cb, settings);
		FILE *file = fopen(pathStr.c_str(), "w");
		if (file) {
			json_dumpf(settings, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
			fclose(file);
		}
		json_decref(settings);
	}
	void saveCollectionToJson(ColorCollectionButton *cb, json_t *node) {
		json_object_set_new(node, "name", json_string(cb->name.c_str()));
		json_t *a1 = json_array();
		for (NVGcolor col: cb->colors) {
			std::string s = color::toHexString(col);
			json_array_append_new(a1, json_string(s.c_str()));
		}
		json_object_set_new(node, "colors", a1);
		json_t *a2 = json_array();
		if (cb->labels.size() > 0) {  // check for labels first
			for (std::string label: cb->labels) {
				json_array_append_new(a2, json_string(label.c_str()));
			}
			json_object_set_new(node, "labels", a2);
		}
	}
	void changeCollectionName(ColorCollectionButton *cb, std::string text) {
		if (cb->name == text)
			return;
		std::string oldName = cb->name;
		int index = cb->index();
		cb->name = text;
		saveSettings();
		APP->history->push(new EventWidgetAction(
			"Change Collection Name",
			[oldName, index]() {
				if (masterWireManager) {
					ColorCollectionButton *btn = masterWireManager->findCollectionButton(index);
					if (btn) {
						btn->name = oldName;
						masterWireManager->saveSettings();
					}
				}
			},
			[text, index] {
				if (masterWireManager) {
					ColorCollectionButton *btn = masterWireManager->findCollectionButton(index);
					if (btn) {
						btn->name = text;
						masterWireManager->saveSettings();
					}
				}
			}
		));
		
	}
	void addWireMenu(WireButton *wb) {
		Menu *menu = createMenu();
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(color::toHexString(wb->color));
		paramField->changeHandler = [=](std::string text) {
			if (wb->setColor(text)) {
				this->saveSettings();
			};
		};
		menu->addChild(paramField);
		menu->addChild(new MenuSeparator);
		
		EventWidgetMenuItem *ed = new EventWidgetMenuItem();
		ed->text = "Edit...";
		ed->clickHandler = [=]() {
			this->editDialog(wb);
		};
		menu->addChild(ed);
		EventWidgetMenuItem *sc = new EventWidgetMenuItem();
		sc->text = "Color one wire...";
		sc->clickHandler = [=]() {
			this->selectWirePanel(wb->color);
		};
		menu->addChild(sc);
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
		dm->text = "Delete Color...";
		dm->clickHandler = [=]() {
			this->deleteDialog(wb);
		};
		menu->addChild(dm);
	}
	void saveAsCollection() {
		std::vector<NVGcolor> colors = currentCollectionColors();
		std::vector<std::string> labels = currentCollectionLabels();
		unsigned int index = addCollection(std::string("[Unnamed]"), colors, labels)->index();
		saveSettings();
		APP->history->push(new EventWidgetAction(
			"Save as Collection",
			[index]() {
				if (masterWireManager) {
					ColorCollectionButton *cb = masterWireManager->findCollectionButton(index);
					if (cb) {
						masterWireManager->collectionScrollWidget->container->removeChild(cb);
						delete cb;
						masterWireManager->reflowCollections();
						masterWireManager->saveSettings();
					}
				}
			},
			[colors, labels]() {
				if (masterWireManager) {
					masterWireManager->addCollection(std::string("Unnamed"), colors, labels);
					masterWireManager->saveSettings();
				}
			}
		));
		promptForCollectionName(index);
	}
	void promptForCollectionName(unsigned int index) {
		// jump to collections list (if not already visible)
		collectionsDialog();
		ColorCollectionButton *cb = findCollectionButton(index);
		// TODO: scroll to show the n-th collection (vs. jump to end of list)
		math::Rect box = math::Rect(math::Vec(INFINITY, INFINITY), math::Vec(INFINITY, INFINITY));
		collectionScrollWidget->scrollTo(box);
		// prompt for this collection's name/position/etc.
		addCollectionMenu(cb, true);  // use sensible menu position
	}
	void deleteCollection(ColorCollectionButton *cb) {
		unsigned int index = cb->index();
		std::string name = cb->name;
		std::vector<NVGcolor> colors = cb->colors;
		std::vector<std::string> labels = cb->labels;
		collectionScrollWidget->container->removeChild(cb);
		delete cb;
		reflowCollections();
		saveSettings();
		APP->history->push(new EventWidgetAction(
			"Delete Collection",
			[name, colors, labels, index]() {
				if (masterWireManager) {
					masterWireManager->insertCollection(name, colors, labels, index);
					masterWireManager->saveSettings();
				}
			},
			[index]() {
				if (masterWireManager) {
					ColorCollectionButton *cb = masterWireManager->findCollectionButton(index);
					if (cb) {
						masterWireManager->collectionScrollWidget->container->removeChild(cb);
						delete cb;
						masterWireManager->reflowCollections();
						masterWireManager->saveSettings();
					}
				}
			}
		));
	}
	void deleteWire(WireButton *wb) {
		NVGcolor color = wb->color;
		std::string label = wb->label;
		bool selected = wb->checkBox->selected;
		unsigned int index = wb->index();
		scrollWidget->container->removeChild(wb);
		delete wb;
		reflow();
		saveSettings();
		APP->history->push(new EventWidgetAction(
			"Delete Color",
			[color, label, selected, index]() {
				if (masterWireManager) {
					masterWireManager->insertColor(color, label, selected, index);
					masterWireManager->saveSettings();
				}
			},
			[index]() {
				if (masterWireManager) {
					WireButton *wb = masterWireManager->findWireButton(index);
					if (wb) {
						masterWireManager->scrollWidget->container->removeChild(wb);
						delete wb;
						masterWireManager->reflow();
						masterWireManager->saveSettings();
					}
				}
			}
		));
	}
	void reflow() {
		unsigned int y = 0;
		for (Widget *widget : scrollWidget->container->children) {
			widget->box.pos.y = y;
			y += 21;
		}
		
	}
	void reflowCollections() {
		unsigned int y = 0;
		for (Widget *widget : collectionScrollWidget->container->children) {
			widget->box.pos.y = y;
			y += 24;
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
		std::string lab = wb1->label;
		wb1->color = wb2->color;
		wb1->label = wb2->label;
		wb2->color = col;
		wb2->label = lab;
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
		deleteConfirmPanel->cancelTargetWidget = NULL;
	}
	void deleteCollectionDialog(ColorCollectionButton *cb) {
		deleteLabel->label = "Delete Collection?";
		deleteOkButton->clickHandler = [=]() {
			this->deleteCollectionOk(cb);
		};
		collectionPanel->visible = false;
		deleteConfirmPanel->visible = true;
		deleteConfirmPanel->cancelTargetWidget = collectionPanel;
	}
	void recolorAllDialog() {
		deleteLabel->label = "Recolor All Wires?";
		deleteOkButton->clickHandler = [=]() {
			this->cableCount = -1;
			this->cancel();
		};
		backPanel->visible = false;
		deleteConfirmPanel->visible = true;
		deleteConfirmPanel->cancelTargetWidget = NULL;
	}
	void cancel() {
		hidePanels();
		if (deleteConfirmPanel->cancelTargetWidget) {
			deleteConfirmPanel->cancelTargetWidget->visible = true;
			deleteConfirmPanel->cancelTargetWidget = NULL;
			return;
		}
		backPanel->visible = true;
	}
	void deleteOk(WireButton *wb) {
		this->deleteWire(wb);
		cancel();
	}
	void deleteCollectionOk(ColorCollectionButton *cb) {
		this->deleteCollection(cb);
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
	void collectionsDialog() {
		backPanel->visible = false;
		collectionPanel->visible = true;
	}
	void editAdd(NVGcolor col, std::string label) {
		std::string defaultLabel = "";
		addColor(col, defaultLabel, false);
		unsigned int index = scrollWidget->container->children.size() - 1;
		APP->history->push(new EventWidgetAction(
			"Add Color",
			[index]() {
				if (masterWireManager) {
					WireButton *wb = masterWireManager->findWireButton(index);
					if (wb) {
						masterWireManager->scrollWidget->container->removeChild(wb);
						delete wb;
						masterWireManager->saveSettings();
					}
				}
			},
			[index, col, defaultLabel]() {
				if (masterWireManager) {
					masterWireManager->insertColor(col, defaultLabel, false, index);
					masterWireManager->saveSettings();
				}
			}
		));
	}
	void editEdit(WireButton *wb, NVGcolor col, std::string label) {
		NVGcolor oldCol = wb->color;
		std::string oldLabel = wb->label;
		unsigned int index = wb->index();
		wb->color = col;
		wb->label = label;
		APP->history->push(new EventWidgetAction(
			"Edit Color",
			[index, oldCol, oldLabel]() {
				if (masterWireManager) {
					WireButton *wb = masterWireManager->findWireButton(index);
					if (wb) {
						wb->color = oldCol;
						wb->label = oldLabel;
						masterWireManager->saveSettings();
					}
				}
			},
			[index, col, label]() {
				if (masterWireManager) {
					WireButton *wb = masterWireManager->findWireButton(index);
					if (wb) {
						wb->color = col;
						wb->label = label;
						masterWireManager->saveSettings();
					}
				}
			}
		));
	}
	void editDialog(WireButton *wb) {
		backPanel->visible = false;
		editPanel->completeHandler = [=](NVGcolor col, std::string label) {
			if (wb) {
				editEdit(wb, col, label);
			}
			else {
				editAdd(col, label);
			}
			saveSettings();
			cancel();
		};
		if (wb) {
			editPanel->r->value = wb->color.r;
			editPanel->g->value = wb->color.g;
			editPanel->b->value = wb->color.b;
			editPanel->labelField->setText(wb->label);
		}
		else {
			editPanel->r->value = 0.5f;
			editPanel->g->value = 0.5f;
			editPanel->b->value = 0.5f;
			editPanel->labelField->setText("");
		}
		editPanel->addChild(editPanel->labelField);
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
					masterWireManager->highlightChangedCore(oldValue);
					masterWireManager->saveSettings();
				}
			},
			[value]() {
				if (masterWireManager) {
					masterWireManager->highlightChangedCore(value);
					masterWireManager->saveSettings();
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
	void hidePanels() {
		blockingPanel->visible = false;
		backPanel->visible = false;	
		if (editPanel->visible) {
			editPanel->removeChild(editPanel->labelField);
		}
		editPanel->visible = false;
		wirePanel->visible = false;
		settingsPanel->visible = false;
		collectionPanel->visible = false;
		deleteConfirmPanel->visible = false;	
		billboardPanel->visible = false;
		viewToggle->box.pos = Vec(141, 1);
		minButton->visible = true;
	}
	void showBillboard() {
		hidePanels();
		billboardPanel->visible = true;
		viewToggle->box.pos = Vec(1,1);
		minButton->visible = false;
	}
};

struct WM102 : SchemeModuleWidget, WM_Base {
	std::vector<NVGcolor> colors;
	std::vector<std::string> labels;
	SchemePanel *schemePanel;
	WM102(Module *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		schemePanel = new SchemePanel(this->box.size);
		addChild(schemePanel);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "WM-102");
		Rect renderBox;
		renderBox.pos = Vec(0, 15);
		renderBox.size = Vec(box.size.x, box.size.y - 30);
		nvgSave(vg);
		nvgTranslate(vg, renderBox.pos.x, renderBox.pos.y);
		drawBillboardBase(vg, renderBox, colors, labels, true);
		nvgRestore(vg);
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		menu->addChild(new MenuEntry);
		EventWidgetMenuItem *mi = createMenuItem<EventWidgetMenuItem>("Load collection");
		mi->clickHandler = [=]() {
			this->loadCollectionDialog();
		};
		menu->addChild(mi);
	}
	void loadCollectionFromDisk(std::string pathC) override {
		json_error_t error;
		FILE *file = fopen(pathC.c_str(), "r");
		if (!file) {
			return;
		}
		json_t *rootJ = json_loadf(file, 0, &error);
		fclose(file);
		if (!rootJ) {
			WARN("Submarine Free WM-102: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
			return;
		}
		ColorCollectionButton *cb = loadCollectionFromJson(rootJ);
		colors = cb->colors;
		labels = cb->labels;
		delete cb;
		json_decref(rootJ);
		schemePanel->dirty = true;
	}
	ColorCollectionButton *addCollection(std::string name, std::vector<NVGcolor> colors, std::vector<std::string> labels) override {
		ColorCollectionButton *btn = new ColorCollectionButton();
		btn->box.pos = Vec(0, 0);
		btn->box.size = Vec(140, 24);
		btn->name = name;
		btn->colors = colors;
		btn->labels = labels;
		return btn;
	}
	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		json_t *a1 = json_array();
		for (NVGcolor col: colors) {
			std::string s = color::toHexString(col);
			json_array_append_new(a1, json_string(s.c_str()));
		}
		json_object_set_new(rootJ, "colors", a1);
		json_t *a2 = json_array();
		if (labels.size() > 0) {  // check for labels first
			for (std::string label: labels) {
				json_array_append_new(a2, json_string(label.c_str()));
			}
			json_object_set_new(rootJ, "labels", a2);
		}

		return rootJ;
	}
	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);
		ColorCollectionButton *cb = loadCollectionFromJson(rootJ);
		colors = cb->colors;
		labels = cb->labels;
		delete cb;
		schemePanel->dirty = true;
	}
};

Model *modelWM101 = createModel<Module, WM101>("WM-101");
Model *modelWM102 = createModel<Module, WM102>("WM-102");
