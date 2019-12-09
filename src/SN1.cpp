//SubTag W2

#include "SubmarineFree.hpp"

struct SN_1 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SN_1() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs &args) override {
	}
};

/*

inline __m128i advanceLFSR(__m128i &lfsr) {
	__m128i mask = _mm_set1_epi16(0x0u);
	mask = _mm_cmpgt_epi16(mask, lfsr);
	mask = _mm_and_si128(mask, _mm_set1_epi16(0x100Bu));
	lfsr = _mm_slli_epi16(lfsr, 1);
	lfsr = _mm_xor_si128(lfsr, mask);


0.098017140329561
0.290284677254462
0.471396736825998
0.634393284163645
0.773010453362737
0.881921264348355
0.956940335732209
0.995184726672197
0.995184726672197
0.956940335732209
0.881921264348355
0.773010453362737
0.634393284163645
0.471396736825998
0.290284677254462
0.098017140329561
-0.098017140329561
-0.290284677254462
-0.471396736825998
-0.634393284163645
-0.773010453362737
-0.881921264348355
-0.956940335732209
-0.995184726672197
-0.995184726672197
-0.956940335732209
-0.881921264348355
-0.773010453362737
-0.634393284163646
-0.471396736825998
-0.290284677254462
-0.098017140329561


*/

struct SN101 : SchemeModuleWidget {
	SN101(SN_1 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addOutput(createOutputCentered<BluePort>(Vec(15,45.5), module, SN_1::OUTPUT_1));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "SN-101");
		nvgStrokeWidth(vg, 1);
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		for (int i = 0; i < 5; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 74.5, 45.5 + i * 58);
			nvgLineTo(vg, 74.5, 74.5 + i * 58);
			nvgLineTo(vg, 29.5, 74.5 + i * 58);
			nvgLineTo(vg, 29.5, 103.5 + i * 58);
			nvgLineTo(vg, 38.5, 103.5 + i * 58);	
			nvgStroke(vg);
		}
		nvgStrokeColor(vg, gScheme.getContrast(module));
		for (int i = 0; i < 6; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 16, 31.5 + i * 58);
			nvgLineTo(vg, 34, 31.5 + i * 58);
			nvgLineTo(vg, 34, 42.5 + i * 58);
			nvgLineTo(vg, 39, 42.5 + i * 58);
			nvgMoveTo(vg, 16, 59.5 + i * 58);
			nvgLineTo(vg, 34, 59.5 + i * 58);
			nvgLineTo(vg, 34, 48.5 + i * 58);
			nvgLineTo(vg, 39, 48.5 + i * 58);
			nvgMoveTo(vg, 55, 45.5 + i * 58);
			nvgLineTo(vg, 74, 45.5 + i * 58);
			nvgMoveTo(vg, 39, 34.5 + i * 58);
			nvgLineTo(vg, 39, 56.5 + i * 58);
			nvgLineTo(vg, 43.5, 56.5 + i * 58);
			nvgArc(vg, 43.5, 45.5 + i * 58, 11, M_PI / 2.0f, -M_PI / 2.0f, NVG_CCW);
			nvgClosePath(vg);
			nvgStroke(vg);
		}
	}
};

Model *modelSN101 = createModel<SN_1, SN101>("SN-101");
