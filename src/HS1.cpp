//SubTag W30

/* Portions of this code derive from Fundamental/src/Scope.cpp - Copyright 2017 by Andrew Belt */
#include <string.h>
#include "SubmarineFree.hpp"

struct HS_101 : Module {
	enum ParamIds {
		PARAM_TIME,
		PARAM_RUN,
		PARAM_X_PAN,
		PARAM_X_SCALE,
		PARAM_Y_PAN,
		PARAM_Y_SCALE,
		PARAM_COLORS,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		INPUT_TRIGGER,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_TRIGGER,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_STORING,
		NUM_LIGHTS
	};

	bool dataCaptured = false;
	int bufferCount = 0;
	int bufferSize = 0;
	float oldTimeParam = -20.0f;
	float *buffer = NULL;
	float time = 0;
	int bufferIndex = 0;
	bool running = false;
	dsp::SchmittTrigger trigger;
	dsp::PulseGenerator triggerOut;
	float minValue = +INFINITY;
	float maxValue = -INFINITY;
	std::vector<float *>mipEntries;
	
	HS_101() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_TIME, -4.0f, 6.0f, -4.0f, "Time base");
		configParam(PARAM_RUN, 0.0f, 1.0f, 1.0f, "Run");
		configParam(PARAM_X_PAN, 0.0f, 1.0f, 0.5f, "X Pan");
		configParam(PARAM_X_SCALE, 0.0f, +18.0f, 0.0f, "X Scale");
		configParam(PARAM_Y_PAN, 0.0f, 1.0f, 0.5f, "Y Pan");
		configParam(PARAM_Y_SCALE, 0.0f, +20.0f, 0.0f, "Y Scale");
		configParam(PARAM_COLORS, 0.0f, 1.0f, 0.0f, "Match cable colors");
	}

	~HS_101() {
		if (buffer) {
			free(buffer);
		}
		buffer = NULL;
		bufferSize = 0;
		dataCaptured = false;
	}

	void process(const ProcessArgs &args) override {
		Module::process(args);
		float newTimeParam = params[PARAM_TIME].getValue();
		if (newTimeParam != oldTimeParam) {
			resetMemory(args, newTimeParam);
		}
		if (!running) {
			float gate = inputs[INPUT_TRIGGER].getVoltage();
			int triggered = trigger.process(rescale(gate, 2.4f, 2.5f, 0.0f, 1.0f)); 
			if (params[PARAM_RUN].getValue() > 0.5f) {
				triggered = true;
				params[PARAM_RUN].setValue(0.0f);
			}
			running = triggered;
			if (running) {
				minValue = +INFINITY;
				maxValue = -INFINITY;
				triggerOut.trigger(0.01f);
			}
		}
		lights[LIGHT_STORING].setBrightness(running);
		outputs[OUTPUT_TRIGGER].setVoltage(10.0f * triggerOut.process(args.sampleTime));
		if (running) {
			trigger.reset();
			storeData(inputs[INPUT_1].getVoltage());
			if (bufferIndex >= bufferCount) {
				dataCaptured = true;
				running = false;
				bufferIndex = 0;
			}
		}
	}

	void storeData(float value) {
		buffer[bufferIndex] = value;
		minValue = std::min(minValue, value);
		maxValue = std::max(maxValue, value);
		bool notFirst = (bufferIndex & 0x3);
		int mipIndex = bufferIndex >> 2;
		for(float *mipPointer : mipEntries) {
			if (notFirst) {
				mipPointer[mipIndex * 2] = std::min(mipPointer[mipIndex * 2], value);
				mipPointer[mipIndex * 2 + 1] = std::max(mipPointer[mipIndex * 2 + 1], value);
			}
			else {
				mipPointer[mipIndex * 2]  = mipPointer[mipIndex * 2 + 1] = value;
			}
			assert((mipPointer + mipIndex * 2) < (buffer + (int)(bufferCount * 1.785f)));
			notFirst = (mipIndex & 0x3);
			mipIndex >>= 2;
		}
		bufferIndex++;
	}

	void generateMips() {
		mipEntries.clear();
		int dataSize = bufferCount;
		float * mipPointer = buffer + bufferCount; 
		while (dataSize > 1000) {
			mipEntries.push_back(mipPointer);
			dataSize >>= 1;
			mipPointer += dataSize;
			dataSize >>= 1;
		}
	}

	void resetMemory(const ProcessArgs &args, float timeParam) {
		if (buffer)
			free(buffer);
		oldTimeParam = timeParam;
		time = powf(2.0f, timeParam);
		bufferCount = time * args.sampleRate; 
		bufferSize = sizeof(float) * bufferCount * 1.875f;  // 1.875 gives additional space for the mipmap
		dataCaptured = false;
		buffer = (float *)malloc(bufferSize);
		bufferIndex = 0;
		running = false;
		trigger.reset();
		minValue = +INFINITY;
		maxValue = -INFINITY;
		generateMips();
	}

	void onSampleRateChange() override {
		oldTimeParam = -20.0f;
	}
};
	
