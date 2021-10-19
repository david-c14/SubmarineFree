//SubTag W16 AM WP

#include "SubmarineFree.hpp"
#include "shared/torpedo.hpp"

struct TD_116;

struct TD_116 : Module {
	TD_116() : Module () {
		config(0, 1, 1, 0);
		configInput(0, "Deprecated");
		configOutput(0, "Deprecated");
	}
	void processExpander(float *message) {
		if (!std::isnan(message[0])) {
			NVGcolor newFg = nvgRGBf(message[0], message[1], message[2]);
			if (!COLOR_EQ(newFg, fg))
				isDirtyC = true;
			fg = newFg;
		}
		if (!std::isnan(message[3])) {
			NVGcolor newBg = nvgRGBf(message[3], message[4], message[5]);
			if (!COLOR_EQ(newBg, bg))
				isDirtyC = true;
			bg = newBg;
		}
		if (!std::isnan(message[6])) {
			int newSize = clamp(message[6], 6.0f, 26.0f);
			if (newSize != fontSize)
				isDirtyC = true;
			fontSize = newSize;
		}
	}
	void process(const ProcessArgs &args) override {
		/*
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
		*/
	}
	void sendText(std::string textValue) {
		text = textValue;

	}
	void onReset() override {
		reset = 1;
		Module::onReset();
	}
	json_t *dataToJson() override {
		json_t *rootJ = json_object();

		json_object_set_new(rootJ, "text", json_string(text.c_str()));
		json_object_set_new(rootJ, "size", json_real(fontSize));
		json_object_set_new(rootJ, "fg", json_string(color::toHexString(fg).c_str()));
		json_object_set_new(rootJ, "bg", json_string(color::toHexString(bg).c_str()));

		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override {

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ) {
			text = json_string_value(textJ);
			isDirty = true;
		}
		json_t *sizeJ = json_object_get(rootJ, "size");
		if (sizeJ) {
			fontSize = json_number_value(sizeJ);
			isDirtyC = true;
		}
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ) {
			fg = color::fromHexString(json_string_value(fgJ));
			isDirtyC = true;
		}
		json_t *bgJ = json_object_get(rootJ, "bg");
		if (bgJ) {
			bg = color::fromHexString(json_string_value(bgJ));
			isDirtyC = true;
		}
	}

	std::string text;
	int reset = 0;
	float fontSize = 12.0f;
	NVGcolor fg = SUBLIGHTBLUE;
	NVGcolor bg = nvgRGB(0,0,0);
	int isDirty = false;
	int isDirtyC = false;
};

namespace {

	struct TD1Text : SubText {
		TD_116 *tdModule;
		TD1Text() {
			color = SUBLIGHTBLUE;
		}
		void onChange(const event::Change &e) override {
			LedDisplayTextField::onChange(e);
			if (tdModule) {
				tdModule->sendText(text);
			}
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

} // end namespace
	
struct TD116 : SchemeModuleWidget {
	TD1Text *textField;

	TD116(TD_116 *module) {
		setModule(module);
		this->box.size = Vec(240, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BlackPort>(Vec(16.5,31.5), module, 0));
		addOutput(createOutputCentered<BlackPort>(Vec(223.5,31.5), module, 0));	

		textField = createWidget<TD1Text>(mm2px(Vec(3.39962, 15.8373)));
		textField->box.size = mm2px(Vec(74.480, 102.753));
		textField->multiline = true;
		textField->tdModule = module;
		addChild(textField);
	}

	void step() override {
		TD_116 *tdModule = dynamic_cast<TD_116 *>(module);
		if (!tdModule) {
			return;
		}
		if (tdModule->isDirty) {
			textField->text = tdModule->text;
			tdModule->isDirty = false;
		}
		if (tdModule->isDirtyC) {
			textField->fontSize = tdModule->fontSize;
			textField->color = tdModule->fg;
			textField->bgColor = tdModule->bg;
			tdModule->isDirtyC = false;
		}
		if (tdModule->reset) {
			textField->fontSize = 12;
			textField->text = "";
			textField->color = SUBLIGHTBLUE;
			textField->bgColor = nvgRGB(0,0,0);	
			tdModule->reset = 0;
		}
		tdModule->text = textField->text;
		tdModule->fontSize = textField->fontSize;
		tdModule->fg = textField->color;
		tdModule->bg = textField->bgColor;
		ModuleWidget::step();
	}

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
	textField->appendContextMenu(menu);
	}

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-116");
	}
};

Model *modelTD116 = createModel<TD_116, TD116>("TD-116");
