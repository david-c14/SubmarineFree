//SubTag W16

#include "SubmarineFree.hpp"
#include "window.hpp"

struct TD_316 : Module {
	TD_316() : Module () {
		config(0, 0, 0, 0);
	}
	void processExpander(float *message) {
		if (!std::isnan(message[0])) {
			fg = nvgRGBf(message[0], message[1], message[2]);
			fgDirty = true;
		}
		if (!std::isnan(message[3])) {
			bg = nvgRGBf(message[3], message[4], message[5]);
			bgDirty = true;
		}
		if (!std::isnan(message[6])) {
			fontSize = clamp(message[6], 6.0f, 26.0f);
			fontSizeDirty = true;
		}
	}
	void process(const ProcessArgs &args) override {
		if (leftExpander.module) {
			if ((leftExpander.module->model == modelTF101) || (leftExpander.module->model == modelTF102)) {
				processExpander((float *)(leftExpander.module->rightExpander.consumerMessage));
			}
		}
		if (rightExpander.module) {
			if ((rightExpander.module->model == modelTF101) || (rightExpander.module->model == modelTF102)) {
				processExpander((float *)(rightExpander.module->leftExpander.consumerMessage));
			}
		}
	}
	void onReset() override {
		reset = 1;
		Module::onReset();
	}
	int reset = 0;
	float fontSize = 12.0f;
	NVGcolor fg = SUBLIGHTBLUE;
	NVGcolor bg = nvgRGB(0,0,0);
	bool fgDirty = false;
	bool bgDirty = false;
	bool fontSizeDirty = false;
};

struct TD3Text : SubText {
	TD3Text() {
		color = SUBLIGHTBLUE;
	}
	void foregroundMenu(Menu *menu) override {
		SubText::foregroundMenu(menu);
		menu->addChild(createForegroundMenuItem("Black", nvgRGB(0, 0, 0)));
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

struct TD316 : SchemeModuleWidget {
	TD3Text *textField;

	TD316(TD_316 *module) {
		setModule(module);
		this->box.size = Vec(240, 380);
		addChild(new SchemePanel(this->box.size));

		textField = createWidget<TD3Text>(Vec(4, 18));
		textField->box.size = Vec(232, 344);
		textField->multiline = true;
		addChild(textField);
	}

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		json_object_set_new(rootJ, "size", json_real(textField->fontSize));
		json_object_set_new(rootJ, "fg", json_string(color::toHexString(textField->color).c_str()));
		json_object_set_new(rootJ, "bg", json_string(color::toHexString(textField->bgColor).c_str()));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *sizeJ = json_object_get(rootJ, "size");
		if (sizeJ)
			textField->fontSize = json_number_value(sizeJ);
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
		TD_316 *tdModule = dynamic_cast<TD_316 *>(module);
		if (!tdModule) {
			return;
		}
		if (tdModule->fontSizeDirty) {
			textField->fontSize = tdModule->fontSize;
			tdModule->fontSizeDirty = false;
		}
		if (tdModule->fgDirty) {
			textField->color = tdModule->fg;
			tdModule->fgDirty = false;
		}
		if (tdModule->bgDirty) {
			textField->bgColor = tdModule->bg;
			tdModule->bgDirty = false;
		}
		if (tdModule->reset) {
			textField->fontSize = 12;
			textField->text = "";
			textField->color = SUBLIGHTBLUE;
			textField->bgColor = nvgRGB(0,0,0);	
			tdModule->reset = 0;
		}
		ModuleWidget::step();
	}

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		textField->appendContextMenu(menu);
	}

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-316");
	}
};

Model *modelTD316 = createModel<TD_316, TD316>("TD-316");
