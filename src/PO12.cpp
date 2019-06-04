//SubTag W12 W10

#include "SubmarineFree.hpp"

struct PO_Util {
	static constexpr float deg0   = 0.0f;
	static constexpr float deg30  =         M_PI / 6.0f;
	static constexpr float deg45  =         M_PI / 4.0f;
	static constexpr float deg60  =         M_PI / 3.0f;
	static constexpr float deg90  =         M_PI / 2.0f;
	static constexpr float deg120 = 2.0f  * M_PI / 3.0f;
	static constexpr float deg135 = 3.0f  * M_PI / 4.0f;
	static constexpr float deg150 = 5.0f  * M_PI / 6.0f;
	static constexpr float ph0 = 0.0f;
	static constexpr float ph30 = 1.0f / 12.0f;
	static constexpr float ph45 = 0.125f;
	static constexpr float ph60 = 1.0f / 6.0f;
	static constexpr float ph90 = 0.25f;
	static constexpr float ph120 = 1.0f / 3.0f;
	static constexpr float ph135 = 0.375f;
	static constexpr float ph150 = 5.0f / 12.0f;
	static constexpr float ph180 = 0.5f;
	static constexpr float ph210 = 7.0f / 12.0f;
	static constexpr float ph225 = 0.625;
	static constexpr float ph240 = 2.0f / 3.0f;
	static constexpr float ph270 = 0.75f;
	static constexpr float ph300 = 5.0f / 6.0f;
	static constexpr float ph315 = 0.875f;
	static constexpr float ph330 = 11.0f / 12.0f;

	float sin(float phase);
	float tri(float phase);
	float saw(float phase);
	float sqr(float phase);
	float rsn(float phase);
};

float PO_Util::sin(float phase) {
	return 5.0f * sinf(phase);
}

float PO_Util::tri(float phase) {
	phase -= floor(phase);
	if (phase < 0.25f)
		return 20.0f * phase;
	if (phase < 0.75f)
		return 20.0f * (0.5f - phase);
	return 20.0f * (phase - 1.0f);
}

float PO_Util::saw(float phase) {
	phase -= floor(phase);
	if (phase < 0.5f)
		return 10.0f * phase;
	return 10.0f * (phase - 1.0f);
}

float PO_Util::sqr(float phase) {
	phase -= floor(phase);
	return (phase < 0.5f)?5.0f:-5.0f;
}

float PO_Util::rsn(float phase) {
	return 10.0f * fabs(sinf(phase)) - 5.0f; 
}

struct PO_101 : Module, PO_Util {
	
