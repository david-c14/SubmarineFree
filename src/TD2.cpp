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
	void onButton(const event::Button &e) override {
		if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
			e.consume(this);
			Menu *menu = createMenu();
			appendContextMenu(menu);
		}
		else {
			SubText::onButton(e);
		}
	}
};

struct TD_202 : Module {
	TD_202() : Module() {
		config(0, 0, 0, 0);
	}
	int reset = 0;
	void onReset() override {
		reset = 1;
		Module::onReset();
	}
};

struct TD202 : SchemeModuleWidget {
	TDVText *textField;

	TD202(TD_202 *module) : SchemeModuleWidget(module) {
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

	void step() override {
		TD_202 *tdModule = dynamic_cast<TD_202 *>(module);
		if (!tdModule) {
			return;
		}
		if (tdModule->reset) {
			textField->text = "";
			textField->multiline = false;
			textField->color = SUBLIGHTBLUE;
			textField->bgColor = nvgRGBA(0, 0, 0, 0);
			tdModule->reset = 0;
		}
	}

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		textField->appendContextMenu(menu);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-202");
	}
};

Model *modelTD202 = createModel<TD_202, TD202>("TD-202");
