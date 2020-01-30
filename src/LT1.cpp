//SubTag W20

#include <string.h>
#include "SubmarineFree.hpp"

namespace {
	struct RotaryKnob : TinyKnob<LightKnob> {
		std::function<void(RotaryKnob *)> rightClickHandler;
		void onButton(const event::Button &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
				e.consume(this);
				if (rightClickHandler) {
					rightClickHandler(this);
				}
				return;
			}
			LightKnob::onButton(e);
		}
		void onDoubleClick(const event::DoubleClick &e) override {
			e.consume(this);
			resetActionOverride();
		}
		void resetActionOverride() {
			if (paramQuantity) {
				float oldValue = paramQuantity->getValue();
				paramQuantity->reset();
				float newValue = paramQuantity->getValue();
		
				if (oldValue != newValue) {
					// Push ParamChange history action
					history::ParamChange* h = new history::ParamChange;
					h->name = "reset parameter";
					h->moduleId = paramQuantity->module->id;
					h->paramId = paramQuantity->paramId;
					h->oldValue = oldValue;
					h->newValue = newValue;
					APP->history->push(h);
				}
				this->oldValue = this->snapValue = paramQuantity->getValue();
			}
		}
	};
}

struct LT_116 : Module {
	enum ParamIds {
		PARAM_COEFF_1,
		PARAM_OUTPUT_CHANNELS = PARAM_COEFF_1 + 256,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	int numberOfInputs = 1;
	int numberOfOutputs = 16;
	
	LT_116() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				configParam(PARAM_COEFF_1 + i * 16 + j, -INFINITY, INFINITY, 0.0f, string::f("Coefficient [%d,%d]", i + 1, j + 1));
			}
		}
		configParam(PARAM_OUTPUT_CHANNELS, 1.0f, 16.0f, 16.0f, "Number of channels in output");
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		json_t *arr = json_array();
		for (unsigned int i = 0; i < 256; i++) {
			json_array_append_new(arr, json_real(params[PARAM_COEFF_1 + i].getValue()));
		}
		json_object_set_new(rootJ, "coefficients", arr);
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		json_t *arr = json_object_get(rootJ, "coefficients");
		if (arr) {
			int size = json_array_size(arr);
			if (size > 256)
				size = 256;
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					APP->engine->setParam(this, PARAM_COEFF_1 + i, json_real_value(j1));
				}
			}
		}
	}

	void process(const ProcessArgs &args) override {
		numberOfInputs = inputs[INPUT_1].getChannels();
		numberOfOutputs = params[PARAM_OUTPUT_CHANNELS].getValue();
		for (unsigned int i = 0; i < numberOfOutputs; i += 4) {
			float working[4] = {0};
			for (unsigned int j = 0; j < numberOfInputs; j++) {
				float input = inputs[INPUT_1].getVoltage(j);
				for (unsigned int n = 0; n < 4; n++) {
					working[n] += input * params[(i + n) * 16 + j].getValue();
				}
			}
			for (unsigned int n = 0; n < 4; n++) {
				outputs[OUTPUT_1].setVoltage(working[n], i + n);
			}
		}
		outputs[OUTPUT_1].setChannels(numberOfOutputs);
	}
};
	
struct LT116 : SchemeModuleWidget {
	RotaryKnob *knobs[256];
	LT116(LT_116 *module) {
		setModule(module);
		this->box.size = Vec(300, 380);
		addChild(new SchemePanel(this->box.size));

		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				knobs[i * 16 + j] = createParamCentered<RotaryKnob>(Vec(15 + i * 18, 30 + j * 18), module, LT_116::PARAM_COEFF_1 + i * 16 + j);
				knobs[i * 16 + j]->rightClickHandler = [=](RotaryKnob *knob) {
					addKnobMenu(knob);
				};
				addParam(knobs[i * 16 + j]);
			}
		}
		addInput(createInputCentered<SilverPort>(Vec(35, 330), module, LT_116::INPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(265, 330), module, LT_116::OUTPUT_1));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(200, 330), module, LT_116::PARAM_OUTPUT_CHANNELS));
	}
	void appendContextMenu(Menu *menu) override {
		if (!module)
			return;
		menu->addChild(new MenuSeparator);
	/*
		EventWidgetMenuItem *vmi = createMenuItem<EventWidgetMenuItem>("Match Cable Colors");
		vmi->stepHandler = [=]() {
			vmi->rightText = CHECKMARK(module->params[EO_102::PARAM_COLORS].getValue());
		};
		vmi->clickHandler = [=]() {
			bool val = module->params[EO_102::PARAM_COLORS].getValue();
			module->params[EO_102::PARAM_COLORS].setValue(!val);
		};
		menu->addChild(vmi);
*/
	}
	void step() override {
		if (!module)
			return;
		LT_116 *ltModule = dynamic_cast<LT_116 *>(module);
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				knobs[i * 16 + j]->setEnabled((j < ltModule->numberOfInputs) && (i < ltModule->numberOfOutputs));
			}
		}
		SchemeModuleWidget::step();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LT-116");

		// Text
		drawText(vg, 35, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 265, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUTPUT");
		drawText(vg, 200, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CHANNELS");
		drawText(vg, 50, 330, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x93"); 
		drawText(vg, 240, 330, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x92"); 
	}
	void addKnobMenu(RotaryKnob *knob) {
		if (!knob->paramQuantity)
			return;
		Menu *menu = createMenu();

		MenuLabel *label = new MenuLabel();
		label->text = knob->paramQuantity->getString();
		menu->addChild(label);	

		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(knob->paramQuantity->getDisplayValueString());
		paramField->changeHandler = [=](std::string text) {
			knob->paramQuantity->setDisplayValueString(text);
		};
		menu->addChild(paramField);

		EventWidgetMenuItem *entry = new EventWidgetMenuItem();
		entry->text = "Initialize";
		entry->rightText = "Double Click";
		entry->clickHandler = [=]() {
			knob->resetActionOverride();
		};
		menu->addChild(entry);

	}
};

Model *modelLT116 = createModel<LT_116, LT116>("LT-116");
