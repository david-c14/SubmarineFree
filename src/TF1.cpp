//SubTag W6

#include "SubmarineFree.hpp"
#include "torpedo.hpp"

struct TF_101 : Module  {
	enum ParamIds {
		PARAM_FG_RED,
		PARAM_FG_GREEN,
		PARAM_FG_BLUE,
		PARAM_BG_RED,
		PARAM_BG_GREEN,
		PARAM_BG_BLUE,
		PARAM_FONT_SIZE,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_FG_RED,
		INPUT_FG_GREEN,
		INPUT_FG_BLUE,
		INPUT_BG_RED,
		INPUT_BG_GREEN,
		INPUT_BG_BLUE,
		INPUT_FONT_SIZE,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_TOR,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_FG_RED,
		LIGHT_FG_GREEN,
		LIGHT_FG_BLUE,
		LIGHT_BG_RED,
		LIGHT_BG_GREEN,
		LIGHT_BG_BLUE,
		NUM_LIGHTS
	};

	float prevValues[7];
	int isDirty = false;
	Torpedo::PatchOutputPort outPort = Torpedo::PatchOutputPort(this, OUTPUT_TOR);	
	TF_101() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_FG_RED, 0.0f, 1.0f, 0.1569f);
		configParam(PARAM_FG_GREEN, 0.0f, 1.0f, 0.6902f);
		configParam(PARAM_FG_BLUE, 0.0f, 1.0f, 0.9529f);
		configParam(PARAM_BG_RED, 0.0f, 1.0f, 0.0f);
		configParam(PARAM_BG_GREEN, 0.0f, 1.0f, 0.0f);
		configParam(PARAM_BG_BLUE, 0.0f, 1.0f, 0.0f);
		configParam(PARAM_FONT_SIZE, 6.0f, 26.0f, 12.0f);
		prevValues[0] = 0.1569f;
		prevValues[1] = 0.6902f;
		prevValues[2] = 0.9529f;
		prevValues[6] = 12.0f;
		outPort.size(1);
	}
	void process(const ProcessArgs &args) override;
	std::string encodeColor(float r, float g, float b) {
		std::string out;
		out.push_back('A'+(int)(r * 255) / 16);	
		out.push_back('A'+(int)(r * 255) % 16);	
		out.push_back('A'+(int)(g * 255) / 16);	
		out.push_back('A'+(int)(g * 255) % 16);	
		out.push_back('A'+(int)(b * 255) / 16);	
		out.push_back('A'+(int)(b * 255) % 16);	
	
		return out;
	}
};

void TF_101::process(const ProcessArgs &args) {
	for (int i = 0; i < 6; i++) {
		float newValue = clamp(params[PARAM_FG_RED + i].getValue() + inputs[INPUT_FG_RED + i].getVoltage() / 10.0f, 0.0f, 1.0f); 
		lights[LIGHT_FG_RED + i].setBrightness(newValue); 
		if (prevValues[i] != newValue) {
			isDirty = true;
			prevValues[i] = newValue;
		}
	}
	float newValue = clamp(params[PARAM_FONT_SIZE].getValue() + inputs[INPUT_FONT_SIZE].getVoltage() * 2.0f, 6.0f, 26.0f); 
	if (prevValues[6] != newValue) {
		isDirty = true;
		prevValues[6] = newValue;
	}
	if (isDirty) {
		isDirty = false;
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "fg", json_string(encodeColor(prevValues[0], prevValues[1], prevValues[2]).c_str()));
		json_object_set_new(rootJ, "bg", json_string(encodeColor(prevValues[3], prevValues[4], prevValues[5]).c_str()));
		json_object_set_new(rootJ, "size", json_real(prevValues[6]));
		outPort.send("SubmarineFree", "TDNotesColor", rootJ);
	}
	outPort.process();
}

struct RGBLight : GrayModuleLightWidget {
	RGBLight() {
		addBaseColor(nvgRGB(0xff, 0x00, 0x00));
		addBaseColor(nvgRGB(0x00, 0xff, 0x00));
		addBaseColor(nvgRGB(0x00, 0x00, 0xff));
	}
};


struct TF101 : SchemeModuleWidget {
	TF101(TF_101 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(16.5,79), module, TF_101::INPUT_FG_RED));
		addInput(createInputCentered<SilverPort>(Vec(16.5,119), module, TF_101::INPUT_FG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(16.5,159), module, TF_101::INPUT_FG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(16.5,213), module, TF_101::INPUT_BG_RED));
		addInput(createInputCentered<SilverPort>(Vec(16.5,253), module, TF_101::INPUT_BG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(16.5,293), module, TF_101::INPUT_BG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(16.5,347), module, TF_101::INPUT_FONT_SIZE));

		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 79), module, TF_101::PARAM_FG_RED));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 119), module, TF_101::PARAM_FG_GREEN));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 159), module, TF_101::PARAM_FG_BLUE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 213), module, TF_101::PARAM_BG_RED));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 253), module, TF_101::PARAM_BG_GREEN));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 293), module, TF_101::PARAM_BG_BLUE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 347), module, TF_101::PARAM_FONT_SIZE));

		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(14.5, 55.5), module, TF_101::LIGHT_FG_RED));
		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(14.5, 189.5), module, TF_101::LIGHT_BG_RED));

		addOutput(createOutputCentered<BlackPort>(Vec(73.5,31.5), module, TF_101::OUTPUT_TOR));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TF-101");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);		
		nvgMoveTo(vg, 4, 47.5);
		nvgLineTo(vg, 86, 47.5);
		nvgMoveTo(vg, 4, 181.5);
		nvgLineTo(vg, 86, 181.5);
		nvgMoveTo(vg, 4, 315.5);
		nvgLineTo(vg, 86, 315.5);
		nvgStroke(vg);
		drawText(vg, 59, 35, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT");
		drawText(vg, 55, 57, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "FOREGROUND");
		drawText(vg, 55, 191, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BACKGROUND");
		drawText(vg, 55, 325, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TEXT-SIZE");
		drawText(vg, 38, 82, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "R");
		drawText(vg, 38, 122, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "G");
		drawText(vg, 38, 162, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "B");
		drawText(vg, 38, 216, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "R");
		drawText(vg, 38, 256, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "G");
		drawText(vg, 38, 296, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "B");
	}
};

Model *modelTF101 = createModel<TF_101, TF101>("TF-101");
