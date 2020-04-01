//SubTag W10

#include "SubmarineFree.hpp"
#include "window.hpp"

namespace {

	unsigned int nextId = 0;

	struct TD4Text : OpaqueWidget {
		unsigned int id = 0;
		std::shared_ptr<Font> font;
		NVGcolor color = SUBLIGHTBLUE;
		std::string text = "Sample";
		int alignment = NVG_ALIGN_CENTER;
		std::function<void ()> addMenuHandler;
		TD4Text() {
			font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
			this->box.size = Vec(142, 20);
		}
		void draw(const DrawArgs &args) override {
			nvgFontFaceId(args.vg, font->handle);
			nvgFontSize(args.vg, 20);
			nvgFillColor(args.vg, color);
			if (alignment & NVG_ALIGN_LEFT) {
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, 0, box.size.y / 2, text.c_str(), NULL);
			}
			else if (alignment & NVG_ALIGN_RIGHT) {
				nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, box.size.x, box.size.y / 2, text.c_str(), NULL);
			}
			else {
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, box.size.x / 2, box.size.y / 2, text.c_str(), NULL);
			}
		}
		void onButton(const event::Button &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
				e.consume(this);
				if (addMenuHandler) {
					addMenuHandler();
				}
			}
			else {
				OpaqueWidget::onButton(e);
			}
		}
	};

} // end namespace

struct TD410 : SchemeModuleWidget {
	std::vector<TD4Text *> textItems;  

