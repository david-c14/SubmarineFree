#include "SubmarineFree.hpp"
#include <mutex>
#include "torpedo.hpp"

struct WK_Tuning {
	std::string name;
	float offsets[12];
};

std::vector<WK_Tuning> tunings;

int tuningsLoaded = false;

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
	for (int i = 0; i < size; i++) {
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
	void loadTunings(const char *path);
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
		loadTunings("WK_Standard.tunings");
		loadTunings("WK_Custom.tunings");
		tuningsLoaded = true;
	}
	void appendContextMenu(Menu *menu) override;
	void step() override;
};

void WK101::appendContextMenu(Menu *menu) {
	WK_101 *module = dynamic_cast<WK_101 *>(this->module);
	menu->addChild(MenuEntry::create());
	for (unsigned int i = 0; i < tunings.size(); i++) { 
		WK_MenuItem *m = MenuItem::create<WK_MenuItem>(tunings[i].name.c_str());
		m->module = module;
		m->index = i;
		menu->addChild(m);
	}
}

void WK101::loadTunings(const char *path) {
	if (tuningsLoaded)
		return;
	FILE *file = fopen(assetPlugin(plugin, path).c_str(), "r");
	if (!file) {
		return;
	}
	int defaultSize = tunings.size();
	
	json_error_t error;
	json_t *rootJ = json_loadf(file, 0, &error);
	if (rootJ) {
		int size = json_array_size(rootJ);
		for (int i = 0; i < size; i++) {
			json_t *j0 = json_array_get(rootJ, i);
			if (j0) {
				json_t *jname = json_object_get(j0, "name");
				if (jname) {
					json_t *joffsets = json_object_get(j0, "tunings");
					if (joffsets) {
						tunings.push_back(WK_Tuning());
						tunings[i + defaultSize].name.assign(json_string_value(jname));
						int tsize = json_array_size(joffsets);
						for (int j = 0; j < 12; j++) {
							if (j < tsize) {
								json_t *joffset = json_array_get(joffsets, j);
								if (joffset) {
									tunings[i + defaultSize].offsets[j] = json_number_value(joffset);
								}
							}
							else {
								tunings[i + defaultSize].offsets[j] = 0.0f;
							}
						}
					}	
				}
			}
		}
		json_decref(rootJ);
	}
	else {
		std::string message = stringf("SubmarineFree WK: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
		debug(message.c_str());
	}
	fclose(file);
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