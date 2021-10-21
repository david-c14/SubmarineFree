//SubTag W2 W6

#include "SubmarineFree.hpp"

template<bool hasParams>
struct TF : Module  {
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

	float prevValues[7] {NAN,NAN,NAN,NAN,NAN,NAN,NAN};
	int isDirty = false;
	float messages[2][7] = {{NAN,NAN,NAN,NAN,NAN,NAN,NAN},{NAN,NAN,NAN,NAN,NAN,NAN,NAN}};
	TF() : Module() {
		config(hasParams * NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		if (hasParams) {
			configParam(PARAM_FG_RED, 0.0f, 1.0f, 0.1569f, "Foreground red", "%", 0.f, 100.f);
			configParam(PARAM_FG_GREEN, 0.0f, 1.0f, 0.6902f, "Foreground green", "%", 0.f, 100.f);
			configParam(PARAM_FG_BLUE, 0.0f, 1.0f, 0.9529f, "Foreground blue", "%", 0.f, 100.f);
			configParam(PARAM_BG_RED, 0.0f, 1.0f, 0.0f, "Background red", "%", 0.f, 100.f);
			configParam(PARAM_BG_GREEN, 0.0f, 1.0f, 0.0f, "Background green", "%", 0.f, 100.f);
			configParam(PARAM_BG_BLUE, 0.0f, 1.0f, 0.0f, "Background blue", "%", 0.f, 100.f);
			configParam(PARAM_FONT_SIZE, 6.0f, 26.0f, 12.0f, "Font size", "pts");
		}
		configInput(INPUT_FG_RED, "Foreground Red");
		configInput(INPUT_FG_GREEN, "Foreground Green");
		configInput(INPUT_FG_BLUE, "Foreground Blue");
		configInput(INPUT_BG_RED, "Background Red");
		configInput(INPUT_BG_GREEN, "Background Green");
		configInput(INPUT_BG_BLUE, "Background Blue");
		configInput(INPUT_FONT_SIZE, "Text Size");
		configOutput(OUTPUT_TOR, "Deprecated");
		configLight(LIGHT_FG_RED, "Foreground Colour");
		configLight(LIGHT_BG_RED, "Background Colour");
		prevValues[0] = 0.1569f;
		prevValues[1] = 0.6902f;
		prevValues[2] = 0.9529f;
		prevValues[6] = 12.0f;
		leftExpander.producerMessage = rightExpander.producerMessage = messages[0];
		leftExpander.consumerMessage = rightExpander.consumerMessage = messages[1];	
	}
	void process(const ProcessArgs &args) override {
		for (int i = 0; i < 6; i++) {
			float newValue = clamp(hasParams?params[PARAM_FG_RED + i].getValue():0 + inputs[INPUT_FG_RED + i].getVoltage() / 10.0f, 0.0f, 1.0f); 
			lights[LIGHT_FG_RED + i].setBrightness(newValue); 
			if (prevValues[i] != newValue) {
				isDirty = true;
				prevValues[i] = newValue;
			}
		}
		float newValue = clamp(hasParams?params[PARAM_FONT_SIZE].getValue():0 + inputs[INPUT_FONT_SIZE].getVoltage() * 2.0f, 6.0f, 26.0f); 
		if (prevValues[6] != newValue) {
			isDirty = true;
			prevValues[6] = newValue;
		}
		if (isDirty) {
			isDirty = false;
			json_t *rootJ = json_object();
			if (hasParams || inputs[INPUT_FG_RED].isConnected() || inputs[INPUT_FG_GREEN].isConnected() || inputs[INPUT_FG_BLUE].isConnected())
				json_object_set_new(rootJ, "fg", json_string(encodeColor(prevValues[0], prevValues[1], prevValues[2]).c_str()));
			if (hasParams || inputs[INPUT_BG_RED].isConnected() || inputs[INPUT_BG_GREEN].isConnected() || inputs[INPUT_BG_BLUE].isConnected())
				json_object_set_new(rootJ, "bg", json_string(encodeColor(prevValues[3], prevValues[4], prevValues[5]).c_str()));
			if (hasParams || inputs[INPUT_FONT_SIZE].isConnected())
				json_object_set_new(rootJ, "size", json_real(prevValues[6]));
		}
		float *message = (float *)(leftExpander.producerMessage);
		if (hasParams || inputs[INPUT_FG_RED].isConnected() || inputs[INPUT_FG_GREEN].isConnected() || inputs[INPUT_FG_BLUE].isConnected()) {
			message[0] = prevValues[0];
			message[1] = prevValues[1];
			message[2] = prevValues[2];
		}
		else {
			message[0] = NAN;
		}
		if (hasParams || inputs[INPUT_BG_RED].isConnected() || inputs[INPUT_BG_GREEN].isConnected() || inputs[INPUT_BG_BLUE].isConnected()) {
			message[3] = prevValues[3];
			message[4] = prevValues[4];
			message[5] = prevValues[5];
		}
		else {
			message[3] = NAN;
		}
		if (hasParams || inputs[INPUT_FONT_SIZE].isConnected()) {
			message[6] = prevValues[6];
		}
		else {
			message[6] = NAN;
		}
		leftExpander.messageFlipRequested = true;
		rightExpander.messageFlipRequested = true;
	}
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

namespace {
	struct RGBLight : GrayModuleLightWidget {
		RGBLight() {
			addBaseColor(nvgRGB(0xff, 0x00, 0x00));
			addBaseColor(nvgRGB(0x00, 0xff, 0x00));
			addBaseColor(nvgRGB(0x00, 0x00, 0xff));
		}
	};
} // end namespace

struct TF101 : SchemeModuleWidget {
	TF101(TF<true> *module) {
		setModule(module);
		this->box.size = Vec(90, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(16.5,79), module, TF<true>::INPUT_FG_RED));
		addInput(createInputCentered<SilverPort>(Vec(16.5,119), module, TF<true>::INPUT_FG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(16.5,159), module, TF<true>::INPUT_FG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(16.5,213), module, TF<true>::INPUT_BG_RED));
		addInput(createInputCentered<SilverPort>(Vec(16.5,253), module, TF<true>::INPUT_BG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(16.5,293), module, TF<true>::INPUT_BG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(16.5,347), module, TF<true>::INPUT_FONT_SIZE));

		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 79), module, TF<true>::PARAM_FG_RED));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 119), module, TF<true>::PARAM_FG_GREEN));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 159), module, TF<true>::PARAM_FG_BLUE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 213), module, TF<true>::PARAM_BG_RED));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 253), module, TF<true>::PARAM_BG_GREEN));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 293), module, TF<true>::PARAM_BG_BLUE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(65, 347), module, TF<true>::PARAM_FONT_SIZE));

		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(14.5, 55.5), module, TF<true>::LIGHT_FG_RED));
		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(14.5, 189.5), module, TF<true>::LIGHT_BG_RED));

		addOutput(createOutputCentered<DeprecatedPort>(Vec(73.5,31.5), module, TF<true>::OUTPUT_TOR));
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

