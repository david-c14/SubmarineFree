//SubTag W10 WP AM

#include "SubmarineFree.hpp"
#include "window.hpp"

namespace {

	unsigned int nextId = 0;

	struct TD5Data {
		std::string text = "New Label";
		NVGcolor color = SUBLIGHTBLUE;
		float position = 20;
		int alignment = NVG_ALIGN_CENTER;
		int fontSize = 20;
	};

	struct TD5Text : OpaqueWidget {
		unsigned int id = 0;
		TD5Data *data;
		std::shared_ptr<Font> font;
		std::function<void ()> addMenuHandler;
		std::function<void (int oldPostion, int newPosition)> posHandler;
		int oldPosition = 0;
		TD5Text() {
			font = APP->window->loadFont(asset::system("res/fonts/ShareTechMono-Regular.ttf"));
			this->box.size = Vec(20, 350);
		}
		void draw(const DrawArgs &args) override {
			nvgFontFaceId(args.vg, font->handle);
			nvgFontSize(args.vg, data->fontSize);
			nvgFillColor(args.vg, data->color);
			nvgSave(args.vg);
			nvgRotate(args.vg, M_PI * 0.5f);
			if (data->alignment & NVG_ALIGN_LEFT) {
				nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, 0, -box.size.x / 2, data->text.c_str(), NULL);
			}
			else if (data->alignment & NVG_ALIGN_RIGHT) {
				nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, box.size.y, -box.size.x / 2, data->text.c_str(), NULL);
			}
			else {
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, box.size.y / 2, -box.size.x / 2, data->text.c_str(), NULL);
			}
			nvgRestore(args.vg);
		}
		void onButton(const event::Button &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_RIGHT && e.action == GLFW_PRESS) {
				e.consume(this);
				if (addMenuHandler) {
					addMenuHandler();
				}
			}
			OpaqueWidget::onButton(e);
		}
		void onDragStart(const event::DragStart &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				APP->window->cursorLock();
				e.consume(this);
				oldPosition = box.pos.x;
			}
			OpaqueWidget::onDragStart(e);
		}
		void onDragEnd(const event::DragEnd &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				APP->window->cursorUnlock();
				e.consume(this);
				if (box.pos.x != oldPosition) {
					if (posHandler) {
						posHandler(oldPosition, box.pos.x);
					}
				}
			}
			OpaqueWidget::onDragEnd(e);
		}
		void onDragMove(const event::DragMove &e) override {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
				e.consume(this);
				box.pos.x += e.mouseDelta.x;
			}
			OpaqueWidget::onDragMove(e);
		}
	};

} // end namespace

struct TD_510 : Module {
	std::vector<TD5Data *> dataItems;  
	TD_510() : Module () {
		config(0, 0, 0, 0);
	}
	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		json_object_set_new(rootJ, "width", json_real(moduleSize));
		json_t *arr = json_array();
		for (TD5Data *data : dataItems) {
			json_t *item = json_object();
			json_object_set_new(item, "text", json_string(data->text.c_str()));
			json_object_set_new(item, "color", json_string(color::toHexString(data->color).c_str()));
			json_object_set_new(item, "position", json_real(data->position));
			json_object_set_new(item, "alignment", json_real(data->alignment));
			json_object_set_new(item, "fontSize", json_real(data->fontSize));
			json_array_append_new(arr, item);
		}
		json_object_set_new(rootJ, "items", arr);
		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override {
		json_t *sizeJ = json_object_get(rootJ, "width");
		if (sizeJ)
			moduleSize = clamp(json_number_value(sizeJ), 75.0f, 300.0f);
		json_t *a1 = json_object_get(rootJ, "items");
		if (a1) {
			int asize = json_array_size(a1);
			for (int j = 0; j < asize; j++) {
				json_t *i = json_array_get(a1, j);
				if (i) {
					TD5Data *item = new TD5Data;
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
						item->position = json_number_value(pos);
					}
					json_t *align = json_object_get(i, "alignment");
					if (align) {
						item->alignment = json_number_value(align);
					}
					json_t *fSize = json_object_get(i, "fontSize");
					if (fSize) {
						item->fontSize = json_number_value(fSize);
					}
					dataItems.push_back(item);
				}
			}
		}
	}
	float moduleSize = 150.0f;
};

struct TD510 : SchemeModuleWidget {
	std::vector<TD5Text *> textItems;  
	SchemePanel *schemePanel;

	TD510(TD_510 *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		schemePanel = new SchemePanel(this->box.size, 75.0f, 300.0f);
		addChild(schemePanel);
	}