	enum ParamIds {
		PARAM_TUNE,
		PARAM_FINE,
		PARAM_WAVE,
		PARAM_PHASE_1,
		PARAM_PHASE_2,
		PARAM_PHASE_3,
		PARAM_PHASE_4,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_NOTE_CV,
		INPUT_PHASE_1,
		INPUT_PHASE_2,
		INPUT_PHASE_3,
		INPUT_PHASE_4,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		OUTPUT_2,
		OUTPUT_3,
		OUTPUT_4,
		OUTPUT_5,
		OUTPUT_6,
		OUTPUT_7,
		OUTPUT_8,
		OUTPUT_9,
		OUTPUT_10,
		OUTPUT_11,
		OUTPUT_12,
		OUTPUT_13,
		OUTPUT_14,
		OUTPUT_15,
		OUTPUT_16,
		OUTPUT_17,
		OUTPUT_18,
		OUTPUT_19,
		OUTPUT_20,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	PO_101() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void step() override;
	void sin(float phase);
	void tri(float phase);
	void saw(float phase);
	void sqr(float phase);
	void rsn(float phase);
	float phase = 0.0f;
	float baseFreq = 261.626f;
};

void PO_101::sin(float phase) {
	phase *= (2 * M_PI);
	if (outputs[OUTPUT_1].active || outputs[OUTPUT_9].active)
		outputs[OUTPUT_9].value = -(outputs[OUTPUT_1].value = PO_Util::sin(phase + deg0));
	if (outputs[OUTPUT_2].active || outputs[OUTPUT_10].active)
		outputs[OUTPUT_10].value = -(outputs[OUTPUT_2].value = PO_Util::sin(phase + deg30)); 
	if (outputs[OUTPUT_3].active || outputs[OUTPUT_11].active)
		outputs[OUTPUT_11].value = -(outputs[OUTPUT_3].value = PO_Util::sin(phase + deg45)); 
	if (outputs[OUTPUT_4].active || outputs[OUTPUT_12].active)
		outputs[OUTPUT_12].value = -(outputs[OUTPUT_4].value = PO_Util::sin(phase + deg60)); 
	if (outputs[OUTPUT_5].active || outputs[OUTPUT_13].active)
		outputs[OUTPUT_13].value = -(outputs[OUTPUT_5].value = PO_Util::sin(phase + deg90)); 
	if (outputs[OUTPUT_6].active || outputs[OUTPUT_14].active)
		outputs[OUTPUT_14].value = -(outputs[OUTPUT_6].value = PO_Util::sin(phase + deg120)); 
	if (outputs[OUTPUT_7].active || outputs[OUTPUT_15].active)
		outputs[OUTPUT_15].value = -(outputs[OUTPUT_7].value = PO_Util::sin(phase + deg135)); 
	if (outputs[OUTPUT_8].active || outputs[OUTPUT_16].active)
		outputs[OUTPUT_16].value = -(outputs[OUTPUT_8].value = PO_Util::sin(phase + deg150)); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_17 + i].active) {
			float offset = params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			offset *= 2 * M_PI;
			outputs[OUTPUT_17 + i].value = PO_Util::sin(phase + offset);
		}	
	}
}

void PO_101::tri(float phase) {
	if (outputs[OUTPUT_1].active || outputs[OUTPUT_9].active)
		outputs[OUTPUT_9].value = -(outputs[OUTPUT_1].value = PO_Util::tri(phase + ph0));
	if (outputs[OUTPUT_2].active || outputs[OUTPUT_10].active)
		outputs[OUTPUT_10].value = -(outputs[OUTPUT_2].value = PO_Util::tri(phase + ph30)); 
	if (outputs[OUTPUT_3].active || outputs[OUTPUT_11].active)
		outputs[OUTPUT_11].value = -(outputs[OUTPUT_3].value = PO_Util::tri(phase + ph45)); 
	if (outputs[OUTPUT_4].active || outputs[OUTPUT_12].active)
		outputs[OUTPUT_12].value = -(outputs[OUTPUT_4].value = PO_Util::tri(phase + ph60)); 
	if (outputs[OUTPUT_5].active || outputs[OUTPUT_13].active)
		outputs[OUTPUT_13].value = -(outputs[OUTPUT_5].value = PO_Util::tri(phase + ph90)); 
	if (outputs[OUTPUT_6].active || outputs[OUTPUT_14].active)
		outputs[OUTPUT_14].value = -(outputs[OUTPUT_6].value = PO_Util::tri(phase + ph120)); 
	if (outputs[OUTPUT_7].active || outputs[OUTPUT_15].active)
		outputs[OUTPUT_15].value = -(outputs[OUTPUT_7].value = PO_Util::tri(phase + ph135)); 
	if (outputs[OUTPUT_8].active || outputs[OUTPUT_16].active)
		outputs[OUTPUT_16].value = -(outputs[OUTPUT_8].value = PO_Util::tri(phase + ph150)); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_17 + i].active) {
			float offset = params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			outputs[OUTPUT_17 + i].value = PO_Util::tri(phase + offset);
		}	
	}
}

