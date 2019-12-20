//SubTag W2

#include "SubmarineFree.hpp"
#include <random>
#include <chrono>

namespace {

	static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());

	struct Gridcell {
		float llx;// x-coord lower left
		float lrx;// x-coord lower right
		float ulx;// x-coord upper left
		float urx;// x-coord upper right
		float lly;// y-coord lower left
		float lry;// y-coord lower right
		float uly;// y-coord upper left
		float ury;// y-coord upper right
	};

	const unsigned int maxGridWidth = 16;

	float sineLookup[16] = {
		0.195090322016128f,
		0.555570233019602f,
		0.831469612302545f,
		0.980785280403230f,
		0.980785280403230f,
		0.831469612302545f,
		0.555570233019602f,
		0.195090322016128f,
		-0.195090322016128f,
		-0.555570233019602f,
		-0.831469612302545f,
		-0.980785280403230f,
		-0.980785280403230f,
		-0.831469612302545f,
		-0.555570233019602f,
		-0.195090322016128f
	};

	float normaliserLookup[32] = {
		1.0f / (0.01247338367 * (1.0f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f)),
		1.0f / (0.01247338367 * (1.0f + 0.25f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.25f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.25f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.25f)),
		1.0f / (0.01247338367 * (1.0f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.25f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.25f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.25f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.25f + 0.2f)),
		1.0f / (0.01247338367 * (1.0f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.25f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.25f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.25f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.25f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.25f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.25f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.3333333333f + 0.25f + 0.2f + 0.125f)),
		1.0f / (0.01247338367 * (1.0f + 0.5f + 0.3333333333f + 0.25f + 0.2f + 0.125f))
	};
}

struct SN_1 : Module {
	enum ParamIds {
		PARAM_FREQ,
		PARAM_EVOL,
		PARAM_LENGTH,
		PARAM_HARM_2,
		PARAM_HARM_3,
		PARAM_HARM_4,
		PARAM_HARM_5,
		PARAM_HARM_8,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_FREQ,
		INPUT_EVOL,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	alignas(64) Gridcell grid[maxGridWidth];
	__m128i lfsr;
	int effectiveGridWidth = 4.0f;
	float x = 0.0f;
	float y = 0.0f;

	SN_1() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_FREQ, -54.0f, +54.0f, 0.0f, "Frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
		configParam(PARAM_EVOL, 0.0f, 1.0f, 0.0f, "Evolution");
		configParam(PARAM_LENGTH, 2.0f, maxGridWidth, effectiveGridWidth, "Cycle Length");
		configParam(PARAM_HARM_2, 0.0f, 1.0f, 0.0f, "2x Harmonic");
		configParam(PARAM_HARM_3, 0.0f, 1.0f, 0.0f, "3x Harmonic");
		configParam(PARAM_HARM_4, 0.0f, 1.0f, 0.0f, "4x Harmonic");
		configParam(PARAM_HARM_5, 0.0f, 1.0f, 0.0f, "5x Harmonic");
		configParam(PARAM_HARM_8, 0.0f, 1.0f, 0.0f, "8x Harmonic");
		initLFSR(lfsr);
		initGridRow();
		shiftGridRows();
		initGridRow();
	}

	inline void initLFSR(__m128i &lfsr) {
		alignas(16) uint16_t seeds[8];
		std::uniform_int_distribution<int> distribution(1, 0xffffu);
		for (unsigned int i = 0; i < 8; i++) {
			seeds[i] = distribution(generator);
		}
		lfsr = _mm_load_si128((__m128i *)(seeds));
	}

	inline void advanceLFSR(__m128i &lfsr) {
		__m128i mask = _mm_set1_epi16(0x0u);
		mask = _mm_cmpgt_epi16(mask, lfsr);
		mask = _mm_and_si128(mask, _mm_set1_epi16(0x100Bu));
		lfsr = _mm_slli_epi16(lfsr, 1);
		lfsr = _mm_xor_si128(lfsr, mask);
	}
	
