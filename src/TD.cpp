#include "SubmarineFree.hpp"
#include "torpedo.hpp"

struct TD_116;

struct TDInput : Torpedo::PatchInputPort {
	TD_116 *tdModule;
	TDInput(TD_116 *module, unsigned int portNum) : Torpedo::PatchInputPort((Module *)module, portNum) { tdModule = module; }
	void received(std::string pluginName, std::string moduleName, json_t *rootJ) override;
};

struct TD_116 : Module {
	TDInput inPort = TDInput(this, 0);
	Torpedo::PatchOutputPort outPort = Torpedo::PatchOutputPort(this, 0);
	TD_116() : Module (0, 1, 1, 0) {}
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
	int isDirty = false;
};

struct TDText : LedDisplayTextField {
	TD_116 *tdModule;
	void onTextChange() override {
		LedDisplayTextField::onTextChange();
		tdModule->sendText(text);
	}
};

void TDInput::received(std::string pluginName, std::string moduleName, json_t *rootJ) {
	if (pluginName.compare("SubmarineFree")) return;
	if (moduleName.compare("TDNotesText")) return;
	json_t *text = json_object_get(rootJ, "text");
	if (text) {
		tdModule->text.assign(json_string_value(text));
		tdModule->isDirty = true;
	}
}

struct TD116 : ModuleWidget {
	TDText *textField;

	TD116(TD_116 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/TD-116.svg")));

		addInput(Port::create<sub_port_black>(Vec(4,19), Port::INPUT, module, 0));
		addOutput(Port::create<sub_port_black>(Vec(211,19), Port::OUTPUT, module, 0));	

		textField = Widget::create<TDText>(mm2px(Vec(3.39962, 15.8373)));
		textField->box.size = mm2px(Vec(74.480, 100.753));
		textField->multiline = true;
		textField->tdModule = module;
		addChild(textField);
	}

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		// text
		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		// text
		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
	}

	void step() override {
		TD_116 *tdModule = dynamic_cast<TD_116 *>(module);
		if (tdModule->isDirty) {
			textField->text = tdModule->text;
			tdModule->isDirty = false;
		}
		ModuleWidget::step();
	}
};


Model *modelTD116 = Model::create<TD_116, TD116>("SubmarineFree", "TD-116", "TD-116 Text Display", VISUAL_TAG);