struct TF102 : SchemeModuleWidget {
	TF102(TF<false> *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(15,90), module, TF<false>::INPUT_FG_RED));
		addInput(createInputCentered<SilverPort>(Vec(15,120), module, TF<false>::INPUT_FG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(15,150), module, TF<false>::INPUT_FG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(15,205), module, TF<false>::INPUT_BG_RED));
		addInput(createInputCentered<SilverPort>(Vec(15,235), module, TF<false>::INPUT_BG_GREEN));
		addInput(createInputCentered<SilverPort>(Vec(15,265), module, TF<false>::INPUT_BG_BLUE));
		addInput(createInputCentered<SilverPort>(Vec(15,315), module, TF<false>::INPUT_FONT_SIZE));

		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(15, 71), module, TF<true>::LIGHT_FG_RED));
		addChild(createLightCentered<MediumLight<RGBLight>>(Vec(15, 186), module, TF<true>::LIGHT_BG_RED));

		addOutput(createOutputCentered<DeprecatedPort>(Vec(15,40), module, TF<true>::OUTPUT_TOR));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TF-102");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);		
		nvgMoveTo(vg, 2, 55);
		nvgLineTo(vg, 28, 55);
		nvgMoveTo(vg, 2, 170);
		nvgLineTo(vg, 28, 170);
		nvgMoveTo(vg, 2, 285);
		nvgLineTo(vg, 28, 285);
		nvgStroke(vg);
		drawText(vg, 15, 65, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "FORE");
		drawText(vg, 15, 180, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BACK");
		drawText(vg, 15, 295, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SIZE");
	}
};

Model *modelTF101 = createModel<TF<true>, TF101>("TF-101");
Model *modelTF102 = createModel<TF<false>, TF102>("TF-102");
