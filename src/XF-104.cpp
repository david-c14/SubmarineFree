//SubTag W12

#include "XF.hpp"

struct XF_104 : XF {
	static const int deviceCount = 4;
	enum ParamIds {
		PARAM_CV_1, PARAM_CV_2, PARAM_CV_3, PARAM_CV_4,
		PARAM_MODE_1, PARAM_MODE_2, PARAM_MODE_3, PARAM_MODE_4,
		PARAM_FADE_1, PARAM_FADE_2, PARAM_FADE_3, PARAM_FADE_4,
		PARAM_LINK_1, PARAM_LINK_2,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1, INPUT_A_2, INPUT_A_3, INPUT_A_4,
		INPUT_B_1, INPUT_B_2, INPUT_B_3, INPUT_B_4,
		INPUT_CV_1, INPUT_CV_2, INPUT_CV_3, INPUT_CV_4,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1, OUTPUT_2, OUTPUT_3, OUTPUT_4,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1, LIGHT_LIN_2, LIGHT_LIN_3, LIGHT_LIN_4,
		LIGHT_LOG_1, LIGHT_LOG_2, LIGHT_LOG_3, LIGHT_LOG_4,
		LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2, LIGHT_AUTO_3, LIGHT_INV_3, LIGHT_AUTO_4, LIGHT_INV_4,
		NUM_LIGHTS
	};
	XF_Correlator correlators[deviceCount];
	XF_Controls controls[(int)(deviceCount * 1.5f)];

	XF_104() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		for (int i = 0; i < deviceCount; i++) {
			controls[i].a = INPUT_A_1 + i;
			controls[i].ar = 0;
			controls[i].b = INPUT_B_1 + i;
			controls[i].br = 0;
			controls[i].fader = PARAM_FADE_1 + i;
			controls[i].cv = INPUT_CV_1 + i;
			controls[i].out = OUTPUT_1 + i;
			controls[i].outr = 0;
			controls[i].polar = PARAM_CV_1 + i;
			controls[i].mode = PARAM_MODE_1 + i;
			controls[i].light1 = LIGHT_LIN_1 + i;
			controls[i].light2 = LIGHT_LOG_1 + i;
			controls[i].light3 = LIGHT_AUTO_1 + i * 2;
			controls[i].correlator = &correlators[i];
		}
		for (int i = 0; i < deviceCount / 2; i++) {
			int x = i * 2;
			controls[i + deviceCount].a = INPUT_A_1 + x;
			controls[i + deviceCount].ar = INPUT_A_2 + x;
			controls[i + deviceCount].b = INPUT_B_1 + x;
			controls[i + deviceCount].br = INPUT_B_2 + x;
			controls[i + deviceCount].fader = PARAM_FADE_1 + x;
			controls[i + deviceCount].cv = INPUT_CV_1 + x;
			controls[i + deviceCount].out = OUTPUT_1 + x;
			controls[i + deviceCount].outr = OUTPUT_2 + x;
			controls[i + deviceCount].polar = PARAM_CV_1 + x;
			controls[i + deviceCount].mode = PARAM_MODE_1 + x;
			controls[i + deviceCount].light1 = LIGHT_LIN_1 + x;
			controls[i + deviceCount].light2 = LIGHT_LOG_1 + x;
			controls[i + deviceCount].light3 = LIGHT_AUTO_1 + x * 2;
			controls[i + deviceCount].correlator = &correlators[x];
		}
	}
	void process(const ProcessArgs &args) override;
};

void XF_104::process(const ProcessArgs &args) {
	if (params[PARAM_LINK_1].getValue() > 0.5f) {
		crossFade(&controls[4]);
	}
	else {
		crossFade(&controls[0]);
		crossFade(&controls[1]);
	}
	if (params[PARAM_LINK_2].getValue() > 0.5f) {
		crossFade(&controls[5]);
	}
	else {
		crossFade(&controls[2]);
		crossFade(&controls[3]);
	}
}

