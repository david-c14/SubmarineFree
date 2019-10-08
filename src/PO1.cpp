//SubTag W12

#include "SubmarineFree.hpp"

struct PO_101 : Module {
	
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
		configParam(PARAM_FINE, -1.0f, +1.0f, 0.0f, "Fine frequency");
		configParam(PARAM_WAVE, 0.0f, +4.0f, 0.0f, "Wave shape");
		for (unsigned int i = 0; i < 4; i++) {
			configParam(PARAM_PHASE_1 + i, -1.0f, +1.0f, 0.0f, "Phase shift", "\xc2\xb0", 0.f, 360.f);
		}
		configParam(PARAM_TUNE, -54.0f, +54.0f, 0.0f, "Frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
	}
	void process(const ProcessArgs &args) override;
	float phase = 0.0f;
	float baseFreq = 261.626f;
};

struct PO_102 : PO_101 {
	PO_102() : PO_101() {
		configParam(PARAM_TUNE, -96.0f, 72.0f, -12.0f, "Frequency", " Hz", dsp::FREQ_SEMITONE, 1.0f);
		baseFreq = 1.0f;
	}
};

static __m128 mOffsets = _mm_set_ps(1.0f / 6.0f, 1.0f / 8.0f, 1.0f / 12.0f, 0.0f);
static __m128 mRange = _mm_set_ps1(5.0f);
static __m128 mSignMask = _mm_set_ps1(-0.0f);

static inline __m128 truncx(__m128 x) {
	return (_mm_cvtepi32_ps(_mm_cvttps_epi32(x)));
}

static inline __m128 fmodx(__m128 x) {
	return _mm_sub_ps(x, truncx(x));
}

static inline __m128 sin_ps(__m128 x) { // any x
	__m128 xmm1, xmm3, sign_bit, y;

	__m128i emm0, emm2;
	sign_bit = x;
	/* take the absolute value */
	const __m128 sign_mask = _mm_set_ps1(-0.0f);
	x = _mm_andnot_ps(sign_mask, x);
	/* extract the sign bit (upper one) */
	sign_bit = _mm_and_ps(sign_bit, sign_mask);

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

	__m128 swap_sign_bit = _mm_castsi128_ps(emm0);
	__m128 poly_mask = _mm_castsi128_ps(emm2);
	sign_bit = _mm_xor_ps(sign_bit, swap_sign_bit);

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

static inline void sincos_ps(__m128 x, __m128* s, __m128* c) {
	__m128 xmm1, xmm2, xmm3 = _mm_setzero_ps(), sign_bit_sin, y;
	__m128i emm0, emm2, emm4;
	sign_bit_sin = x;
	/* take the absolute value */
	const __m128 sign_mask = _mm_set_ps1(-0.0f);
	x = _mm_andnot_ps(sign_mask, x);
	/* extract the sign bit (upper one) */
	sign_bit_sin = _mm_and_ps(sign_bit_sin, sign_mask);

	/* scale by eight */
	const __m128 mEight = _mm_set_ps1(8.0f); // 8
	y = _mm_mul_ps(x, mEight);

	/* store the integer part of y in emm2 */
	emm2 = _mm_cvttps_epi32(y);

	/* j=(j+1) & (~1) (see the cephes sources) */
	emm2 = _mm_add_epi32(emm2, _mm_set1_epi32(1));
	emm2 = _mm_and_si128(emm2, _mm_set1_epi32(~1));
	y = _mm_cvtepi32_ps(emm2);

	emm4 = emm2;

	/* get the swap sign flag for the sine */
	emm0 = _mm_and_si128(emm2, _mm_set1_epi32(4));
	emm0 = _mm_slli_epi32(emm0, 29);
	__m128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

	/* get the polynom selection mask for the sine*/
	emm2 = _mm_and_si128(emm2, _mm_set1_epi32(2));
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
	__m128 poly_mask = _mm_castsi128_ps(emm2);

	xmm1 = _mm_set_ps1(-0.125f);
	xmm1 = _mm_mul_ps(y, xmm1);
	x = _mm_add_ps(x, xmm1);

	emm4 = _mm_sub_epi32(emm4, _mm_set1_epi32(2));
	emm4 = _mm_andnot_si128(emm4, _mm_set1_epi32(4));
	emm4 = _mm_slli_epi32(emm4, 29);
	__m128 sign_bit_cos = _mm_castsi128_ps(emm4);

	sign_bit_sin = _mm_xor_ps(sign_bit_sin, swap_sign_bit_sin);

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
	__m128 ysin2 = _mm_and_ps(xmm3, y2);
	__m128 ysin1 = _mm_andnot_ps(xmm3, y);
	y2 = _mm_sub_ps(y2, ysin2);
	y = _mm_sub_ps(y, ysin1);

	xmm1 = _mm_add_ps(ysin1, ysin2);
	xmm2 = _mm_add_ps(y, y2);

	/* update the sign */
	*s = _mm_xor_ps(xmm1, sign_bit_sin);
	*c = _mm_xor_ps(xmm2, sign_bit_cos);
}

static inline void sseSine(float phase, float *ports) {
	__m128 sin, cos;
	__m128 mPhase = _mm_set_ps1(phase);
	__m128 mInputs = _mm_add_ps(mPhase, _mm_load_ps(ports + 16));
	sin = sin_ps(mInputs);
	_mm_store_ps(ports + 16, sin);

	mInputs = _mm_add_ps(mPhase, mOffsets);
	sincos_ps(mInputs, &sin, &cos);
	_mm_store_ps(ports, sin);
	_mm_store_ps(ports + 4, cos);
	sin = _mm_xor_ps(sin, mSignMask);
	cos = _mm_xor_ps(cos, mSignMask);
	_mm_store_ps(ports + 8, sin);
	_mm_store_ps(ports + 12, cos);
}

static inline void sseTriangle(float phase, float *ports) {
	__m128 mPhase = _mm_set_ps1(phase + 20.0f);
	__m128 mInputs = fmodx(_mm_add_ps(mPhase, _mm_load_ps(ports + 16)));
	__m128 mOutputs = _mm_mul_ps(mInputs, mRange);
	mOutputs = _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange);
	mOutputs = _mm_or_ps(_mm_add_ps(mOutputs, mOutputs), mSignMask);
	_mm_store_ps(ports + 16, _mm_add_ps(mOutputs, mRange));

	mInputs = fmodx(_mm_add_ps(mPhase, mOffsets));
	mOutputs = _mm_mul_ps(mInputs, mRange);
	mOutputs = _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange);
	mOutputs = _mm_add_ps(_mm_or_ps(_mm_add_ps(mOutputs, mOutputs), mSignMask), mRange);
	_mm_store_ps(ports, mOutputs);
	_mm_store_ps(ports + 8, _mm_xor_ps(mOutputs, mSignMask));

	mInputs = fmodx(_mm_add_ps(mInputs, _mm_set_ps1(0.25f)));
	mOutputs = _mm_mul_ps(mInputs, mRange);
	mOutputs = _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange);
	mOutputs = _mm_add_ps(_mm_or_ps(_mm_add_ps(mOutputs, mOutputs), mSignMask), mRange);
	_mm_store_ps(ports + 4, mOutputs);
	_mm_store_ps(ports + 12, _mm_xor_ps(mOutputs, mSignMask));
}

