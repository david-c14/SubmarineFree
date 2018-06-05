#include "SubmarineFree.hpp"
#include "dsp/functions.hpp"

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
};

float PO_Util::sin(float phase) {
	return 5.0f * sinf(phase);
}

float PO_Util::tri(float phase) {
	if (phase >= 1.0f)
		phase -= 1.0f;
	if (phase < 0.25f)
		return 20.0f * phase;
	if (phase < 0.75f)
		return 20.0f * (0.5f - phase);
	return 20.0f * (phase - 1.0f);
}

float PO_Util::saw(float phase) {
	if (phase >= 1.0f)
		phase -= 1.0f;
	if (phase < 0.5f)
		return 10.0f * phase;
	return 10.0f * (phase - 1.0f);
}

float PO_Util::sqr(float phase) {
	return ((phase < 0.5f) || (phase >= 1.0f))?5.0f:-5.0f;
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

	PO_101() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	void sin(std::vector<float> *outputs, float phase);
	void tri(std::vector<float> *outputs, float phase);
	void saw(std::vector<float> *outputs, float phase);
	void sqr(std::vector<float> *outputs, float phase);
	float phase = 0.0f;
};

void PO_101::sin(std::vector<float> *outputs, float phase) {
	phase *= (2 * M_PI);
	outputs->at(0) = PO_Util::sin(phase + deg0);
	outputs->at(1) = PO_Util::sin(phase + deg30); 
	outputs->at(2) = PO_Util::sin(phase + deg45); 
	outputs->at(3) = PO_Util::sin(phase + deg60); 
	outputs->at(4) = PO_Util::sin(phase + deg90); 
	outputs->at(5) = PO_Util::sin(phase + deg120); 
	outputs->at(6) = PO_Util::sin(phase + deg135); 
	outputs->at(7) = PO_Util::sin(phase + deg150); 
	outputs->at(8) = -outputs->at(0); 
	outputs->at(9) = -outputs->at(1); 
	outputs->at(10) = -outputs->at(2); 
	outputs->at(11) = -outputs->at(3); 
	outputs->at(12) = -outputs->at(4); 
	outputs->at(13) = -outputs->at(5); 
	outputs->at(14) = -outputs->at(6); 
	outputs->at(15) = -outputs->at(7); 
}

void PO_101::tri(std::vector<float> *outputs, float phase) {
	outputs->at(0) = PO_Util::tri(phase + ph0);
	outputs->at(1) = PO_Util::tri(phase + ph30); 
	outputs->at(2) = PO_Util::tri(phase + ph45); 
	outputs->at(3) = PO_Util::tri(phase + ph60); 
	outputs->at(4) = PO_Util::tri(phase + ph90); 
	outputs->at(5) = PO_Util::tri(phase + ph120); 
	outputs->at(6) = PO_Util::tri(phase + ph135); 
	outputs->at(7) = PO_Util::tri(phase + ph150); 
	outputs->at(8) = -outputs->at(0); 
	outputs->at(9) = -outputs->at(1); 
	outputs->at(10) = -outputs->at(2); 
	outputs->at(11) = -outputs->at(3); 
	outputs->at(12) = -outputs->at(4); 
	outputs->at(13) = -outputs->at(5); 
	outputs->at(14) = -outputs->at(6); 
	outputs->at(15) = -outputs->at(7); 
}

void PO_101::saw(std::vector<float> *outputs, float phase) {
	outputs->at(0) = PO_Util::saw(phase + ph0);
	outputs->at(1) = PO_Util::saw(phase + ph30); 
	outputs->at(2) = PO_Util::saw(phase + ph45); 
	outputs->at(3) = PO_Util::saw(phase + ph60); 
	outputs->at(4) = PO_Util::saw(phase + ph90); 
	outputs->at(5) = PO_Util::saw(phase + ph120); 
	outputs->at(6) = PO_Util::saw(phase + ph135); 
	outputs->at(7) = PO_Util::saw(phase + ph150); 
	outputs->at(8) = PO_Util::saw(phase + ph180);
	outputs->at(9) = PO_Util::saw(phase + ph210); 
	outputs->at(10) = PO_Util::saw(phase + ph225); 
	outputs->at(11) = PO_Util::saw(phase + ph240); 
	outputs->at(12) = PO_Util::saw(phase + ph270); 
	outputs->at(13) = PO_Util::saw(phase + ph300); 
	outputs->at(14) = PO_Util::saw(phase + ph315); 
	outputs->at(15) = PO_Util::saw(phase + ph330); 
}