	TD410(Module *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));
	}


	json_t *toJson() override {
		json_t *rootJ = ModuleWidget::toJson();
		json_t *arr = json_array();
		for (TD4Text *text : textItems) {
			json_t *item = json_object();
			json_object_set_new(item, "text", json_string(text->text.c_str()));
			json_object_set_new(item, "color", json_string(color::toHexString(text->color).c_str()));
			json_object_set_new(item, "position", json_real(text->box.pos.y));
			json_object_set_new(item, "alignment", json_real(text->alignment));
			json_array_append_new(arr, item);
		}
		json_object_set_new(rootJ, "items", arr);
		return rootJ;
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);
		json_t *a1 = json_object_get(rootJ, "items");
		if (a1) {
			int asize = json_array_size(a1);
			for (int j = 0; j < asize; j++) {
				json_t *i = json_array_get(a1, j);
				if (i) {
					TD4Text *item = createWidget<TD4Text>(Vec(4, 18));
					addClickHandler(item);
					item->id = nextId++;
					json_t *text = json_object_get(i, "text");
					if (text) {
						item->text = json_string_value(text);
					}
					json_t *color = json_object_get(i, "color");
					if (color) {
						item->color = color::fromHexString(json_string_value(color));
					}
					json_t *pos = json_object_get(i, "position");
					if (pos) {
						item->box.pos.y = clamp(json_number_value(pos), 20.0f, box.size.y - 30);
					}
					json_t *align = json_object_get(i, "alignment");
					if (align) {
						item->alignment = json_number_value(align);
					}
					addText(item);
				}
			}
		}
	}

	void textNameSubMenu(Menu *menu, TD4Text *textItem) {
			EventParamField *paramField = new EventParamField();
			paramField->box.size.x = 100;
			paramField->setText(textItem->text);
			paramField->changeHandler = [=](std::string text) {
				setText(textItem, text);
			};
			menu->addChild(paramField);
	}

	EventWidgetMenuItem *colorMenuItem(TD4Text *textItem, std::string label, NVGcolor color) {
		EventWidgetMenuItem *menuItem = createMenuItem<EventWidgetMenuItem>(label);
		menuItem->clickHandler = [=]() {
			setColor(textItem, color);
		};
		return menuItem;
	}

	void colorSubMenu(Menu *menu, TD4Text *textItem) {
			EventParamField *paramField = new EventParamField();
			paramField->box.size.x = 100;
			paramField->setText(color::toHexString(textItem->color));
			paramField->changeHandler = [=](std::string text) {
				if (text[0] != '#')
					return;
				for (unsigned int i = 1; i < 7; i++) {
					if (!( (text[i] >= '0' && text[i] <= '9') ||
						(text[i] >= 'A' && text[i] <= 'F') ||
						(text[i] >= 'a' && text[i] <= 'f')))
						return;
				}
				setColor(textItem, color::fromHexString(text));
			};
			menu->addChild(paramField);
			menu->addChild(colorMenuItem(textItem, "Blue", SUBLIGHTBLUE));
			menu->addChild(colorMenuItem(textItem, "Yellow", nvgRGB(0xc9, 0xb7, 0x0e)));
			menu->addChild(colorMenuItem(textItem, "Red", nvgRGB(0xff, 0x13, 0x13)));
			menu->addChild(colorMenuItem(textItem, "Green", nvgRGB(0x0a, 0xff, 0x13)));
			menu->addChild(colorMenuItem(textItem, "Orange", nvgRGB(0xff, 0xa5, 0x2d)));
			menu->addChild(colorMenuItem(textItem, "Pink", nvgRGB(0xff, 0x7d, 0xec)));
			menu->addChild(colorMenuItem(textItem, "White", nvgRGB(0xff, 0xff, 0xff)));
	}

	void addClickHandler(TD4Text *textItem) {
		textItem->addMenuHandler = [=]() {
			Menu *menu = createMenu();
			EventWidgetMenuItem *textMenu = createMenuItem<EventWidgetMenuItem>("Label: " + textItem->text);
			textMenu->rightText = SUBMENU;
			textMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				textNameSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(textMenu);

			EventWidgetMenuItem *colorMenu = createMenuItem<EventWidgetMenuItem>("Color: " + color::toHexString(textItem->color));
			colorMenu->rightText = SUBMENU;
			colorMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				colorSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(colorMenu);

			EventWidgetMenuItem *leftAlign = createMenuItem<EventWidgetMenuItem>("Left Align");
			leftAlign->stepHandler = [=]() {
				leftAlign->rightText = CHECKMARK(textItem->alignment & NVG_ALIGN_LEFT);
			};
			leftAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_LEFT);
			};
			menu->addChild(leftAlign);
			
			EventWidgetMenuItem *centerAlign = createMenuItem<EventWidgetMenuItem>("Center Align");
			centerAlign->stepHandler = [=]() {
				centerAlign->rightText = CHECKMARK(textItem->alignment & NVG_ALIGN_CENTER);
			};
			centerAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_CENTER);
			};
			menu->addChild(centerAlign);
			
			EventWidgetMenuItem *rightAlign = createMenuItem<EventWidgetMenuItem>("Right Align");
			rightAlign->stepHandler = [=]() {
				rightAlign->rightText = CHECKMARK(textItem->alignment & NVG_ALIGN_RIGHT);
			};
			rightAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_RIGHT);
			};
			menu->addChild(rightAlign);

			menu->addChild(new MenuSeparator);
			
			EventWidgetMenuItem *deleteItem = createMenuItem<EventWidgetMenuItem>("Delete Label");
			deleteItem->clickHandler = [=]() {
				removeTextWithHistory(textItem);
			};
			menu->addChild(deleteItem);
		};
	}

	TD4Text *getTextItem(unsigned int itemId) {
		for (TD4Text *textItem : textItems) {
			if (textItem->id == itemId)
				return textItem;
		}
		return NULL;
	}
	
	void setText(TD4Text *textItem, std::string newText) {
		std::string oldText = textItem->text;
		textItem->text = newText;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-410 Change Label",
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->text = oldText;
					}
				}
			},
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem)
						foundItem->text = newText;
				}
			}
		));
	}

	void setColor(TD4Text *textItem, NVGcolor newColor) {
		NVGcolor oldColor = textItem->color;
		textItem->color = newColor;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-410 Change Color",
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->color = oldColor;
					}
				}
			},
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->color = newColor;
					}
				}
			}
		));
	}

	void setAlignment(TD4Text *textItem, int newAlignment) {
		int oldAlignment = textItem->alignment;
		textItem->alignment = newAlignment;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-410 Change Alignment",
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->alignment = oldAlignment;
					}
				}
			},
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD4Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->alignment = newAlignment;
					}
				}
			}
		));
	}

	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		EventWidgetMenuItem *mi = createMenuItem<EventWidgetMenuItem>("Add Label");
		mi->clickHandler = [=]() {
			this->addNewText();
		};
		menu->addChild(mi);
	}

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "TD-416");
		nvgFillColor(vg, nvgRGB(0,0,0));
		nvgBeginPath(vg);
		nvgRect(vg, 4, 15, box.size.x - 8, box.size.y - 30);
		nvgFill(vg);
	}

	void removeText(TD4Text *text) {
		removeChild(text);
		textItems.erase(std::remove(textItems.begin(), textItems.end(), text), textItems.end());
	}

	void removeText(unsigned int index) {
		TD4Text *foundItem = getTextItem(index);
		if (foundItem) 
			removeText(foundItem);
	}

	void addText(TD4Text * textField) {
		addChild(textField);
		textItems.push_back(textField);
	}

	void addText(unsigned int id, std::string text, NVGcolor color, int position, int alignment) {
		TD4Text *newItem = createWidget<TD4Text>(Vec(4, position));
		newItem->id = id;
		addClickHandler(newItem);
		newItem->color = color;
		newItem->text = text;
		newItem->alignment = alignment;
		addText(newItem);
	}

	void addNewText() {
		int position = 20;
		bool found = false;
		while (!found) {
			found = true;
			for (TD4Text *text: textItems) {
				if (abs(text->box.pos.y - position) < 20) {
					found = false;
					if ((text->box.pos.y + 20) > position) {
						position = text->box.pos.y + 20;
					}
					break;
				} 
			}
			if (position > (box.size.y - 30)) {
				position = 0;
				found = true;
			}
		}	
		TD4Text *newItem = createWidget<TD4Text>(Vec(4, position));
		newItem->id = nextId++;
		addClickHandler(newItem);
		addNewTextWithHistory(newItem);
	}
	
	void addNewTextWithHistory(TD4Text *newItem) {
		addText(newItem);
		if (!module)
			return;
		int moduleId = module->id;
		int index = newItem->id;
		NVGcolor color = newItem->color;
		std::string text = newItem->text;
		int position = newItem->box.pos.y;
		int alignment = newItem->alignment;
		
		APP->history->push(new EventWidgetAction(
			"TD-410 Add Label",
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->removeText(index);
				}
			},
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->addText(index, text, color, position, alignment);
				}
			}
		));
	}

	void removeTextWithHistory(TD4Text *oldItem) {
		removeText(oldItem);
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = oldItem->id;
		NVGcolor color = oldItem->color;
		std::string text = oldItem->text;
		int alignment = oldItem->alignment;
		int position = oldItem->box.pos.y;
	
		APP->history->push(new EventWidgetAction(
			"TD-410 Remove Label",
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->addText(index, text, color, position, alignment);
				}
			},
			[=]() {
				TD410 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->removeText(index);
				}
			}
		));
	}

	TD410 *getModuleWidgetById(int moduleId) {
		for (Widget *widget : APP->scene->rack->moduleContainer->children) {
			TD410 *mw = dynamic_cast<TD410 *>(widget);
			if (mw) {
				if (mw->module) {
					if (mw->module->id == moduleId) {
						return mw;
					}
				}
			}
		}
		return NULL;
	}
};

Model *modelTD410 = createModel<Module, TD410>("TD-410");
