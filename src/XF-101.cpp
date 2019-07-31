//SubTag W12

#include "XF.hpp"

struct XF_101 : XF {
	enum ParamIds {
		PARAM_CV_1,
		PARAM_MODE_1,
		PARAM_FADE_1,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1,
		INPUT_B_1,
		INPUT_CV_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1,
		LIGHT_LOG_1,
		LIGHT_AUTO_1,
		LIGHT_INV_1,
		NUM_LIGHTS
	};
	XF_Correlator correlators[1];
	XF_Controls controls[1] = {
		{
			INPUT_A_1,
			0,
			INPUT_B_1,
			0,
			PARAM_FADE_1,
			INPUT_CV_1,
			OUTPUT_1,
			0,
			PARAM_CV_1,
			PARAM_MODE_1,
			LIGHT_LIN_1,
			LIGHT_LOG_1,
			LIGHT_AUTO_1,
			&correlators[0]
		}
	};

	XF_101() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(PARAM_CV_1, 0.0f, 1.0f, 0.0f, "CV is bipolar");
		configParam(PARAM_MODE_1, 0.0f, 2.0f, 0.0f, "Fade profile");
		configParam(PARAM_FADE_1, 0.0f, 10.0f, 5.0f, "A/B blend", "%", 0.f, 10.f );
	}

	void process(const ProcessArgs &args) override;
};

void XF_101::process(const ProcessArgs &args) {
	crossFade(&controls[0]);
}

struct XF101 : SchemeModuleWidget {
	XF101(XF_101 *module) : SchemeModuleWidget(module) {
		XF_LightKnob *fader;
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(40,30.5), module, XF_101::INPUT_A_1));
		addInput(createInputCentered<SilverPort>(Vec(140,30.5), module, XF_101::INPUT_B_1));
		addInput(createInputCentered<SilverPort>(Vec(40,86.5), module, XF_101::INPUT_CV_1));

		addOutput(createOutputCentered<SilverPort>(Vec(140,86.5), module, XF_101::OUTPUT_1));

		addParam(createParamCentered<SubSwitch2>(Vec(48, 58.5), module, XF_101::PARAM_CV_1));
		addParam(createParamCentered<SubSwitch3>(Vec(132, 58.5), module, XF_101::PARAM_MODE_1));
		fader = createParamCentered<XF_LightKnob>(Vec(90, 58), module, XF_101::PARAM_FADE_1);
		fader->cv = XF_101::INPUT_CV_1;
		fader->link = 0;
		addParam(fader);

		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 48.5), module, XF_101::LIGHT_LIN_1));
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 58.5), module, XF_101::LIGHT_LOG_1));
		addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(142.5, 68.5), module, XF_101::LIGHT_AUTO_1));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-101");
		
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
	
		nvgBeginPath(vg);
		nvgMoveTo(vg, 65.000000, 83.500000);
		nvgBezierTo(vg, 66.333336, 82.166664, 67.666664, 80.833336, 69.000000, 79.500000);
		nvgBezierTo(vg, 60.295670, 70.968048, 57.618584, 58.017834, 62.226555, 46.733948);
		nvgBezierTo(vg, 66.834526, 35.450062, 77.811501, 28.075714, 90.000000, 28.075714);
		nvgBezierTo(vg, 102.188499, 28.075714, 113.165482, 35.450069, 117.773453, 46.733955);
		nvgBezierTo(vg, 122.381424, 58.017841, 119.704330, 70.968056, 111.000000, 79.500000);
		nvgBezierTo(vg, 112.333336, 80.833336, 113.666664, 82.166664, 115.000000, 83.500000);
		nvgStroke(vg);

		drawText(vg, 57, 87, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
		drawText(vg, 116, 87, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "B");
		drawText(vg, 39, 55, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "UNI");
		drawText(vg, 39, 67, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BI");
		drawText(vg, 25, 36, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "A");
		drawText(vg, 27, 92, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "CV");
		drawText(vg, 155, 36, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "B");
		drawText(vg, 155, 92, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 18, gScheme.getContrast(module), "\xe2\x86\xa6");
		drawText(vg, 146, 52, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LIN");
		drawText(vg, 146, 61, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LOG");
		drawText(vg, 146, 70, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "AUTO");
	}
};

Model *modelXF101 = createModel<XF_101, XF101>("XF-101");