void PO_101::sqr(std::vector<float> *outputs, float phase) {
	outputs->at(0) = PO_Util::sqr(phase + ph0);
	outputs->at(1) = PO_Util::sqr(phase + ph30); 
	outputs->at(2) = PO_Util::sqr(phase + ph45); 
	outputs->at(3) = PO_Util::sqr(phase + ph60); 
	outputs->at(4) = PO_Util::sqr(phase + ph90); 
	outputs->at(5) = PO_Util::sqr(phase + ph120); 
	outputs->at(6) = PO_Util::sqr(phase + ph135); 
	outputs->at(7) = PO_Util::sqr(phase + ph150); 
	outputs->at(8) = -outputs->at(0); 
	outputs->at(9) = -outputs->at(1); 
	outputs->at(10) = -outputs->at(2); 
	outputs->at(11) = -outputs->at(3); 
	outputs->at(12) = -outputs->at(4); 
	outputs->at(13) = -outputs->at(5); 
	outputs->at(14) = -outputs->at(6); 
	outputs->at(15) = -outputs->at(7); 
}
	
void PO_101::step() {
	float freq = 440.0f * powf(2.0f, (params[PARAM_TUNE].value + 3.0f * quadraticBipolar(params[PARAM_FINE].value)) / 12.0f + (inputs[INPUT_NOTE_CV].active?inputs[INPUT_NOTE_CV].value:0.0f) - 0.75f);
	float deltaTime = freq / engineGetSampleRate();
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart); 
	
	{
		std::vector<float> offsets(16,0.0f);
		float waveShape = clamp(params[PARAM_WAVE].value, 0.0f, 3.0f);
		if (waveShape < 0.5f)
			sin(&offsets, phase);
		else if (waveShape < 1.5f)
			tri(&offsets, phase);
		else if (waveShape < 2.5f)
			saw(&offsets, phase);
		else
			sqr(&offsets, phase);
			
		for(unsigned int i = 0; i < 16; i++) {
			outputs[OUTPUT_1 + i].value = offsets[i];
		}
	}

}

struct PO101 : ModuleWidget {
	PO101(PO_101 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/PO-101.svg")));

		addParam(ParamWidget::create<sub_knob_med>(Vec(11, 39), module, PO_101::PARAM_TUNE, -54.0f, +54.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(56, 39), module, PO_101::PARAM_FINE, -1.0f, +1.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med_snap>(Vec(121, 39), module, PO_101::PARAM_WAVE, 0.0f, +3.0f, 0.0f));

		addInput(Port::create<sub_port>(Vec(40,19), Port::INPUT, module, PO_101::INPUT_NOTE_CV));

		addOutput(Port::create<sub_port>(Vec(77.5,100), Port::OUTPUT, module, PO_101::OUTPUT_1));
		addOutput(Port::create<sub_port>(Vec(105,107.5), Port::OUTPUT, module, PO_101::OUTPUT_2));
		addOutput(Port::create<sub_port>(Vec(132.5,100), Port::OUTPUT, module, PO_101::OUTPUT_3));
		addOutput(Port::create<sub_port>(Vec(125,127.5), Port::OUTPUT, module, PO_101::OUTPUT_4));
		addOutput(Port::create<sub_port>(Vec(132.5,155), Port::OUTPUT, module, PO_101::OUTPUT_5));
		addOutput(Port::create<sub_port>(Vec(125,182.5), Port::OUTPUT, module, PO_101::OUTPUT_6));
		addOutput(Port::create<sub_port>(Vec(132.5,210), Port::OUTPUT, module, PO_101::OUTPUT_7));
		addOutput(Port::create<sub_port>(Vec(105,202.5), Port::OUTPUT, module, PO_101::OUTPUT_8));
		addOutput(Port::create<sub_port>(Vec(77.5,210), Port::OUTPUT, module, PO_101::OUTPUT_9));
		addOutput(Port::create<sub_port>(Vec(50,202.5), Port::OUTPUT, module, PO_101::OUTPUT_10));
		addOutput(Port::create<sub_port>(Vec(22.5,210), Port::OUTPUT, module, PO_101::OUTPUT_11));
		addOutput(Port::create<sub_port>(Vec(30,182.5), Port::OUTPUT, module, PO_101::OUTPUT_12));
		addOutput(Port::create<sub_port>(Vec(22.5,155), Port::OUTPUT, module, PO_101::OUTPUT_13));
		addOutput(Port::create<sub_port>(Vec(30,127.5), Port::OUTPUT, module, PO_101::OUTPUT_14));
		addOutput(Port::create<sub_port>(Vec(22.5,100), Port::OUTPUT, module, PO_101::OUTPUT_15));
		addOutput(Port::create<sub_port>(Vec(50,107.5), Port::OUTPUT, module, PO_101::OUTPUT_16));
	}
};

Model *modelPO101 = Model::create<PO_101, PO101>("SubmarineFree", "PO-101", "PO-101 Phased VCO", OSCILLATOR_TAG, MULTIPLE_TAG);
