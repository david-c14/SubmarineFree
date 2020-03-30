//SubTag W2 W10

#include "SubmarineFree.hpp"
#include "torpedo.hpp"
#include <fstream>
#include <cctype>

struct WK_101;

namespace {

	struct WK_Tuning {
		std::string name;
		float offsets[12];
	};

	std::vector<WK_Tuning> tunings;

	int tuningsLoaded = false;

	struct WK_Tunings {
		static void loadTuningsFromWK(const char *path);
		static void loadTuningsFromScala(Plugin *pluginInstance);
		static void loadScalaFile(std::string path);
		static void loadTunings(Plugin *pluginInstance) {
			if (tuningsLoaded)
				return;
			tuningsLoaded = true;
			loadTuningsFromWK(asset::plugin(pluginInstance, "WK_Custom.tunings").c_str());
			loadTuningsFromScala(pluginInstance);
		}
	};

	void WK_Tunings::loadTuningsFromWK(const char *path) {
		FILE *file = fopen(path, "r");
		if (!file) {
			return;
		}
		int defaultSize = tunings.size();
		
		json_error_t error;
		json_t *rootJ = json_loadf(file, 0, &error);
		if (rootJ) {
			int size = json_array_size(rootJ);
			for (int i = 0; i < size; i++) {
				json_t *j0 = json_array_get(rootJ, i);
				if (j0) {
					json_t *jname = json_object_get(j0, "name");
					if (jname) {
						json_t *joffsets = json_object_get(j0, "tunings");
						if (joffsets) {
							tunings.push_back(WK_Tuning());
							tunings[i + defaultSize].name.assign(json_string_value(jname));
							int tsize = json_array_size(joffsets);
							for (int j = 0; j < 12; j++) {
								if (j < tsize) {
									json_t *joffset = json_array_get(joffsets, j);
									if (joffset) {
										tunings[i + defaultSize].offsets[j] = json_number_value(joffset);
									}
								}
								else {
									tunings[i + defaultSize].offsets[j] = 0.0f;
								}
							}
						}	
					}
				}
			}
			json_decref(rootJ);
		}
		else {
			std::string message = string::f("SubmarineFree WK: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
			WARN(message.c_str());
		}
		fclose(file);
	}

	void WK_Tunings::loadScalaFile(std::string path) {
		std::ifstream fs{path, std::ios_base::in};
		if (fs) {
			std::vector<std::string> strings;
			while (!fs.eof()) {
				std::string line;
				getline(fs, line);
				int iscomment = false;
				for (unsigned int i = 0; i < line.size(); i++) {
					if (std::isspace(line[i]))
						continue;
					if (line[i] == '!') {
						iscomment = true;
						break;
					}
				}
				if (iscomment)
					continue;
				strings.push_back(std::string(line));
				if (strings.size() >= 14)
					break;
			}
			fs.close();
			if (strings.size() < 2) return;
			WK_Tuning tuning = { "", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
			tuning.name.assign(strings[0]);
			for (unsigned int i = 2; i < strings.size(); i++) {
				// remove leading whitespace
				while (strings[i].size() && std::isspace(strings[i][0]))
					strings[i].erase(0,1);
				std::string line;
				int decimal = false;
				int ratio = false;
				while (strings[i].size() && !std::isspace(strings[i][0])) {
					char c = strings[i][0];
					line.append(1,c);
					strings[i].erase(0,1);
					if (!std::isdigit(c) && (c != '/') && (c != '.')) {
						WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
						return;
					}
					if (c == '.')
						decimal = true;
					if (c == '/' && !ratio)
						ratio = line.size();
					if (decimal && ratio) {
						WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
						return;
					}
				}
				if (decimal) {
					try {
						float d = std::stof(line, nullptr);
						d -= (i-1) * 100.0;
						if ((d < -50.0) || (d > 50.0)) {
							WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
							return;
						}
						tuning.offsets[(i-1)%12] = d;
					}
					catch (std::exception &err) {
						WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
						return;
					}
				}
				else {
					if (ratio) {
						std::string num = line.substr(0,ratio);
						std::string denom = line.substr(ratio);
						try {
							int inum = std::stoi(num,nullptr);
							int idenom = std::stoi(denom, nullptr);
							if (!idenom) {
								WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
								return;
							}
							float r = (1.0f * inum / idenom);  
							float d = 1200.0 * log2(r);
							d -= (i-1) * 100.0;
							if ((d < -50.0) || (d > 50.0)) {
								WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
								return;
							}
							tuning.offsets[(i-1)%12] = d;
						}
						catch (std::exception &err) {
							WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
							return;
						}
					}
					else {
						try {
							int inum = std::stoi(line, nullptr);
							float d = 1200.0 * log2(inum);
							d -= (i-1) * 100.0;
							if ((d < -50.0) || (d > 50.0)) {
								WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
								return;
							}
							tuning.offsets[(i-1)%12] = d;
						}
						catch (std::exception &err) {
							WARN("SubmarineFree WK: Scala file format error in %s", string::filename(path).c_str());
							return;
						}
					}
				}
			}
			int index = tunings.size();
			tunings.push_back(WK_Tuning());
			tunings[index].name = tuning.name;
			for (int i = 0; i < 12; i++)
				tunings[index].offsets[i] = tuning.offsets[i];
		}
	
	}

	void WK_Tunings::loadTuningsFromScala(Plugin *pluginInstance) {
		std::list<std::string> dirList = system::getEntries(asset::plugin(pluginInstance, "Scala"));
		for (auto entry : dirList) {
			if (system::isDirectory(entry)) continue;
			if (string::lowercase(string::filenameExtension(entry)).compare("scl")) continue;
			loadScalaFile(entry);
		}
	}

	struct WK101_InputPort : Torpedo::PatchInputPort {
		WK_101 *wkModule;
		WK101_InputPort(WK_101 *module, unsigned int portNum):PatchInputPort((Module *)module, portNum) { wkModule = module;};
		void received(std::string pluginName, std::string moduleName, json_t *rootJ) override;
	};

} // end namespace

struct WK_101 : Module {
	enum ParamIds {
		PARAM_1,
		PARAM_2,
		PARAM_3,
		PARAM_4,
		PARAM_5,
		PARAM_6,
		PARAM_7,
		PARAM_8,
		PARAM_9,
		PARAM_10,
		PARAM_11,
		PARAM_12,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_CV,
		INPUT_TOR,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_CV,
		OUTPUT_TOR,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	int toSend = 0;
	unsigned int light = PARAM_1;
	Torpedo::PatchOutputPort outPort = Torpedo::PatchOutputPort(this, OUTPUT_TOR);
	WK101_InputPort inPort = WK101_InputPort(this, INPUT_TOR);

	WK_101() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (unsigned int i = 0; i < NUM_PARAMS; i++) {
			configParam(PARAM_1 + i, -50.0f, 50.0f, 0.0f, "Detune", " cents");
		}
		outPort.size(5);
	}

	void process(const ProcessArgs &args) override {
		int quantized = floor((12.0f * inputs[INPUT_CV].getVoltage()) + 0.5f);
		int note = (120 + quantized) % 12;
		outputs[OUTPUT_CV].setVoltage((params[PARAM_1 + note].getValue() / 1200.0f) + (quantized / 12.0f));	
		light = note;
		if (toSend && !outPort.isBusy()) {
			toSend = 0;
			json_t *rootJ = json_array();
			for (int i = 0; i < 12; i++)
				json_array_append_new(rootJ, json_real(params[PARAM_1 + i].getValue()));
			outPort.send(std::string(TOSTRING(SLUG)), std::string("WK"), rootJ);
		}
		outPort.process();
		inPort.process();
	}
};

namespace {

	void WK101_InputPort::received(std::string pluginName, std::string moduleName, json_t *rootJ) {
		if (pluginName.compare(TOSTRING(SLUG))) return;
		if (moduleName.compare("WK")) return;
		float tunings[12];
		int size = json_array_size(rootJ);
		if (!size) return;
		if (size > 12)
			size = 12;
		for (int i = 0; i < size; i++) {
			json_t *j1 = json_array_get(rootJ, i);
			if (j1)
				tunings[i] = json_number_value(j1);
		}
		for (int i = 0; i < 12; i++)
			wkModule->params[WK_101::PARAM_1 + i].setValue(tunings[i]);
	}
	
	struct WK_Display : TransparentWidget {
		WK_101 *module;
		int index;
		char dspText[20];
		
		void draw(const DrawArgs &args) override {
			if (!module) {
				return;
			}
			float val = APP->engine->getParam(module, WK_101::PARAM_1 + index);
			sprintf(dspText, "%+05.2f", val);
			nvgFontSize(args.vg, 14);
			nvgFontFaceId(args.vg, gScheme.font()->handle);
			nvgFillColor(args.vg, SUBLIGHTBLUE);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
			nvgText(args.vg, 30, 13, dspText, NULL);
		}
	};
	
	struct WK_Param : MedKnob<LightKnob> {
		WK_101 *module;
		unsigned int index;
		void step() override {
			if (module) {
				color = (index == module->light)?SUBLIGHTRED:SUBLIGHTBLUE;
			}
			MedKnob::step();
		}
		void onChange(const event::Change &e) override {
			MedKnob<LightKnob>::onChange(e);
			
			if (module) {
				module->toSend = true;
			}
		}
	};

} // end namespace

struct WK101 : SchemeModuleWidget {
	WK101(WK_101 *module) {
		setModule(module);
		this->box.size = Vec(150, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<SilverPort>(Vec(16.5,41.5), module, WK_101::INPUT_CV));
		addOutput(createOutputCentered<SilverPort>(Vec(55.5,41.5), module, WK_101::OUTPUT_CV));
		addInput(createInputCentered<BlackPort>(Vec(94.5,41.5), module, WK_101::INPUT_TOR));
		addOutput(createOutputCentered<BlackPort>(Vec(133.5,41.5), module, WK_101::OUTPUT_TOR));

		for (int i = 0; i < 5; i++)
		{
			WK_Display *display = new WK_Display();
			display->module = module;
			display->index = i;
			display->box.pos = Vec(45, 79 + 21 * i);
			display->box.size = Vec(60, 20);
			addChild(display);
			WK_Param *widget = createParamCentered<WK_Param>(Vec(23 + 104 * (i%2),89 + 21 * i), module, WK_101::PARAM_1 + i);
			widget->module = module;
			widget->index = i;
			addParam(widget);
		}
		for (int i = 5; i < 12; i++)
		{
			WK_Display *display = new WK_Display();
			display->module = module;
			display->index = i;
			display->box.pos = Vec(45, 100 + 21 * i);
			display->box.size = Vec(60, 20);
			addChild(display);
			WK_Param *widget = createParamCentered<WK_Param>(Vec(127 - 104 * (i%2),110 + 21 * i), module, WK_101::PARAM_1 + i);
			widget->module = module;
			widget->index = i;
			addParam(widget);
		}
		WK_Tunings::loadTunings(pluginInstance);
	}
	void appendContextMenu(Menu *menu) override;

	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "WK-101");
		nvgFillColor(vg, gScheme.getAlternative(module));
		nvgStrokeColor(vg, gScheme.getAlternative(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, 1);
		nvgLineJoin(vg, 1);
		nvgBeginPath(vg);

		nvgMoveTo(vg, 3.500000, 360.500000);
		nvgBezierTo(vg, 51.166668, 360.500000, 98.833328, 360.500000, 146.500000, 360.500000);
		nvgBezierTo(vg, 146.500000, 353.833344, 146.500000, 347.166656, 146.500000, 340.500000);
		nvgBezierTo(vg, 133.166672, 340.500000, 119.833336, 340.500000, 106.500000, 340.500000);
		nvgBezierTo(vg, 106.234787, 340.500000, 105.980431, 340.394623, 105.792892, 340.207092);
		nvgBezierTo(vg, 105.605354, 340.019562, 105.500000, 339.765228, 105.500000, 339.500000);
		nvgBezierTo(vg, 105.500000, 336.500000, 105.500000, 333.500000, 105.500000, 330.500000);
		nvgBezierTo(vg, 85.166664, 330.500000, 64.833336, 330.500000, 44.500000, 330.500000);
		nvgBezierTo(vg, 44.234783, 330.500000, 43.980431, 330.394623, 43.792892, 330.207092);
		nvgBezierTo(vg, 43.605354, 330.019562, 43.500000, 329.765228, 43.500000, 329.500000);
		nvgBezierTo(vg, 43.500000, 326.500000, 43.500000, 323.500000, 43.500000, 320.500000);
		nvgBezierTo(vg, 30.166668, 320.500000, 16.833332, 320.500000, 3.500000, 320.500000);
		nvgBezierTo(vg, 3.500000, 333.833344, 3.500000, 347.166656, 3.500000, 360.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 318.500000);
		nvgBezierTo(vg, 16.833332, 318.500000, 30.166668, 318.500000, 43.500000, 318.500000);
		nvgBezierTo(vg, 43.500000, 315.500000, 43.500000, 312.500000, 43.500000, 309.500000);
		nvgBezierTo(vg, 43.500000, 309.234772, 43.605354, 308.980438, 43.792892, 308.792908);
		nvgBezierTo(vg, 43.980431, 308.605377, 44.234783, 308.500000, 44.500000, 308.500000);
		nvgBezierTo(vg, 64.833336, 308.500000, 85.166664, 308.500000, 105.500000, 308.500000);
		nvgBezierTo(vg, 105.500000, 305.500000, 105.500000, 302.500000, 105.500000, 299.500000);
		nvgBezierTo(vg, 105.500000, 299.234772, 105.605354, 298.980438, 105.792892, 298.792908);
		nvgBezierTo(vg, 105.980431, 298.605377, 106.234787, 298.500000, 106.500000, 298.500000);
		nvgBezierTo(vg, 119.833336, 298.500000, 133.166672, 298.500000, 146.500000, 298.500000);
		nvgBezierTo(vg, 133.166672, 298.500000, 119.833336, 298.500000, 106.500000, 298.500000);
		nvgBezierTo(vg, 106.234787, 298.500000, 105.980431, 298.394623, 105.792892, 298.207092);
		nvgBezierTo(vg, 105.605354, 298.019562, 105.500000, 297.765228, 105.500000, 297.500000);
		nvgBezierTo(vg, 105.500000, 294.500000, 105.500000, 291.500000, 105.500000, 288.500000);
		nvgBezierTo(vg, 85.166664, 288.500000, 64.833336, 288.500000, 44.500000, 288.500000);
		nvgBezierTo(vg, 44.234783, 288.500000, 43.980431, 288.394623, 43.792892, 288.207092);
		nvgBezierTo(vg, 43.605354, 288.019562, 43.500000, 287.765228, 43.500000, 287.500000);
		nvgBezierTo(vg, 43.500000, 284.500000, 43.500000, 281.500000, 43.500000, 278.500000);
		nvgBezierTo(vg, 30.166668, 278.500000, 16.833332, 278.500000, 3.500000, 278.500000);
		nvgBezierTo(vg, 3.500000, 291.833344, 3.500000, 305.166656, 3.500000, 318.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 276.500000);
		nvgBezierTo(vg, 16.833332, 276.500000, 30.166668, 276.500000, 43.500000, 276.500000);
		nvgBezierTo(vg, 43.500000, 273.500000, 43.500000, 270.500000, 43.500000, 267.500000);
		nvgBezierTo(vg, 43.500000, 267.234772, 43.605354, 266.980438, 43.792892, 266.792908);
		nvgBezierTo(vg, 43.980431, 266.605377, 44.234783, 266.500000, 44.500000, 266.500000);
		nvgBezierTo(vg, 64.833336, 266.500000, 85.166664, 266.500000, 105.500000, 266.500000);
		nvgBezierTo(vg, 105.500000, 263.500000, 105.500000, 260.500000, 105.500000, 257.500000);
		nvgBezierTo(vg, 105.500000, 257.234772, 105.605354, 256.980438, 105.792892, 256.792908);
		nvgBezierTo(vg, 105.980431, 256.605377, 106.234787, 256.500000, 106.500000, 256.500000);
		nvgBezierTo(vg, 119.833336, 256.500000, 133.166672, 256.500000, 146.500000, 256.500000);
		nvgBezierTo(vg, 133.166672, 256.500000, 119.833336, 256.500000, 106.500000, 256.500000);
		nvgBezierTo(vg, 106.234787, 256.500000, 105.980431, 256.394623, 105.792892, 256.207092);
		nvgBezierTo(vg, 105.605354, 256.019562, 105.500000, 255.765213, 105.500000, 255.500000);
		nvgBezierTo(vg, 105.500000, 252.500000, 105.500000, 249.500000, 105.500000, 246.500000);
		nvgBezierTo(vg, 85.166664, 246.500000, 64.833336, 246.500000, 44.500000, 246.500000);
		nvgBezierTo(vg, 44.234783, 246.500000, 43.980431, 246.394638, 43.792892, 246.207108);
		nvgBezierTo(vg, 43.605354, 246.019577, 43.500000, 245.765213, 43.500000, 245.500000);
		nvgBezierTo(vg, 43.500000, 242.500000, 43.500000, 239.500000, 43.500000, 236.500000);
		nvgBezierTo(vg, 30.166668, 236.500000, 16.833332, 236.500000, 3.500000, 236.500000);
		nvgBezierTo(vg, 3.500000, 249.833328, 3.500000, 263.166656, 3.500000, 276.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 234.500000);
		nvgBezierTo(vg, 16.833332, 234.500000, 30.166668, 234.500000, 43.500000, 234.500000);
		nvgBezierTo(vg, 43.500000, 231.500000, 43.500000, 228.500000, 43.500000, 225.500000);
		nvgBezierTo(vg, 43.500000, 225.234787, 43.605354, 224.980423, 43.792892, 224.792892);
		nvgBezierTo(vg, 43.980431, 224.605362, 44.234783, 224.500000, 44.500000, 224.500000);
		nvgBezierTo(vg, 64.833336, 224.500000, 85.166664, 224.500000, 105.500000, 224.500000);
		nvgBezierTo(vg, 105.500000, 221.500000, 105.500000, 218.500000, 105.500000, 215.500000);
		nvgBezierTo(vg, 105.500000, 215.234787, 105.605354, 214.980423, 105.792892, 214.792892);
		nvgBezierTo(vg, 105.980431, 214.605362, 106.234787, 214.500000, 106.500000, 214.500000);
		nvgBezierTo(vg, 119.833336, 214.500000, 133.166672, 214.500000, 146.500000, 214.500000);
		nvgBezierTo(vg, 146.500000, 207.833328, 146.500000, 201.166672, 146.500000, 194.500000);
		nvgBezierTo(vg, 98.833328, 194.500000, 51.166668, 194.500000, 3.500000, 194.500000);
		nvgBezierTo(vg, 3.500000, 207.833328, 3.500000, 221.166672, 3.500000, 234.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 192.500000);
		nvgBezierTo(vg, 51.166668, 192.500000, 98.833328, 192.500000, 146.500000, 192.500000);
		nvgBezierTo(vg, 146.500000, 185.833328, 146.500000, 179.166672, 146.500000, 172.500000);
		nvgBezierTo(vg, 133.166672, 172.500000, 119.833336, 172.500000, 106.500000, 172.500000);
		nvgBezierTo(vg, 106.234787, 172.500000, 105.980431, 172.394638, 105.792892, 172.207108);
		nvgBezierTo(vg, 105.605354, 172.019577, 105.500000, 171.765213, 105.500000, 171.500000);
		nvgBezierTo(vg, 105.500000, 168.500000, 105.500000, 165.500000, 105.500000, 162.500000);
		nvgBezierTo(vg, 85.166664, 162.500000, 64.833336, 162.500000, 44.500000, 162.500000);
		nvgBezierTo(vg, 44.234783, 162.500000, 43.980431, 162.394638, 43.792892, 162.207108);
		nvgBezierTo(vg, 43.605354, 162.019577, 43.500000, 161.765213, 43.500000, 161.500000);
		nvgBezierTo(vg, 43.500000, 158.500000, 43.500000, 155.500000, 43.500000, 152.500000);
		nvgBezierTo(vg, 30.166668, 152.500000, 16.833332, 152.500000, 3.500000, 152.500000);
		nvgBezierTo(vg, 3.500000, 165.833328, 3.500000, 179.166672, 3.500000, 192.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 150.500000);
		nvgBezierTo(vg, 16.833332, 150.500000, 30.166668, 150.500000, 43.500000, 150.500000);
		nvgBezierTo(vg, 43.500000, 147.500000, 43.500000, 144.500000, 43.500000, 141.500000);
		nvgBezierTo(vg, 43.500000, 141.234787, 43.605354, 140.980423, 43.792892, 140.792892);
		nvgBezierTo(vg, 43.980431, 140.605362, 44.234783, 140.500000, 44.500000, 140.500000);
		nvgBezierTo(vg, 64.833336, 140.500000, 85.166664, 140.500000, 105.500000, 140.500000);
		nvgBezierTo(vg, 105.500000, 137.500000, 105.500000, 134.500000, 105.500000, 131.500000);
		nvgBezierTo(vg, 105.500000, 131.234787, 105.605354, 130.980423, 105.792892, 130.792892);
		nvgBezierTo(vg, 105.980431, 130.605362, 106.234787, 130.500000, 106.500000, 130.500000);
		nvgBezierTo(vg, 119.833336, 130.500000, 133.166672, 130.500000, 146.500000, 130.500000);
		nvgBezierTo(vg, 133.166672, 130.500000, 119.833336, 130.500000, 106.500000, 130.500000);
		nvgBezierTo(vg, 106.234787, 130.500000, 105.980431, 130.394638, 105.792892, 130.207108);
		nvgBezierTo(vg, 105.605354, 130.019577, 105.500000, 129.765213, 105.500000, 129.500000);
		nvgBezierTo(vg, 105.500000, 126.500000, 105.500000, 123.500000, 105.500000, 120.500000);
		nvgBezierTo(vg, 85.166664, 120.500000, 64.833336, 120.500000, 44.500000, 120.500000);
		nvgBezierTo(vg, 44.234783, 120.500000, 43.980431, 120.394646, 43.792892, 120.207108);
		nvgBezierTo(vg, 43.605354, 120.019569, 43.500000, 119.765213, 43.500000, 119.500000);
		nvgBezierTo(vg, 43.500000, 116.500000, 43.500000, 113.500000, 43.500000, 110.500000);
		nvgBezierTo(vg, 30.166668, 110.500000, 16.833332, 110.500000, 3.500000, 110.500000);
		nvgBezierTo(vg, 3.500000, 123.833336, 3.500000, 137.166672, 3.500000, 150.500000);
		nvgClosePath(vg);
		
		nvgMoveTo(vg, 3.500000, 108.500000);
		nvgBezierTo(vg, 16.833332, 108.500000, 30.166668, 108.500000, 43.500000, 108.500000);
		nvgBezierTo(vg, 43.500000, 105.500000, 43.500000, 102.500000, 43.500000, 99.500000);
		nvgBezierTo(vg, 43.500000, 99.234787, 43.605354, 98.980431, 43.792892, 98.792892);
		nvgBezierTo(vg, 43.980431, 98.605354, 44.234783, 98.500000, 44.500000, 98.500000);
		nvgBezierTo(vg, 64.833336, 98.500000, 85.166664, 98.500000, 105.500000, 98.500000);
		nvgBezierTo(vg, 105.500000, 95.500000, 105.500000, 92.500000, 105.500000, 89.500000);
		nvgBezierTo(vg, 105.500000, 89.234787, 105.605354, 88.980431, 105.792892, 88.792892);
		nvgBezierTo(vg, 105.980431, 88.605354, 106.234787, 88.500000, 106.500000, 88.500000);
		nvgBezierTo(vg, 119.833336, 88.500000, 133.166672, 88.500000, 146.500000, 88.500000);
		nvgBezierTo(vg, 146.500000, 81.833336, 146.500000, 75.166664, 146.500000, 68.500000);
		nvgBezierTo(vg, 98.833328, 68.500000, 51.166668, 68.500000, 3.500000, 68.500000);
		nvgBezierTo(vg, 3.500000, 81.833336, 3.500000, 95.166664, 3.500000, 108.500000);
		nvgClosePath(vg);
		
		nvgFill(vg);
		nvgStroke(vg);

		nvgFillColor(vg, nvgRGB(0x00, 0x00, 0x00));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, 50, 80, 50, 17, 4);
		nvgRoundedRect(vg, 50, 101, 50, 17, 4);
		nvgRoundedRect(vg, 50, 122, 50, 17, 4);
		nvgRoundedRect(vg, 50, 143, 50, 17, 4);
		nvgRoundedRect(vg, 50, 164, 50, 17, 4);
		nvgRoundedRect(vg, 50, 206, 50, 17, 4);
		nvgRoundedRect(vg, 50, 227, 50, 17, 4);
		nvgRoundedRect(vg, 50, 248, 50, 17, 4);
		nvgRoundedRect(vg, 50, 269, 50, 17, 4);
		nvgRoundedRect(vg, 50, 290, 50, 17, 4);
		nvgRoundedRect(vg, 50, 311, 50, 17, 4);
		nvgRoundedRect(vg, 50, 332, 50, 17, 4);
		nvgFill(vg);

		drawText(vg, 36, 25, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "V/OCT");
		drawText(vg, 114, 25, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SYNC");
		drawText(vg, 16.5, 61, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "IN");
		drawText(vg, 55.5, 61, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT");
		drawText(vg, 94.5, 61, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "IN");
		drawText(vg, 133.5, 61, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "OUT");
	}
};

void WK101::appendContextMenu(Menu *menu) {
	SchemeModuleWidget::appendContextMenu(menu);
	WK_101 *module = dynamic_cast<WK_101 *>(this->module);
	if (module) {
		menu->addChild(new MenuEntry);
		for (unsigned int i = 0; i < tunings.size(); i++) { 
			EventWidgetMenuItem *m = createMenuItem<EventWidgetMenuItem>(tunings[i].name.c_str());
			m->clickHandler = [=]() {
				for (int j = 0; j < 12; j++) {
					APP->engine->setParam(module, WK_101::PARAM_1 + j, tunings[i].offsets[j]);
				}
				module->toSend = true;
			};
			menu->addChild(m);
		}
	}
}

struct WK_205;

namespace {

	struct WK205_InputPort : Torpedo::PatchInputPort {
		WK_205 *wkModule;
		WK205_InputPort(WK_205 *module, unsigned int portNum):PatchInputPort((Module *)module, portNum) { wkModule = module;};
		void received(std::string pluginName, std::string moduleName, json_t *rootJ) override;
	};

} // end namespace

struct WK_205 : Module {
	static const int deviceCount = 5;
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_CV_1,
		INPUT_CV_2,
		INPUT_CV_3,
		INPUT_CV_4,
		INPUT_CV_5,
		INPUT_TOR,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_CV_1,
		OUTPUT_CV_2,
		OUTPUT_CV_3,
		OUTPUT_CV_4,
		OUTPUT_CV_5,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
	float tunings[12];
	WK205_InputPort inPort = WK205_InputPort(this, INPUT_TOR);

	WK_205() : Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}
	void process(const ProcessArgs &args) override;
	json_t *dataToJson(void) override {
		json_t *rootJ = json_array();
		for (int i = 0; i < 12; i++)
			json_array_append_new(rootJ, json_real(tunings[i]));
		return rootJ;
	}
	void dataFromJson(json_t *rootJ) override {
		int size = json_array_size(rootJ);
		if (!size) return;
		if (size > 12)
			size = 12;
		for (int i = 0; i < size; i++) {
			json_t *j1 = json_array_get(rootJ, i);
			if (j1)
				tunings[i] = json_number_value(j1);
		}
	}
};

void WK_205::process(const ProcessArgs &args) {
	for (int i = 0; i < deviceCount; i++) {
		int quantized = floor((12.0f * inputs[INPUT_CV_1 + i].getVoltage()) + 0.5f);
		int note = (120 + quantized) % 12;
		outputs[OUTPUT_CV_1 + i].setVoltage((tunings[note] / 1200.0f) + (quantized / 12.0f));	
	}
	inPort.process();
}

namespace {
	void WK205_InputPort::received(std::string pluginName, std::string moduleName, json_t *rootJ) {
		if (pluginName.compare(TOSTRING(SLUG))) return;
		if (moduleName.compare("WK")) return;
		int size = json_array_size(rootJ);
		if (!size) return;
		if (size > 12)
			size = 12;
		for (int i = 0; i < size; i++) {
			json_t *j1 = json_array_get(rootJ, i);
			if (j1)
				wkModule->tunings[i] = json_number_value(j1);
		}
	}
} // end namespace

struct WK205 : SchemeModuleWidget {
	WK205(WK_205 *module) {
		setModule(module);
		this->box.size = Vec(30, 380);
		addChild(new SchemePanel(this->box.size));

		addInput(createInputCentered<BlackPort>(Vec(15,31.5), module, WK_205::INPUT_TOR));
		for (int i = 0; i < WK_205::deviceCount; i++) {
			addInput(createInputCentered<SilverPort>(Vec(15,75.5 + i * 60), module, WK_205::INPUT_CV_1 + i));
			addOutput(createOutputCentered<SilverPort>(Vec(15,104.5 + i * 60), module, WK_205::OUTPUT_CV_1 + i));
		}

		WK_Tunings::loadTunings(pluginInstance);
	}
	void appendContextMenu(Menu *menu) override {
		SchemeModuleWidget::appendContextMenu(menu);
		WK_205 *module = dynamic_cast<WK_205 *>(this->module);
		if (module) {
			menu->addChild(new MenuEntry);
			for (unsigned int i = 0; i < tunings.size(); i++) { 
				EventWidgetMenuItem *m = createMenuItem<EventWidgetMenuItem>(tunings[i].name.c_str());
				m->clickHandler = [=]() {
					for (int j = 0; j < 12; j++) {
						module->tunings[j] = tunings[i].offsets[j];
					}
				};
				menu->addChild(m);
			}
		}
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
		drawBase(vg, "WK-205");
		drawText(vg, 15, 52, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.getContrast(module), "SYNC");
		nvgStrokeColor(vg, gScheme.getContrast(module));
		nvgStrokeWidth(vg, 1);
		nvgLineCap(vg, NVG_ROUND);
		nvgBeginPath(vg);
		nvgMoveTo(vg, 15, 75.5);
		nvgLineTo(vg, 15, 104.5);
		nvgMoveTo(vg, 15, 135.5);
		nvgLineTo(vg, 15, 164.5);
		nvgMoveTo(vg, 15, 195.5);
		nvgLineTo(vg, 15, 224.5);
		nvgMoveTo(vg, 15, 255.5);
		nvgLineTo(vg, 15, 284.5);
		nvgMoveTo(vg, 15, 315.5);
		nvgLineTo(vg, 15, 344.5);
		nvgStroke(vg);
	}
};

Model *modelWK101 = createModel<WK_101, WK101>("WK-101");
Model *modelWK205 = createModel<WK_205, WK205>("WK-205");
