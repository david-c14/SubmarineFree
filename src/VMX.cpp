//SubTag W2 W10

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
		PARAM_LOAD,
		PARAM_ATTENUATOR,
		NUM_PARAMS
	};
	enum OutputIds {
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
			x_0 = std::abs(sample);
			y_2 = y_1;
			y_1 = y_0;
		
			y_0 = c->a0 * (c->b0_2 * x_0 + c->b1 * x_1 + c->b0_2 * x_2 - c->a1 * y_1 - c->a2 * y_2);
		}
	};

	float squareScale(float x, float min, float max) {
		x = rescale(x, -20.0f, 3.0f, 0.0f, 1.0f);
		x *= x * x;
		return rescale(x, 0.0f, 1.0f, min, max);
	}

	struct VM_LinearDisplay : LightWidget {
		float value = 0.0f;
	
		void draw(const DrawArgs &args) override {
			float zeroPoint = squareScale(0, box.size.y, 0);
			float meter = squareScale(value, 0.0f, box.size.y);
			
			NVGpaint grad = nvgLinearGradient(args.vg, 0, zeroPoint - 10.0f, 0, zeroPoint + 10.0f, SUBLIGHTRED, nvgRGB(30,255,0));
			nvgFillPaint(args.vg, grad);
			
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, box.size.y - meter, box.size.x, meter); 
			nvgFill(args.vg);
			Widget::draw(args);
		}
	};

	struct VM_NeedleCanvas : Widget {
	
		void drawText(NVGcontext *vg, float x, float y, int align, float size, NVGcolor col, const char *txt) {
			nvgFontFaceId(vg, gScheme.font(vg));
			nvgFontSize(vg, size * 90 / SVG_DPI);
			nvgTextAlign(vg, align);
			nvgFillColor(vg, col);
			nvgText(vg, x, y, txt, NULL);
		}
		void drawText(NVGcontext *vg, float point, NVGcolor col, const char *txt) {
			float tick = squareScale(point, M_PI * 0.75, M_PI * 0.25);
			drawText(vg, box.size.x * 0.5 + cos(tick) * box.size.y * 0.65f, box.size.y - sin(tick) * box.size.y * 0.7f,
				NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8, col, txt);
		}

		void addTick(const DrawArgs &args, float point) {
			float tick = squareScale(point, M_PI * 0.75, M_PI * 0.25);
			nvgMoveTo(args.vg, box.size.x * 0.5 + cos(tick) * box.size.y * 0.75f, box.size.y - sin(tick) * box.size.y * 0.75f);
			nvgLineTo(args.vg, box.size.x * 0.5 + cos(tick) * box.size.y * 0.8f, box.size.y - sin(tick) * box.size.y * 0.8f);
		}

		void draw(const DrawArgs &args) override {
			// White backpanel
			nvgBeginPath(args.vg);
			nvgFillColor(args.vg, nvgRGB(255,255,255));
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFill(args.vg);

			// Change from black to red scale
			float zeroPoint = squareScale(0.0f, M_PI * 0.75, M_PI * 0.25);

			// Red arc
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokeColor(args.vg, SUBLIGHTRED);
			nvgBeginPath(args.vg);
			nvgArc(args.vg,
				box.size.x * 0.5f,
				box.size.y,
				box.size.y * 0.8f,
				-zeroPoint,
				M_PI * -0.25f,
				NVG_CW);
			addTick(args, 1.0f);
			addTick(args, 2.0f);
			addTick(args, 3.0f);
			nvgStroke(args.vg);

			// Black arc
			nvgStrokeColor(args.vg, nvgRGB(0,0,0));
			nvgBeginPath(args.vg);
			nvgArc(args.vg,
				box.size.x * 0.5f,
				box.size.y,
				box.size.y * 0.8f,
				M_PI * -0.75f,
				-zeroPoint,
				NVG_CW);
			addTick(args, 0.0f);
			addTick(args, -1.0f);
			addTick(args, -2.0f);
			addTick(args, -3.0f);
			addTick(args, -4.0f);
			addTick(args, -5.0f);
			addTick(args, -6.0f);
			addTick(args, -7.0f);
			addTick(args, -8.0f);
			addTick(args, -9.0f);
			addTick(args, -10.0f);
			addTick(args, -20.0f);
			nvgStroke(args.vg);

			drawText(args.vg, box.size.x * 0.5, box.size.y * 0.9, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 12, nvgRGB(0,0,0), "VU");
			drawText(args.vg, 10, 30, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 10, nvgRGB(0,0,0), "-");
			drawText(args.vg, box.size.x - 10, 30, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 10, SUBLIGHTRED, "+");
			drawText(args.vg, -27, nvgRGB(0,0,0), "20");
			drawText(args.vg, -10, nvgRGB(0,0,0), "10");
			drawText(args.vg, -6, nvgRGB(0,0,0), "6");
			drawText(args.vg, -3, nvgRGB(0,0,0), "3");
			drawText(args.vg, 0, nvgRGB(0,0,0), "0");
			drawText(args.vg, 1, SUBLIGHTRED, "1");
			drawText(args.vg, 2, SUBLIGHTRED, "2");
			drawText(args.vg, 3.3, SUBLIGHTRED, "3");

			Widget::draw(args);
		}
	};

	struct VM_NeedleDisplay : LightWidget {
		float value = 0.0f;
		FramebufferWidget *fb;
		VM_NeedleDisplay(float width, float height) : LightWidget() {
			box.size = Vec(width, height);
			fb = new FramebufferWidget();
			VM_NeedleCanvas *canvas = new VM_NeedleCanvas();
			canvas->box.size = box.size;
			fb->addChild(canvas);
			addChild(fb);
		}
		void draw(const DrawArgs &args) override {
			Widget::draw(args);
			float meter = squareScale(value, M_PI * 0.75, M_PI * 0.25);
			nvgStrokeColor(args.vg, nvgRGB(0,0,0));
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, box.size.x * 0.5f, box.size.y);
			nvgLineTo(args.vg, box.size.x * 0.5f + cos(meter) * box.size.y * 0.8f, box.size.y - sin(meter) * box.size.y * 0.8f);
			nvgStrokeWidth(args.vg, 1);
			nvgStroke(args.vg);
		}
	};
}