void PO_101::saw(float phase) {
	if (outputs[OUTPUT_1].active)
		outputs[OUTPUT_1].value = PO_Util::saw(phase + ph0);
	if (outputs[OUTPUT_2].active)
		outputs[OUTPUT_2].value = PO_Util::saw(phase + ph30); 
	if (outputs[OUTPUT_3].active)
		outputs[OUTPUT_3].value = PO_Util::saw(phase + ph45); 
	if (outputs[OUTPUT_4].active)
		outputs[OUTPUT_4].value = PO_Util::saw(phase + ph60); 
	if (outputs[OUTPUT_5].active)
		outputs[OUTPUT_5].value = PO_Util::saw(phase + ph90); 
	if (outputs[OUTPUT_6].active)
		outputs[OUTPUT_6].value = PO_Util::saw(phase + ph120); 
	if (outputs[OUTPUT_7].active)
		outputs[OUTPUT_7].value = PO_Util::saw(phase + ph135); 
	if (outputs[OUTPUT_8].active)
		outputs[OUTPUT_8].value = PO_Util::saw(phase + ph150); 
	if (outputs[OUTPUT_9].active)
		outputs[OUTPUT_9].value = PO_Util::saw(phase + ph180);
	if (outputs[OUTPUT_10].active)
		outputs[OUTPUT_10].value = PO_Util::saw(phase + ph210); 
	if (outputs[OUTPUT_11].active)
		outputs[OUTPUT_11].value = PO_Util::saw(phase + ph225); 
	if (outputs[OUTPUT_12].active)
		outputs[OUTPUT_12].value = PO_Util::saw(phase + ph240); 
	if (outputs[OUTPUT_13].active)
		outputs[OUTPUT_13].value = PO_Util::saw(phase + ph270); 
	if (outputs[OUTPUT_14].active)
		outputs[OUTPUT_14].value = PO_Util::saw(phase + ph300); 
	if (outputs[OUTPUT_15].active)
		outputs[OUTPUT_15].value = PO_Util::saw(phase + ph315); 
	if (outputs[OUTPUT_16].active)
		outputs[OUTPUT_16].value = PO_Util::saw(phase + ph330); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_17 + i].active) {
			float offset = params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			outputs[OUTPUT_17 + i].value = PO_Util::saw(phase + offset);
		}	
	}
}

void PO_101::sqr(float phase) {
	if (outputs[OUTPUT_1].active || outputs[OUTPUT_9].active)
		outputs[OUTPUT_9].value = -(outputs[OUTPUT_1].value = PO_Util::sqr(phase + ph0));
	if (outputs[OUTPUT_2].active || outputs[OUTPUT_10].active)
		outputs[OUTPUT_10].value = -(outputs[OUTPUT_2].value = PO_Util::sqr(phase + ph30)); 
	if (outputs[OUTPUT_3].active || outputs[OUTPUT_11].active)
		outputs[OUTPUT_11].value = -(outputs[OUTPUT_3].value = PO_Util::sqr(phase + ph45)); 
	if (outputs[OUTPUT_4].active || outputs[OUTPUT_12].active)
		outputs[OUTPUT_12].value = -(outputs[OUTPUT_4].value = PO_Util::sqr(phase + ph60)); 
	if (outputs[OUTPUT_5].active || outputs[OUTPUT_13].active)
		outputs[OUTPUT_13].value = -(outputs[OUTPUT_5].value = PO_Util::sqr(phase + ph90)); 
	if (outputs[OUTPUT_6].active || outputs[OUTPUT_14].active)
		outputs[OUTPUT_14].value = -(outputs[OUTPUT_6].value = PO_Util::sqr(phase + ph120)); 
	if (outputs[OUTPUT_7].active || outputs[OUTPUT_15].active)
		outputs[OUTPUT_15].value = -(outputs[OUTPUT_7].value = PO_Util::sqr(phase + ph135)); 
	if (outputs[OUTPUT_8].active || outputs[OUTPUT_16].active)
		outputs[OUTPUT_16].value = -(outputs[OUTPUT_8].value = PO_Util::sqr(phase + ph150)); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_17 + i].active) {
			float offset = params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			outputs[OUTPUT_17 + i].value = PO_Util::sqr(phase + offset);
		}	
	}
}
	
