#include "SubmarineFree.hpp"
#include "window.hpp"
#include "torpedo.hpp"

static const NVGcolor wireColors[] = {
	nvgRGB(0xc9, 0xb7, 0x0e), // yellow
	nvgRGB(0xc9, 0x18, 0x47), // red
	nvgRGB(0x0c, 0x8e, 0x15), // green
	nvgRGB(0x09, 0x86, 0xad), // blue
	// nvgRGB(0x44, 0x44, 0x44), // black
	// nvgRGB(0x66, 0x66, 0x66), // gray
	// nvgRGB(0x88, 0x88, 0x88), // light gray
	// nvgRGB(0xaa, 0xaa, 0xaa), // white
};
static int lastWireColorId = -1;

struct TDVText : LedDisplayTextField {
	TDVText() {
		multiline = false;
		color = nvgRGB(0x28, 0xb0, 0xf3);
		color = wireColors[++lastWireColorId];
	}
	void draw(NVGcontext *vg) override {
		nvgScissor(vg, 0, 0, box.size.x, box.size.y);

		nvgTranslate(vg, 25, 0);
		nvgRotate(vg, M_PI / 2.0f);
		//Text
		if (font->handle >= 0) {
			bndSetFont(font->handle);
			
			NVGcolor highlightColor = color;
			highlightColor.a = 0.5;
			int begin = min(cursor, selection);
			int end = (this == gFocusedWidget) ? max(cursor, selection) : -1;
			bndIconLabelCaret(vg, textOffset.y, textOffset.x,
				box.size.y - 2*textOffset.y, box.size.x - 2*textOffset.x,
				-1, color, 24, text.c_str(), highlightColor, begin, end);
		}
		nvgResetScissor(vg);
		bndSetFont(gGuiFont->handle);
	}
};

struct TD202 : ModuleWidget {
	TDVText *textField;

	TD202(Module *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/TD-202.svg")));

		textField = Widget::create<TDVText>(Vec(2, 15));
		textField->box.size = Vec(26, 350);
		addChild(textField);
	}

	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();

		json_object_set_new(rootJ, "text", json_string(textField->text.c_str()));
		json_object_set_new(rootJ, "fg", colorToJson(textField->color));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ)
			textField->color = jsonToColor(fgJ);
	}

	void reset() override {
		textField->text = "";
		textField->multiline = false;
		textField->color = nvgRGB(0x28, 0xb0, 0xf3);
		ModuleWidget::reset();
	}
};


Model *modelTD202 = Model::create<Module, TD202>("SubmarineFree", "TD-202", "TD-202 Vertical Text Display", VISUAL_TAG);
