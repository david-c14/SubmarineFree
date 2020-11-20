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
}

/*

a₀⋅y₀ = b₀⋅x₀ + b₁⋅x₋₁ + b₂⋅x₋₂ - a₁⋅y₋₁ - a₂⋅y₋₂

a₀ = 4 + 4⋅ζ⋅ωn⋅Td + ωn²⋅Td²
a₁ = -8 + 2⋅ωn²⋅Td
a₂ = 4 - 4⋅ζ⋅ωn⋅Td + ωn²⋅Td²
b₀ = ωn²⋅Td²
b₁ = 2⋅ωn²⋅Td²
b₂ = ωn²⋅Td²


*/

struct VM_102 : Module {

	VM_102() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_IMPEDANCE, 0.0f, +1.0f, 0.0f, "Impedance", "\xe2\x84\xa6");
	}
	double y_0 = 0.0f;
	double y_1 = 0.0f;
	double y_2 = 0.0f;
	double x_0 = 0.0f;
	double x_1 = 0.0f;
	double x_2 = 0.0f;
	
	void process(const ProcessArgs &args) override {
		x_2 = x_1;
		x_1 = x_0;
		x_0 = inputs[INPUT_1].getVoltage(0);
		x_0 = abs(x_0);
		y_2 = y_1;
		y_1 = y_0;
		double eta = 0.81272;
		double wn = 13.5119;
		double Td = args.sampleTime;
		double a0 = 4 + 4 * eta * wn * Td + wn * wn * Td * Td;
		double a1 = -8 + 2 * wn * wn * Td * Td;
		double a2 = 4 - 4 * eta * wn * Td + wn * wn * Td * Td; 
		double b0 = Td * Td * wn * wn;
		double b1 = b0 * 2.0f;
		double b2 = b0;
	
		y_0 = - a1 * y_1 - a2 * y_2 + b0 * x_0 + b1 * x_1 + b2 * x_2;
		y_0 /= a0;
		outputs[OUTPUT_1].setVoltage(y_0, 0);
		
	}
};

struct VM102 : SchemeModuleWidget {
	LightKnob *lengthKnob;
	VM102(VM_102 *module) : SchemeModuleWidget() {
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
	void step() override {
		if (module) {
			VM_102 *vmModule = dynamic_cast<VM_102 *>(module);
			for (unsigned int i = 0; i < 1; i++) {
	//			float sample = std::log(vmModule->transfer[i] / bufferSize);
	//			vmModule->outputVal[i] = sample;
			}
		}	
		SchemeModuleWidget::step();
	}
};

Model *modelVM102 = createModel<VM_102, VM102>("VM-102");