void PO_101::rsn(float phase) {
	phase *= (2 * M_PI);
	if (outputs[OUTPUT_1].active || outputs[OUTPUT_9].active)
		outputs[OUTPUT_9].value = (outputs[OUTPUT_1].value = PO_Util::rsn(phase + deg0));
	if (outputs[OUTPUT_2].active || outputs[OUTPUT_10].active)
		outputs[OUTPUT_10].value = (outputs[OUTPUT_2].value = PO_Util::rsn(phase + deg30)); 
	if (outputs[OUTPUT_3].active || outputs[OUTPUT_11].active)
		outputs[OUTPUT_11].value = (outputs[OUTPUT_3].value = PO_Util::rsn(phase + deg45)); 
	if (outputs[OUTPUT_4].active || outputs[OUTPUT_12].active)
		outputs[OUTPUT_12].value = (outputs[OUTPUT_4].value = PO_Util::rsn(phase + deg60)); 
	if (outputs[OUTPUT_5].active || outputs[OUTPUT_13].active)
		outputs[OUTPUT_13].value = (outputs[OUTPUT_5].value = PO_Util::rsn(phase + deg90)); 
	if (outputs[OUTPUT_6].active || outputs[OUTPUT_14].active)
		outputs[OUTPUT_14].value = (outputs[OUTPUT_6].value = PO_Util::rsn(phase + deg120)); 
	if (outputs[OUTPUT_7].active || outputs[OUTPUT_15].active)
		outputs[OUTPUT_15].value = (outputs[OUTPUT_7].value = PO_Util::rsn(phase + deg135)); 
	if (outputs[OUTPUT_8].active || outputs[OUTPUT_16].active)
		outputs[OUTPUT_16].value = (outputs[OUTPUT_8].value = PO_Util::rsn(phase + deg150)); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_17 + i].active) {
			float offset = params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			offset *= 2 * M_PI;
			outputs[OUTPUT_17 + i].value = PO_Util::rsn(phase + offset);
		}	
	}
}

void PO_101::step() {
	float freq = baseFreq * powf(2.0f, (params[PARAM_TUNE].value + 3.0f * quadraticBipolar(params[PARAM_FINE].value)) / 12.0f + (inputs[INPUT_NOTE_CV].active?inputs[INPUT_NOTE_CV].value:0.0f));
	float deltaTime = freq / engineGetSampleRate();
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart); 
	
	{
		float waveShape = clamp(params[PARAM_WAVE].value, 0.0f, 4.0f);
		if (waveShape < 0.5f)
			sin(phase);
		else if (waveShape < 1.5f)
			tri(phase);
		else if (waveShape < 2.5f)
			saw(phase);
		else if (waveShape < 3.5f)
			sqr(phase);
		else
			rsn(phase);
	}

}

struct PO_204 : Module, PO_Util {
	
