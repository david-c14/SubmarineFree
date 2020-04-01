//SubTag W8 W12
#include "SubmarineFree.hpp"

namespace {

	struct XF_Correlator {
		static const int frameSize = 1024;
		float samples_a[frameSize];
		float samples_b[frameSize];
		int n = 0;
		int sp = 0;
		float covariance = 0;
		float sigma_a = 0;
		float sigma_b = 0;
		float sigma_a2 = 0;
		float sigma_b2 = 0;
		int schmitt = 0;
		float correlation = 0;
	
		int correlate(float a, float b) {
			//Remove old samples
			if (n == frameSize) {
				covariance -= (samples_a[sp] * samples_b[sp]);
				sigma_a -= samples_a[sp];
				sigma_b -= samples_b[sp];
				sigma_a2 -= (samples_a[sp] * samples_a[sp]);
				sigma_b2 -= (samples_b[sp] * samples_b[sp]);
			}
			else {
				n++;
			}
		
			//Add new samples
			covariance += (a * b);
			sigma_a += samples_a[sp] = a;
			sigma_b += samples_b[sp] = b;
			sigma_a2 += (a * a);
			sigma_b2 += (b * b);
			sp++;
			if (sp > frameSize - 1) {
				sp -= frameSize;
			}
			float stdev_a = powf(sigma_a2 - (sigma_a * sigma_a / n), 0.5f);
			float stdev_b = powf(sigma_b2 - (sigma_b * sigma_b / n), 0.5f);
			if (stdev_a * stdev_b == 0.0f)
				correlation = (stdev_a == stdev_b);
			else
				correlation = covariance / (stdev_a * stdev_b);
			if (schmitt) {
				if (fabs(correlation) < 0.4)
					schmitt = 0;
			}
			else {
				if (fabs(correlation) > 0.6)
					schmitt = 1;
			}
			return schmitt;
		}

	};

	struct XF_Controls {
		int a;
		int ar;
		int b;
		int br;
		int fader;
		int cv;
		int out;
		int outr;
		int polar;
		int mode;
		int light1;
		int light2;
		int light3;
		XF_Correlator *correlator;
	}; 

	struct XF_LightKnob : LargeKnob<NarrowKnob<LightKnob>> {
		int cv;
		int link;
		void step() override {
			if (paramQuantity) {
				Module *module = paramQuantity->module;
				if (link) {
					setEnabled(!module->inputs[cv].isConnected() && (module->params[link].getValue() < 0.5f));
				}
				else {
					setEnabled(!module->inputs[cv].isConnected());
				}
			}
			Knob::step();
		}
	};

	struct XF : Module {
		XF(int p, int i, int o, int l) : Module() {
			config(p, i, o, l);
		}
		void crossFade(XF_Controls *controls) {
			float fade = clamp((inputs[controls->cv].isConnected()?params[controls->polar].getValue() * 5.0f + inputs[controls->cv].getVoltage():params[controls->fader].getValue())/10.0f, 0.0f, 1.0f);
			int mode = 0;
			if (params[controls->mode].getValue() > 1.5f) {
				mode = controls->correlator->correlate(inputs[controls->a].getVoltage(), inputs[controls->b].getVoltage());
				if (controls->correlator->correlation < -0.1f) {
					lights[controls->light3].setBrightness(0.0f);
					lights[controls->light3 + 1].setBrightness(1.0f);
				}
				else {
					lights[controls->light3].setBrightness(1.0f);
					lights[controls->light3 + 1].setBrightness(0.0f);
				}
			}
			else if (params[controls->mode].getValue() > 0.5f) {
				mode = 0;
				lights[controls->light3].setBrightness(0.0f);
				lights[controls->light3 + 1].setBrightness(0.0f);
			}
			else {
				mode = 1;
				lights[controls->light3].setBrightness(0.0f);
				lights[controls->light3 + 1].setBrightness(0.0f);
			}
			if (mode == 0) {
				outputs[controls->out].setVoltage(inputs[controls->a].getVoltage() * powf(1.0f - fade, 0.5f) + inputs[controls->b].getVoltage() * powf(fade, 0.5f));
				if (controls->outr)
					outputs[controls->outr].setVoltage(inputs[controls->ar].getVoltage() * powf(1.0f - fade, 0.5f) + inputs[controls->br].getVoltage() * powf(fade, 0.5f));
				lights[controls->light1].setBrightness(0.0f);
				lights[controls->light2].setBrightness(1.0f);
			}
			else {
				outputs[controls->out].setVoltage(inputs[controls->a].getVoltage() * (1.0f - fade) + inputs[controls->b].getVoltage() * fade);
				if (controls->outr)
					outputs[controls->outr].setVoltage(inputs[controls->ar].getVoltage() * (1.0f - fade) + inputs[controls->br].getVoltage() * fade);
				lights[controls->light1].setBrightness(1.0f);
				lights[controls->light2].setBrightness(0.0f);
			}
		}
	};

}	

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

	void process(const ProcessArgs &args) override {
		crossFade(&controls[0]);
	}
};

