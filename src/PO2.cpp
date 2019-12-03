//SubTag W10

#include "SubmarineFree.hpp"

struct PO_204 : Module {
	
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

static inline __m128 truncx(__m128 x) {
	return (_mm_cvtepi32_ps(_mm_cvttps_epi32(x)));
}

static inline __m128 fmodx(__m128 x) {
	return _mm_sub_ps(x, truncx(x));
}

static inline __m128 sin_ps(__m128 x) { // any x
	__m128 xmm1, xmm3, sign_bit, y;

	__m128i emm0, emm2;

	/* scale by eight */
	const __m128 mEight = _mm_set_ps1(8.0f); // 8
	y = _mm_mul_ps(x, mEight);

	/* store the integer part of y in mm0 */
	emm2 = _mm_cvttps_epi32(y);
	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, _mm_set1_epi32(1));
	emm2 = _mm_and_si128(emm2, _mm_set1_epi32(~1));
	y = _mm_cvtepi32_ps(emm2);

	/* get the swap sign flag */
	emm0 = _mm_and_si128(emm2, _mm_set1_epi32(4));
	emm0 = _mm_slli_epi32(emm0, 29);
	/* get the polynom selection mask
	   there is one polynom for 0 <= x <= Pi/4
	   and another one for Pi/4<x<=Pi/2

	   Both branches will be computed.
	*/
	emm2 = _mm_and_si128(emm2, _mm_set1_epi32(2));
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());

	sign_bit = _mm_castsi128_ps(emm0);
	__m128 poly_mask = _mm_castsi128_ps(emm2);

	xmm1 = _mm_set_ps1(-0.125f);
	xmm1 = _mm_mul_ps(y, xmm1);
	x = _mm_add_ps(x, xmm1);

	/* Evaluate the first polynom  (0 <= x <= 0.125) */
	__m128 z = _mm_mul_ps(x, x);
	y = _mm_set_ps1(3.01223206859383e2f);
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, _mm_set_ps1(-4.2728406033469e2f));
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, _mm_set_ps1(3.24696970113341e2f));
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, _mm_set_ps1(-9.86960440108936e1f));
	y = _mm_mul_ps(y, z);
	y = _mm_add_ps(y, _mm_set_ps1(5.0));

	/* Evaluate the second polynom  (0.125 <= x <= 0.25) */

	__m128 y2 = _mm_set_ps1(-3.83529298765307e2f);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, _mm_set_ps1(4.08026246380375e2f));
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, _mm_set_ps1(-2.06708511201999e2f));
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, _mm_set_ps1(3.14159265358979e1f));
	y2 = _mm_mul_ps(y2, x);

	/* select the correct result from the two polynoms */
	xmm3 = poly_mask;
	y2 = _mm_and_ps(xmm3, y2); //, xmm3);
	y = _mm_andnot_ps(xmm3, y);
	y = _mm_add_ps(y, y2);
	/* update the sign */
	y = _mm_xor_ps(y, sign_bit);
	return y;
}


static inline void sseProcess(float phase, float ports[]) {
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
}


void PO_204::process(const ProcessArgs &args) {
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
}

struct PO204 : SchemeModuleWidget {
	PO204(PO_204 *module) {
		setModule(module);
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
