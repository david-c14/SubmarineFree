//SubTag W2

#include "SubmarineFree.hpp"

/*

Difference function for mass-damped spring needle VU-meter 

a₀⋅y₀ = b₀⋅x₀ + b₁⋅x₋₁ + b₂⋅x₋₂ - a₁⋅y₋₁ - a₂⋅y₋₂

a₀ = 4 + 4⋅ζ⋅ωn⋅Td + ωn²⋅Td²
a₁ = -8 + 2⋅ωn²⋅Td
a₂ = 4 - 4⋅ζ⋅ωn⋅Td + ωn²⋅Td²
b₀ = ωn²⋅Td²
b₁ = 2⋅ωn²⋅Td²
b₂ = ωn²⋅Td²

*/

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

	const double zeta = 0.81272; 
	const double omega_n = 13.5119;
	const double omega_n2 = omega_n * omega_n;

	struct Coefficients {
		double a0;
		double a1;
		double a2;
		double b0_2;
		double b1;

		Coefficients(float sampleTime) {
			Recalculate(sampleTime);
		}
		
		void Recalculate(float Td) {
			double Td2omega_n2        = Td * Td * omega_n2;
			double zeta_omega_n_Td4   = 4 * zeta * omega_n * Td;

			a0   = 1.0f / (4 + zeta_omega_n_Td4 + Td2omega_n2);
			a1   = -8 + 2 * Td2omega_n2;
			a2   = 4 - zeta_omega_n_Td4 + Td2omega_n2; 
			b0_2 = Td2omega_n2;
			b1   = b0_2 * 2.0f;
		}
	};
}

struct VM_ : Module {
	VM_() : Module() { }
	Coefficients c { { APP->engine->getSampleTime() } };
	void onSampleRateChange() override {
		c.Recalculate(APP->engine->getSampleTime());
	}
};


struct VM_102 : VM_ {

	VM_102() : VM_() {
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
	
		y_0 = - c.a1 * y_1 - c.a2 * y_2 + c.b0_2 * x_0 + c.b1 * x_1 + c.b0_2 * x_2;
		y_0 *= c.a0;
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
