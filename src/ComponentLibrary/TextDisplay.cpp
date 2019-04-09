#include "../SubmarineFree.hpp"
#include "window.hpp"

#define COLOR_EQ(x,y) (((x).a == (y).a) && ((x).r == (y).r) && ((x).g == (y).g) && ((x).b == (y).b))

Menu *SubTextForegroundParent::createChildMenu()  {
	Menu *menu = new Menu();
	subText->foregroundMenu(menu);
	return menu;	
}

void SubTextForegroundMenu::onAction(EventAction &e) {
	subText->color = color;
}

void SubTextForegroundMenu::step() {
	rightText = CHECKMARK(COLOR_EQ(subText->color, color));
	MenuItem::step();
}

Menu *SubTextBackgroundParent::createChildMenu() {
	Menu *menu = new Menu();
	subText->backgroundMenu(menu);
	return menu;
}

void SubTextBackgroundMenu::onAction(EventAction &e) {
	subText->bgColor = color;
}

void SubTextBackgroundMenu::step() {
	rightText = CHECKMARK(COLOR_EQ(subText->bgColor, color));
	MenuItem::step();
}

int SubText::getTextPosition(Vec mousePos) {
    bndSetFont(font->handle);
    int textPos = bndIconLabelTextPosition(gVg, textOffset.x, textOffset.y,
      box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
      -1, fontSize, text.c_str(), mousePos.x, mousePos.y);
    bndSetFont(gGuiFont->handle);
    return textPos;
}

void SubText::draw(NVGcontext *vg) {
	nvgScissor(vg, 0, 0, box.size.x, box.size.y);
	//Background
	nvgBeginPath(vg);
	nvgRoundedRect(vg, 0, 0, box.size.x, box.size.y, 5.0);
	nvgFillColor(vg, bgColor);
	nvgFill(vg);

	//Text
	if (font->handle >= 0) {
		bndSetFont(font->handle);
		
		NVGcolor highlightColor = color;
		highlightColor.a = 0.5;
		int begin = min(cursor, selection);
		int end = (this == gFocusedWidget) ? max(cursor, selection) : -1;
		bndIconLabelCaret(vg, textOffset.x, textOffset.y,
			box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
			-1, color, fontSize, text.c_str(), highlightColor, begin, end);
	}
	nvgResetScissor(vg);
	bndSetFont(gGuiFont->handle);
}

void SubText::appendContextMenu(Menu *menu) {
	SubTextForegroundParent *fp = MenuItem::create<SubTextForegroundParent>("Foreground");
	fp->subText = this;
	fp->rightText = SUBMENU; 	
	menu->addChild(fp);
	SubTextBackgroundParent *bp = MenuItem::create<SubTextBackgroundParent>("Background");
	bp->subText = this;
	bp->rightText = SUBMENU;
	menu->addChild(bp);
}

SubTextForegroundMenu *SubText::createForegroundMenuItem(std::string label, NVGcolor color) {
	SubTextForegroundMenu *menu = MenuItem::create<SubTextForegroundMenu>(label);
	menu->subText = this;
	menu->color = color;
	return menu;
}

SubTextBackgroundMenu *SubText::createBackgroundMenuItem(std::string label, NVGcolor color) {
	SubTextBackgroundMenu *menu = MenuItem::create<SubTextBackgroundMenu>(label);
	menu->subText = this;
	menu->color = color;
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
		
