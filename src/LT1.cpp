//SubTag W20

#include <string.h>
#include "SubmarineFree.hpp"

namespace {
	alignas(16) float clipboard[256];
	bool clipboardUsed = false;
	int clipboardRow = -1;
	int clipboardColumn = -1;
	const int bulkParamSize = sizeof(float) * 256;

	struct LTKnob : LightKnob 
	{
		std::function<void(ui::Menu *)> contextMenuCallback;
		void appendContextMenu(ui::Menu *menu) override {
			ParamWidget::appendContextMenu(menu);
			if (contextMenuCallback) {
				contextMenuCallback(menu);
			}
		};
	};
}

struct LT_116 : Module {
	enum ParamIds {
		PARAM_OUTPUT_CHANNELS,
		PARAM_COEFF_1,
		NUM_PARAMS = PARAM_COEFF_1 + 256
	};
	enum InputIds {
		INPUT_1,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_1,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	alignas(16) float bulkParams[256] = {.0f};
	unsigned char tick = 0;

	int numberOfInputs = 1;
	int numberOfOutputs = 16;
	
	LT_116() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM_OUTPUT_CHANNELS, 1.0f, 16.0f, 16.0f, "Number of channels in output");
		configInput(INPUT_1, "Signal");
		configOutput(OUTPUT_1, "Signal");
		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				configParam(PARAM_COEFF_1 + i * 16 + j, -100, +100, .0f, string::f("Coefficient [%d,%d]", j + 1, i + 1));
			}
		}
	}

	void dataFromJson(json_t *rootJ) override {
		json_t *arr = json_object_get(rootJ, "coefficients");
		if (arr) {
			int size = json_array_size(arr);
			if (size > 256)
				size = 256;
			for (int i = 0; i < size; i++) {
				json_t *j1 = json_array_get(arr, i);
				if (j1) {
					params[PARAM_COEFF_1 + i].setValue(json_real_value(j1));
				}
			}
			gatherBulkParams();
			tick = 1;
		}
	}
	void gatherBulkParams() {
		for (unsigned int i = 0; i < 256; i++) {
			bulkParams[i] = params[PARAM_COEFF_1 + i].getValue();
		}
	}

	void process(const ProcessArgs &args) override {
		if (!tick++) {
			gatherBulkParams();
		}
		numberOfInputs = inputs[INPUT_1].getChannels();
		numberOfOutputs = params[PARAM_OUTPUT_CHANNELS].getValue();

		alignas(16) float portValues[16];
		inputs[INPUT_1].readVoltages(portValues);
		
		__m128 a,b,c,d;
		a = b = c = d = _mm_setzero_ps();
		float *index = bulkParams;
		for (int j = 0; j < numberOfInputs; j++) {
			__m128 i = _mm_set_ps1(portValues[j]);
			__m128 x = _mm_load_ps(index);
			a = _mm_add_ps(a, _mm_mul_ps(i,x));
			index += 4;
			x = _mm_load_ps(index);
			b = _mm_add_ps(b, _mm_mul_ps(i,x));
			index += 4;
			x = _mm_load_ps(index);
			c = _mm_add_ps(c, _mm_mul_ps(i,x));
			index += 4;
			x = _mm_load_ps(index);
			d = _mm_add_ps(d, _mm_mul_ps(i,x));
			index += 4; 
		}
		_mm_store_ps(portValues, a);
		_mm_store_ps(portValues + 4, b);
		_mm_store_ps(portValues + 8, c);
		_mm_store_ps(portValues + 12, d);

		outputs[OUTPUT_1].setChannels(numberOfOutputs);
		outputs[OUTPUT_1].writeVoltages(portValues);
	}
};

