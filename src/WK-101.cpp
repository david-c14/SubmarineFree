#include "SubmarineFree.hpp"
#include <mutex>
#include "torpedo.hpp"

struct WK_Tuning {
	std::string name;
	float offsets[12];
};

WK_Tuning tunings[] = {
	{ "Equal Temperament", { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f } },
	{ "Pythagorean", { 0.0f, 13.7f, 3.9f, -5.9f, -7.8f, -2.0f, 11.7f, 2.0f, -7.8f, 5.9f, 3.9f, 9.8f } },
	{ "Werckmeister III", { 0.0f, -9.775f, -7.82f, -5.865f, -9.775f, -1.955f, -11.73f, -3.94f, -7.82f, -11.93f, -3.91f, -7.82f } },
	{ "Young", { 0.0f, -6.1f, -4.2f, -2.2f, -8.3f, -0.1f, -8.1f, -2.1f, -4.2f, -6.2f, -0.2f, -8.2f } }
};

struct WK_101;

struct WK_InputPort : Torpedo::PatchInputPort {
	WK_101 *wkModule;
	WK_InputPort(WK_101 *module, unsigned int portNum):PatchInputPort((Module *)module, portNum) { wkModule = module;};
	void received(std::string pluginName, std::string moduleName, json_t *rootJ) override;
};

struct WK_101 : Module {
	static const int deviceCount = 12;
	enum ParamIds {
		PARAM_1,
		PARAM_2,
		PARAM_3,
		PARAM_4,
		PARAM_5,
		PARAM_6,
		PARAM_7,
		PARAM_8,
		PARAM_9,
		PARAM_10,
		PARAM_11,
		PARAM_12,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_CV,
		INPUT_TOR,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_CV,
		OUTPUT_TOR,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	float tunings[12];
	int isDirty = 0;
	int toSend = 0;
	std::mutex mtx;
	Torpedo::PatchOutputPort outPort = Torpedo::PatchOutputPort(this, OUTPUT_TOR);
	WK_InputPort inPort = WK_InputPort(this, INPUT_TOR);

	WK_101() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
};

void WK_101::step() {
	int quantized = floor((12.0f * inputs[INPUT_CV].value) + 0.5f);
	int note = (120 + quantized) % 12;
	outputs[OUTPUT_CV].value = (tunings[note] / 1200.0f) + (quantized / 12.0f);	
	if (toSend && !outPort.isBusy()) {
		toSend = 0;
		json_t *rootJ = json_array();
		for (int i = 0; i < 12; i++)
			json_array_append_new(rootJ, json_real(tunings[i]));
		outPort.send(std::string(TOSTRING(SLUG)), std::string("WK"), rootJ);
	}
	outPort.process();
	inPort.process();

}

void WK_InputPort::received(std::string pluginName, std::string moduleName, json_t *rootJ) {
	if (pluginName.compare(TOSTRING(SLUG))) return;
	if (moduleName.compare("WK")) return;
	float tunings[12];
	int size = json_array_size(rootJ);
	if (!size) return;
	if (size > 12)
		size = 12;
	for (int i = 0; i < 12; i++) {
		json_t *j1 = json_array_get(rootJ, i);
		if (j1)
			tunings[i] = json_number_value(j1);
	}
	{
		std::lock_guard<std::mutex> guard(wkModule->mtx);
		for (int i = 0; i < 12; i++)
			wkModule->tunings[i] = tunings[i];
		wkModule->isDirty = true;
	}
}

struct WK_Display : TransparentWidget {
	std::shared_ptr<Font> font;
	WK_101 *module;
	int index;
	char dspText[20];
	
	WK_Display() {
		font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));
	}

	void draw(NVGcontext *vg) override {
		float val = module->tunings[index];
		sprintf(dspText, "%+5.3f", val);
		nvgFontSize(vg, 14);
		nvgFontFaceId(vg, font->handle);
		nvgFillColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		nvgTextAlign(vg, NVG_ALIGN_CENTER);
		nvgText(vg, 30, 13, dspText, NULL);
	}
};

struct WK_MenuItem : MenuItem {
	WK_101 *module;
	int index;
	void onAction(EventAction &e) override {
		for (int i = 0; i < WK_101::deviceCount; i++)
			module->tunings[i] = tunings[index].offsets[i];
		module->isDirty = true;
		module->toSend = true;
	}
};

struct WK_Param : sub_knob_med {
	
	void onChange(EventChange &e) override {
		sub_knob_med::onChange(e);
		WK_101 *module = dynamic_cast<WK_101 *>(this->module);
		module->tunings[paramId - WK_101::PARAM_1] = value;
		module->toSend = true;
	}
};

struct WK101 : ModuleWidget {
	WK_Param *widgets[12];
	WK101(WK_101 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/WK-101.svg")));

		addInput(Port::create<sub_port>(Vec(4,29), Port::INPUT, module, WK_101::INPUT_CV));
		addOutput(Port::create<sub_port>(Vec(43,29), Port::OUTPUT, module, WK_101::OUTPUT_CV));
		addInput(Port::create<sub_port_black>(Vec(82,29), Port::INPUT, module, WK_101::INPUT_TOR));
		addOutput(Port::create<sub_port_black>(Vec(121,29), Port::OUTPUT, module, WK_101::OUTPUT_TOR));

		for (int i = 0; i < 5; i++)
		{
			WK_Display *display = new WK_Display();
			display->module = module;
			display->index = i;
			display->box.pos = Vec(45, 79 + 21 * i);
			display->box.size = Vec(60, 20);
			addChild(display);
			widgets[i] = ParamWidget::create<WK_Param>(Vec(4 + 104 * (i%2),70 + 21 * i), module, WK_101::PARAM_1 + i, -99.0f, 99.0f, 0.0f);
			addParam(widgets[i]);
		}
		for (int i = 5; i < 12; i++)
		{
			WK_Display *display = new WK_Display();
			display->module = module;
			display->index = i;
			display->box.pos = Vec(45, 100 + 21 * i);
			display->box.size = Vec(60, 20);
			addChild(display);
			widgets[i] = ParamWidget::create<WK_Param>(Vec(108 - 104 * (i%2),91 + 21 * i), module, WK_101::PARAM_1 + i, -99.0f, 99.0f, 0.0f);
			addParam(widgets[i]);
		}
	}
	void appendContextMenu(Menu *menu) override;
	void step() override;
};

void WK101::appendContextMenu(Menu *menu) {
	WK_101 *module = dynamic_cast<WK_101 *>(this->module);
	menu->addChild(MenuEntry::create());
	for (int i = 0; i < 4; i++) { 
		WK_MenuItem *m = MenuItem::create<WK_MenuItem>(tunings[i].name.c_str());
		m->module = module;
		m->index = i;
		menu->addChild(m);
	}
}

void WK101::step() {
	float tunings[12];
	int isDirty = 0;
	WK_101 *module = dynamic_cast<WK_101 *>(this->module);
	{
		std::lock_guard<std::mutex> guard(module->mtx);
		if (module->isDirty) {
			for (int i = 0; i < 12; i++)
				tunings[i] = module->tunings[i];
			isDirty = 1;
		}
	}
	if (isDirty) {
		for (int i = 0; i < 12; i++) {
			if (widgets[i]->value != tunings[i])
				widgets[i]->setValue(tunings[i]);	
		}
	}

	ModuleWidget::step();
}

Model *modelWK101 = Model::create<WK_101, WK101>("SubmarineFree", "WK-101", "WK-101 Das Wohltemperierte Klavier", QUANTIZER_TAG, TUNER_TAG);