	enum ParamIds {
		PARAM_TUNE,
		PARAM_FINE,
		PARAM_WAVE_1,
		PARAM_WAVE_2,
		PARAM_WAVE_3,
		PARAM_WAVE_4,
		PARAM_PHASE_1,
		PARAM_PHASE_2,
		PARAM_PHASE_3,
		PARAM_PHASE_4,
		PARAM_MULT_1,
		PARAM_MULT_2,
		PARAM_MULT_3,
		PARAM_MULT_4,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_TUNE,
		INPUT_WAVE_1,
		INPUT_WAVE_2,
		INPUT_WAVE_3,
		INPUT_WAVE_4,
		INPUT_PHASE_1,
		INPUT_PHASE_2,
		INPUT_PHASE_3,
		INPUT_PHASE_4,
		INPUT_MULT_1,
		INPUT_MULT_2,
		INPUT_MULT_3,
		INPUT_MULT_4,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		OUTPUT_2,
		OUTPUT_3,
		OUTPUT_4,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	PO_204() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void step() override;
	float phase = 0.0f;
	float baseFreq = 261.626f;
};

void PO_204::step() {
	float freq = baseFreq * powf(2.0f, (params[PARAM_TUNE].value + 3.0f * quadraticBipolar(params[PARAM_FINE].value)) / 12.0f + (inputs[INPUT_TUNE].active?inputs[INPUT_TUNE].value:0.0f));
	float deltaTime = freq / engineGetSampleRate();
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_1 + i].active) {
			float offset = phase + params[PARAM_PHASE_1 + i].value;
			if (inputs[INPUT_PHASE_1 + i].active)
				offset += inputs[INPUT_PHASE_1 + i].value * 0.4f;
			offset *= floor(clamp(params[PARAM_MULT_1 + i].value + (inputs[INPUT_MULT_1 + i].active?inputs[INPUT_MULT_1 + i].value:0.0f) * 16.0f / 10.0f, 1.0f, 16.5f));
			float wave = params[PARAM_WAVE_1 + i].value + (inputs[INPUT_WAVE_1 + i].active?inputs[INPUT_WAVE_1 + i].value:0.0f);
			double waveSection;
			wave = modf(clamp(wave, 0.0f, 10.0f), &waveSection);		
			float w1 = 0.0f;
			float w2 = 0.0f;
			switch ((int)waveSection) {
				case 0:
					w1 = PO_Util::sin(offset * 2 * M_PI);
					w2 = PO_Util::saw(offset);
					break;
				case 1:
					w1 = PO_Util::saw(offset);
					w2 = PO_Util::rsn(offset * 2 * M_PI);
					break;
				case 2:
					w1 = PO_Util::rsn(offset * 2 * M_PI);
					w2 = PO_Util::tri(offset);
					break;
				case 3:
					w1 = PO_Util::tri(offset);
					w2 = PO_Util::sqr(offset);
					break;
				case 4:
					w1 = PO_Util::sqr(offset);
					w2 = PO_Util::sin(offset * 2 * M_PI);
					break;
				case 5:
					w1 = PO_Util::sin(offset * 2 * M_PI);
					w2 = PO_Util::tri(offset);
					break;
				case 6:
					w1 = PO_Util::tri(offset);
					w2 = PO_Util::saw(offset);
					break;
				case 7:
					w1 = PO_Util::saw(offset);
					w2 = PO_Util::sqr(offset);
					break;
				case 8:
					w1 = PO_Util::sqr(offset);
					w2 = PO_Util::rsn(offset * 2 * M_PI);
					break;
				case 9:
					w1 = PO_Util::rsn(offset * 2 * M_PI);
					w2 = PO_Util::sin(offset * 2 * M_PI);
					break;
				default:
					w2 = w1 = PO_Util::sin(offset * 2 * M_PI);
					break;
			}
			outputs[OUTPUT_1 + i].value = w1 * (1.0f - wave) + w2 * wave;
		}	
	}
}

