#include "../SubmarineFree.hpp"
#include "window.hpp"

#define COLOR_EQ(x,y) (((x).a == (y).a) && ((x).r == (y).r) && ((x).g == (y).g) && ((x).b == (y).b))

int SubText::getTextPosition(Vec mousePos) {
    bndSetFont(font->handle);
    int textPos = bndIconLabelTextPosition(APP->window->vg, textOffset.x, textOffset.y,
      box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
      -1, fontSize, text.c_str(), mousePos.x, mousePos.y);
    bndSetFont(APP->window->uiFont->handle);
    return textPos;
}

void SubText::draw(const DrawArgs &args) {
	nvgScissor(args.vg, 0, 0, box.size.x, box.size.y);
	//Background
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 5.0);
	nvgFillColor(args.vg, bgColor);
	nvgFill(args.vg);

	//Text
	if (font->handle >= 0) {
		bndSetFont(font->handle);
		
		NVGcolor highlightColor = color;
		highlightColor.a = 0.5;
		int begin = std::min(cursor, selection);
		int end = (this == APP->event->selectedWidget) ? std::max(cursor, selection) : -1;
		bndIconLabelCaret(args.vg, textOffset.x, textOffset.y,
			box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
			-1, color, fontSize, text.c_str(), highlightColor, begin, end);
	}
	nvgResetScissor(args.vg);
	bndSetFont(APP->window->uiFont->handle);
}

void SubText::appendContextMenu(Menu *menu) {
	EventWidgetMenuItem *fp = createMenuItem<EventWidgetMenuItem>("Foreground");
	fp->rightText = SUBMENU;
	fp->childMenuHandler = [=]() {
		Menu *menu = new Menu();
		this->foregroundMenu(menu);
		return menu;
	};
	menu->addChild(fp);
	EventWidgetMenuItem *bp = createMenuItem<EventWidgetMenuItem>("Background");
	bp->rightText = SUBMENU;
	bp->childMenuHandler = [=]() {
		Menu *menu = new Menu();
		this->backgroundMenu(menu);
		return menu;
	};
	menu->addChild(bp);
}

MenuItem *SubText::createForegroundMenuItem(std::string label, NVGcolor color) {
	EventWidgetMenuItem *menu = createMenuItem<EventWidgetMenuItem>(label);
	menu->stepHandler = [=]() {
		menu->rightText = CHECKMARK(COLOR_EQ(this->color, color));
	};
	menu->clickHandler = [=]() {
		this->color = color;
	};	
	return menu;
}

MenuItem *SubText::createBackgroundMenuItem(std::string label, NVGcolor color) {
	EventWidgetMenuItem *menu = createMenuItem<EventWidgetMenuItem>(label);
	menu->stepHandler = [=]() {
		menu->rightText = CHECKMARK(COLOR_EQ(this->bgColor, color));
	};
	menu->clickHandler = [=]() {
		this->bgColor = color;
	};	
	return menu;
}

void SubText::foregroundMenu(Menu *menu) {
	menu->addChild(createForegroundMenuItem("Blue", SUBLIGHTBLUE));
	menu->addChild(createForegroundMenuItem("Yellow", nvgRGB(0xc9, 0xb7, 0x0e)));
	menu->addChild(createForegroundMenuItem("Red", nvgRGB(0xff, 0x13, 0x13)));
	menu->addChild(createForegroundMenuItem("Green", nvgRGB(0x0a, 0xff, 0x13)));
	menu->addChild(createForegroundMenuItem("Orange", nvgRGB(0xff, 0xa5, 0x2d)));
	menu->addChild(createForegroundMenuItem("Pink", nvgRGB(0xff, 0x7d, 0xec)));
	menu->addChild(createForegroundMenuItem("White", nvgRGB(0xff, 0xff, 0xff)));
}

void SubText::backgroundMenu(Menu *menu) {
	menu->addChild(createBackgroundMenuItem("None", nvgRGBA(0, 0, 0, 0)));
	menu->addChild(createBackgroundMenuItem("Black", nvgRGB(0, 0, 0)));
	menu->addChild(createBackgroundMenuItem("White", nvgRGB(0xff, 0xff, 0xff)));
}
		