struct VM_Base : Module {
	VM_Base() : Module() { }
	
	Menu * addLoadMenu() {
		Menu *menu = new Menu();
		Module * module = this;
		EventWidgetMenuItem *m = createMenuItem<EventWidgetMenuItem>("150\xe2\x84\xa6");
		m->clickHandler = [=]() {
			APP->engine->setParam(module, PARAM_LOAD, 150);
		};
		menu->addChild(m);
		m = createMenuItem<EventWidgetMenuItem>("600\xe2\x84\xa6");
		m->clickHandler = [=]() {
			APP->engine->setParam(module, PARAM_LOAD, 600);
		};
		menu->addChild(m);
		m = createMenuItem<EventWidgetMenuItem>("1000\xe2\x84\xa6");
		m->clickHandler = [=]() {
			APP->engine->setParam(module, PARAM_LOAD, 1000);
		};
		menu->addChild(m);
		return menu;
	}

	void addMenu(Menu *menu) {
		char loadStr[50];
		sprintf(loadStr, "Load Resistor (%d\xe2\x84\xa6)", (int)(params[PARAM_LOAD].getValue()));
		EventWidgetMenuItem *m = createMenuItem<EventWidgetMenuItem>(loadStr);
		m->rightText = SUBMENU;
		m->childMenuHandler = [=]() {
			return addLoadMenu();
		};
		menu->addChild(m);
	}
};

template <int x>
struct VM_ : VM_Base {
	enum InputIds {
		INPUT_1,
		NUM_INPUTS = INPUT_1 + x
	};

	VM_() : VM_Base() { 
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_LOAD, 50.0f, 20000.0f, 600.0f, "Load Resistor", "\xe2\x84\xa6");
		configParam(PARAM_ATTENUATOR, -2.0f, 4.0f, 0.0f, "Attenuator", "x", 2.0f);
	}

	Coefficients c { { APP->engine->getSampleTime() } };

	void onSampleRateChange() override {
		c.Recalculate(APP->engine->getSampleTime());
	}
	double calculate(double y_0) {
		double vRef = 1 / std::sqrt(params[PARAM_LOAD].getValue() * 0.001);
		double atten = params[PARAM_ATTENUATOR].getValue() * 6.0f;
		double sample = 20 * std::log10(y_0 * vRef) - atten;
		if (std::isnan(sample)) {
			sample = -20.0f;
		}
		sample = clamp(sample, -20.0f, 3.0f);	
		return sample;
	}
};

struct VM_xx1 : VM_<1> {

	VM_xx1() : VM_<1>() { }

	Samples samples;

	void process(const ProcessArgs &args) override {
		samples.process(&c, inputs[INPUT_1].getVoltage(0));
	}
};

struct VM_102 : VM_<1> {

	VM_102() : VM_<1>() { }