static inline void sseSawtooth(float phase, float *ports) {
	__m128 mPhase = _mm_set_ps1(phase + 20.0f);
	__m128 mInputs = fmodx(_mm_add_ps(mPhase, _mm_load_ps(ports + 16)));
	__m128 mOutputs = _mm_mul_ps(mInputs, mRange);
	_mm_store_ps(ports + 16, _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange));

	mInputs = _mm_add_ps(mPhase, mOffsets);
	mInputs = fmodx(mInputs);
	mOutputs = _mm_mul_ps(mInputs, mRange);
	_mm_store_ps(ports + 0, _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange));
	mInputs = _mm_add_ps(mInputs, _mm_set_ps1(0.25f));
	mOutputs = _mm_mul_ps(fmodx(mInputs), mRange);
	_mm_store_ps(ports + 4, _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange));
	mInputs = _mm_add_ps(mInputs, _mm_set_ps1(0.25f));
	mOutputs = _mm_mul_ps(fmodx(mInputs), mRange);
	_mm_store_ps(ports + 8, _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange));
	mInputs = _mm_add_ps(mInputs, _mm_set_ps1(0.25f));
	mOutputs = _mm_mul_ps(fmodx(mInputs), mRange);
	_mm_store_ps(ports + 12, _mm_sub_ps(_mm_add_ps(mOutputs, mOutputs), mRange));
}

static inline void sseSquare(float phase, float *ports) {
	__m128 mPhase = _mm_set_ps1(phase + 20.0f);
	__m128 mHalf = _mm_set_ps1(0.5f);
	__m128 mInputs = fmodx(_mm_add_ps(mPhase, _mm_load_ps(ports + 16)));
	__m128 mOutputs = _mm_add_ps(mRange, mRange);
	__m128 mMask = _mm_cmplt_ps(mInputs, mHalf);
	_mm_store_ps(ports + 16, _mm_sub_ps(_mm_and_ps(mOutputs, mMask), mRange));

	mInputs = fmodx(_mm_add_ps(mPhase, mOffsets));
	mOutputs = _mm_add_ps(mRange, mRange);
	mMask = _mm_cmplt_ps(mInputs, mHalf);
	mOutputs = _mm_sub_ps(_mm_and_ps(mOutputs, mMask), mRange);
	_mm_store_ps(ports, mOutputs);
	_mm_store_ps(ports + 8, _mm_xor_ps(mOutputs, mSignMask));

	mInputs = fmodx(_mm_add_ps(mInputs, _mm_set_ps1(0.25f)));
	mOutputs = _mm_add_ps(mRange, mRange);
	mMask = _mm_cmplt_ps(mInputs, mHalf);
	mOutputs = _mm_sub_ps(_mm_and_ps(mOutputs, mMask), mRange);
	_mm_store_ps(ports + 4, mOutputs);
	_mm_store_ps(ports + 12, _mm_xor_ps(mOutputs, mSignMask));
}

