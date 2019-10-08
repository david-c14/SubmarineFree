//SubTag W10

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
		configParam(PARAM_TUNE, -90.0f, +54.0f, 0.0f, "Frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
		configParam(PARAM_FINE, -1.0f, +1.0f, 0.0f, "Fine frequency");
		for(unsigned int i = 0; i < 4; i++) {
			configParam(PARAM_WAVE_1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d wave shape", i + 1));
			configParam(PARAM_PHASE_1 + i, -1.0f, +1.0f, 0.0f, string::f("Operator #%d phase shift", i + 1), "\xc2\xb0", 0.f, 360.f);
			configParam(PARAM_MULT_1 + i, 1.0f, 16.0f, 1.0f, string::f("Operator #%d frequency multiplier", i + 1));
		}
	}
	void process(const ProcessArgs &args) override;
	float phase = 0.0f;
	float baseFreq = 261.626f;
};

void PO_204::process(const ProcessArgs &args) {
	float freq = baseFreq * powf(2.0f, (params[PARAM_TUNE].getValue() + 3.0f * dsp::quadraticBipolar(params[PARAM_FINE].getValue())) / 12.0f + (inputs[INPUT_TUNE].isConnected()?inputs[INPUT_TUNE].getVoltage():0.0f));
	float deltaTime = freq / args.sampleRate;
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart); 
	for (int i = 0; i < 4; i++) {
		if (outputs[OUTPUT_1 + i].isConnected()) {
			float offset = phase + params[PARAM_PHASE_1 + i].getValue();
			if (inputs[INPUT_PHASE_1 + i].isConnected())
				offset += inputs[INPUT_PHASE_1 + i].getVoltage() * 0.4f;
			offset *= floor(clamp(params[PARAM_MULT_1 + i].getValue() + (inputs[INPUT_MULT_1 + i].isConnected()?inputs[INPUT_MULT_1 + i].getVoltage():0.0f) * 16.0f / 10.0f, 1.0f, 16.5f));
			float wave = params[PARAM_WAVE_1 + i].getValue() + (inputs[INPUT_WAVE_1 + i].isConnected()?inputs[INPUT_WAVE_1 + i].getVoltage():0.0f);
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
			outputs[OUTPUT_1 + i].setVoltage(w1 * (1.0f - wave) + w2 * wave);
		}	
	}
}

struct PO204 : SchemeModuleWidget {
	PO204(PO_204 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(79, 38), module, PO_204::PARAM_TUNE));
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(124, 38), module, PO_204::PARAM_FINE));
		addInput(createInputCentered<SilverPort>(Vec(30, 38), module, PO_204::INPUT_TUNE));

		for (int i = 0; i < 4; i++) {
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(17, 101 + 70 * i), module, PO_204::PARAM_WAVE_1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(57, 101 + 70 * i), module, PO_204::PARAM_PHASE_1 + i));
			addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(97, 101 + 70 * i), module, PO_204::PARAM_MULT_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(17, 137.5 + 70 * i), module, PO_204::INPUT_WAVE_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(57, 137.5 + 70 * i), module, PO_204::INPUT_PHASE_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(97, 137.5 + 70 * i), module, PO_204::INPUT_MULT_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(133, 137.5 + 70 * i), module, PO_204::OUTPUT_1 + i));
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-204");
		nvgStrokeColor(vg, gScheme.getAlternative(module));
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
		nvgStrokeColor(vg, gScheme.getContrast(module));
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
		
		drawText(vg, 30.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "V/Oct");
		drawText(vg, 79.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "TUNE");
		drawText(vg, 124.5, 67, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "FINE");
		for (unsigned int i = 0; i < 4; i++) {
			drawText(vg, 17, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "WAVE");
			drawText(vg, 57, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "PHASE");
			drawText(vg, 97, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "MULT");
			drawText(vg, 133, 123.6 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "OUT");
		}

	}
};

Model *modelPO204 = createModel<PO_204, PO204>("PO-204");