	Samples samples_1;
	Samples samples_2;
	
	void process(const ProcessArgs &args) override {
		samples_1.process(&c, inputs[INPUT_1].getVoltage(0));
		samples_2.process(&c, inputs[INPUT_1].getVoltage(1));
	}
};

struct VM_202 : VM_<2> {

	VM_202() : VM_<2>() { }

	Samples samples_1;
	Samples samples_2;

	void process(const ProcessArgs &args) override {
		samples_1.process(&c, inputs[INPUT_1].getVoltage(0));
		if (inputs[INPUT_1 + 1].isConnected()) {
			samples_2.process(&c, inputs[INPUT_1 + 1].getVoltage(0));
		}
		else {
			samples_2.process(&c, inputs[INPUT_1].getVoltage(1));
		}
	}
};

struct VMxxx : SchemeModuleWidget {
	VMxxx() : SchemeModuleWidget() { }

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		VM_Base *module = dynamic_cast<VM_Base *>(this->module);
		if (module) {
			module->addMenu(menu);
		}
	}
};

struct VM101 : VMxxx {
	const float displayHeight = 276.0f;
	const float displayPos = 19.5f;
	VM_LinearDisplay *display;
	VM101(VM_xx1 *module) : VMxxx() {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		display = new VM_LinearDisplay();
		display->box.pos = Vec(10, displayPos);
		display->box.size = Vec(10, displayHeight);
		addChild(display);

		addInput(createInputCentered<SilverPort>(Vec(15,350), module, VM_xx1::INPUT_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 315), module, PARAM_ATTENUATOR));
	}
	void addTick(NVGcontext *vg, float point) {
		float tick = displayPos + squareScale(point, displayHeight, 0.0f);
		nvgMoveTo(vg, 10, tick);
		nvgLineTo(vg, 28, tick);
	}
	void addText(NVGcontext *vg, float point, NVGcolor col, const char *txt) {
		float tick = displayPos + squareScale(point, displayHeight, 0.0f);	
		drawText(vg, 5.0f, tick, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.0f, col, txt);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-101");
		drawText(vg, 25, 332, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "\xe2\x84\xa6");
		nvgStrokeWidth(vg, 1);
		nvgBeginPath(vg);
		nvgStrokeColor(vg, SUBLIGHTRED);
		addTick(vg, 3);
		addTick(vg, 2);
		addTick(vg, 1);
		nvgStroke(vg);

		nvgBeginPath(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		addTick(vg, 0);
		addTick(vg, -1);
		addTick(vg, -2);
		addTick(vg, -3);
		addTick(vg, -4);
		addTick(vg, -5);
		addTick(vg, -6);
		addTick(vg, -7);
		addTick(vg, -8);
		addTick(vg, -9);
		addTick(vg, -10);
		addTick(vg, -20);
		nvgStroke(vg);
		addText(vg, 3, SUBLIGHTRED, "3"); 
		addText(vg, 1, SUBLIGHTRED, "+"); 
		addText(vg, 0, gScheme.getContrast(module), "0");
		addText(vg, -1, gScheme.getContrast(module), "-");
		addText(vg, -3, gScheme.getContrast(module), "3");
		addText(vg, -6, gScheme.getContrast(module), "6");
		addText(vg, -10, gScheme.getContrast(module), "10");
		addText(vg, -20, gScheme.getContrast(module), "20");
	}
	void step() override {
		if (module) {
			VM_xx1 *vmModule = dynamic_cast<VM_xx1 *>(module);
			display->value = vmModule->calculate(vmModule->samples.y_0);
		}	
		SchemeModuleWidget::step();
	}
};

