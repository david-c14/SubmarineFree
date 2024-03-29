#include "../SubmarineFree.hpp"
#include "window/Window.hpp"

int SubText::getTextPosition(Vec mousePos) {
	std::shared_ptr<window::Font> font = APP->window->loadFont("res/fonts/ShareTechMono-Regular.ttf");
	if (font && font->handle >= 0) {
    		bndSetFont(font->handle);
		int textPos = bndIconLabelTextPosition(APP->window->vg, textOffset.x, textOffset.y,
	      		box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
	      		-1, fontSize, text.c_str(), mousePos.x, mousePos.y);
	    	bndSetFont(APP->window->uiFont->handle);
	    	return textPos;
	}
	return 0;
}

void SubText::draw(const DrawArgs &args) {
	//Background
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 5.0);
	nvgFillColor(args.vg, bgColor);
	nvgFill(args.vg);
}

void SubText::drawLayer(const DrawArgs &args, int layer) {
	if (layer == 1) {
		//Text
		nvgScissor(args.vg, 0, 0, box.size.x, box.size.y);
		std::shared_ptr<Font> font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
		if (font && font->handle >= 0) {
			bndSetFont(font->handle);
			
			NVGcolor highlightColor = color;
			highlightColor.a = 0.5;
			int begin = std::min(cursor, selection);
			int end = (this == APP->event->selectedWidget) ? std::max(cursor, selection) : -1;
			bndIconLabelCaret(args.vg, textOffset.x, textOffset.y,
				box.size.x - 2*textOffset.x, box.size.y - 2*textOffset.y,
				-1, color, fontSize, text.c_str(), highlightColor, begin, end);
    			bndSetFont(APP->window->uiFont->handle);
		}
		nvgResetScissor(args.vg);
	}
	Widget::drawLayer(args, layer);
}

void SubText::appendContextMenu(Menu *menu) {
	if (textMenu) {
		MenuLabel *label = new MenuLabel();
		label->text = "Text Size:";
		menu->addChild(label);
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		char str[20];
		snprintf(str, 20, "%0.3f", fontSize);
		paramField->setText(str);
		paramField->changeHandler = [=](std::string text) {
			try {
				fontSize = clamp(stof(text, NULL), 6.0f, 26.0f);
			}
			catch (...) {
			}
		};
		menu->addChild(paramField);
		menu->addChild(new MenuSeparator);
	}
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
	if (colorMenu) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(color::toHexString(color));
		paramField->changeHandler = [=](std::string text) {
			if (text[0] != '#')
				return;
			for (unsigned int i = 1; i < 7; i++) {
				if (text.length() <= i)
					return;
				if (!( (text[i] >= '0' && text[i] <= '9') ||
					(text[i] >= 'A' && text[i] <= 'F') ||
					(text[i] >= 'a' && text[i] <= 'f')))
					return;
			}
			this->color = color::fromHexString(text);
		};
		menu->addChild(paramField);
		menu->addChild(new MenuSeparator);
	}
	menu->addChild(createForegroundMenuItem("Blue", SUBLIGHTBLUE));
	menu->addChild(createForegroundMenuItem("Yellow", nvgRGB(0xc9, 0xb7, 0x0e)));
	menu->addChild(createForegroundMenuItem("Red", nvgRGB(0xff, 0x13, 0x13)));
	menu->addChild(createForegroundMenuItem("Green", nvgRGB(0x0a, 0xff, 0x13)));
	menu->addChild(createForegroundMenuItem("Orange", nvgRGB(0xff, 0xa5, 0x2d)));
	menu->addChild(createForegroundMenuItem("Pink", nvgRGB(0xff, 0x7d, 0xec)));
	menu->addChild(createForegroundMenuItem("White", nvgRGB(0xff, 0xff, 0xff)));
}

void SubText::backgroundMenu(Menu *menu) {
	if (colorMenu) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(color::toHexString(bgColor));
		paramField->changeHandler = [=](std::string text) {
			if (text[0] != '#')
				return;
			for (unsigned int i = 1; i < 7; i++) {
				if (text.length() <= i)
					return;
				if (!( (text[i] >= '0' && text[i] <= '9') ||
					(text[i] >= 'A' && text[i] <= 'F') ||
					(text[i] >= 'a' && text[i] <= 'f')))
					return;
			}
			this->bgColor = color::fromHexString(text);
		};
		menu->addChild(paramField);
		menu->addChild(new MenuSeparator);
	}
	menu->addChild(createBackgroundMenuItem("None", nvgRGBA(0, 0, 0, 0)));
	menu->addChild(createBackgroundMenuItem("Black", nvgRGB(0, 0, 0)));
	menu->addChild(createBackgroundMenuItem("White", nvgRGB(0xff, 0xff, 0xff)));
}

void SubText::onChange(const ChangeEvent &e) {
	if (changeHandler) {
		changeHandler();
	}
	LedDisplayTextField::onChange(e);
}
		