namespace {
	struct HS_Info : TransparentWidget {
		HS_101 *module;
		void draw(const DrawArgs &args) override {
			nvgFontSize(args.vg, 12);
			nvgFontFaceId(args.vg, gScheme.font()->handle);
			nvgFillColor(args.vg, SUBLIGHTBLUE);
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);
			if (!module) {
				nvgFontSize(args.vg, 18);
				nvgText(args.vg, 2, 12, "Submarine", NULL);
				nvgText(args.vg, 2, 24, "High Resolution", NULL);
				nvgText(args.vg, 2, 36, "Storage Scope", NULL);
				return;
			}
			
			if (module->running) {
				nvgText(args.vg, 2, 12, "Storing", NULL);
			}
			else {
				if (module->dataCaptured) {
					nvgText(args.vg, 2, 12, "Stored", NULL);
				}
				else {
					nvgText(args.vg, 2, 12, "No Data", NULL);
				}
			}
			nvgText(args.vg, 2, 24, string::f("%.3fs", module->time).c_str(), NULL);
			if (std::isfinite(module->minValue)) 
				nvgText(args.vg, 2, 36, string::f("min %.3fV", module->minValue).c_str(), NULL);
			nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BASELINE);
			nvgText(args.vg, box.size.x - 2, 24, string::f("%.3fMb", module->bufferSize / 1000000.0f).c_str(), NULL);
			if (std::isfinite(module->maxValue))
				nvgText(args.vg, box.size.x - 2, 36, string::f("max %.3fV", module->maxValue).c_str(), NULL);
		}
	};

	struct HS_Display : OpaqueWidget {
		HS_101 *module;
		PortWidget *port;
		int minX, maxX;
		float minY, maxY;

		void draw(const DrawArgs &args) override {
			if (!module) {
				drawEasterEgg(args.vg);
				return;
			}
			if (!module->dataCaptured) {
				return;
			}
			NVGcolor col = SUBLIGHTBLUETRANS;
			if (module->params[HS_101::PARAM_COLORS].getValue()) {
				col = APP->scene->rack->getTopCable(port)->color;
				col.a = 1.0f;
			}
			drawTrace(args.vg, col, module->buffer, module->bufferCount);
		}
		void drawTrace(NVGcontext *vg, NVGcolor col, float *values, int bufferSize) {
			
			float maxfX = bufferSize / powf(2.0f, module->params[HS_101::PARAM_X_SCALE].getValue());
			float offset = (bufferSize - maxfX) * module->params[HS_101::PARAM_X_PAN].getValue();
			minX = std::max(0, (int)offset);
			maxX = std::min(bufferSize - 1, (int)(maxfX + offset));

			float rangeY = module->maxValue - module->minValue;
			float multiplier = 1 / powf(2.0f, module->params[HS_101::PARAM_Y_SCALE].getValue());
			offset = (rangeY - rangeY * multiplier) * module->params[HS_101::PARAM_Y_PAN].getValue();
			minY = module->minValue + offset;
			maxY = minY + (rangeY * multiplier);

			int entry = -1;
			int range = (maxX - minX);
			while (range > 1000) {
				entry++;
				if (entry == (signed int)module->mipEntries.size()) {
					entry--;
					break;
				}
				maxX >>= 2;
				minX >>= 2;
				range >>= 2;
			}
			if (entry == -1) 
				drawFullScale(vg, col, values, bufferSize);
			else 
				drawMipped(vg, col, module->mipEntries[entry], bufferSize >> (2 * (entry + 1)));
		}
		void drawFullScale(NVGcontext *vg, NVGcolor col, float *values, int bufferSize) {
			nvgSave(vg);
			Rect b = Rect(Vec(0,0), box.size);
			nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
			nvgBeginPath(vg);

			for (int i = minX; i <= maxX; i++) {
				float x, y;
				
				x = rescale(i, minX, maxX, 0, b.size.x);
				y = rescale(values[i], minY, maxY, b.size.y - 2, 2);
				if (i == minX)
					nvgMoveTo(vg, x, y);
				else
					nvgLineTo(vg, x, y);
			}
			nvgStrokeColor(vg, col);
			nvgLineCap(vg, NVG_ROUND);
			nvgMiterLimit(vg, 2.0f);
			nvgStrokeWidth(vg, 1.5f);
			nvgStroke(vg);
			nvgResetScissor(vg);
			nvgRestore(vg);
		}
		void drawMipped(NVGcontext *vg, NVGcolor col, float *values, int bufferSize) {
			nvgSave(vg);
			Rect b = Rect(Vec(0,0), box.size);
			nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
			nvgBeginPath(vg);

			for (int i = minX; i <= maxX; i++) {
				float x, y;
				
				x = rescale(i, minX, maxX, 0, b.size.x);
				y = rescale(values[i * 2 + 1], minY, maxY, b.size.y - 2, 2);
				if (i == minX)
					nvgMoveTo(vg, x, y);
				else
					nvgLineTo(vg, x, y);
			}
			nvgStrokeColor(vg, col);
			nvgLineCap(vg, NVG_ROUND);
			nvgMiterLimit(vg, 2.0f);
			nvgStrokeWidth(vg, 1.5f);
			nvgStroke(vg);
			nvgResetScissor(vg);
			nvgRestore(vg);
		}
/*
			drawTrace(args.vg, module->buffer[i], module->params[HS_101::PARAM_OFFSET_1 + i].getValue(), module->params[HS_101::PARAM_SCALE_1 + i].getValue(), col, module->traceMode[i]); 
		void drawTrace(NVGcontext *vg, float *values, float offset, float scale, NVGcolor col, int mode) {
			if (!values)
				return;
			float scaling = powf(2.0, scale);
			nvgSave(vg);
			Rect b = Rect(Vec(0, 0), box.size);
			nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
			nvgBeginPath(vg);
			for (int i = 0; i < BUFFER_SIZE; i++) {
				float x, y;
				x = (float)i / (BUFFER_SIZE - 1) * b.size.x;
				y = ((values[i] * scaling + offset ) / 20.0f - 0.8f) * -b.size.y;
				if (i == 0)
					nvgMoveTo(vg, x, y);
				else
					nvgLineTo(vg, x, y);
			} 
			if (mode) {
				nvgLineTo(vg, b.size.x, (offset / 20.0f - 0.8f) * -b.size.y);
				nvgLineTo(vg, 0, (offset / 20.0f - 0.8f) * -b.size.y);
				nvgClosePath(vg);
				nvgFillColor(vg, col);
				nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
				nvgFill(vg);
			}
			else {
				nvgStrokeColor(vg, col);
				nvgLineCap(vg, NVG_ROUND);
				nvgMiterLimit(vg, 2.0f);
				nvgStrokeWidth(vg, 1.5f);
				nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
				nvgStroke(vg);
			}
			nvgResetScissor(vg);
			nvgRestore(vg);	
		}*/
	

		void drawEasterEgg(NVGcontext *vg) {
			scheme::drawLogoPath(vg, 0, 0, 15, 0);
			nvgFillColor(vg, SUBLIGHTREDTRANS);
			nvgFill(vg);
			scheme::drawLogoPath(vg, 100, 50, 12, 0);
			nvgStrokeColor(vg, SUBLIGHTBLUETRANS);
			nvgStroke(vg);
		}
	};

