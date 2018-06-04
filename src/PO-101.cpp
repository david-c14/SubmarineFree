#include "SubmarineFree.hpp"

struct PO_101 : Module {
	enum ParamIds {
		PARAM_TUNE,
		PARAM_FINE,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_NOTE_CV,
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

	PO_101() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;
	float phase = 0.0f;
};

void PO_101::step() {
	float freq = 440.0f * powf(2.0f, params[PARAM_TUNE].value + params[PARAM_FINE].value);
	float deltaTime = freq / engineGetSampleRate();
	phase += deltaTime;
	double intPart;
	phase = modf(phase, &intPart); 
	
	outputs[OUTPUT_1].value = 5.0f * sinf(phase * 2 * M_PI);
	outputs[OUTPUT_2].value = 5.0f * sinf(phase * 2 * M_PI + M_PI / 6);
	outputs[OUTPUT_3].value = 5.0f * sinf(phase * 2 * M_PI + M_PI / 4);
	outputs[OUTPUT_4].value = 5.0f * sinf(phase * 2 * M_PI + M_PI / 3);
	outputs[OUTPUT_5].value = 5.0f * sinf(phase * 2 * M_PI + M_PI / 2);
	outputs[OUTPUT_6].value = 5.0f * sinf(phase * 2 * M_PI + 2 * M_PI / 3);
	outputs[OUTPUT_7].value = 5.0f * sinf(phase * 2 * M_PI + 3 * M_PI / 4);
	outputs[OUTPUT_8].value = 5.0f * sinf(phase * 2 * M_PI + 5 * M_PI / 6);
	outputs[OUTPUT_9].value = 5.0f * sinf(phase * 2 * M_PI + M_PI);
	outputs[OUTPUT_10].value = 5.0f * sinf(phase * 2 * M_PI + 7 * M_PI / 6);
	outputs[OUTPUT_11].value = 5.0f * sinf(phase * 2 * M_PI + 5 * M_PI / 4);
	outputs[OUTPUT_12].value = 5.0f * sinf(phase * 2 * M_PI + 4 * M_PI / 3);
	outputs[OUTPUT_13].value = 5.0f * sinf(phase * 2 * M_PI + 3 * M_PI / 2);
	outputs[OUTPUT_14].value = 5.0f * sinf(phase * 2 * M_PI + 5 * M_PI / 3);
	outputs[OUTPUT_15].value = 5.0f * sinf(phase * 2 * M_PI + 7 * M_PI / 4);
	outputs[OUTPUT_16].value = 5.0f * sinf(phase * 2 * M_PI + 11 * M_PI / 6);

}

struct PO101 : ModuleWidget {
	PO101(PO_101 *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/PO-101.svg")));

		addParam(ParamWidget::create<sub_knob_med>(Vec(11, 19), module, PO_101::PARAM_TUNE, -5.0f, +5.0f, 0.0f));
		addParam(ParamWidget::create<sub_knob_med>(Vec(71, 19), module, PO_101::PARAM_FINE, -0.1f, +0.1f, 0.0f));

		addInput(Port::create<sub_port_blue>(Vec(4,19), Port::INPUT, module, PO_101::INPUT_NOTE_CV));

		addOutput(Port::create<sub_port_blue>(Vec(62,33), Port::OUTPUT, module, PO_101::OUTPUT_1));
		addOutput(Port::create<sub_port_blue>(Vec(62,63), Port::OUTPUT, module, PO_101::OUTPUT_2));
		addOutput(Port::create<sub_port_blue>(Vec(62,93), Port::OUTPUT, module, PO_101::OUTPUT_3));
		addOutput(Port::create<sub_port_blue>(Vec(62,123), Port::OUTPUT, module, PO_101::OUTPUT_4));
		addOutput(Port::create<sub_port_blue>(Vec(62,153), Port::OUTPUT, module, PO_101::OUTPUT_5));
		addOutput(Port::create<sub_port_blue>(Vec(62,183), Port::OUTPUT, module, PO_101::OUTPUT_6));
		addOutput(Port::create<sub_port_blue>(Vec(62,213), Port::OUTPUT, module, PO_101::OUTPUT_7));
		addOutput(Port::create<sub_port_blue>(Vec(62,243), Port::OUTPUT, module, PO_101::OUTPUT_8));
		addOutput(Port::create<sub_port_blue>(Vec(62,273), Port::OUTPUT, module, PO_101::OUTPUT_9));
		addOutput(Port::create<sub_port_blue>(Vec(92,93), Port::OUTPUT, module, PO_101::OUTPUT_10));
		addOutput(Port::create<sub_port_blue>(Vec(92,123), Port::OUTPUT, module, PO_101::OUTPUT_11));
		addOutput(Port::create<sub_port_blue>(Vec(92,153), Port::OUTPUT, module, PO_101::OUTPUT_12));
		addOutput(Port::create<sub_port_blue>(Vec(92,183), Port::OUTPUT, module, PO_101::OUTPUT_13));
		addOutput(Port::create<sub_port_blue>(Vec(92,213), Port::OUTPUT, module, PO_101::OUTPUT_14));
		addOutput(Port::create<sub_port_blue>(Vec(92,243), Port::OUTPUT, module, PO_101::OUTPUT_15));
		addOutput(Port::create<sub_port_blue>(Vec(92,273), Port::OUTPUT, module, PO_101::OUTPUT_16));
	}
};

Model *modelPO101 = Model::create<PO_101, PO101>("SubmarineFree", "PO-101", "PO-101 Phased VCO", OSCILLATOR_TAG, MULTIPLE_TAG);