	inline void shiftGridRows() {
		std::memmove(grid, (uint8_t *)(grid) + 2 * sizeof(float), sizeof(grid) - 2 * sizeof(float));
	}
	
	inline void initGridRow() {
		alignas(16) uint16_t randomValues[maxGridWidth];
		for (unsigned int i = 0; i < maxGridWidth; i += 8) {
			for (unsigned int x = 0; x < 4; x++) {
				advanceLFSR(lfsr);
			}
			_mm_store_si128((__m128i *)(randomValues + i), lfsr);
		}
		for (unsigned int i = 0; i < maxGridWidth; i++) {
			grid[i].ulx = sineLookup[randomValues[i] & 0xf];
			randomValues[i] += 4;
			grid[i].uly = sineLookup[randomValues[i] & 0xf];
		}
		for (unsigned int i = 0; i < maxGridWidth; i++) {
			unsigned int j = (i + 1) % maxGridWidth;
			grid[i].urx = grid[j].ulx;
			grid[i].ury = grid[j].uly;
		}
		grid[(effectiveGridWidth - 1)].urx = grid[0].ulx;
		grid[(effectiveGridWidth - 1)].ury = grid[0].uly;
	}

	void resetY() {
		y = y - (int)y;
		shiftGridRows();
		initGridRow();
	}

	void resetX() {
		int intX = (int)x;
		x -= intX;
		x += intX % effectiveGridWidth;
		int newEffectiveGridWidth = clamp(params[PARAM_LENGTH].getValue(), 2.0f, maxGridWidth);
		if (effectiveGridWidth != newEffectiveGridWidth) {
			grid[(effectiveGridWidth - 1)].lrx = grid[effectiveGridWidth % maxGridWidth].llx;
			grid[(effectiveGridWidth - 1)].urx = grid[effectiveGridWidth % maxGridWidth].ulx;
			grid[(effectiveGridWidth - 1)].lry = grid[effectiveGridWidth % maxGridWidth].lly;
			grid[(effectiveGridWidth - 1)].ury = grid[effectiveGridWidth % maxGridWidth].uly;
			effectiveGridWidth = newEffectiveGridWidth;
			grid[(effectiveGridWidth - 1)].lrx = grid[0].llx;
			grid[(effectiveGridWidth - 1)].urx = grid[0].ulx;
			grid[(effectiveGridWidth - 1)].lry = grid[0].lly;
			grid[(effectiveGridWidth - 1)].ury = grid[0].uly;
		}
	}
	inline float noiseFunction(float xoffset)
	{
		int cell = (int)xoffset;
		xoffset = xoffset - cell;
		cell %= effectiveGridWidth;

		__m128 dvX = _mm_set_ps(xoffset - 1.0f, xoffset, xoffset - 1.0f, xoffset);
		__m128 dvY = _mm_set_ps(y - 1.0f, y - 1.0f, y, y);

		dvY = _mm_mul_ps(dvY, _mm_set1_ps(0.5f));
		dvX = _mm_sub_ps(dvX, dvY);
		dvY = _mm_mul_ps(dvY, _mm_set1_ps(1.73205080757f));

		__m128 dot = _mm_add_ps(_mm_mul_ps(dvX, _mm_load_ps(&(grid[cell].llx))), _mm_mul_ps(dvY, _mm_load_ps(&(grid[cell].lly))));

		dvX = _mm_mul_ps(dvX, dvX);
		dvY = _mm_mul_ps(dvY, dvY);
		dvX = _mm_sub_ps(_mm_set1_ps(0.75f), _mm_add_ps(dvX, dvY));
		dvX = _mm_max_ps(dvX, _mm_set1_ps(0.0f));
		dvX = _mm_mul_ps(dvX, dvX);
		dvX = _mm_mul_ps(dvX, dvX);
		dvX = _mm_mul_ps(dvX, dot);
		alignas(16) float nodes[4];
		_mm_store_ps(nodes, dvX);
		return (nodes[0] + nodes[1] + nodes[2] + nodes[3]);
	}