/*
	struct EO_Display : TransparentWidget {
		HS_101 *module;
		PortWidget *ports[2];
	
		void drawTrace(NVGcontext *vg, float *values, float offset, float scale, NVGcolor col, int mode) {
			if (!values)
				return;
			float scaling = powf(2.0, scale);
			nvgSave(vg);
			Rect b = Rect(Vec(0, 0), box.size);
			nvgScissor(vg, b.pos.x, b.pos.y, b.size.x, b.size.y);
			nvgBeginPath(vg);
			for (int i = 0; i < BUFFER_SIZE; i++) {
				float x, y;
				x = (float)i / (BUFFER_SIZE - 1) * b.size.x;
				y = ((values[i] * scaling + offset ) / 20.0f - 0.8f) * -b.size.y;
				if (i == 0)
					nvgMoveTo(vg, x, y);
				else
					nvgLineTo(vg, x, y);
			} 
			if (mode) {
				nvgLineTo(vg, b.size.x, (offset / 20.0f - 0.8f) * -b.size.y);
				nvgLineTo(vg, 0, (offset / 20.0f - 0.8f) * -b.size.y);
				nvgClosePath(vg);
				nvgFillColor(vg, col);
				nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
				nvgFill(vg);
			}
			else {
				nvgStrokeColor(vg, col);
				nvgLineCap(vg, NVG_ROUND);
				nvgMiterLimit(vg, 2.0f);
				nvgStrokeWidth(vg, 1.5f);
				nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
				nvgStroke(vg);
			}
			nvgResetScissor(vg);
			nvgRestore(vg);	
		}
	
	};
	
	struct EO_Measure_Horz : EO_Measure {
		void updateText() override {
			if (!module) {
				return;
			} 
			float deltaTime = powf(2.0f, module->params[HS_101::PARAM_TIME].getValue());
			int frameCount = (int)ceilf(deltaTime * APP->engine->getSampleRate());
			frameCount *= BUFFER_SIZE;
			float width = (float)frameCount * fabs(module->params[HS_101::PARAM_INDEX_1].getValue() - module->params[HS_101::PARAM_INDEX_2].getValue()) / APP->engine->getSampleRate(); 
			
			if (width < 0.00000995f)
				sprintf(measureText, "%4.3f\xc2\xb5s", width * 1000000.0f);
			else if (width < 0.0000995f)
				sprintf(measureText, "%4.2f\xc2\xb5s", width * 1000000.0f);
			else if (width < 0.000995f)
				sprintf(measureText, "%4.1f\xc2\xb5s", width * 1000000.0f);
			else if (width < 0.00995f)
				sprintf(measureText, "%4.3fms", width * 1000.0f);
			else if (width < 0.0995f)
				sprintf(measureText, "%4.2fms", width * 1000.0f);
			else if (width < 0.995f)
				sprintf(measureText, "%4.1fms", width * 1000.0f);
			else if (width < 9.95f)
				sprintf(measureText, "%4.3fs", width);
			else if (width < 99.5f)
				sprintf(measureText, "%4.2fs", width);
			else
				sprintf(measureText, "%4.1fs", width);
		}
	};

*/
} // end namespace