struct XF101 : SchemeModuleWidget {
	XF101(XF_101 *module) {
		setModule(module);
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

struct XF_102 : XF {
	static const int deviceCount = 2;
	enum ParamIds {
		PARAM_CV_1, PARAM_CV_2,
		PARAM_MODE_1, PARAM_MODE_2,
		PARAM_FADE_1, PARAM_FADE_2,
		PARAM_LINK_1,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1, INPUT_A_2,
		INPUT_B_1, INPUT_B_2,
		INPUT_CV_1, INPUT_CV_2,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1, OUTPUT_2,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1, LIGHT_LIN_2,
		LIGHT_LOG_1, LIGHT_LOG_2,
		LIGHT_AUTO_1, LIGHT_INV_1, LIGHT_AUTO_2, LIGHT_INV_2,
		NUM_LIGHTS
	};
	XF_Correlator correlators[deviceCount];
	XF_Controls controls[(int)(deviceCount * 1.5f)];

	XF_102() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam(PARAM_LINK_1, 0.0f, 1.0f, 0.0f, "Link faders");
		for (int i = 0; i < deviceCount; i++) {
			configParam(PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f, string::f("Fader %d CV is bipolar", i + 1));
			configParam(PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f, string::f("Fader %d Fade profile", i + 1));
			configParam(PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f, string::f("Fader %d A/B blend", i + 1), "%", 0.f, 10.f);
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

	void process(const ProcessArgs &args) override {
		if (params[PARAM_LINK_1].getValue() > 0.5f) {
			crossFade(&controls[2]);
		}
		else {
			crossFade(&controls[0]);
			crossFade(&controls[1]);
		}
	}
};

struct XF102 : SchemeModuleWidget {
	XF102(XF_102 *module) {
		setModule(module);
		XF_LightKnob *fader;
		this->box.size = Vec(180, 380);
		addChild(new SchemePanel(this->box.size));
		for (int i = 0; i < XF_102::deviceCount; i++) {
			int offset = 88 * i;
			addInput(createInputCentered<SilverPort>(Vec(40,30.5 + offset), module, XF_102::INPUT_A_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(140,30.5 + offset), module, XF_102::INPUT_B_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(40,86.5 + offset), module, XF_102::INPUT_CV_1 + i));

			addOutput(createOutputCentered<SilverPort>(Vec(140,86.5 + offset), module, XF_102::OUTPUT_1 + i));

			addParam(createParamCentered<SubSwitch2>(Vec(48, 58.5 + offset), module, XF_102::PARAM_CV_1 + i));
			addParam(createParamCentered<SubSwitch3>(Vec(132, 58.5 + offset), module, XF_102::PARAM_MODE_1 + i));
			fader = createParamCentered<XF_LightKnob>(Vec(90, 58 + offset), module, XF_102::PARAM_FADE_1 + i);
			fader->cv = XF_102::INPUT_CV_1 + i;
			fader->link = i?XF_102::PARAM_LINK_1:0;
			addParam(fader);

			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 48.5 + offset), module, XF_102::LIGHT_LIN_1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 58.5 + offset), module, XF_102::LIGHT_LOG_1 + i));
			addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(142.5, 68.5 + offset), module, XF_102::LIGHT_AUTO_1 + i * 2));
		}

		addParam(createParamCentered<LightButton>(Vec(98, 102.5), module, XF_102::PARAM_LINK_1));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-102");
		
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
		nvgStroke(vg);

		nvgStrokeColor(vg, gScheme.getContrast(module));
	
		for (unsigned int i = 0; i < 2; i++) {
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
	}
};

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
		configParam(PARAM_LINK_1, 0.0f, 1.0f, 0.0f, "Link faders 1 & 2");
		configParam(PARAM_LINK_2, 0.0f, 1.0f, 0.0f, "Link faders 3 & 4");
		for (int i = 0; i < deviceCount; i++) {
			configParam(PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f, string::f("Fader %d CV is bipolar", i + 1));
			configParam(PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f, string::f("Fader %d Fade profile", i + 1));
			configParam(PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f, string::f("Fader %d A/B blend", i + 1), "%", 0.f, 10.f);
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

	void process(const ProcessArgs &args) override {
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
};

struct XF104 : SchemeModuleWidget {
	XF104(XF_104 *module) {
		setModule(module);
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

			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 48.5 + offset), module, XF_104::LIGHT_LIN_1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(142.5, 58.5 + offset), module, XF_104::LIGHT_LOG_1 + i));
			addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(142.5, 68.5 + offset), module, XF_104::LIGHT_AUTO_1 + i * 2));
		}

		addParam(createParamCentered<LightButton>(Vec(98, 102.5), module, XF_104::PARAM_LINK_1));
		addParam(createParamCentered<LightButton>(Vec(98, 278.5), module, XF_104::PARAM_LINK_2));
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

