#include "SubmarineFree.hpp"
#include "window.hpp"
#include "torpedo.hpp"

struct TDVText : LedDisplayTextField {
	TDVText() {
		multiline = false;
		color = nvgRGB(0x28, 0xb0, 0xf3);
	}
	void draw(NVGcontext *vg) override {
		nvgScissor(vg, 0, 0, box.size.x, box.size.y);

		nvgTranslate(vg, 24, 0);
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
				-1, color, 28, text.c_str(), highlightColor, begin, end);
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
		json_object_set_new(rootJ, "fg", json_string(colorToHexString(textField->color).c_str()));

		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);

		json_t *textJ = json_object_get(rootJ, "text");
		if (textJ)
			textField->text = json_string_value(textJ);
		json_t *fgJ = json_object_get(rootJ, "fg");
		if (fgJ) {
			if (json_is_object(fgJ)) {
				textField->color = jsonToColor(fgJ);
			}
			else {
				textField->color = colorFromHexString(json_string_value(fgJ));
			}
		}
	}

	void reset() override {
		textField->text = "";
		textField->multiline = false;
		textField->color = nvgRGB(0x28, 0xb0, 0xf3);
		ModuleWidget::reset();
	}
	
	void appendContextMenu(Menu *menu) override;
};

struct TD202_MenuItem : MenuItem {
	TD202 *widget;
	NVGcolor color;
	void onAction(EventAction &e) override {
		widget->textField->color = color;
	}
};

void TD202::appendContextMenu(Menu *menu) {
	menu->addChild(MenuEntry::create());
	TD202_MenuItem *m = MenuItem::create<TD202_MenuItem>("Blue");
	m->widget = this;
	m->color = nvgRGB(0x28, 0xb0, 0xf3);
	menu->addChild(m);
	
	m = MenuItem::create<TD202_MenuItem>("Yellow");
	m->widget = this;
	m->color = nvgRGB(0xc9, 0xb7, 0x0e);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("Red");
	m->widget = this;
	m->color = nvgRGB(0xff, 0x13, 0x13);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("Green");
	m->widget = this;
	m->color = nvgRGB(0x0a, 0xff, 0x13);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("Orange");
	m->widget = this;
	m->color = nvgRGB(0xff, 0xa5, 0x2d);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("Pink");
	m->widget = this;
	m->color = nvgRGB(0xff, 0x7d, 0xec);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("White");
	m->widget = this;
	m->color = nvgRGB(0xff, 0xff, 0xff);
	menu->addChild(m);

	m = MenuItem::create<TD202_MenuItem>("Black");
	m->widget = this;
	m->color = nvgRGB(0x00, 0x00, 0x00);
	menu->addChild(m);

}


Model *modelTD202 = Model::create<Module, TD202>("SubmarineFree", "TD-202", "TD-202 Vertical Text Display", VISUAL_TAG);