struct VM102 : VMxxx {
	const float displayHeight = 276.0f;
	const float displayPos = 19.5f;
	VM_LinearDisplay *display1;
	VM_LinearDisplay *display2;
	VM102(VM_102 *module) : VMxxx() {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		display1 = new VM_LinearDisplay();
		display1->box.pos = Vec(2, displayPos);
		display1->box.size = Vec(8, displayHeight);
		addChild(display1);

		display2 = new VM_LinearDisplay();
		display2->box.pos = Vec(20, displayPos);
		display2->box.size = Vec(8, displayHeight);
		addChild(display2);

		addInput(createInputCentered<SilverPort>(Vec(15,350), module, VM_102::INPUT_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(15, 315), module, PARAM_ATTENUATOR));
	}
	void addTick(NVGcontext *vg, float point) {
		float tick = displayPos + squareScale(point, displayHeight, 0.0f);
		nvgMoveTo(vg, 2, tick);
		nvgLineTo(vg, 10, tick);
		nvgMoveTo(vg, 20, tick);
		nvgLineTo(vg, 28, tick);
	}
	void addText(NVGcontext *vg, float point, NVGcolor col, const char *txt) {
		float tick = displayPos + squareScale(point, displayHeight, 0.0f);	
		drawText(vg, 15.0f, tick, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.0f, col, txt);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-102");
		drawText(vg, 25.0f, 332.0f, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8.0f, gScheme.getContrast(module), "\xe2\x84\xa6");
		nvgStrokeWidth(vg, 1);
		nvgBeginPath(vg);
		nvgStrokeColor(vg, SUBLIGHTRED);
		addTick(vg, 1);
		addTick(vg, 2);
		addTick(vg, 3);
		nvgStroke(vg);
		nvgBeginPath(vg);
		nvgStrokeColor(vg, gScheme.getContrast(module));
		addTick(vg, 0);
		addTick(vg, -1);
		addTick(vg, -2);
		addTick(vg, -3);
		addTick(vg, -4);
		addTick(vg, -5);
		addTick(vg, -6);
		addTick(vg, -7);
		addTick(vg, -8);
		addTick(vg, -9);
		addTick(vg, -10);
		addTick(vg, -20);
		nvgStroke(vg);
		addText(vg, 3.0f, SUBLIGHTRED, "3"); 
		addText(vg, 1.0f, SUBLIGHTRED, "+"); 
		addText(vg, 0.0f, gScheme.getContrast(module), "0");
		addText(vg, -1.0f, gScheme.getContrast(module), "-");
		addText(vg, -3.0f, gScheme.getContrast(module), "3");
		addText(vg, -6.0f, gScheme.getContrast(module), "6");
		addText(vg, -10.0f, gScheme.getContrast(module), "10");
		addText(vg, -20.0f, gScheme.getContrast(module), "20");
	}
	void step() override {
		if (module) {
			VM_102 *vmModule = dynamic_cast<VM_102 *>(module);
			display1->value = vmModule->calculate(vmModule->samples_1.y_0);
			display2->value = vmModule->calculate(vmModule->samples_2.y_0);
		}	
		SchemeModuleWidget::step();
	}
};

struct VM201 : VMxxx {
	VM_NeedleDisplay *display;
	VM201(VM_xx1 *module) : VMxxx() {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));

		display = new VM_NeedleDisplay(130, 90);
		display->box.pos = Vec(10, 20);
		addChild(display);

		addInput(createInputCentered<SilverPort>(Vec(20,330), module, VM_xx1::INPUT_1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(115, 330), module, PARAM_ATTENUATOR));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-201");
		drawText(vg, 20, 355, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 115, 355, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ATTENUATOR");
	}
	void step() override {
		if (module) {
			VM_xx1 *vmModule = dynamic_cast<VM_xx1 *>(module);
			display->value = vmModule->calculate(vmModule->samples.y_0);
		}	
		SchemeModuleWidget::step();
	}
};

struct VM202 : VMxxx {
	VM_NeedleDisplay *display1;
	VM_NeedleDisplay *display2;
	VM202(VM_202 *module) : VMxxx() {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));

		display1 = new VM_NeedleDisplay(130, 90);
		display1->box.pos = Vec(10, 20);
		addChild(display1);

		display2 = new VM_NeedleDisplay(130, 90);
		display2->box.pos = Vec(10, 120);
		addChild(display2);

		addInput(createInputCentered<SilverPort>(Vec(20,330), module, VM_202::INPUT_1));
		addInput(createInputCentered<RedPort>(Vec(55,330), module, VM_202::INPUT_1 + 1));
		addParam(createParamCentered<SmallKnob<LightKnob>>(Vec(115, 330), module, PARAM_ATTENUATOR));
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "VM-202");
		drawText(vg, 37.5f, 355, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 115, 355, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "ATTENUATOR");
	}
	void step() override {
		if (module) {
			VM_202 *vmModule = dynamic_cast<VM_202 *>(module);
			display1->value = vmModule->calculate(vmModule->samples_1.y_0);
			display2->value = vmModule->calculate(vmModule->samples_2.y_0);
		}	
		SchemeModuleWidget::step();
	}
};

Model *modelVM101 = createModel<VM_xx1, VM101>("VM-101");
Model *modelVM102 = createModel<VM_102, VM102>("VM-102");
Model *modelVM201 = createModel<VM_xx1, VM201>("VM-201");
Model *modelVM202 = createModel<VM_202, VM202>("VM-202");
