//SubTag W8

#include "XF.hpp"

struct XF_202 : XF {
	static const int deviceCount = 2;
	enum ParamIds {
		PARAM_CV_1, PARAM_CV_2,
		PARAM_MODE_1, PARAM_MODE_2,
		PARAM_FADE_1, PARAM_FADE_2,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1, INPUT_A_2,
		INPUT_AR_1, INPUT_AR_2,
		INPUT_B_1, INPUT_B_2,
		INPUT_BR_1, INPUT_BR_2,
		INPUT_CV_1, INPUT_CV_2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1, OUTPUT_2,
		OUTPUTR_1, OUTPUTR_2,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1, LIGHT_LIN_2,
		LIGHT_LOG_1, LIGHT_LOG_2,
		LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2,
		NUM_LIGHTS
	};
	XF_Correlator correlators[deviceCount];
	XF_Controls controls[deviceCount];

	XF_202() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		for (int i = 0; i < deviceCount; i++) {
			controls[i].a = INPUT_A_1 + i;
			controls[i].ar = INPUT_AR_1 + i;
			controls[i].b = INPUT_B_1 + i;
			controls[i].br = INPUT_BR_1 + i;
			controls[i].fader = PARAM_FADE_1 + i;
			controls[i].cv = INPUT_CV_1 + i;
			controls[i].out = OUTPUT_1 + i;
			controls[i].outr = OUTPUTR_1 + i;
			controls[i].polar = PARAM_CV_1 + i;
			controls[i].mode = PARAM_MODE_1 + i;
			controls[i].light1 = LIGHT_LIN_1 + i;
			controls[i].light2 = LIGHT_LOG_1 + i;
			controls[i].light3 = LIGHT_AUTO_1 + i * 2;
			controls[i].correlator = &correlators[i];
		}
	}
	void step() override;
};

void XF_202::step() {
	crossFade(&controls[0]);
	crossFade(&controls[1]);
}

struct XF202 : SchemeModuleWidget {
	XF202(XF_202 *module) : SchemeModuleWidget(module) {
		XF_LightKnob *fader;
		this->box.size = Vec(120, 380);
		addChild(new SchemePanel(this->box.size));
		for (int i = 0; i < XF_202::deviceCount; i++) {
			int offset = 176 * i;
			addInput(createInputCentered<SilverPort>(Vec(15.5,30.5 + offset), module, XF_202::INPUT_A_1 + i));
			addInput(createInputCentered<RedPort>(Vec(15.5,57.5 + offset), module, XF_202::INPUT_AR_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(104.5,30.5 + offset), module, XF_202::INPUT_B_1 + i));
			addInput(createInputCentered<RedPort>(Vec(104.5,57.5 + offset), module, XF_202::INPUT_BR_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(15.5,132.5 + offset), module, XF_202::INPUT_CV_1 + i));

			addOutput(createOutputCentered<SilverPort>(Vec(104.5,105.5 + offset), module, XF_202::OUTPUT_1 + i));
			addOutput(createOutputCentered<RedPort>(Vec(104.5,132.5 + offset), module, XF_202::OUTPUTR_1 + i));

			addParam(createParamCentered<SubSwitch2>(Vec(35, 167 + offset), module, XF_202::PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f));
			addParam(createParamCentered<SubSwitch3>(Vec(72.5, 167 + offset), module, XF_202::PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f));
			fader = createParamCentered<XF_LightKnob>(Vec(60, 78 + offset), module, XF_202::PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f);
			fader->cv = XF_202::INPUT_CV_1 + i;
			fader->link = 0;
			addParam(fader);

			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(82.5, 157.5 + offset), module, XF_202::LIGHT_LIN_1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(82.5, 167.5 + offset), module, XF_202::LIGHT_LOG_1 + i));
			addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(82.5, 177.5 + offset), module, XF_202::LIGHT_AUTO_1 + i * 2));
		}

	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-202");
		
		// Dividers
		nvgStrokeColor(vg, gScheme.alternative);
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 3, 190.5);
		nvgLineTo(vg, 117, 190.5);
		nvgStroke(vg);

		nvgStrokeColor(vg, gScheme.contrast);
	
		for (unsigned int i = 0; i < 2; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 35.000000, 103.500000 + 176 * i);
			nvgBezierTo(vg, 36.333336, 102.166664 + 176 * i, 37.666664, 100.833336 + 176 * i, 39.000000, 99.500000 + 176 * i);
			nvgBezierTo(vg, 30.295670, 90.968048 + 176 * i, 27.618584, 78.017834 + 176 * i, 32.226555, 66.733948 + 176 * i);
			nvgBezierTo(vg, 36.834526, 55.450062 + 176 * i, 47.811501, 48.075714 + 176 * i, 60.000000, 48.075714 + 176 * i);
			nvgBezierTo(vg, 72.188499, 48.075714 + 176 * i, 83.165482, 55.450069 + 176 * i, 87.773453, 66.733955 + 176 * i);
			nvgBezierTo(vg, 92.381424, 78.017841 + 176 * i, 89.704330, 90.968056 + 176 * i, 81.000000, 99.500000 + 176 * i);
			nvgBezierTo(vg, 82.333336, 100.833336 + 176 * i, 83.666664, 102.166664 + 176 * i, 85.000000, 103.500000 + 176 * i);
			nvgStroke(vg);

			drawText(vg, 34, 113 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "A");
			drawText(vg, 80, 113 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.contrast, "B");
			drawText(vg, 26, 164 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "UNI");
			drawText(vg, 26, 176 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "BI");
			drawText(vg, 30, 36 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.contrast, "A");
			drawText(vg, 28, 138 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.contrast, "CV");
			drawText(vg, 90, 36 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.contrast, "B");
			drawText(vg, 90, 138 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 18, gScheme.contrast, "\xe2\x86\xa6");
			drawText(vg, 86, 161 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "LIN");
			drawText(vg, 86, 170 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "LOG");
			drawText(vg, 86, 179 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.contrast, "AUTO");
		}
	}
};

Model *modelXF202 = Model::create<XF_202, XF202>("XF-202");
