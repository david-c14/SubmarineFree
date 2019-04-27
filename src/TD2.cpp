//SubTag W2

#include "SubmarineFree.hpp"
#include "window.hpp"
#include <cstring>

struct TDVText : SubText {
	TDVText() {
		multiline = false;
		color = SUBLIGHTBLUE;
		bgColor = nvgRGBA(0, 0, 0, 0);
		fontSize = 28;
	}
};

struct MouseTransformWidget:VirtualWidget {
	float transform[6];
	float inverse[6];
	float invLinear[6];
	int hasInverse;
	MouseTransformWidget() {
		identity();
	}
	Rect getChildrenBoundingBox() override {
		Rect bound = Widget::getChildrenBoundingBox();
		Vec topLeft = bound.pos;
		Vec bottomRight = bound.getBottomRight();
		nvgTransformPoint(&topLeft.x, &topLeft.y, transform, topLeft.x, topLeft.y);
		nvgTransformPoint(&bottomRight.x, &bottomRight.y, transform, bottomRight.x, bottomRight.y);
		return Rect(topLeft, bottomRight.minus(topLeft));
	}
	void identity() {
		nvgTransformIdentity(transform);
		nvgTransformIdentity(inverse);
		std::memcpy(invLinear, inverse, 4 * sizeof(float));
		invLinear[4] = invLinear[5] = 0.0;
		hasInverse = true;
	}
	void translate(Vec delta) {
		float t[6];
		nvgTransformTranslate(t, delta.x, delta.y);
		nvgTransformPremultiply(transform, t);
		hasInverse = nvgTransformInverse(inverse, transform);
		std::memcpy(invLinear, inverse, 4 * sizeof(float));
	}
	void rotate(float angle) {
		float t[6];
		nvgTransformRotate(t, angle);
		nvgTransformPremultiply(transform, t);
		hasInverse = nvgTransformInverse(inverse, transform);
		std::memcpy(invLinear, inverse, 4 * sizeof(float));
	}
	void scale(Vec s) {
		float t[6];
		nvgTransformScale(t, s.x, s.y);
		nvgTransformPremultiply(transform, t);
		hasInverse = nvgTransformInverse(inverse, transform);
		std::memcpy(invLinear, inverse, 4 * sizeof(float));
	}
	void draw(NVGcontext *vg) override {
		// No need to save the state because that is done in the parent
		nvgTransform(vg, transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]);
		Widget::draw(vg);
	}
	void onMouseDown(EventMouseDown &e) override {
		Vec pos = e.pos;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		}
		Widget::onMouseDown(e);
		e.pos = pos;
	}
	void onMouseUp(EventMouseUp &e) override {
		Vec pos = e.pos;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		}
		Widget::onMouseUp(e);
		e.pos = pos;
	}
	void onMouseMove(EventMouseMove &e) override {
		Vec pos = e.pos;
		Vec mouseRel = e.mouseRel;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
			nvgTransformPoint(&e.mouseRel.x, &e.mouseRel.y, invLinear, e.mouseRel.x, e.mouseRel.y);
		}
		Widget::onMouseMove(e);
		e.pos = pos;
		e.mouseRel = mouseRel;
	}
	void onHoverKey(EventHoverKey &e) override {
		Vec pos = e.pos;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		}
		Widget::onHoverKey(e);
		e.pos = pos;
	}
	void onScroll(EventScroll &e) override {
		Vec pos = e.pos;
		Vec scrollRel = e.scrollRel;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
			nvgTransformPoint(&e.scrollRel.x, &e.scrollRel.y, invLinear, e.scrollRel.x, e.scrollRel.y);
		}
		Widget::onScroll(e);
		e.pos = pos;
		e.scrollRel = scrollRel;
	}
	void onPathDrop(EventPathDrop &e) override {
		Vec pos = e.pos;
		if (hasInverse) {
			nvgTransformPoint(&e.pos.x, &e.pos.y, inverse, e.pos.x, e.pos.y); 
		}
		Widget::onPathDrop(e);
		e.pos = pos;
	}
};

struct TD202 : SchemeModuleWidget {
	TDVText *textField;

	TD202(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));
	
		MouseTransformWidget *tw = Widget::create<MouseTransformWidget>(Vec(2, 15));
		tw->rotate(M_PI / 2.0f);
		addChild(tw);

		textField = Widget::create<TDVText>(Vec(0, -26));
		textField->box.size = Vec(350, 26);
		tw->addChild(textField);
	}

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		json_object_set_new(rootJ, "fg", json_string(colorToHexString(textField->color).c_str()));
		json_object_set_new(rootJ, "bg", json_string(colorToHexString(textField->bgColor).c_str()));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ) {
			if (json_is_object(fgJ))
				textField->color = jsonToColor(fgJ);
			else
				textField->color = colorFromHexString(json_string_value(fgJ));
		}
		json_t *bgJ = json_object_get(rootJ, "bg");
		if (bgJ) {
			if (json_is_object(bgJ))
				textField->bgColor = jsonToColor(bgJ);
			else
				textField->bgColor = colorFromHexString(json_string_value(bgJ));
		}
	}

	void reset() override {
		textField->text = "";
		textField->multiline = false;
		textField->color = SUBLIGHTBLUE;
		textField->bgColor = nvgRGBA(0, 0, 0, 0);
		ModuleWidget::reset();
	}
	
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		textField->appendContextMenu(menu);
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-202");
	}
};

Model *modelTD202 = Model::create<Module, TD202>("Submarine (Free)", "TD-202", "TD-202 Vertical Text Display", VISUAL_TAG);
