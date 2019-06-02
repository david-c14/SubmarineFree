//SubTag W2

#include "SubmarineFree.hpp"
#include "window.hpp"

struct TDVText : SubText {
	TDVText() {
		multiline = false;
		color = SUBLIGHTBLUE;
		bgColor = nvgRGBA(0, 0, 0, 0);
		fontSize = 28;
	}
	void onMouseDown(EventMouseDown &e) override {
		if (e.button == 1) {
			e.consumed = true;
			Menu *menu = createMenu();
			appendContextMenu(menu);
		}
		else {
			SubText::onMouseDown(e);
		}
	}
};

struct TD202 : SchemeModuleWidget {
	TDVText *textField;

	TD202(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));
	
		MouseTransformWidget *tw = createWidget<MouseTransformWidget>(Vec(2, 15));
		tw->rotate(M_PI / 2.0f);
		addChild(tw);

		textField = createWidget<TDVText>(Vec(0, -25));
		textField->box.size = Vec(350, 25);
		tw->addChild(textField);
	}

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		json_object_set_new(rootJ, "fg", json_string(color::toHexString(textField->color).c_str()));
		json_object_set_new(rootJ, "bg", json_string(color::toHexString(textField->bgColor).c_str()));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ) {
			textField->color = color::fromHexString(json_string_value(fgJ));
		}
		json_t *bgJ = json_object_get(rootJ, "bg");
		if (bgJ) {
			textField->bgColor = color::fromHexString(json_string_value(bgJ));
		}
	}

	void onReset() override {
		textField->text = "";
		textField->multiline = false;
		textField->color = SUBLIGHTBLUE;
		textField->bgColor = nvgRGBA(0, 0, 0, 0);
		ModuleWidget::onReset();
	}
	
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		textField->appendContextMenu(menu);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-202");
	}
};

Model *modelTD202 = createModel<Module, TD202>("TD-202");