struct PO_Layout : SchemeModuleWidget {
	PO_Layout(PO_101 *module) : SchemeModuleWidget(module) {}
	void Layout() {
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(85, 58), module, PO_101::PARAM_FINE));
		addParam(createParamCentered<NarrowKnob<SnapKnob<MedKnob<LightKnob>>>>(Vec(140, 58), module, PO_101::PARAM_WAVE));
		if (module) {
			module->configParam(PO_101::PARAM_FINE, -1.0f, +1.0f, 0.0f);
			module->configParam(PO_101::PARAM_WAVE, 0.0f, +4.0f, 0.0f);
		}

		addInput(createInputCentered<SilverPort>(Vec(57.5,31.5), module, PO_101::INPUT_NOTE_CV));

		addOutput(createOutputCentered<SilverPort>(Vec(90,112.5), module, PO_101::OUTPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(122.5,121.5), module, PO_101::OUTPUT_2));
		addOutput(createOutputCentered<SilverPort>(Vec(155,112.5), module, PO_101::OUTPUT_3));
		addOutput(createOutputCentered<SilverPort>(Vec(146,145), module, PO_101::OUTPUT_4));
		addOutput(createOutputCentered<SilverPort>(Vec(155,177.5), module, PO_101::OUTPUT_5));
		addOutput(createOutputCentered<SilverPort>(Vec(146,210), module, PO_101::OUTPUT_6));
		addOutput(createOutputCentered<SilverPort>(Vec(155,242.5), module, PO_101::OUTPUT_7));
		addOutput(createOutputCentered<SilverPort>(Vec(122.5,233.5), module, PO_101::OUTPUT_8));
		addOutput(createOutputCentered<SilverPort>(Vec(90,242.5), module, PO_101::OUTPUT_9));
		addOutput(createOutputCentered<SilverPort>(Vec(57.5,233.5), module, PO_101::OUTPUT_10));
		addOutput(createOutputCentered<SilverPort>(Vec(25,242.5), module, PO_101::OUTPUT_11));
		addOutput(createOutputCentered<SilverPort>(Vec(34,210), module, PO_101::OUTPUT_12));
		addOutput(createOutputCentered<SilverPort>(Vec(25,177.5), module, PO_101::OUTPUT_13));
		addOutput(createOutputCentered<SilverPort>(Vec(34,145), module, PO_101::OUTPUT_14));
		addOutput(createOutputCentered<SilverPort>(Vec(25,112.5), module, PO_101::OUTPUT_15));
		addOutput(createOutputCentered<SilverPort>(Vec(57.5,121.5), module, PO_101::OUTPUT_16));

		for (int i = 0; i < 4; i++) {
			addInput(createInputCentered<SilverPort>(Vec(22.5 + 45 * i,272.5), module, PO_101::INPUT_PHASE_1 + i));
			addParam(createParamCentered<MedKnob<LightKnob>>(Vec(22.5 + 45 * i, 309), module, PO_101::PARAM_PHASE_1 + i));
			if (module) {
				module->configParam(PO_101::PARAM_PHASE_1 + i, -1.0f, +1.0f, 0.0f);
			}
			addOutput(createOutputCentered<SilverPort>(Vec(22.5 + 45 * i,345.5), module, PO_101::OUTPUT_17 + i));
		}
	}
	void LayoutScreen(NVGcontext *vg) {
		nvgFillColor(vg, gScheme.background);
		nvgStrokeColor(vg, gScheme.contrast);
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		for (unsigned int i = 0; i < 4; i++) {
			nvgMoveTo(vg, 22.5 + 45 * i, 272.5);
			nvgLineTo(vg, 22.5 + 45 * i, 345.5);
		}
		nvgMoveTo(vg, 155, 112.5);
		nvgLineTo(vg, 25, 242.5);
		nvgMoveTo(vg, 25, 112.5);
		nvgLineTo(vg, 155, 242.5);
		nvgMoveTo(vg, 115, 78);
		nvgBezierTo(vg, 115, 75, 120, 75, 120, 78);
		nvgBezierTo(vg, 120, 81, 125, 81, 125, 78);
		nvgMoveTo(vg, 109, 47);
		nvgLineTo(vg, 111.5, 44.5);
		nvgLineTo(vg, 116.5, 49.5);
		nvgLineTo(vg, 119, 47);
		nvgMoveTo(vg, 135, 29.5);
		nvgLineTo(vg, 135, 32);
		nvgLineTo(vg, 145, 27);
		nvgLineTo(vg, 145, 29.5);
		nvgMoveTo(vg, 161, 47);
		nvgLineTo(vg, 161, 44.5);
		nvgLineTo(vg, 166, 44.5);
		nvgLineTo(vg, 166, 49.5);
		nvgLineTo(vg, 171, 49.5);
		nvgLineTo(vg, 171, 47);
		nvgMoveTo(vg, 155, 80.5);
		nvgBezierTo(vg, 155, 74.5, 160, 74.5, 160, 80.5);
		nvgBezierTo(vg, 160, 74.5, 165, 74.5, 165, 80.5);
		nvgStroke(vg);
		nvgBeginPath(vg);
		nvgCircle(vg, 90, 177.5, 65);
		nvgFill(vg);
		nvgStroke(vg);

		drawText(vg, 30, 89, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "TUNE");
		drawText(vg, 85, 89, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "FINE");
		drawText(vg, 90, 177, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "PHASED");
		drawText(vg, 90, 187, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "OUTPUT");
		drawText(vg, 90, 135, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "0");
		drawText(vg, 90, 229, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "\xcf\x80");
		drawText(vg, 38, 182, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "3\xcf\x80/2");
		drawText(vg, 142, 182, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "\xcf\x80/2");
	}
};