	void fromJson(json_t *rootJ) override {
		ModuleWidget::fromJson(rootJ);
		TD_510 *tdModule = dynamic_cast<TD_510 *>(module);
		if (!tdModule) return;
		
		for(TD5Data *data : tdModule->dataItems) {
			TD5Text *item = new TD5Text();
			item->data = data;
			item->box.pos = Vec(4, 15);
			addClickHandler(item);
			item->id = nextId++;
			item->box.size.x = data->fontSize = clampFontSize(data->fontSize);
			item->box.pos.x = data->position;
			addText(item);
		}
		box.size.x = tdModule->moduleSize;
		json_t *a1 = json_object_get(rootJ, "items");
		if (a1) {
			int asize = json_array_size(a1);
			for (int j = 0; j < asize; j++) {
				json_t *i = json_array_get(a1, j);
				if (i) {
					TD5Data *data = new TD5Data;
					tdModule->dataItems.push_back(data);
					TD5Text *item = new TD5Text();
					item->data = data;
					item->box.pos = Vec(4, 15);
					addClickHandler(item);
					item->id = nextId++;
					json_t *text = json_object_get(i, "text");
					if (text) {
						data->text = json_string_value(text);
					}
					json_t *color = json_object_get(i, "color");
					if (color) {
						data->color = color::fromHexString(json_string_value(color));
					}
					json_t *pos = json_object_get(i, "position");
					if (pos) {
						item->box.pos.x = data->position = json_number_value(pos);
					}
					json_t *align = json_object_get(i, "alignment");
					if (align) {
						data->alignment = json_number_value(align);
					}
					addText(item);
				}
			}
		}
		schemePanel->resize(this, box);
	}

	int clampPosition(int input) {
		return clamp(input, 4, (int)(box.size.x) - 28);
	}

	int clampFontSize(int input) {
		return clamp(input, 6, 26);
	}