static inline void sseHalfSine(float phase, float *ports) {
	__m128 sin, cos;
	__m128 mPhase = _mm_set_ps1(phase);
	__m128 mInputs = _mm_add_ps(mPhase, _mm_load_ps(ports + 16));
	sin = sin_ps(mInputs);
	sin = _mm_andnot_ps(mSignMask, sin);
	_mm_store_ps(ports + 16, _mm_sub_ps( _mm_add_ps(sin, sin), mRange));

	mInputs = _mm_add_ps(mPhase, mOffsets);
	sincos_ps(mInputs, &sin, &cos);
	sin = _mm_andnot_ps(mSignMask, sin);
	cos = _mm_andnot_ps(mSignMask, cos);
	sin = _mm_sub_ps(_mm_add_ps(sin, sin), mRange);
	cos = _mm_sub_ps(_mm_add_ps(cos, cos), mRange);
	_mm_store_ps(ports, sin);
	_mm_store_ps(ports + 4, cos);
	_mm_store_ps(ports + 8, sin);
	_mm_store_ps(ports + 12, cos);
}

void PO_101::process(const ProcessArgs &args) {

	float freq = baseFreq * powf(2.0f, (params[PARAM_TUNE].getValue() + 3.0f * dsp::quadraticBipolar(params[PARAM_FINE].getValue())) * (1.0f / 12.0f) + (inputs[INPUT_NOTE_CV].isConnected()?inputs[INPUT_NOTE_CV].getVoltage():0.0f));
	float deltaTime = freq * args.sampleTime;
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart);

	alignas (16) float ports[20];
	for (int i = 0; i < 4; i++) {
		ports[16 + i] = params[PARAM_PHASE_1 + i].getValue();
		if (inputs[INPUT_PHASE_1 + i].isConnected())
			ports[16 + i] += inputs[INPUT_PHASE_1 + i].getVoltage() * 0.4f;
	}
	int waveShape = floor(clamp(params[PARAM_WAVE].getValue(), 0.0f, 4.0f));
	switch(waveShape) {
		case 0:
			sseSine(phase, ports);
			break;
		case 1:
			sseTriangle(phase, ports);
			break;
		case 2:
			sseSawtooth(phase, ports);
			break;
		case 3:
			sseSquare(phase, ports);
			break;
		case 4:
			sseHalfSine(phase, ports);
			break;
		default:
			break;
	}
	for (int i = 0; i < 20; i++) {
		outputs[OUTPUT_1 + i].setVoltage(ports[i]);
	}
}

struct PO_Layout : SchemeModuleWidget {
	PO_Layout(PO_101 *module) : SchemeModuleWidget(module) {}
	void Layout() {
		addParam(createParamCentered<MedKnob<LightKnob>>(Vec(85, 58), module, PO_101::PARAM_FINE));
		addParam(createParamCentered<NarrowKnob<SnapKnob<MedKnob<LightKnob>>>>(Vec(140, 58), module, PO_101::PARAM_WAVE));

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
			addOutput(createOutputCentered<SilverPort>(Vec(22.5 + 45 * i,345.5), module, PO_101::OUTPUT_17 + i));
		}
	}
	void LayoutScreen(NVGcontext *vg) {
		nvgFillColor(vg, gScheme.getBackground(module));
		nvgStrokeColor(vg, gScheme.getContrast(module));
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

		drawText(vg, 30, 89, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "TUNE");
		drawText(vg, 85, 89, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "FINE");
		drawText(vg, 90, 177, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "PHASED");
		drawText(vg, 90, 187, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "OUTPUT");
		drawText(vg, 90, 135, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "0");
		drawText(vg, 90, 229, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "\xcf\x80");
		drawText(vg, 38, 182, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "3\xcf\x80/2");
		drawText(vg, 142, 182, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "\xcf\x80/2");
	}
	
};

struct PO101 : PO_Layout {
	PO101(PO_101 *module) : PO_Layout(module) {
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParam<MedKnob<LightKnob>>(Vec(11, 39), module, PO_101::PARAM_TUNE));
		Layout();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-101");
		LayoutScreen(vg);
	}
};

struct PO102 : PO_Layout {
	PO102(PO_102 *module) : PO_Layout(module) {
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		addParam(createParam<MedKnob<LightKnob>>(Vec(11, 39), module, PO_101::PARAM_TUNE));
		Layout();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "PO-102");
		LayoutScreen(vg);
	}
};

Model *modelPO101 = createModel<PO_101, PO101>("PO-101");
Model *modelPO102 = createModel<PO_102, PO102>("PO-102");
