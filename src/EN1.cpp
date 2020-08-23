//SubTag W10

#include "SubmarineFree.hpp"

struct EN_104 : Module {
	
	enum ParamIds {
		PARAM_A1,
		PARAM_D1 = PARAM_A1 + 4,
		PARAM_S1 = PARAM_D1 + 4,
		PARAM_R1 = PARAM_S1 + 4,
		PARAM_T1 = PARAM_R1 + 4,
		NUM_PARAMS = PARAM_T1 + 4
	};
	enum InputIds {
		INPUT_TRIGGER,
		INPUT_GATE,
		INPUT_1,
		INPUT_A1 = INPUT_1 + 4,
		INPUT_D1 = INPUT_A1 + 4,
		INPUT_S1 = INPUT_D1 + 4,
		INPUT_R1 = INPUT_S1 + 4,
		INPUT_T1 = INPUT_R1 + 4,
		NUM_INPUTS = INPUT_T1 + 4
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS = OUTPUT_1 + 4
	};
	enum LightIds {
		LIGHT_A1,
		LIGHT_D1 = LIGHT_A1 + 4,
		LIGHT_S1 = LIGHT_D1 + 4,
		LIGHT_R1 = LIGHT_S1 + 4,
		NUM_LIGHTS = LIGHT_R1 + 4
	};