struct LT116 : SchemeModuleWidget {
	LTKnob *knobs[256];
	LT116(LT_116 *module) {
		setModule(module);
		this->box.size = Vec(300, 380);
		addChild(new SchemePanel(this->box.size));

		for (unsigned int i = 0; i < 16; i++) {
			for (unsigned int j = 0; j < 16; j++) {
				int index = i * 16 + j;
				knobs[index] = createParamCentered<TinyKnob<LTKnob>>(Vec(15 + j * 18, 30 + i * 18), module, LT_116::PARAM_COEFF_1 + index);
				
				if (module) {
					knobs[index]->contextMenuCallback = [=](Menu *menu) {
						this->appendOperationMenu(menu, i, j);
					};
				}
				
				addChild(knobs[index]);
			}
		}
		addInput(createInputCentered<SilverPort>(Vec(35, 330), module, LT_116::INPUT_1));
		addOutput(createOutputCentered<SilverPort>(Vec(265, 330), module, LT_116::OUTPUT_1));
		addParam(createParamCentered<SnapKnob<SmallKnob<LightKnob>>>(Vec(200, 330), module, LT_116::PARAM_OUTPUT_CHANNELS));
	}
	void appendOperationMenu(Menu *menu, int row, int column) {
		if (!module) {
			return;
		}
		menu->addChild(new MenuSeparator);
		appendCopyMenu(menu, row, column);
		appendPasteMenu(menu, row, column);
		appendPresetMenu(menu);
		appendNormaliseMenu(menu, column);
	}
	void appendCopyMenu(Menu *menu, int row, int column) {
		EventWidgetMenuItem *cmi = createMenuItem<EventWidgetMenuItem>("Copy");
		cmi->clickHandler = [=]() {
			copy();
		};
		menu->addChild(cmi);
		if (row > -1) {
			cmi->rightText = SUBMENU;
			cmi->childMenuHandler = [=]() {
				return appendCopySubMenu(row, column);
			};
		}
	}
	Menu *appendCopySubMenu(int row, int column) {
		Menu *menu = new Menu();
		EventWidgetMenuItem *cmi = createMenuItem<EventWidgetMenuItem>("Copy");
		cmi->clickHandler = [=]() {
			copy();
		};
		menu->addChild(cmi);
		cmi = createMenuItem<EventWidgetMenuItem>("Copy Row");
		cmi->clickHandler = [=]() {
			copyRow(row);
		};
		menu->addChild(cmi);
		cmi = createMenuItem<EventWidgetMenuItem>("Copy Column");
		cmi->clickHandler = [=]() {
			copyColumn(column);
		};
		menu->addChild(cmi);
		cmi = createMenuItem<EventWidgetMenuItem>("Copy Cell");
		cmi->clickHandler = [=]() {
			copyCell(row, column);
		};
		menu->addChild(cmi);
		return menu;
	}
	void appendPasteMenu(Menu *menu, int row, int column) {
		if (!clipboardUsed) {
			return;
		}
		EventWidgetMenuItem *pmi;
		std::string menuLabel;
		if (clipboardRow > -1) {
			if (clipboardColumn > -1) {
				menuLabel = "Paste Cell";
			}
			else {
				menuLabel = "Paste Row";
			}
		}
		else {
			if (clipboardColumn > -1) {
				menuLabel = "Paste Column";
			}
			else {
				menuLabel = "Paste";
			}
		}
		pmi = createMenuItem<EventWidgetMenuItem>(menuLabel);
		pmi->clickHandler = [=]() {
			paste(row, column);
		};
		menu->addChild(pmi);
		pmi->rightText = SUBMENU;
		pmi->childMenuHandler = [=]() {
			return appendPasteSubMenu(menuLabel, row, column);
		};
	}
	Menu *appendPasteSubMenu(std::string menuLabel, int row, int column) {
		Menu *menu = new Menu();
		EventWidgetMenuItem *pmi = createMenuItem<EventWidgetMenuItem>(menuLabel);
		pmi->clickHandler = [=]() {
			paste(row, column);
		};
		menu->addChild(pmi);

		pmi = createMenuItem<EventWidgetMenuItem>("Paste Multiply");
		pmi->clickHandler = [=]() {
			this->pasteMultiply(row, column);
		};
		menu->addChild(pmi);

		pmi = createMenuItem<EventWidgetMenuItem>("Paste Add");
		pmi->clickHandler = [=]() {
			this->pasteAdd(row, column);
		};
		menu->addChild(pmi);

		pmi = createMenuItem<EventWidgetMenuItem>("Paste Subtract");
		pmi->clickHandler = [=]() {
			this->pasteSubtract(row, column);
		};
		menu->addChild(pmi);
		return menu;
	}
	void appendPresetMenu(Menu *menu) {
		if (!module)
			return;
		EventWidgetMenuItem *pmi = createMenuItem<EventWidgetMenuItem>("Presets");
		pmi->rightText = SUBMENU;
		pmi->childMenuHandler = [=]() {
			Menu *thisMenu = new Menu();
			EventWidgetMenuItem *cmi = createMenuItem<EventWidgetMenuItem>("Identity");
			cmi->clickHandler = [=]() {
				this->opIdentity();
			};
			thisMenu->addChild(cmi);
			cmi = createMenuItem<EventWidgetMenuItem>("Upper");
			cmi->clickHandler = [=]() {
				this->opUpper();
			};
			thisMenu->addChild(cmi);
			cmi = createMenuItem<EventWidgetMenuItem>("Lower");
			cmi->clickHandler = [=]() {
				this->opLower();
			};
			thisMenu->addChild(cmi);
			return thisMenu;
		};
		menu->addChild(pmi);
	}
	void appendNormaliseMenu(Menu *menu, int column) {
		if (!module)
			return;
		EventWidgetMenuItem *pmi = createMenuItem<EventWidgetMenuItem>("Normalise");
		pmi->rightText = SUBMENU;
		pmi->childMenuHandler = [=]() {
			Menu *thisMenu = new Menu();
			EventWidgetMenuItem *cmi = createMenuItem<EventWidgetMenuItem>("Normalise");
			cmi->clickHandler = [=]() {
				this->normalise();
			};
			thisMenu->addChild(cmi);
			if (column > -1) {
				cmi = createMenuItem<EventWidgetMenuItem>("Normalize Column");
				cmi->clickHandler = [=]() {
					this->normalise(column);
				};
				thisMenu->addChild(cmi);
			}
			cmi = createMenuItem<EventWidgetMenuItem>("Normalise All Columns");
			cmi->clickHandler = [=]() {
				this->normaliseAll();
			};
			thisMenu->addChild(cmi);
			return thisMenu;
		};
		menu->addChild(pmi);
	}
	void appendContextMenu(Menu *menu) override {
		appendOperationMenu(menu, -1, -1);
		SchemeModuleWidget::appendContextMenu(menu);
	}
	void step() override {
		if (!module)
			return;
		LT_116 *ltModule = dynamic_cast<LT_116 *>(module);
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				knobs[i * 16 + j]->setEnabled((j < ltModule->numberOfOutputs) && (i < ltModule->numberOfInputs));
			}
		}
		SchemeModuleWidget::step();
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "LT-116");

		// Text
		drawText(vg, 35, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "INPUT");
		drawText(vg, 265, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUTPUT");
		drawText(vg, 200, 352, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "CHANNELS");
		drawText(vg, 50, 330, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x93"); 
		drawText(vg, 240, 330, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 16, gScheme.getContrast(module), "\xE2\x86\x92"); 
	}
	void getKnobs(float* values) {
		for (unsigned int i = 0; i < 256; i++) {
			values[i] = knobs[i]->getParamQuantity()->getValue();
		}
	}
	void setKnobs(const float *values) {
		for (unsigned int i = 0; i < 256; i++) {
			knobs[i]->getParamQuantity()->setValue(values[i]);
		}
	}
	void bulkChangeWithHistory(std::string label, std::function<void (float *params)> func) {
		float oldValues[256];
		float newValues[256];
		float bulkParams[256];
		getKnobs(bulkParams);
		memcpy(oldValues, bulkParams, bulkParamSize);
		func(bulkParams);
		memcpy(newValues, bulkParams, bulkParamSize);
		setKnobs(newValues);
		int64_t moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			label,
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->setKnobs(oldValues);
				}
			},
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->setKnobs(newValues);
				}
			}
		));	
	}
	void bulkChangeWithHistory(std::string label, float *values) {
		bulkChangeWithHistory(label, [=](float *params) {
			memcpy(params, values, bulkParamSize);	
			setKnobs(values);
		});
		
	}
	void opIdentity() {
		bulkChangeWithHistory("set LT116 identity", [=](float *params) {
			for (int i = 0; i < 256; i++) {
				params[i] = ((i % 17) == 0)?1:0;
			}
		});
	}
	void opUpper() {
		bulkChangeWithHistory("set LT116 upper", [=](float *params) {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 16; j++) {
					params[i * 16 + j] = (i <=j )?1:0;
				}
			}
		});
	}
	void opLower() {
		bulkChangeWithHistory("set LT116 lower", [=](float *params) {
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 16; j++) {
					params[i * 16 + j] = (i >=j )?1:0;
				}
			}
		});
	}
	void copy() {
		clipboardUsed = true;
		getKnobs(clipboard);
		clipboardRow = clipboardColumn = -1;
	}
	void copyRow(int row) {
		clipboardUsed = true;
		getKnobs(clipboard);
		clipboardRow = row;
		clipboardColumn = -1;
	}
	void copyColumn(int column) {
		clipboardUsed = true;
		getKnobs(clipboard);
		clipboardRow = -1;
		clipboardColumn = column;
	}
	void copyCell(int row, int column) {
		clipboardUsed = true;
		getKnobs(clipboard);
		clipboardRow = row;
		clipboardColumn = column;
	}
	void pasteCell(int row, int column) {
		float bulkParams[256];
	        getKnobs(bulkParams);
		float oldValue = bulkParams[column + 16 * row];
		bulkParams[column + 16 * row] = clipboard[clipboardColumn + 16 * clipboardRow];
		float newValue = bulkParams[column + 16 * row];
		setKnobs(bulkParams);
		int64_t moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			"LT116 paste cell",
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(oldValue);
				}
			},
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(newValue);
				}
			}
		));	
	}
	void pasteRow(int row) {
		bulkChangeWithHistory("LT116 paste row", [=](float *params) {
			int sIndex = clipboardRow * 16;
			int dIndex = row * 16;
			memcpy(params + dIndex, clipboard + sIndex, sizeof(float) * 16);
		});
	}
	void pasteColumn(int column) {
		bulkChangeWithHistory("LT116 paste column", [=](float *params) {
			int sIndex = clipboardColumn;
			int dIndex = column;
			for (int i = 0; i < 16; i++) {
				params[dIndex] = clipboard[sIndex];
				sIndex += 16;
				dIndex += 16;
			}
		});
	}
	void pasteAll() {
		bulkChangeWithHistory("LT116 paste", clipboard);
	}
	void paste(int row, int column) {
		if (row > -1 && clipboardRow > -1) {
			if (column > -1 && clipboardColumn > -1) {
				pasteCell(row, column);
			}
			else {
				pasteRow(row);
			} 
		}
		else {
			if (column > -1 && clipboardColumn > -1) {
				pasteColumn(column);
			}
			else {
				pasteAll();
			}
		}
	}
	void pasteMultiplyCell(int row, int column) {
		float bulkParams[256];
	       	getKnobs(bulkParams);
		float oldValue = bulkParams[column + 16 * row];
		bulkParams[column + 16 * row] *= clipboard[clipboardColumn + 16 * clipboardRow];
		float newValue = bulkParams[column + 16 * row];
		setKnobs(bulkParams);
		int64_t moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			"LT116 paste multiply cell",
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(oldValue);
				}
			},
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(newValue);
				}
			}
		));	
	}
	void pasteMultiplyRow(int row) {
		bulkChangeWithHistory("LT116 paste multiply row", [=](float *params) {
			int sIndex = clipboardRow * 16;
			int dIndex = row * 16;
			for (int i = 0; i < 16; i++) {
				params[dIndex++] *= clipboard[sIndex++];
			}
		});
	}
	void pasteMultiplyColumn(int column) {
		bulkChangeWithHistory("LT116 paste multiply column", [=](float *params) {
			int sIndex = clipboardColumn;
			int dIndex = column;
			for (int i = 0; i < 16; i++) {
				params[dIndex] *= clipboard[sIndex];
				sIndex += 16;
				dIndex += 16;
			}
		});
	}
	void pasteMultiplyAll() {
		bulkChangeWithHistory("LT116 paste multiply", [=](float *params) {
			for (int i = 0; i < 256; i += 4) {
				__m128 d = _mm_load_ps(params + i);
				__m128 s = _mm_load_ps(clipboard + i);
				d = _mm_mul_ps(d, s);
				_mm_store_ps(params + i, d);
			}
		});
	}
	void pasteMultiply(int row, int column) {
		if (row > -1 && clipboardRow > -1) {
			if (column > -1 && clipboardColumn > -1) {
				pasteMultiplyCell(row, column);
			}
			else {
				pasteMultiplyRow(row);
			} 
		}
		else {
			if (column > -1 && clipboardColumn > -1) {
				pasteMultiplyColumn(column);
			}
			else {
				pasteMultiplyAll();
			}
		}
	}
	void pasteAddCell(int row, int column) {
		float bulkParams[256];
	       	getKnobs(bulkParams);
		float oldValue = bulkParams[column + 16 * row];
		bulkParams[column + 16 * row] += clipboard[clipboardColumn + 16 * clipboardRow];
		float newValue = bulkParams[column + 16 * row];
		setKnobs(bulkParams);
		int64_t moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			"LT116 paste add cell",
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(oldValue);
				}
			},
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(newValue);
				}
			}
		));	
	}
	void pasteAddRow(int row) {
		bulkChangeWithHistory("LT116 paste add row", [=](float *params) {
			int sIndex = clipboardRow * 16;
			int dIndex = row * 16;
			for (int i = 0; i < 16; i++) {
				params[dIndex++] += clipboard[sIndex++];
			}
		});
	}
	void pasteAddColumn(int column) {
		bulkChangeWithHistory("LT116 paste add column", [=](float *params) {
			int sIndex = clipboardColumn;
			int dIndex = column;
			for (int i = 0; i < 16; i++) {
				params[dIndex] += clipboard[sIndex];
				sIndex += 16;
				dIndex += 16;
			}
		});
	}
	void pasteAddAll() {
		bulkChangeWithHistory("LT116 paste add", [=](float *params) {
			for (int i = 0; i < 256; i += 4) {
				__m128 d = _mm_load_ps(params + i);
				__m128 s = _mm_load_ps(clipboard + i);
				d = _mm_add_ps(d, s);
				_mm_store_ps(params + i, d);
			}
		});
	}
	void pasteAdd(int row, int column) {
		if (row > -1 && clipboardRow > -1) {
			if (column > -1 && clipboardColumn > -1) {
				pasteAddCell(row, column);
			}
			else {
				pasteAddRow(row);
			} 
		}
		else {
			if (column > -1 && clipboardColumn > -1) {
				pasteAddColumn(column);
			}
			else {
				pasteAddAll();
			}
		}
	}
	void pasteSubtractCell(int row, int column) {
		float bulkParams[256];
		getKnobs(bulkParams);
		float oldValue = bulkParams[column + 16 * row];
		bulkParams[column + 16 * row] -= clipboard[clipboardColumn + 16 * clipboardRow];
		float newValue = bulkParams[column + 16 * row];
		setKnobs(bulkParams);
		int64_t moduleId = module->id;
		APP->history->push(new EventWidgetAction(
			"LT116 paste subtract cell",
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(oldValue);
				}
			},
			[=]() {
				LT116 *mw = dynamic_cast<LT116 *>(APP->scene->rack->getModule(moduleId));	
				if (mw) {
					mw->knobs[column + 16 * row]->getParamQuantity()->setValue(newValue);
				}
			}
		));	
	}
	void pasteSubtractRow(int row) {
		bulkChangeWithHistory("LT116 paste subtract row", [=](float *params) {
			int sIndex = clipboardRow * 16;
			int dIndex = row * 16;
			for (int i = 0; i < 16; i++) {
				params[dIndex++] -= clipboard[sIndex++];
			}
		});
	}
	void pasteSubtractColumn(int column) {
		bulkChangeWithHistory("LT116 paste subtract column", [=](float *params) {
			int sIndex = clipboardColumn;
			int dIndex = column;
			for (int i = 0; i < 16; i++) {
				params[dIndex] -= clipboard[sIndex];
				sIndex += 16;
				dIndex += 16;
			}
		});
	}
	void pasteSubtractAll() {
		bulkChangeWithHistory("LT116 paste subtract", [=](float *params) {
			for (int i = 0; i < 256; i += 4) {
				__m128 d = _mm_load_ps(params + i);
				__m128 s = _mm_load_ps(clipboard + i);
				d = _mm_sub_ps(d, s);
				_mm_store_ps(params + i, d);
			}
		});
	}
	void pasteSubtract(int row, int column) {
		if (row > -1 && clipboardRow > -1) {
			if (column > -1 && clipboardColumn > -1) {
				pasteSubtractCell(row, column);
			}
			else {
				pasteSubtractRow(row);
			} 
		}
		else {
			if (column > -1 && clipboardColumn > -1) {
				pasteSubtractColumn(column);
			}
			else {
				pasteSubtractAll();
			}
		}
	}
	void normalise() {
		bulkChangeWithHistory("LT116 normalise", [=](float *params) {
			alignas(16) float res[4];
			__m128 a = _mm_setzero_ps();
			for (int i = 0; i < 256; i += 4) {
				__m128 s = _mm_load_ps(params + i);
				a = _mm_add_ps(a, s);
			}
			_mm_store_ps(res, a);
			res[0] += (res[1] + res[2] + res[3]);
			a = _mm_set_ps1(res[0]);
			__m128 mask = _mm_cmpneq_ps(a, _mm_setzero_ps());
			a = _mm_rcp_ps(a);
			a = _mm_and_ps(a, mask);
			_mm_store_ps(res, a);
			for (int i = 0; i < 256; i += 4) {
				__m128 s = _mm_load_ps(params + i);
				s = _mm_mul_ps(s, a);
				_mm_store_ps(params + i, s);
			}
		});
	}
	void normalise(int column) {
		bulkChangeWithHistory("LT116 normalise column", [=](float *params) {
			float a = 0.0f;
			int index = column;
			for(int i = 0; i < 16; i++) {
				a += params[index];
				index += 16;
			}
			if (a)
				a = 1.0f/a;
			for(int i = 0; i < 16; i++) {
				index -= 16;
				params[index] *= a;
			}
		});
	}
	void normaliseAll() {
		bulkChangeWithHistory("LT116 normalise all columns", [=](float *params) {
			__m128 a1, a2, a3, a4 = _mm_setzero_ps();
			for (int i = 0; i < 256; i += 16) {
				__m128 s = _mm_load_ps(params + i);
				a1 = _mm_add_ps(a1, s);
				s = _mm_load_ps(params + i + 4);
				a2 = _mm_add_ps(a2, s);
				s = _mm_load_ps(params + i + 8);
				a3 = _mm_add_ps(a3, s);
				s = _mm_load_ps(params + i + 12);
				a4 = _mm_add_ps(a4, s);	
			}
			__m128 mask = _mm_cmpneq_ps(a1, _mm_setzero_ps());
			a1 = _mm_rcp_ps(a1);
			a1 = _mm_and_ps(a1, mask);
			mask = _mm_cmpneq_ps(a2, _mm_setzero_ps());
			a2 = _mm_rcp_ps(a2);
			a2 = _mm_and_ps(a2, mask);
			mask = _mm_cmpneq_ps(a3, _mm_setzero_ps());
			a3 = _mm_rcp_ps(a3);
			a3 = _mm_and_ps(a3, mask);
			mask = _mm_cmpneq_ps(a4, _mm_setzero_ps());
			a4 = _mm_rcp_ps(a4);
			a4 = _mm_and_ps(a4, mask);
			for (int i = 0; i < 256; i += 16) {
				__m128 s = _mm_load_ps(params + i);
				__m128 d = _mm_mul_ps(s, a1);
				_mm_store_ps(params + i, d);
				s = _mm_load_ps(params + i + 4);
				d = _mm_mul_ps(s, a2);
				_mm_store_ps(params + i + 4, d);
				s = _mm_load_ps(params + i + 8);
				d = _mm_mul_ps(s, a3);
				_mm_store_ps(params + i + 8, d);
				s = _mm_load_ps(params + i + 12);
				d = _mm_mul_ps(s, a4);
				_mm_store_ps(params + i + 12, d);
			}
		});
	}
};

Model *modelLT116 = createModel<LT_116, LT116>("LT-116");
