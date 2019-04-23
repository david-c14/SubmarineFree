//SubTag W2

#include "SubmarineFree.hpp"
#include "window.hpp"

struct TDVText : SubText {
	TDVText() {
		multiline = false;
		color = SUBLIGHTBLUE;
		bgColor = nvgRGBA(0, 0, 0, 0);
		fontSize = 28;
	}
};

#define TRANSFORM_POSITION { \
	Vec pos = e.pos; \
	Vec newPos = pos; \
	float t[6]; \
	if (nvgTransformInverse(t, transform)) \
		nvgTransformPoint(&newPos.x, &newPos.y, t, newPos.x, newPos.y); 

#define TRANSFORM_REL(_rel) \
	t[4] = t[5] = 0.0f; \
	nvgTransformPoint(&e._rel.x, &e._rel.y, t, e._rel.x, e._rel.y);

#define RECURSE_EVENT_POSITION(_method) \
	for (auto it = children.rbegin(); it != children.rend(); it++) { \
		Widget *child = *it; \
		if (!child->visible) \
			continue; \
		if (child->box.contains(newPos)) { \
			e.pos = newPos.minus(child->box.pos); \
			child->_method(e); \
			if (e.consumed) \
				break; \
		} \
	} \
	e.pos = pos; \
}

struct TDTW : TransformWidget {
	TDTW() {};
	void onMouseDown(EventMouseDown &e) override {
		TRANSFORM_POSITION
		RECURSE_EVENT_POSITION(onMouseDown);
	}
	void onMouseUp(EventMouseUp &e) override {
		TRANSFORM_POSITION
		RECURSE_EVENT_POSITION(onMouseUp);
	}
	void onMouseMove(EventMouseMove &e) override {
		TRANSFORM_POSITION
		TRANSFORM_REL(mouseRel);
		RECURSE_EVENT_POSITION(onMouseMove);
	}
	void onHoverKey(EventHoverKey &e) override {
		TRANSFORM_POSITION
		RECURSE_EVENT_POSITION(onHoverKey);
	}
	void onScroll(EventScroll &e) override {
		TRANSFORM_POSITION
		TRANSFORM_REL(scrollRel);
		RECURSE_EVENT_POSITION(onScroll);
	}
	void onPathDrop(EventPathDrop &e) override {
		TRANSFORM_POSITION
		RECURSE_EVENT_POSITION(onPathDrop);
	}
};

struct TD202 : SchemeModuleWidget {
	TDVText *textField;

	TD202(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));
	
		TDTW *tw = Widget::create<TDTW>(Vec(2, 15));
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