	EN_104
() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for(unsigned int i = 0; i < 4; i++) {
			configParam(PARAM_A1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d Attack Rack", i + 1));
			configParam(PARAM_D1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d Decay Rate", i + 1));
			configParam(PARAM_S1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d Sustain Level", i + 1));
			configParam(PARAM_R1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d Release Rate", i + 1));
			configParam(PARAM_T1 + i, 0.0f, 10.0f, 5.0f, string::f("Operator #%d Total Level", i + 1));
		}
		
	}
	dsp::SchmittTrigger trigger;
	unsigned char skipParams = 0;
	__m128 level = _mm_set_ps1(0.0f);
	__m128 attack;
	__m128 decay;
	__m128 sustain;
	__m128 release;
	__m128 phase = _mm_set_ps1(0.0f);
	void process(const ProcessArgs &args) override;
	void getParams();
};

static inline void sseProcess(float phase, float ports[]) {
/*
	//Calculate wave from param and input clamp to 0-10 and separate into int and remainder
	__m128 wave = _mm_add_ps(_mm_load_ps(ports + 16), _mm_load_ps(ports + 20));
	wave = _mm_max_ps(wave, _mm_set_ps1(0.0f));
	wave = _mm_min_ps(wave, _mm_set_ps1(10.0f)); 
	__m128i waveType = _mm_cvttps_epi32(wave);
	wave = _mm_sub_ps(wave, _mm_cvtepi32_ps(waveType));

	//Calculate multiplier from param and input (scaled to 0-10V) clamp between 1 and 16 and floor
	__m128 mult = _mm_add_ps(_mm_mul_ps(_mm_load_ps(ports + 12), _mm_set_ps1(1.6f)),_mm_load_ps(ports+8));
	mult = _mm_max_ps(mult, _mm_set_ps1(1.0f));
	mult = _mm_min_ps(mult, _mm_set_ps1(16.0f));
	mult = truncx(mult); 
	
	//Calculate phase from base phase, param and input (scaled to +/- 2 cycles), multiply by the multiplier and fmod
	__m128 offset = _mm_add_ps(_mm_set_ps1(phase), _mm_add_ps(_mm_mul_ps(_mm_load_ps(ports + 4), _mm_set_ps1(0.4f)), _mm_load_ps(ports)));
	offset = fmodx(_mm_mul_ps(offset, mult));

	// Sine wave
	__m128 workingWave = sin_ps(offset); 

	__m128 iWave = _mm_sub_ps(_mm_set_ps1(1.0f), wave);
	__m128 mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(5)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(0))));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(10))));
	__m128 output = _mm_and_ps(_mm_mul_ps(workingWave, iWave), mask);

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(4)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(9))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, wave), mask));

	// Half sine wave
	__m128 v5 = _mm_set_ps1(5.0f);
	__m128 sign = _mm_set_ps1(-0.0f);
	workingWave = _mm_andnot_ps(sign, workingWave);
	workingWave = _mm_add_ps(workingWave, workingWave);
	workingWave = _mm_sub_ps(workingWave, v5);

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(2)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(9))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, iWave), mask));

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(1)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(8))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, wave), mask));

	// Sawtooth wave
	offset = _mm_mul_ps(offset, v5);
	offset = _mm_add_ps(offset, offset);
	mask = _mm_cmpgt_ps(offset, v5);
	workingWave = _mm_sub_ps(offset, _mm_and_ps(mask, _mm_set_ps1(10.0f)));

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(1)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(7))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, iWave), mask));

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(0)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(6))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, wave), mask));
	
	// Triangle wave
	workingWave = _mm_add_ps(offset, offset);
	workingWave = _mm_sub_ps(workingWave, v5);
	workingWave = _mm_andnot_ps(sign, workingWave);
	workingWave = _mm_sub_ps(workingWave, _mm_set_ps1(10.0f));
	workingWave = _mm_andnot_ps(sign, workingWave);
	workingWave = _mm_sub_ps(workingWave, v5);

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(3)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(6))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, iWave), mask));

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(2)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(5))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, wave), mask));

	// Square wave
	mask = _mm_and_ps(sign, workingWave);
	workingWave = _mm_xor_ps(mask, v5);
	
	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(4)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(8))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, iWave), mask));

	mask = _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(3)));
	mask = _mm_or_ps(mask, _mm_castsi128_ps(_mm_cmpeq_epi32(waveType, _mm_set1_epi32(7))));
	output = _mm_add_ps(output, _mm_and_ps(_mm_mul_ps(workingWave, wave), mask));

	_mm_store_ps(ports, output);
*/
}

void EN_104::getParams() {
	alignas(16) float a[4];
	alignas(16) float d[4];
	alignas(16) float s[4];
	alignas(16) float r[4];
	alignas(16) float t[4];
	for (unsigned int i = 0; i < 4; i++) {
		a[i] = clamp(params[PARAM_A1 + i].getValue() + inputs[INPUT_A1 + i].getVoltage(), 0.0f, 10.0f);
		d[i] = clamp(params[PARAM_D1 + i].getValue() + inputs[INPUT_D1 + i].getVoltage(), 0.0f, 10.0f);
		s[i] = clamp(params[PARAM_S1 + i].getValue() + inputs[INPUT_S1 + i].getVoltage(), 0.0f, 10.0f);
		r[i] = clamp(params[PARAM_R1 + i].getValue() + inputs[INPUT_R1 + i].getVoltage(), 0.0f, 10.0f);
		t[i] = clamp(params[PARAM_T1 + i].getValue() + inputs[INPUT_T1 + i].getVoltage(), 0.0f, 10.0f);
	}
	attack = _mm_load_ps(a);
	decay = _mm_load_ps(d);
	sustain = _mm_load_ps(s);
	release = _mm_load_ps(r);
}

void EN_104::process(const ProcessArgs &args) {
	float triggerVal = inputs[INPUT_TRIGGER].getVoltage();
	float gateVal = inputs[INPUT_GATE].getVoltage();
	bool gate = 0;
	bool triggered = 0;
	int triggerType = inputs[INPUT_TRIGGER].isConnected() * 2 + inputs[INPUT_GATE].isConnected();
	switch (triggerType) {
		case 1: 		// Trigger not connected, gate connected
			if (trigger.process(rescale(gateVal, 2.4f, 2.5f, 0.0f, 1.0f))) {
				triggered = true;
			}
			break;
		case 2:			// Trigger connected, gate not connected
			if (trigger.process(rescale(triggerVal, 2.4f, 2.5f, 0.0f, 1.0f))) {
				triggered = true;
			}
			break;
		case 3:			// Trigger connected, gate connected 
			if (trigger.process(rescale(triggerVal, 2.4f, 2.5f, 0.0f, 1.0f))) {
				triggered = true;
			}
			break;
		default:		// Nothing connected
			break;
	}
	if (!skipParams++) {
		getParams();
	}
/*
	float freq = baseFreq * powf(2.0f, (params[PARAM_TUNE].getValue() + 3.0f * dsp::quadraticBipolar(params[PARAM_FINE].getValue())) * (1.0f / 12.0f) + (inputs[INPUT_TUNE].isConnected()?inputs[INPUT_TUNE].getVoltage():0.0f));
	float deltaTime = freq * args.sampleTime;
	phase += deltaTime;
	while(phase > 1.0f)
		phase -= 1.0f;
	alignas(16) float ports[24];
	for (int i = 0; i < 4; i++) {
		ports[0 + i] = params[PARAM_PHASE_1 + i].getValue();
		ports[4 + i] = inputs[INPUT_PHASE_1 + i].isConnected()?inputs[INPUT_PHASE_1 + i].getVoltage():0.0f;
		ports[8 + i] = params[PARAM_MULT_1 + i].getValue();
		ports[12 + i] = inputs[INPUT_MULT_1 + i].isConnected()?inputs[INPUT_MULT_1 + i].getVoltage():0.0f;
		ports[16 + i] = params[PARAM_WAVE_1 + i].getValue();
		ports[20 + i] = inputs[INPUT_WAVE_1 + i].isConnected()?inputs[INPUT_WAVE_1 + i].getVoltage():0.0f;
	}
	sseProcess(phase + 5, ports);
	for (int i = 0; i < 4; i++) {
		outputs[OUTPUT_1 + i].setVoltage(ports[i]);
	}
*/
}

struct EN104 : SchemeModuleWidget {
	EN104(EN_104 *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));
		addInput(createInputCentered<SilverPort>(Vec(15, 33), module, EN_104::INPUT_TRIGGER));
		addInput(createInputCentered<SilverPort>(Vec(15, 67), module, EN_104::INPUT_GATE));

		for (int i = 0; i < 4; i++) {
			addInput(createInputCentered<SilverPort>(Vec(45 + 30 * i, 33), module, EN_104::INPUT_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(45 + 30 * i, 67), module, EN_104::OUTPUT_1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 103 + 70 * i), module, EN_104::PARAM_A1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(45, 103 + 70 * i), module, EN_104::PARAM_D1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(75, 103 + 70 * i), module, EN_104::PARAM_S1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(105, 103 + 70 * i), module, EN_104::PARAM_R1 + i));
			addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(135, 103 + 70 * i), module, EN_104::PARAM_T1 + i));
			addInput(createInputCentered<SilverPort>(Vec(15, 137 + 70 * i), module, EN_104::INPUT_A1 + i));
			addInput(createInputCentered<SilverPort>(Vec(45, 137 + 70 * i), module, EN_104::INPUT_D1 + i));
			addInput(createInputCentered<SilverPort>(Vec(75, 137 + 70 * i), module, EN_104::INPUT_S1 + i));
			addInput(createInputCentered<SilverPort>(Vec(105, 137 + 70 * i), module, EN_104::INPUT_R1 + i));
			addInput(createInputCentered<SilverPort>(Vec(135, 137 + 70 * i), module, EN_104::INPUT_T1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(23, 80 + 70 * i), module, EN_104::LIGHT_A1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(53, 80 + 70 * i), module, EN_104::LIGHT_D1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(83, 80 + 70 * i), module, EN_104::LIGHT_S1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(113, 80 + 70 * i), module, EN_104::LIGHT_R1 + i));
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "EN-104");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);

		for (unsigned int i = 0; i < 4; i++) {
			nvgMoveTo(vg, 3, 85.5 + 70 * i);
			nvgLineTo(vg, 147, 85.5 + 70 * i);
		}

		nvgStroke(vg);
		
		drawText(vg, 2, 20, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "TR.");
		drawText(vg, 2, 54, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "GT.");
		drawText(vg, 45, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "1");
		drawText(vg, 75, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "2");
		drawText(vg, 105, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "3");
		drawText(vg, 135, 54, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "4");
		for (unsigned int i = 0; i < 4; i++) {
			drawText(vg, 15, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
			drawText(vg, 45, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "D");
			drawText(vg, 75, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "S");
			drawText(vg, 105, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "R");
			drawText(vg, 135, 124 + 70 * i, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "LVL.");
		}

	}
};

Model *modelEN104 = createModel<EN_104, EN104>("EN-104");
