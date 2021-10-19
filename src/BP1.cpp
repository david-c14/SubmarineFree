//SubTag W1 W2 W4 W8 W10 W12 W16 W20 W24 W32
#include "SubmarineFree.hpp"

template <int x>
struct BP1 : SchemeModuleWidget {
	BP1(Module *module) {
		setModule(module);
		SchemePanel *panel = new SchemePanel(Vec(x * 15, 380));
		addChild(panel);
		this->box.size = Vec(x * 15, 380);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, box.size.x - 20, 365, 1, 0);
		drawText(vg, 3, 377, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 12, gScheme.getAlternative(module), "submarine");
	}
};

struct BP1_1 : BP1<1> {
	BP1_1(Module *module) : BP1<1>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 20, 1, -M_PI / 2.0f);
		nvgSave(vg);
		nvgTranslate(vg, 1, 377);
		nvgRotate(vg, -M_PI / 2.0f);
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine");
		nvgRestore(vg);
	}
};

struct BP1_2 : BP1<2> {
	SubText *textField;
	BP1_2(Module *module) : BP1<2>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 5, 365, 1, 0);
	}
};

struct BP1_4 : BP1<4> {
	BP1_4(Module *module) : BP1<4>(module) {}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 20, 365, 1, 0);
	}
};

/** Transforms appearance only, not positions of events */
struct TransformWidget2 : Widget {
	/** The transformation matrix */
	float transform[6];

	TransformWidget2() {
		identity();
	}

	void identity() {
		nvgTransformIdentity(transform);
	}

	void translate(math::Vec delta) {
		float t[6];
		nvgTransformTranslate(t, VEC_ARGS(delta));
		nvgTransformPremultiply(transform, t);
	}

	void rotate(float angle) {
		float t[6];
		nvgTransformRotate(t, angle);
		nvgTransformPremultiply(transform, t);
	}

	void rotate(float angle, math::Vec origin) {
		translate(origin);
		rotate(angle);
		translate(origin.neg());
	}

	void scale(math::Vec s) {
		float t[6];
		nvgTransformScale(t, s.x, s.y);
		nvgTransformPremultiply(transform, t);
	}

	void draw(const DrawArgs& args) override {
		// No need to save the state because that is done in the parent
		nvgTransform(args.vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
		Widget::draw(args);
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		nvgTransform(args.vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
		Widget::drawLayer(args, layer);
	}
};


struct TestWidget: Widget 
{
	void draw(const DrawArgs &args) override {
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 50, 50, 40);
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 50, 10, 10);
		nvgStroke(args.vg);
		Widget::draw(args);
	}
	void drawLayer(const DrawArgs &args, int layer) override {
		if (layer == 1) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 50, 10, 5);
			nvgStroke(args.vg);
		}
		Widget::drawLayer(args, layer);

	}
};

struct BP1_20 : BP1<20> {
	BP1_20(Module *module) : BP1<20>(module) {
		TransformWidget2 *tw = new TransformWidget2();
		tw->box.pos.x = 100;
		tw->box.pos.y = 100;
		tw->box.size.x = 100;
		tw->box.size.y = 100;
		addChild(tw);
		TestWidget *widget = new TestWidget();
		widget->box.pos.x = 0;
		widget->box.pos.y = 0;
		widget->box.size.x = 100;
		widget->box.size.y = 100;
		tw->addChild(widget);
		tw->rotate(1);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBackground(vg);
		drawLogo(vg, 0, 70, 15, 0);
		drawText(vg, 150, 377, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 12, gScheme.getAlternative(module), "submarine");
	}
};

Model *modelBP101 = createModel<Module, BP1_1>("BP-101");
Model *modelBP102 = createModel<Module, BP1_2>("BP-102");
Model *modelBP104 = createModel<Module, BP1_4>("BP-104");
Model *modelBP108 = createModel<Module, BP1<8>>("BP-108");
Model *modelBP110 = createModel<Module, BP1<10>>("BP-110");
Model *modelBP112 = createModel<Module, BP1<12>>("BP-112");
Model *modelBP116 = createModel<Module, BP1<16>>("BP-116");
Model *modelBP120 = createModel<Module, BP1_20>("BP-120");
Model *modelBP124 = createModel<Module, BP1<24>>("BP-124");
Model *modelBP132 = createModel<Module, BP1<32>>("BP-132");