struct HS101 : SchemeModuleWidget {
	LightButton *paramRun;
	HS101(HS_101 *module) {
		setModule(module);
		this->box.size = Vec(450, 380);
		addChild(new SchemePanel(this->box.size));

		HS_Info *info = new HS_Info();
		info->module = module;
		info->box.pos = Vec(270, 325);
		info->box.size = Vec(177.5, 40);
		addChild(info);

		HS_Display * display = new HS_Display();
		display->module = module;
		display->box.pos = Vec(2.5, 14);
		display->box.size = Vec(445, 310);
		addChild(display);

		PortWidget *port = createInputCentered<SilverPort>(Vec(17, 341), module, HS_101::INPUT_1);
		addInput(port);
		display->port = port;

		addInput(createInputCentered<SilverPort>(Vec(47, 341), module, HS_101::INPUT_TRIGGER));
		addOutput(createOutputCentered<SilverPort>(Vec(72, 341), module, HS_101::OUTPUT_TRIGGER));
		
		paramRun = createParamCentered<LightButton>(Vec(102, 341), module, HS_101::PARAM_RUN);
		addParam(paramRun);
		addChild(createLightCentered<TinyLight<BlueLight>>(Vec(115, 341), module, HS_101::LIGHT_STORING));

		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(140, 341), module, HS_101::PARAM_TIME));

		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(170, 341), module, HS_101::PARAM_X_PAN));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(195, 341), module, HS_101::PARAM_X_SCALE));

		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(225, 341), module, HS_101::PARAM_Y_PAN));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(250, 341), module, HS_101::PARAM_Y_SCALE));
	}

	void appendContextMenu(Menu *menu) override {
		if (!module)
			return;
		menu->addChild(new MenuSeparator);
		EventWidgetMenuItem *vmi = createMenuItem<EventWidgetMenuItem>("Match Cable Colors");
		vmi->stepHandler = [=]() {
			vmi->rightText = CHECKMARK(module->params[HS_101::PARAM_COLORS].getValue());
		};
		vmi->clickHandler = [=]() {
			bool val = module->params[HS_101::PARAM_COLORS].getValue();
			module->params[HS_101::PARAM_COLORS].setValue(!val);
		};
		menu->addChild(vmi);
	}

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "HS-101");

		// Display panels
		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 2.5, 14, 445, 310, 2);
		nvgRoundedRect(vg, 270, 325, 177.5, 40, 2);
		nvgFill(vg);
		
		// Text
		drawText(vg, 17, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");

		drawText(vg, 59.5, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "TRIGGER");

		drawText(vg, 102, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "STORE");

		drawText(vg, 140, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LENGTH");

		drawText(vg, 170, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "PAN");
		drawText(vg, 182.5, 357, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "X");
		drawText(vg, 195, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SCALE");

		drawText(vg, 225, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "PAN");
		drawText(vg, 237.5, 357, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "Y");
		drawText(vg, 250, 365, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SCALE");
	}
};

Model *modelHS101 = createModel<HS_101, HS101>("HS-101");