	void process(const ProcessArgs &args) override {
		float baseFreq = 261.626f;
		float freq = baseFreq * powf(2.0f, params[PARAM_FREQ].getValue() * (1.0f / 12.0f) + (inputs[INPUT_FREQ].isConnected()?inputs[INPUT_FREQ].getVoltage():0.0f));
		float deltaTime = freq * args.sampleTime;
		x = x + deltaTime;
		while (x >= effectiveGridWidth)
			resetX();
		float evol = params[PARAM_EVOL].getValue() + (inputs[INPUT_EVOL].isConnected()?inputs[INPUT_EVOL].getVoltage():0.0f) * 0.1f;
		evol *= evol;
		y += clamp(evol, 0.0f, 1.0f) * args.sampleTime;
		if (y >= 1)
			resetY();	

		float output = noiseFunction(x);
		int normaliser = 0;
		if (params[PARAM_HARM_2].getValue()) {
			output += noiseFunction(x * 2) * 0.5f;
			normaliser += 1;
		}
		if (params[PARAM_HARM_3].getValue()) {
			output += noiseFunction(x * 3) * 0.33333333333f;
			normaliser += 2;
		}
		if (params[PARAM_HARM_4].getValue()) {
			output += noiseFunction(x * 4) * 0.25f;
			normaliser += 4;
		}
		if (params[PARAM_HARM_5].getValue()) {
			output += noiseFunction(x * 5) * 0.2f;
			normaliser += 8;
		}
		if (params[PARAM_HARM_8].getValue()) {
			output += noiseFunction(x * 8) * 0.125f;
			normaliser += 16;
		}

		outputs[OUTPUT_1].setVoltage(normaliserLookup[normaliser] * output,0);
	}
};

struct SN101 : SchemeModuleWidget {
	LightKnob *lengthKnob;
	SN101(SN_1 *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(15,31.5), module, SN_1::INPUT_FREQ));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 70), module, SN_1::PARAM_FREQ));

		addInput(createInputCentered<SilverPort>(Vec(15,101.5), module, SN_1::INPUT_EVOL));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 140), module, SN_1::PARAM_EVOL));
		lengthKnob = createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(15, 170), module, SN_1::PARAM_LENGTH);
		addParam(lengthKnob);
		addParam(createParamCentered<LightButton>(Vec(20, 210), module, SN_1::PARAM_HARM_2));
		addParam(createParamCentered<LightButton>(Vec(20, 230), module, SN_1::PARAM_HARM_3));
		addParam(createParamCentered<LightButton>(Vec(20, 250), module, SN_1::PARAM_HARM_4));
		addParam(createParamCentered<LightButton>(Vec(20, 270), module, SN_1::PARAM_HARM_5));
		addParam(createParamCentered<LightButton>(Vec(20, 290), module, SN_1::PARAM_HARM_8));
		addOutput(createOutputCentered<SilverPort>(Vec(15,350), module, SN_1::OUTPUT_1));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "SN-101");
		drawText(vg, 16, 55, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "FREQ.");
		drawText(vg, 16, 125, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "EVOL.");
		drawText(vg, 16, 195, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "LEN.");
		drawText(vg, 7, 210, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "2x");
		drawText(vg, 7, 230, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "3x");
		drawText(vg, 7, 250, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "4x");
		drawText(vg, 7, 270, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "5x");
		drawText(vg, 7, 290, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "8x");
		drawText(vg, 16, 333, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT");
	}
	void step() override {
		if (module) {
			lengthKnob->color = (APP->engine->getParam(module, SN_1::PARAM_LENGTH) == dynamic_cast<SN_1 *>(module)->effectiveGridWidth)?SUBLIGHTBLUE:SUBLIGHTRED;
		}	
		SchemeModuleWidget::step();
	}
};

Model *modelSN101 = createModel<SN_1, SN101>("SN-101");