struct XF_201 : XF {
	static const int deviceCount = 1;
	enum ParamIds {
		PARAM_CV_1,
		PARAM_MODE_1,
		PARAM_FADE_1,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_1,
		INPUT_AR_1,
		INPUT_B_1,
		INPUT_BR_1,
		INPUT_CV_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		OUTPUTR_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_LIN_1,
		LIGHT_LOG_1,
		LIGHT_AUTO_1, LIGHT_INV_1,
		NUM_LIGHTS
	};
	XF_Correlator correlators[deviceCount];
	XF_Controls controls[deviceCount];

	XF_201() : XF(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		for (int i = 0; i < deviceCount; i++) {
			configParam(PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f, string::f("Fader %d CV is bipolar", i + 1));
			configParam(PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f, string::f("Fader %d Fade profile", i + 1));
			configParam(PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f, string::f("Fader %d A/B blend", i + 1), "%", 0.f, 10.f);
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

	void process(const ProcessArgs &args) override {
		crossFade(&controls[0]);
	}
};

struct XF201 : SchemeModuleWidget {
	XF201(XF_201 *module) {
		setModule(module);
		XF_LightKnob *fader;
		this->box.size = Vec(120, 380);
		addChild(new SchemePanel(this->box.size));
		for (int i = 0; i < XF_201::deviceCount; i++) {
			int offset = 176 * i;
			addInput(createInputCentered<SilverPort>(Vec(15.5,30.5 + offset), module, XF_201::INPUT_A_1 + i));
			addInput(createInputCentered<RedPort>(Vec(15.5,57.5 + offset), module, XF_201::INPUT_AR_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(104.5,30.5 + offset), module, XF_201::INPUT_B_1 + i));
			addInput(createInputCentered<RedPort>(Vec(104.5,57.5 + offset), module, XF_201::INPUT_BR_1 + i));
			addInput(createInputCentered<SilverPort>(Vec(15.5,132.5 + offset), module, XF_201::INPUT_CV_1 + i));

			addOutput(createOutputCentered<SilverPort>(Vec(104.5,105.5 + offset), module, XF_201::OUTPUT_1 + i));
			addOutput(createOutputCentered<RedPort>(Vec(104.5,132.5 + offset), module, XF_201::OUTPUTR_1 + i));

			addParam(createParamCentered<SubSwitch2>(Vec(35, 167 + offset), module, XF_201::PARAM_CV_1 + i));
			addParam(createParamCentered<SubSwitch3>(Vec(72, 167 + offset), module, XF_201::PARAM_MODE_1 + i));
			fader = createParamCentered<XF_LightKnob>(Vec(60, 78 + offset), module, XF_201::PARAM_FADE_1 + i);
			fader->cv = XF_201::INPUT_CV_1 + i;
			fader->link = 0;
			addParam(fader);

			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(82.5, 157.5 + offset), module, XF_201::LIGHT_LIN_1 + i));
			addChild(createLightCentered<TinyLight<BlueLight>>(Vec(82.5, 167.5 + offset), module, XF_201::LIGHT_LOG_1 + i));
			addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(82.5, 177.5 + offset), module, XF_201::LIGHT_AUTO_1 + i * 2));
		}

	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-201");
		
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
	
		nvgBeginPath(vg);
		nvgMoveTo(vg, 35.000000, 103.500000);
		nvgBezierTo(vg, 36.333336, 102.166664, 37.666664, 100.833336, 39.000000, 99.500000);
		nvgBezierTo(vg, 30.295670, 90.968048, 27.618584, 78.017834, 32.226555, 66.733948);
		nvgBezierTo(vg, 36.834526, 55.450062, 47.811501, 48.075714, 60.000000, 48.075714);
		nvgBezierTo(vg, 72.188499, 48.075714, 83.165482, 55.450069, 87.773453, 66.733955);
		nvgBezierTo(vg, 92.381424, 78.017841, 89.704330, 90.968056, 81.000000, 99.500000);
		nvgBezierTo(vg, 82.333336, 100.833336, 83.666664, 102.166664, 85.000000, 103.500000);
		nvgStroke(vg);

		drawText(vg, 34, 113, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
		drawText(vg, 80, 113, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "B");
		drawText(vg, 26, 164, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "UNI");
		drawText(vg, 26, 176, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BI");
		drawText(vg, 30, 36, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "A");
		drawText(vg, 28, 138, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "CV");
		drawText(vg, 90, 36, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "B");
		drawText(vg, 90, 138, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 18, gScheme.getContrast(module), "\xe2\x86\xa6");
		drawText(vg, 86, 161, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LIN");
		drawText(vg, 86, 170, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LOG");
		drawText(vg, 86, 179, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "AUTO");
	}
};

struct XF301 : SchemeModuleWidget {
	XF301(XF_201 *module) {
		setModule(module);
		XF_LightKnob *fader;
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));


		addInput(createInputCentered<SilverPort>(Vec(15,30), module, XF_201::INPUT_A_1));
		addInput(createInputCentered<RedPort>(Vec(15,55), module, XF_201::INPUT_AR_1));

		fader = createParamCentered<SmallKnob<XF_LightKnob>>(Vec(15, 105), module, XF_201::PARAM_FADE_1);
		fader->cv = XF_201::INPUT_CV_1;
		addParam(fader);

		addInput(createInputCentered<SilverPort>(Vec(15,155), module, XF_201::INPUT_B_1));
		addInput(createInputCentered<RedPort>(Vec(15,180), module, XF_201::INPUT_BR_1));

		addInput(createInputCentered<SilverPort>(Vec(15,220), module, XF_201::INPUT_CV_1));

		addOutput(createOutputCentered<SilverPort>(Vec(15,260), module, XF_201::OUTPUT_1));
		addOutput(createOutputCentered<RedPort>(Vec(15,285), module, XF_201::OUTPUTR_1));

		addParam(createParamCentered<SubSwitch2>(Vec(9, 315), module, XF_201::PARAM_CV_1));
		addParam(createParamCentered<SubSwitch3>(Vec(9, 345), module, XF_201::PARAM_MODE_1));

		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(19, 335), module, XF_201::LIGHT_LIN_1));
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(19, 345), module, XF_201::LIGHT_LOG_1));
		addChild(createLightCentered<TinyLight<BlueRedLight>>(Vec(19, 355), module, XF_201::LIGHT_AUTO_1));

	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "XF-301");
		
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);

		drawText(vg, 15, 75, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "A");

		drawText(vg, 15, 140, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "B");

		drawText(vg, 15, 205, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CV");

		drawText(vg, 15, 245, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT");

		drawText(vg, 18, 310, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "1P");
		drawText(vg, 18, 322, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "2P");

		nvgBeginPath(vg);
		nvgMoveTo(vg, 22, 338);
		nvgLineTo(vg, 28, 332);
		nvgMoveTo(vg, 22, 348);
		nvgBezierTo(vg, 22, 344, 24, 342, 28, 342);
		nvgStroke(vg);
		drawText(vg, 22, 355, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "A");
	}
};

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
			configParam(PARAM_CV_1 + i, 0.0f, 1.0f, 0.0f, string::f("Fader %d CV is bipolar", i + 1));
			configParam(PARAM_MODE_1 + i, 0.0f, 2.0f, 0.0f, string::f("Fader %d Fade profile", i + 1));
			configParam(PARAM_FADE_1 + i, 0.0f, 10.0f, 5.0f, string::f("Fader %d A/B blend", i + 1), "%", 0.f, 10.f);
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

	void process(const ProcessArgs &args) override {
		crossFade(&controls[0]);
		crossFade(&controls[1]);
	}
};

