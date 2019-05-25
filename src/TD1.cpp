//SubTag W16

#include "SubmarineFree.hpp"
#include "window.hpp"
#include "torpedo.hpp"

struct TD_116;

struct TDInput : Torpedo::PatchInputPort {
	TD_116 *tdModule;
	TDInput(TD_116 *module, unsigned int portNum) : Torpedo::PatchInputPort((Module *)module, portNum) { tdModule = module; }
	void received(std::string pluginName, std::string moduleName, json_t *rootJ) override;
	NVGcolor decodeColor(std::string colorStr);
};

struct TD_116 : Module {
	TDInput inPort = TDInput(this, 0);
	Torpedo::PatchOutputPort outPort = Torpedo::PatchOutputPort(this, 0);
	TD_116() : Module (0, 1, 1, 0) {outPort.size(1);}
	void step() override {
		inPort.process();
		outPort.process();
	}
	void sendText(std::string text) {
		json_t *rootJ = json_object();;

		// text
		json_object_set_new(rootJ, "text", json_string(text.c_str()));

		outPort.send("SubmarineFree", "TDNotesText", rootJ); 
	}
	std::string text;
	int fontSize = 12;
	NVGcolor fg = SUBLIGHTBLUE;
	NVGcolor bg = nvgRGB(0,0,0);
	int isDirty = false;
	int isDirtyC = false;
};

struct TDText : SubText {
	TD_116 *tdModule;
	TDText() {
		color = SUBLIGHTBLUE;
	}
	void onTextChange() override {
		LedDisplayTextField::onTextChange();
		if (tdModule) {
			tdModule->sendText(text);
		}
	}
	void foregroundMenu(Menu *menu) override {
		menu->addChild(createForegroundMenuItem("Black", nvgRGB(0, 0, 0)));
		SubText::foregroundMenu(menu);
	}
	void onMouseDown(EventMouseDown &e) override {
		if (e.button == 1) {
			e.consumed = true;
			Menu *menu = gScene->createMenu();
			appendContextMenu(menu);
		}
		else {
			SubText::onMouseDown(e);
		}
	}
};

NVGcolor TDInput::decodeColor(std::string colorStr) {
	int r = (colorStr[0] - 'A') * 16 + (colorStr[1] - 'A');
	int g = (colorStr[2] - 'A') * 16 + (colorStr[3] - 'A');
	int b = (colorStr[4] - 'A') * 16 + (colorStr[5] - 'A');
	return nvgRGB(r, g, b);
}

void TDInput::received(std::string pluginName, std::string moduleName, json_t *rootJ) {
	if (pluginName.compare("SubmarineFree")) return;
	if (!moduleName.compare("TDNotesText")) { 
		json_t *text = json_object_get(rootJ, "text");
		if (text) {
			tdModule->text.assign(json_string_value(text));
			tdModule->isDirty = true;
		}
	}
	else if (!moduleName.compare("TDNotesColor")) {
		json_t *size = json_object_get(rootJ, "size");
		if (size) {
			tdModule->fontSize = json_number_value(size);
			tdModule->isDirtyC = true;
		}	
		json_t *fg = json_object_get(rootJ, "fg");
		if (fg) {
			tdModule->fg = decodeColor(std::string(json_string_value(fg)));
			tdModule->isDirtyC = true;
		}
		json_t *bg = json_object_get(rootJ, "bg");
		if (bg) {
			tdModule->bg = decodeColor(std::string(json_string_value(bg)));
			tdModule->isDirtyC = true;
		}
	}	
}

struct TD116 : SchemeModuleWidget {
	TDText *textField;

	TD116(TD_116 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(240, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BlackPort>(Vec(16.5,31.5), module, 0));
		addOutput(createOutputCentered<BlackPort>(Vec(223.5,31.5), module, 0));	

		textField = createWidget<TDText>(mm2px(Vec(3.39962, 15.8373)));
		textField->box.size = mm2px(Vec(74.480, 102.753));
		textField->multiline = true;
		textField->tdModule = module;
		addChild(textField);
	}

	json_t *dataToJson() override {
		json_t *rootJ = ModuleWidget::dataToJson();

		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		json_object_set_new(rootJ, "size", json_real(textField->fontSize));
		json_object_set_new(rootJ, "fg", json_string(colorToHexString(textField->color).c_str()));
		json_object_set_new(rootJ, "bg", json_string(colorToHexString(textField->bgColor).c_str()));

		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		ModuleWidget::dataFromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *sizeJ = json_object_get(rootJ, "size");
		if (sizeJ)
			textField->fontSize = json_number_value(sizeJ);
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ) {
			if (json_is_object(fgJ)) 
				textField->color = jsonToColor(fgJ);
			else
				textField->color = colorFromHexString(json_string_value(fgJ));
		}
		json_t *bgJ = json_object_get(rootJ, "bg");
		if (bgJ) {
			if (json_is_object(bgJ))
				textField->bgColor = jsonToColor(bgJ);
			else
				textField->bgColor = colorFromHexString(json_string_value(bgJ));
		}
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
		ModuleWidget::step();
	}

	void reset() override {
		textField->fontSize = 12;
		textField->text = "";
		textField->color = SUBLIGHTBLUE;
		textField->bgColor = nvgRGB(0,0,0);	
		ModuleWidget::reset();
	}

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		textField->appendContextMenu(menu);
	}

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-116");
		drawText(vg, 30, 36, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "SYNC IN");
		drawText(vg, 210, 36, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "SYNC OUT");
	}
};




Model *modelTD116 = createModel<TD_116, TD116>("TD-116");