struct PO101 : PO_Layout {
	PO101(PO_101 *module) : PO_Layout(module) {
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParam<MedKnob<LightKnob>>(Vec(11, 39), module, PO_101::PARAM_TUNE));
		if (module) {
			module->configParam(PO_101::PARAM_TUNE, -54.0f, +54.0f, 0.0f);
		}
		Layout();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-101");
		LayoutScreen(vg);
	}
};

struct PO102 : PO_Layout {
	PO102(PO_101 *module) : PO_Layout(module) {
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParam<MedKnob<LightKnob>>(Vec(11, 39), module, PO_101::PARAM_TUNE));
		if (module) {
			module->configParam(PO_101::PARAM_TUNE, -96.0f, 72.0f, -12.0f);
		}
		module->baseFreq = 1.0f;
		Layout();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-102");
		LayoutScreen(vg);
	}
};

struct PO204 : SchemeModuleWidget {
	PO204(PO_204 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(79, 38), module, PO_204::PARAM_TUNE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(124, 38), module, PO_204::PARAM_FINE));
		addInput(createInputCentered<SilverPort>(Vec(30, 38), module, PO_204::INPUT_TUNE));
		if (module) {
			module->configParam(PO_204::PARAM_TUNE, -90.0f, +54.0f, 0.0f);
			module->configParam(PO_204::PARAM_FINE, -1.0f, +1.0f, 0.0f);
		}

		for (int i = 0; i < 4; i++) {
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(17, 101 + 70 * i), module, PO_204::PARAM_WAVE_1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(57, 101 + 70 * i), module, PO_204::PARAM_PHASE_1 + i));
			addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(97, 101 + 70 * i), module, PO_204::PARAM_MULT_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(17, 137.5 + 70 * i), module, PO_204::INPUT_WAVE_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(57, 137.5 + 70 * i), module, PO_204::INPUT_PHASE_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(97, 137.5 + 70 * i), module, PO_204::INPUT_MULT_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(133, 137.5 + 70 * i), module, PO_204::OUTPUT_1 + i));
			if (module) {
				module->configParam(PO_204::PARAM_WAVE_1 + i, 0.0f, 10.0f, 5.0f);
				module->configParam(PO_204::PARAM_PHASE_1 + i, -1.0f, +1.0f, 0.0f);
				module->configParam(PO_204::PARAM_MULT_1 + i, 1.0f, 16.0f, 1.0f);
			}
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-204");
		nvgStrokeColor(vg, gScheme.alternative);
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);

		nvgMoveTo(vg, 14.5, 79.5);
		nvgLineTo(vg, 17.5, 79.5);

		nvgMoveTo(vg, 27.5, 79.5);
		nvgLineTo(vg, 30.5, 79.5);

		nvgMoveTo(vg, 40.5, 79.5);
		nvgLineTo(vg, 43.5, 79.5);

		nvgMoveTo(vg, 53.5, 79.5);
		nvgLineTo(vg, 56.5, 79.5);

		nvgMoveTo(vg, 66.5, 79.5);
		nvgLineTo(vg, 70, 79.5);

		nvgMoveTo(vg, 80, 79.5);
		nvgLineTo(vg, 83.5, 79.5);

		nvgMoveTo(vg, 93.5, 79.5);
		nvgLineTo(vg, 96.5, 79.5);

		nvgMoveTo(vg, 106.5, 79.5);
		nvgLineTo(vg, 109.5, 79.5);

		nvgMoveTo(vg, 119.5, 79.5);
		nvgLineTo(vg, 122.5, 79.5);

		nvgMoveTo(vg, 132.5, 79.5);
		nvgLineTo(vg, 135.5, 79.5);

		nvgStroke(vg);
		nvgStrokeColor(vg, gScheme.contrast);
		nvgBeginPath(vg);

		nvgMoveTo(vg, 4.5, 79.5);
		nvgBezierTo(vg, 4.5, 75.5, 9.5, 75.5, 9.5, 79.5);
		nvgBezierTo(vg, 9.5, 83.5, 14.5, 83.5, 14.5, 79.5);

		nvgMoveTo(vg, 17.5, 79.5);
		nvgLineTo(vg, 22.5, 76.5);
		nvgLineTo(vg, 22.5, 82.5);
		nvgLineTo(vg, 27.5, 79.5);

		nvgMoveTo(vg, 30.5, 79.5);
		nvgBezierTo(vg, 30.5, 75.5, 35.5, 77.5, 35.5, 83.5);
		nvgBezierTo(vg, 35.5, 77.5, 40.5, 75.5, 40.5, 79.5);

		nvgMoveTo(vg, 43.5, 79.5);
		nvgLineTo(vg, 46, 76.5);
		nvgLineTo(vg, 51, 82.5);
		nvgLineTo(vg, 53.5, 79.5);

		nvgMoveTo(vg, 56.5, 79.5);
		nvgLineTo(vg, 56.5, 76.5);
		nvgLineTo(vg, 61.5, 76.5);
		nvgLineTo(vg, 61.5, 82.5);
		nvgLineTo(vg, 66.5, 82.5);
		nvgLineTo(vg, 66.5, 79.5);

		nvgMoveTo(vg, 70, 79.5);
		nvgBezierTo(vg, 70, 75.5, 75, 75.5, 75, 79.5);
		nvgBezierTo(vg, 75, 83.5, 80, 83.5, 80, 79.5);

		nvgMoveTo(vg, 83.5, 79.5);
		nvgLineTo(vg, 86, 76.5);
		nvgLineTo(vg, 91, 82.5);
		nvgLineTo(vg, 93.5, 79.5);

		nvgMoveTo(vg, 96.5, 79.5);
		nvgLineTo(vg, 101.5, 76.5);
		nvgLineTo(vg, 101.5, 82.5);
		nvgLineTo(vg, 106.5, 79.5);

		nvgMoveTo(vg, 109.5, 79.5);
		nvgLineTo(vg, 109.5, 76.5);
		nvgLineTo(vg, 114.5, 76.5);
		nvgLineTo(vg, 114.5, 82.5);
		nvgLineTo(vg, 119.5, 82.5);
		nvgLineTo(vg, 119.5, 79.5);

		nvgMoveTo(vg, 122.5, 79.5);
		nvgBezierTo(vg, 122.5, 75.5, 127.5, 77.5, 127.5, 83.5);
		nvgBezierTo(vg, 127.5, 77.5, 132.5, 75.5, 132.5, 79.5);

		nvgMoveTo(vg, 135.5, 79.5);
		nvgBezierTo(vg, 135.5, 75.5, 140.5, 75.5, 140.5, 79.5);
		nvgBezierTo(vg, 140.5, 83.5, 145.5, 83.5, 145.5, 79.5);

		for (unsigned int i = 0; i < 3; i++) {
			nvgMoveTo(vg, 3, 154.5 + 70 * i);
			nvgLineTo(vg, 147, 154.5 + 70 * i);
		}

		nvgStroke(vg);
		
		drawText(vg, 30.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "V/Oct");
		drawText(vg, 79.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "TUNE");
		drawText(vg, 124.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "FINE");
		for (unsigned int i = 0; i < 4; i++) {
			drawText(vg, 17, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "WAVE");
			drawText(vg, 57, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "PHASE");
			drawText(vg, 97, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "MULT");
			drawText(vg, 133, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "OUT");
		}

	}
};

Model *modelPO101 = createModel<PO_101, PO101>("PO-101");
Model *modelPO102 = createModel<PO_101, PO102>("PO-102");
Model *modelPO204 = createModel<PO_204, PO204>("PO-204");