struct XF104 : SchemeModuleWidget {
	XF104(XF_104 *module) : SchemeModuleWidget(module) {
		XF_LightKnob *fader;
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		for (int i = 0; i < XF_104::deviceCount; i++) {
			int offset = 88 * i;
			addInput(createInputCentered<SilverPort>(Vec(40,30.5 + offset), module, XF_104::INPUT_A_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(140,30.5 + offset), module, XF_104::INPUT_B_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(40,86.5 + offset), module, XF_104::INPUT_CV_1 + i));

			addOutput(createOutputCentered<SilverPort>(Vec(140,86.5 + offset), module, XF_104::OUTPUT_1 + i));

			addParam(createParamCentered<SubSwitch2>(Vec(48, 58.5 + offset), module, XF_104::PARAM_CV_1 + i));
			addParam(createParamCentered<SubSwitch3>(Vec(132, 58.5 + offset), module, XF_104::PARAM_MODE_1 + i));
			fader = createParamCentered<XF_LightKnob>(Vec(90, 58 + offset), module, XF_104::PARAM_FADE_1 + i);
			fader->cv = XF_104::INPUT_CV_1 + i;
			switch (i) {
				case 0:
				case 2:
					fader->link = 0;
					break;
				case 1:
					fader->link = XF_104::PARAM_LINK_1;
					break;
				case 3:
					fader->link = XF_104::PARAM_LINK_2;
					break;
			}
			addParam(fader);
			if (module) {
				module->configParam(XF_104::PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f);
				module->configParam(XF_104::PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f);
				module->configParam(XF_104::PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f);
			}

			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 48.5 + offset), module, XF_104::LIGHT_LIN_1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 58.5 + offset), module, XF_104::LIGHT_LOG_1 + i));
			addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(142.5, 68.5 + offset), module, XF_104::LIGHT_AUTO_1 + i * 2));
		}

		addParam(createParamCentered<LightButton>(Vec(98, 102.5), module, XF_104::PARAM_LINK_1));
		addParam(createParamCentered<LightButton>(Vec(98, 278.5), module, XF_104::PARAM_LINK_2));
		if (module) {
			module->configParam(XF_104::PARAM_LINK_1, 0.0f, 1.0f, 0.0f);
			module->configParam(XF_104::PARAM_LINK_2, 0.0f, 1.0f, 0.0f);
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-104");
		
		// Dividers
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 3, 102.5);
		nvgLineTo(vg, 67, 102.5);
		nvgMoveTo(vg, 109, 102.5);
		nvgLineTo(vg, 177, 102.5);
		
		nvgMoveTo(vg, 3, 190.5);
		nvgLineTo(vg, 177, 190.5);

		nvgMoveTo(vg, 3, 278.5);
		nvgLineTo(vg, 67, 278.5);
		nvgMoveTo(vg, 109, 278.5);
		nvgLineTo(vg, 177, 278.5);
		nvgStroke(vg);

		nvgStrokeColor(vg, gScheme.getContrast(module));
	
		for (unsigned int i = 0; i < 4; i++) {
			nvgBeginPath(vg);
			nvgMoveTo(vg, 65.000000, 83.500000 + 88 * i);
			nvgBezierTo(vg, 66.333336, 82.166664 + 88 * i, 67.666664, 80.833336 + 88 * i, 69.000000, 79.500000 + 88 * i);
			nvgBezierTo(vg, 60.295670, 70.968048 + 88 * i, 57.618584, 58.017834 + 88 * i, 62.226555, 46.733948 + 88 * i);
			nvgBezierTo(vg, 66.834526, 35.450062 + 88 * i, 77.811501, 28.075714 + 88 * i, 90.000000, 28.075714 + 88 * i);
			nvgBezierTo(vg, 102.188499, 28.075714 + 88 * i, 113.165482, 35.450069 + 88 * i, 117.773453, 46.733955 + 88 * i);
			nvgBezierTo(vg, 122.381424, 58.017841 + 88 * i, 119.704330, 70.968056 + 88 * i, 111.000000, 79.500000 + 88 * i);
			nvgBezierTo(vg, 112.333336, 80.833336 + 88 * i, 113.666664, 82.166664 + 88 * i, 115.000000, 83.500000 + 88 * i);
			nvgStroke(vg);

			drawText(vg, 57, 87 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
			drawText(vg, 116, 87 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "B");
			drawText(vg, 39, 55 + 88 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "UNI");
			drawText(vg, 39, 67 + 88 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BI");
			drawText(vg, 25, 36 + 88 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "A");
			drawText(vg, 27, 92 + 88 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "CV");
			drawText(vg, 155, 36 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "B");
			drawText(vg, 155, 92 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 18, gScheme.getContrast(module), "\xe2\x86\xa6");
			drawText(vg, 146, 52 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LIN");
			drawText(vg, 146, 61 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LOG");
			drawText(vg, 146, 70 + 88 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "AUTO");
		}
		drawText(vg, 88, 105, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LINK");
		drawText(vg, 88, 281, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LINK");
	}
};

Model *modelXF104 = createModel<XF_104, XF104>("XF-104");