	void textNameSubMenu(Menu *menu, TD5Text *textItem) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(textItem->data->text);
		paramField->changeHandler = [=](std::string text) {
			setText(textItem, text);
		};
		menu->addChild(paramField);
	}

	EventWidgetMenuItem *colorMenuItem(TD5Text *textItem, std::string label, NVGcolor color) {
		EventWidgetMenuItem *menuItem = createMenuItem<EventWidgetMenuItem>(label);
		menuItem->clickHandler = [=]() {
			setColor(textItem, color);
		};
		return menuItem;
	}

	void colorSubMenu(Menu *menu, TD5Text *textItem) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		paramField->setText(color::toHexString(textItem->data->color));
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

	void positionSubMenu(Menu *menu, TD5Text *textItem) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		char str[20];
		snprintf(str, 20, "%d", (int)(textItem->data->position));
		paramField->setText(str);
		paramField->changeHandler = [=](std::string text) {
			try {
				setPosition(textItem, textItem->box.pos.x, clampPosition(stoi(text, NULL)));
			}
			catch (...) {
			}
		};
		menu->addChild(paramField);
	}

	void fontSizeSubMenu(Menu *menu, TD5Text *textItem) {
		EventParamField *paramField = new EventParamField();
		paramField->box.size.x = 100;
		char str[20];
		snprintf(str, 20, "%d", textItem->data->fontSize);
		paramField->setText(str);
		paramField->changeHandler = [=](std::string text) {
			try {
				setFontSize(textItem, textItem->data->fontSize, clampFontSize(stoi(text, NULL)));
			}
			catch (...) {
			}
		};
		menu->addChild(paramField);
	}

	void addClickHandler(TD5Text *textItem) {
		textItem->posHandler = [=](int oldPosition, int newPosition) {
			setPosition(textItem, oldPosition, newPosition);
		};
		textItem->addMenuHandler = [=]() {
			Menu *menu = createMenu();
			EventWidgetMenuItem *textMenu = createMenuItem<EventWidgetMenuItem>("Label: " + textItem->data->text);
			textMenu->rightText = SUBMENU;
			textMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				textNameSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(textMenu);

			EventWidgetMenuItem *colorMenu = createMenuItem<EventWidgetMenuItem>("Color: " + color::toHexString(textItem->data->color));
			colorMenu->rightText = SUBMENU;
			colorMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				colorSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(colorMenu);

			char str[30];
			snprintf(str, 20, "Position: %d", (int)(textItem->data->position));
			EventWidgetMenuItem *positionMenu = createMenuItem<EventWidgetMenuItem>(str);
			positionMenu->rightText = SUBMENU;
			positionMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				positionSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(positionMenu);

			snprintf(str, 20, "Font Size: %d", textItem->data->fontSize);
			EventWidgetMenuItem *fontSizeMenu = createMenuItem<EventWidgetMenuItem>(str);
			fontSizeMenu->rightText = SUBMENU;
			fontSizeMenu->childMenuHandler = [=]() {
				Menu *subMenu = new Menu();
				fontSizeSubMenu(subMenu, textItem);
				return subMenu;
			};
			menu->addChild(fontSizeMenu);

			EventWidgetMenuItem *leftAlign = createMenuItem<EventWidgetMenuItem>("Top Align");
			leftAlign->stepHandler = [=]() {
				leftAlign->rightText = CHECKMARK(textItem->data->alignment & NVG_ALIGN_LEFT);
			};
			leftAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_LEFT);
			};
			menu->addChild(leftAlign);
			
			EventWidgetMenuItem *centerAlign = createMenuItem<EventWidgetMenuItem>("Center Align");
			centerAlign->stepHandler = [=]() {
				centerAlign->rightText = CHECKMARK(textItem->data->alignment & NVG_ALIGN_CENTER);
			};
			centerAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_CENTER);
			};
			menu->addChild(centerAlign);
			
			EventWidgetMenuItem *rightAlign = createMenuItem<EventWidgetMenuItem>("Bottom Align");
			rightAlign->stepHandler = [=]() {
				rightAlign->rightText = CHECKMARK(textItem->data->alignment & NVG_ALIGN_RIGHT);
			};
			rightAlign->clickHandler = [=]() {
				setAlignment(textItem, NVG_ALIGN_RIGHT);
			};
			menu->addChild(rightAlign);

			menu->addChild(new MenuSeparator);
			
			EventWidgetMenuItem *duplicate = createMenuItem<EventWidgetMenuItem>("Duplicate");
			duplicate->clickHandler = [=]() {
				duplicateItem(textItem);
			};
			menu->addChild(duplicate);

			menu->addChild(new MenuSeparator);
			
			EventWidgetMenuItem *deleteItem = createMenuItem<EventWidgetMenuItem>("Delete Label");
			deleteItem->clickHandler = [=]() {
				removeTextWithHistory(textItem);
			};
			menu->addChild(deleteItem);
		};
	}

	TD5Text *getTextItem(unsigned int itemId) {
		for (TD5Text *textItem : textItems) {
			if (textItem->id == itemId)
				return textItem;
		}
		return NULL;
	}
	
	void setText(TD5Text *textItem, std::string newText) {
		std::string oldText = textItem->data->text;
		textItem->data->text = newText;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-510 Change Label",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->data->text = oldText;
					}
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem)
						foundItem->data->text = newText;
				}
			}
		));
	}

	void setColor(TD5Text *textItem, NVGcolor newColor) {
		NVGcolor oldColor = textItem->data->color;
		textItem->data->color = newColor;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-510 Change Color",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->data->color = oldColor;
					}
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->data->color = newColor;
					}
				}
			}
		));
	}

	void setAlignment(TD5Text *textItem, int newAlignment) {
		int oldAlignment = textItem->data->alignment;
		textItem->data->alignment = newAlignment;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;
		
		APP->history->push(new EventWidgetAction(
			"TD-510 Change Alignment",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->data->alignment = oldAlignment;
					}
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->data->alignment = newAlignment;
					}
				}
			}
		));
	}
	
	void setPosition(TD5Text *textItem, int oldPosition, int newPosition) {
		newPosition = clampPosition(newPosition);
		textItem->box.pos.x = textItem->data->position = newPosition;
		if (newPosition == oldPosition)
			return;
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;

		APP->history->push(new EventWidgetAction(
			"TD-510 Change Position",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->box.pos.x = foundItem->data->position = oldPosition;
					}
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->box.pos.x = foundItem->data->position = newPosition;
					}
				}
			}
		));
	}

	void setFontSize(TD5Text *textItem, int oldSize, int newSize) {
		newSize = clampFontSize(newSize);
		if (newSize == oldSize)
			return;
		int oldPosition = textItem->data->position;
		textItem->box.size.x = textItem->data->fontSize = newSize;
		int newPosition = textItem->box.pos.x = textItem->data->position = clampPosition(textItem->data->position);
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = textItem->id;

		APP->history->push(new EventWidgetAction(
			"TD-510 Change Font Size",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->box.size.x = foundItem->data->fontSize = oldSize;
						foundItem->box.pos.x = foundItem->data->position = oldPosition;
					}
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					TD5Text *foundItem = mw->getTextItem(index);
					if (foundItem) {
						foundItem->box.size.x = foundItem->data->fontSize = newSize;
						foundItem->box.pos.x = foundItem->data->position = newPosition;
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
		drawBase(vg, "TD-510");
		nvgFillColor(vg, nvgRGB(0,0,0));
		nvgBeginPath(vg);
		nvgRect(vg, 4, 15, box.size.x - 8, box.size.y - 30);
		nvgFill(vg);
	}

	void removeText(TD5Text *text) {
		removeChild(text);
		TD_510 *td = dynamic_cast<TD_510 *>(module);
		td->dataItems.erase(std::remove(td->dataItems.begin(), td->dataItems.end(), text->data), td->dataItems.end());
		textItems.erase(std::remove(textItems.begin(), textItems.end(), text), textItems.end());
	}

	void removeText(unsigned int index) {
		TD5Text *foundItem = getTextItem(index);
		if (foundItem) 
			removeText(foundItem);
	}

	void addText(TD5Text * textField) {
		addChild(textField);
		textItems.push_back(textField);
	}

	void addText(unsigned int id, std::string text, NVGcolor color, int position, int alignment, int fontSize) {
		TD5Data *newData = new TD5Data;
		dynamic_cast<TD_510 *>(module)->dataItems.push_back(newData);
		TD5Text *newItem = new TD5Text();
		newItem->data = newData;
		newItem->box.size.x = fontSize;
		newItem->box.pos = Vec(newData->position = position, 15);
		newItem->id = id;
		addClickHandler(newItem);
		newData->color = color;
		newData->text = text;
		newData->alignment = alignment;
		newData->fontSize = fontSize;
		addText(newItem);
	}

	void duplicateItem(TD5Text *textItem) {
		int index = nextId++;
		int fontSize = textItem->data->fontSize;
		int position = findPosition(fontSize);
		NVGcolor color = textItem->data->color;
		std::string text = textItem->data->text;
		int alignment = textItem->data->alignment;
		addText(index, text, color, position, alignment, fontSize);
		int moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			"TD-510 Duplicate Label",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->removeText(index);
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->addText(index, text, color, position, alignment, fontSize);
				}
			}
		));
	}

	int findPosition(int spacing) {
		int position = clampPosition(0);
		bool found = false;
		while (!found) {
			found = true;
			for (TD5Text *text: textItems) {
				if (abs(text->box.pos.x - position) < spacing) {
					found = false;
					if ((text->box.pos.x + spacing) > position) {
						position = text->box.pos.x + spacing;
					}
					break;
				} 
			}
			if (position > clampPosition(position)) {
				position = clampPosition(0);
				found = true;
			}
		}	
		return position;
	}

	void addNewText() {
		int position = findPosition(20);
		TD5Data *newData = new TD5Data;
		dynamic_cast<TD_510 *>(module)->dataItems.push_back(newData);
		TD5Text *newItem = new TD5Text();
		newItem->data = newData;
		newItem->box.pos = Vec(newData->position = position, 4);
		newItem->id = nextId++;
		addClickHandler(newItem);
		addNewTextWithHistory(newItem);
	}
	
	void addNewTextWithHistory(TD5Text *newItem) {
		addText(newItem);
		if (!module)
			return;
		int moduleId = module->id;
		int index = newItem->id;
		NVGcolor color = newItem->data->color;
		std::string text = newItem->data->text;
		int position = newItem->box.pos.x;
		int alignment = newItem->data->alignment;
		int fontSize = newItem->data->fontSize;
		
		APP->history->push(new EventWidgetAction(
			"TD-510 Add Label",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->removeText(index);
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->addText(index, text, color, position, alignment, fontSize);
				}
			}
		));
	}

	void removeTextWithHistory(TD5Text *oldItem) {
		removeText(oldItem);
		if (!module)
			return;
		int moduleId = module->id;
		unsigned int index = oldItem->id;
		NVGcolor color = oldItem->data->color;
		std::string text = oldItem->data->text;
		int alignment = oldItem->data->alignment;
		int position = oldItem->box.pos.x;
		int fontSize = oldItem->data->fontSize;
	
		APP->history->push(new EventWidgetAction(
			"TD-510 Remove Label",
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->addText(index, text, color, position, alignment, fontSize);
				}
			},
			[=]() {
				TD510 *mw = getModuleWidgetById(moduleId);
				if (mw) {
					mw->removeText(index);
				}
			}
		));
	}

	TD510 *getModuleWidgetById(int moduleId) {
		for (Widget *widget : APP->scene->rack->moduleContainer->children) {
			TD510 *mw = dynamic_cast<TD510 *>(widget);
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

	void onResize(const event::Resize &e) override {
		ModuleWidget::onResize(e);
		onResized();
	}
	void onResized() {
		for (TD5Text *text : textItems) {
			text->box.pos.x = text->data->position = clampPosition(text->data->position);
		}
		if (module) {
			TD_510 *td = dynamic_cast<TD_510 *>(module);
			td->moduleSize = box.size.x;
		}
	}
};

Model *modelTD510 = createModel<TD_510, TD510>("TD-510");
