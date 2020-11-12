//SubTag W2

#include "SubmarineFree.hpp"

namespace {
	enum ParamIds {
		PARAM_IMPEDANCE,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	const static int bufferSize = 1024;
}

template<int x>
struct VM_1 : Module {

	alignas(64) float buffer[bufferSize * x] = { 0.0f };
	alignas(64) float primaryAccumulator[x] = { 0.0f };
	alignas(64) float backupAccumulator[x] = { 0.0f };
	alignas(64) float transfer[x] = { 0.0f };
	alignas(64) float outputVal[x] = { 0.0f };
	int index = 0;

	VM_1() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_IMPEDANCE, 0.0f, +1.0f, 0.0f, "Impedance", "\xe2\x84\xa6");
	}
	
	virtual void accumulate() {
		DEBUG("template");
		
	}

	virtual void output() {
	}

	void process(const ProcessArgs &args) override {
		accumulate();
		output();
	}
};

struct VM_102 : VM_1<2> {
	void accumulate() override { 
		primaryAccumulator[0] -= buffer[index * 2];
		primaryAccumulator[1] -= buffer[index * 2 + 1];
		float sample = inputs[INPUT_1].getVoltage(0);
		sample *= sample;
		primaryAccumulator[0] += sample;
		backupAccumulator[0] += sample;
		buffer[index * 2] = sample;
		sample = inputs[INPUT_1].getVoltage(1);
		primaryAccumulator[1] += sample;
		backupAccumulator[1] += sample;
		buffer[index * 2 + 1] = sample;
		index++;
		if (index == bufferSize) {
			index = 0;
			primaryAccumulator[0] = backupAccumulator[0];
			primaryAccumulator[1] = backupAccumulator[1];
			backupAccumulator[0] = 0;
			backupAccumulator[1] = 1;
		}
		transfer[0] = primaryAccumulator[0];
		transfer[1] = primaryAccumulator[1];
	}
	void output() override {
		outputs[OUTPUT_1].setVoltage(outputVal[0], 0);
		outputs[OUTPUT_1].setVoltage(outputVal[1], 1);
	}
};

template <int x>
struct VM1 : SchemeModuleWidget {
	void step() override {
		if (module) {
			VM_1<x> *vmModule = dynamic_cast<VM_1<x> *>(module);
			for (unsigned int i = 0; i < x; i++) {
				float sample = std::log(vmModule->transfer[i] / bufferSize);
				vmModule->outputVal[i] = sample;
			}
		}	
		SchemeModuleWidget::step();
	}
	VM1() : SchemeModuleWidget() {
	}
};

struct VM102 : VM1<2> {
	LightKnob *lengthKnob;
	VM102(VM_102 *module) : VM1() {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(15,350), module, INPUT_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 320), module, PARAM_IMPEDANCE));
		addOutput(createOutputCentered<SilverPort>(Vec(15, 30), module, OUTPUT_1));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-102");
		drawText(vg, 25, 337, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "\xe2\x84\xa6");
	}
};

Model *modelVM102 = createModel<VM_102, VM102>("VM-102");
