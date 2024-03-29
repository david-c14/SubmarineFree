//SubTag W2 WP AM

#include "SubmarineFree.hpp"


struct TDVText : SubText {
	TDVText() {
		multiline = false;
		color = SUBLIGHTBLUE;
		bgColor = nvgRGBA(0, 0, 0, 0);
		fontSize = 28;
		textMenu = false;
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
	void draw(const DrawArgs &args) override {
		box.size.y = 25;
		SubText::draw(args);
		box.size.y = 30;
	}
};

struct TD_202 : Module {
	NVGcolor fg;
	NVGcolor bg;
	bool txtDirty = false;
	bool fgDirty = false;
	bool bgDirty = false;
	std::string text = "";
	TD_202() : Module() {
		config(0, 0, 0, 2);
		configLight(0, "Module Link");
		configLight(1, "Module Link");
	}
	int reset = 0;
	void onReset() override {
		reset = 1;
		Module::onReset();
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
	}
	void process(const ProcessArgs &args) override {
		bool left = false;
		bool right = false;
		if (leftExpander.module) {
			if ((leftExpander.module->model == modelTF101) || (leftExpander.module->model == modelTF102)) {
				processExpander((float *)(leftExpander.module->rightExpander.consumerMessage));
				left = true;
			}
		}
		if (rightExpander.module) {
			if ((rightExpander.module->model == modelTF101) || (rightExpander.module->model == modelTF102)) {
				processExpander((float *)(rightExpander.module->leftExpander.consumerMessage));
				right = true;
			}
		}
		lights[0].setBrightness(left);
		lights[1].setBrightness(right);
	}
	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "text", json_string(text.c_str()));
		json_object_set_new(rootJ, "fg", json_string(color::toHexString(fg).c_str()));
		json_object_set_new(rootJ, "bg", json_string(color::toHexString(bg).c_str()));

		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override {

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ) {
			text = json_string_value(textJ);
			txtDirty = true;
		}
		json_t *fgJ = json_object_get(rootJ, "fg"); 
		if (fgJ) { 
			fg = color::fromHexString(json_string_value(fgJ));
			fgDirty = true;
		}
		json_t *bgJ = json_object_get(rootJ, "bg");
		if (bgJ) {
			bg = color::fromHexString(json_string_value(bgJ));
			bgDirty = true;
		}
	}

};

struct TD202 : SchemeModuleWidget {
	TDVText *textField;

	void textChanged() {
		TD_202 *tdm = dynamic_cast<TD_202 *>(module);
		if (tdm) 
			tdm->text = textField->text;
	}

	TD202(Module *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		MouseTransformWidget *tw = createWidget<MouseTransformWidget>(Vec(2.5, 15));
		tw->rotate(M_PI / 2.0f);
		addChild(tw);

		textField = createWidget<TDVText>(Vec(0, -25));
		textField->box.size = Vec(350, 30);
		textField->changeHandler = [=]() { textChanged(); };
		tw->addChild(textField);

		addChild(createLightCentered<RightLight>(Vec(3, 14), module, 0));
		addChild(createLightCentered<LeftLight>(Vec(27, 14), module, 1));
	}

	void step() override {
		SchemeModuleWidget::step();
		TD_202 *tdModule = dynamic_cast<TD_202 *>(module);
		if (!tdModule) {
			textField->text = "Submarine TD-202";
			return;
		}
		if (tdModule->reset) {
			textField->text = "";
			textField->multiline = false;
			textField->color = SUBLIGHTBLUE;
			textField->bgColor = nvgRGBA(0, 0, 0, 0);
			tdModule->reset = 0;
		}
		if (tdModule->fgDirty) {
			textField->color = tdModule->fg;
			tdModule->fgDirty = false;
		}
		if (tdModule->bgDirty) {
			textField->bgColor = tdModule->bg;
			tdModule->bgDirty = false;
		}
		if (tdModule->txtDirty) {
			textField->text = tdModule->text;
			tdModule->txtDirty = false;
		}
		tdModule->text = textField->text;
		tdModule->fg = textField->color;
		tdModule->bg = textField->bgColor;
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