struct XF202 : SchemeModuleWidget {
	XF202(XF_202 *module) {
		setModule(module);
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

			addParam(createParamCentered<SubSwitch2>(Vec(35, 167 + offset), module, XF_202::PARAM_CV_1 + i));
			addParam(createParamCentered<SubSwitch3>(Vec(72.5, 167 + offset), module, XF_202::PARAM_MODE_1 + i));
			fader = createParamCentered<XF_LightKnob>(Vec(60, 78 + offset), module, XF_202::PARAM_FADE_1 + i);
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
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgLineJoin(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 3, 190.5);
		nvgLineTo(vg, 117, 190.5);
		nvgStroke(vg);

		nvgStrokeColor(vg, gScheme.getContrast(module));
	
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

			drawText(vg, 34, 113 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "A");
			drawText(vg, 80, 113 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 10, gScheme.getContrast(module), "B");
			drawText(vg, 26, 164 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "UNI");
			drawText(vg, 26, 176 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "BI");
			drawText(vg, 30, 36 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "A");
			drawText(vg, 28, 138 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "CV");
			drawText(vg, 90, 36 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 16, gScheme.getContrast(module), "B");
			drawText(vg, 90, 138 + 176 * i, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE, 18, gScheme.getContrast(module), "\xe2\x86\xa6");
			drawText(vg, 86, 161 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LIN");
			drawText(vg, 86, 170 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LOG");
			drawText(vg, 86, 179 + 176 * i, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "AUTO");
		}
	}
};

Model *modelXF101 = createModel<XF_101, XF101>("XF-101");
Model *modelXF102 = createModel<XF_102, XF102>("XF-102");
Model *modelXF104 = createModel<XF_104, XF104>("XF-104");
Model *modelXF201 = createModel<XF_201, XF201>("XF-201");
Model *modelXF202 = createModel<XF_202, XF202>("XF-202");
Model *modelXF301 = createModel<XF_201, XF301>("XF-301");
