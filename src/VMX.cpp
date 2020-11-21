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

	struct Samples {
		double x_0 = 0.0f;
		double x_1 = 0.0f;
		double x_2 = 0.0f;
		double y_0 = 0.0f;
		double y_1 = 0.0f;
		double y_2 = 0.0f;

		void process(Coefficients *c, float sample) {
			x_2 = x_1;
			x_1 = x_0;
			x_0 = std::abs(sample) + 0.000000001;
			y_2 = y_1;
			y_1 = y_0;
		
			y_0 = c->a0 * (c->b0_2 * x_0 + c->b1 * x_1 + c->b0_2 * x_2 - c->a1 * y_1 - c->a2 * y_2);
		}
	};

	struct VM_LinearDisplay : Widget {
		float value = 0.0f;
	
		void draw(const DrawArgs &args) override {
			float meter = rescale(value, -20.0f, 3.0f, 0.0f, box.size.y);
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, SUBLIGHTBLUE);
			nvgRect(args.vg, 0, box.size.y - meter, box.size.x, meter); 
			nvgFill(args.vg);
			Widget::draw(args);
		}
	};

}

template <int x>
struct VM_ : Module {
	enum ParamIds {
		PARAM_IMPEDANCE,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = INPUT_1 + x
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	VM_() : Module() { 
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_IMPEDANCE, 50.0f, 20000.0f, 600.0f, "Impedance", "\xe2\x84\xa6");
	}

	Coefficients c { { APP->engine->getSampleTime() } };

	void onSampleRateChange() override {
		c.Recalculate(APP->engine->getSampleTime());
	}
};

struct VM_1 : VM_<1> {

	VM_1() : VM_<1>() { }

	Samples samples;
/*
	double y_0 = 0.0f;
	double y_1 = 0.0f;
	double y_2 = 0.0f;
	double x_0 = 0.0f;
	double x_1 = 0.0f;
	double x_2 = 0.0f;
*/	
	void process(const ProcessArgs &args) override {
		samples.process(&c, inputs[INPUT_1].getVoltage(0));
	/*
		x_2 = x_1;
		x_1 = x_0;
		x_0 = inputs[INPUT_1].getVoltage(0);
		x_0 = std::abs(x_0) + 0.000000001;
		y_2 = y_1;
		y_1 = y_0;
	
		y_0 = c.a0 * (c.b0_2 * x_0 + c.b1 * x_1 + c.b0_2 * x_2 - c.a1 * y_1 - c.a2 * y_2);
	*/
	}
};

struct VM_2 : VM_<2> {

	VM_2() : VM_<2>() { }

	double y_0_1 = 0.0f;
	double y_1_1 = 0.0f;
	double y_2_1 = 0.0f;
	double x_0_1 = 0.0f;
	double x_1_1 = 0.0f;
	double x_2_1 = 0.0f;

	double y_0_2 = 0.0f;
	double y_1_2 = 0.0f;
	double y_2_2 = 0.0f;
	double x_0_2 = 0.0f;
	double x_1_2 = 0.0f;
	double x_2_2 = 0.0f;

	void process(const ProcessArgs &args) override {
		x_2_1 = x_1_1;
		x_1_1 = x_0_1;
		x_0_1 = inputs[INPUT_1].getVoltage(0);
		x_0_1 = fabs(x_0_1);
		y_2_1 = y_1_1;
		y_1_1 = y_0_1;
	
		y_0_1 = - c.a1 * y_1_1 - c.a2 * y_2_1 + c.b0_2 * x_0_1 + c.b1 * x_1_1 + c.b0_2 * x_2_1;
		y_0_1 *= c.a0;

		x_2_2 = x_1_2;
		x_1_2 = x_0_2;
		x_0_2 = inputs[INPUT_1].getVoltage(1);
		x_0_2 = fabs(x_0_2);
		y_2_2 = y_1_2;
		y_1_2 = y_0_2;
	
		y_0_2 = - c.a1 * y_1_2 - c.a2 * y_2_2 + c.b0_2 * x_0_2 + c.b1 * x_1_2 + c.b0_2 * x_2_2;
		y_0_2 *= c.a0;
	}
};

struct VM101 : SchemeModuleWidget {
	const float displayHeight = 12;
	const float displayPos = 19.5;
	VM_LinearDisplay *display;
	VM101(VM_1 *module) : SchemeModuleWidget() {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		display = new VM_LinearDisplay();
		display->box.pos = Vec(10, displayPos);
		display->box.size = Vec(10, displayHeight * 23.0f);
		addChild(display);

		addInput(createInputCentered<SilverPort>(Vec(15,350), module, VM_1::INPUT_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 315), module, VM_1::PARAM_IMPEDANCE));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-102");
		drawText(vg, 25, 332, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "\xe2\x84\xa6");
		for(unsigned int i = 0; i <= 23; i++) {
			nvgBeginPath(vg);
			nvgStrokeColor(vg, (i < 3)?SUBLIGHTRED:gScheme.getContrast(module));
			nvgStrokeWidth(vg, 1);
			nvgMoveTo(vg, 10, displayPos + i * displayHeight);
			nvgLineTo(vg, 20, displayPos + i * displayHeight);
			if (i % 2) {
				if (i > 2) {
					nvgLineTo(vg, 28, displayPos + i * displayHeight);
				}
			}
			nvgStroke(vg);
		}
		drawText(vg, 5, displayPos, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, SUBLIGHTRED, "3"); 
		drawText(vg, 5, displayPos + 2 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, SUBLIGHTRED, "+"); 
		drawText(vg, 25, displayPos + 2.5 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "%"); 
		drawText(vg, 5, displayPos + 3 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "0");
		drawText(vg, 5, displayPos + 4 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "-");
		drawText(vg, 5, displayPos + 6 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "3");
		drawText(vg, 5, displayPos + 8 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "5");
		drawText(vg, 5, displayPos + 10 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "7");
		drawText(vg, 5, displayPos + 13 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "10");
		drawText(vg, 5, displayPos + 18 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "15");
		drawText(vg, 5, displayPos + 23 * displayHeight, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, gScheme.getContrast(module), "20");
	}
	void step() override {
		if (module) {
			VM_1 *vmModule = dynamic_cast<VM_1 *>(module);
			double vRef = M_PI * 0.5 / std::sqrt(vmModule->params[VM_1::PARAM_IMPEDANCE].getValue() * 0.001);
			//double sample = 20 * std::log10(vmModule->y_0 / 0.6973820198f);
			double sample = 20 * std::log10(vmModule->samples.y_0 * vRef);
			sample = clamp(sample, -20.0f, 3.0f);	
			display->value = sample;
		}	
		SchemeModuleWidget::step();
	}
};

Model *modelVM101 = createModel<VM_1, VM101>("VM-101");
Model *modelVM102 = createModel<VM_1, VM101>("VM-102");
Model *modelVM201 = createModel<VM_1, VM101>("VM-201");
Model *modelVM202 = createModel<VM_1, VM101>("VM-202");
