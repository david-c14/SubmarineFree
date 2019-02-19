/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#include "../SubmarineFree.hpp"

Scheme::Scheme() {
	json_error_t error;
	FILE *file = fopen(assetLocal("SubmarineFree/Settings.json").c_str(), "r");
	if (!file) {
		return;
	}
	json_t *rootJ = json_loadf(file, 0, &error);
	fclose(file);
	if (!rootJ) {
		std::string message = stringf("Submarine Free Settings: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
		warn(message.c_str());
		return;
	}
	json_t *h1 = json_object_get(rootJ, "flat");
	if (h1) {
		isFlat = (json_number_value(h1) != 0);
	}
	json_t *c1 = json_object_get(rootJ, "scheme");
	if (c1) {
		scheme = json_number_value(c1);
		setColors();
	}
	json_decref(rootJ);
}

void Scheme::save() {
	json_t *settings = json_object();
	json_object_set_new(settings, "flat", json_real(isFlat?1:0));
	json_object_set_new(settings, "scheme", json_real(scheme));

	systemCreateDirectory(assetLocal("SubmarineFree"));
	std::string settingsFilename = assetLocal("SubmarineFree/Settings.json");
	FILE *file = fopen(settingsFilename.c_str(), "w");
	if (file) {
		json_dumpf(settings, file, JSON_INDENT(2) | JSON_REAL_PRECISION(9));
		fclose(file);
	}
	json_decref(settings);
}

void Scheme::setColors() {
	switch (scheme) {
		case Dark:
			background = nvgRGB(0x33, 0x33, 0x33);
			alternative = nvgRGB(0x18, 0x2d, 0x44);
			contrast = nvgRGB(0xff, 0xff, 0xff);
			highlight = nvgRGB(0x55, 0x55, 0x55);
			shadow = nvgRGB(0x00, 0x00, 0x00);
			break;
		case Light:
			background = nvgRGB(0xcc, 0xcc, 0xcc);
			alternative = nvgRGB(0x18, 0x2d, 0x44);
			contrast = nvgRGB(0x00, 0x00, 0x00);
			highlight = nvgRGB(0xff, 0xff, 0xff);
			shadow = nvgRGB(0xaa, 0xaa, 0xaa);
			break;
		default:
			background = nvgRGB(0x29, 0x4f, 0x77);
			alternative = nvgRGB(0x71, 0x9f, 0xcf);
			contrast = nvgRGB(0xff, 0xff, 0xff);
			highlight = nvgRGB(0x3a, 0x6e, 0xa5);
			shadow = nvgRGB(0x18, 0x2d, 0x44);
			break;
	}
	
}
	
std::shared_ptr<Font> Scheme::font() {
	if (!_fontLoaded) {
		_fontLoaded = true;
		_font = Font::load(assetGlobal("res/fonts/DejaVuSans.ttf"));	
	}
	return _font;
}

int Scheme::font(NVGcontext *vg) {
	int ret = nvgFindFont(vg, "subDejaVu");
	if (ret != -1) {
		return ret;
	}
	info("Loading font for subDejaVu");
	return nvgCreateFont(vg, "subDejaVu", assetGlobal("res/fonts/DejaVuSans.ttf").c_str());
}

Scheme gScheme;

SchemePanel::SchemePanel() {
	isFlat = gScheme.isFlat;
	scheme = gScheme.scheme;
	SchemeCanvasWidget *canvas = new SchemeCanvasWidget();
	addChild(canvas);
}

SchemePanel::SchemePanel(Vec size) {
	isFlat = gScheme.isFlat;
	scheme = gScheme.scheme;
	SchemeCanvasWidget *canvas = new SchemeCanvasWidget();
	addChild(canvas);
	canvas->box.size = size;
	box.size = size;
}

void SchemePanel::step() {
	if (isFlat != gScheme.isFlat) {
		dirty = true;
	}
	isFlat = gScheme.isFlat;
	if (scheme != gScheme.scheme) {
		dirty = true;
	}
	scheme = gScheme.scheme;
	oversample = 2.0;
	FramebufferWidget::step();
}

void SchemeCanvasWidget::draw(NVGcontext *vg) {
	SchemeModuleWidget *smw = dynamic_cast<SchemeModuleWidget *>(parent->parent);
	nvgSave(vg);
	smw->render(vg, this);
	nvgRestore(vg);
	// Standard Panel Border
	NVGcolor borderColor = nvgRGBAf(0.5, 0.5, 0.5, 0.5);
	nvgBeginPath(vg);
	nvgRect(vg, 0.5, 0.5, box.size.x - 1.0, box.size.y - 1.0);
	nvgStrokeColor(vg, borderColor);
	nvgStrokeWidth(vg, 1.0);
	nvgStroke(vg);
	Widget::draw(vg);
}

struct SchemeModuleWidgetSchemeMenuItem : MenuItem {
	int scheme;
	void onAction(EventAction &e) override {
		gScheme.scheme = scheme;
		gScheme.setColors();
		gScheme.save();
	}
	void step() override {
		rightText = CHECKMARK(scheme == gScheme.scheme);
		MenuItem::step();
	}
};

struct SchemeModuleWidgetFlatMenuItem : MenuItem {
	void onAction(EventAction &e) override {
		gScheme.isFlat = !gScheme.isFlat;
		gScheme.save();
	}
	void step() override {
		rightText = CHECKMARK(gScheme.isFlat);
		MenuItem::step();
	}
};

struct SchemeModuleWidgetVisualMenuItem : MenuItem {
	Menu *createChildMenu() override {
		Menu *menu = new Menu();
		SchemeModuleWidgetFlatMenuItem *fmi = MenuItem::create<SchemeModuleWidgetFlatMenuItem>("Flat");
		menu->addChild(fmi);
		menu->addChild(MenuSeparator::create<MenuSeparator>());
		SchemeModuleWidgetSchemeMenuItem *vmi = MenuItem::create<SchemeModuleWidgetSchemeMenuItem>("Blue");
		vmi->scheme = 0;
		menu->addChild(vmi);
		vmi = MenuItem::create<SchemeModuleWidgetSchemeMenuItem>("Dark");
		vmi->scheme = 1;
		menu->addChild(vmi);
		vmi = MenuItem::create<SchemeModuleWidgetSchemeMenuItem>("Light");
		vmi->scheme = 2;
		menu->addChild(vmi);
		return menu;
	}
};

void SchemeModuleWidget::drawBackground(NVGcontext *vg) {
	if (gScheme.isFlat) {
		nvgBeginPath(vg);
		nvgRect(vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(vg, gScheme.background);
		nvgFill(vg);	
	}
	else {
		nvgBeginPath(vg);
		nvgMoveTo(vg, 0, 0);
		nvgLineTo(vg, box.size.x, 0);
		nvgLineTo(vg, 0, box.size.y);
		nvgClosePath(vg);
		nvgFillColor(vg, gScheme.highlight);
		nvgFill(vg);
		nvgBeginPath(vg);
		nvgMoveTo(vg, box.size.x, 0);
		nvgLineTo(vg, box.size.x, box.size.y);
		nvgLineTo(vg, 0, box.size.y);
		nvgClosePath(vg);
		nvgFillColor(vg, gScheme.shadow);
		nvgFill(vg);
		nvgBeginPath(vg);
		nvgRect(vg, 1, 1, box.size.x - 2, box.size.y - 2);
		nvgFillColor(vg, gScheme.background);
		nvgFill(vg);	
	}
}

void SchemeModuleWidget::drawLogo(NVGcontext *vg, float left, float top, float scale, float rotate) {
	nvgSave(vg);
	nvgTranslate(vg, left, top);
	nvgRotate(vg, rotate);
	nvgScale(vg, scale, scale);
	nvgBeginPath(vg);
	nvgMoveTo(vg, 12.870770, 7.633020);
	nvgBezierTo(vg, 13.115595, 7.633030, 13.350389, 7.740895, 13.523494, 7.932883);
	nvgBezierTo(vg, 13.696599, 8.124872, 13.793836, 8.385256, 13.793811, 8.656750);
	nvgBezierTo(vg, 13.793811, 9.222018, 13.380578, 9.680299, 12.870804, 9.680340);
	nvgBezierTo(vg, 12.625945, 9.680368, 12.391138, 9.572539, 12.218008, 9.380577);
	nvgBezierTo(vg, 12.044878, 9.188615, 11.947609, 8.928245, 11.947600, 8.656750);
	nvgBezierTo(vg, 11.947600, 8.385126, 12.044878, 8.124732, 12.218029, 7.932758);
	nvgBezierTo(vg, 12.391180, 7.740785, 12.626017, 7.632964, 12.870864, 7.633019);
	nvgBezierTo(vg, 12.870832, 7.633019, 12.870802, 7.633020, 12.870770, 7.633020);
	nvgClosePath(vg);
	nvgPathWinding(vg, NVG_HOLE);
	nvgMoveTo(vg, 9.861490, 7.244100);
	nvgBezierTo(vg, 10.204232, 7.244098, 10.532936, 7.395082, 10.775291, 7.663837);
	nvgBezierTo(vg, 11.017647, 7.932592, 11.153801, 8.297103, 11.153800, 8.677180);
	nvgBezierTo(vg, 11.153800, 9.057261, 11.017645, 9.421771, 10.775290, 9.690525);
	nvgBezierTo(vg, 10.532934, 9.959279, 10.204229, 10.110263, 9.861487, 10.110260);
	nvgBezierTo(vg, 9.147770, 10.110258, 8.569188, 9.468646, 8.569189, 8.677180);
	nvgBezierTo(vg, 8.569189, 7.885711, 9.147773, 7.244101, 9.861492, 7.244100);
	nvgBezierTo(vg, 9.861491, 7.244100, 9.861491, 7.244100, 9.861490, 7.244100);
	nvgClosePath(vg);
	nvgPathWinding(vg, NVG_HOLE);
	nvgMoveTo(vg, 6.076840, 6.834610); 
	nvgBezierTo(vg, 6.517518, 6.834602, 6.940148, 7.028727, 7.251753, 7.374278);
	nvgBezierTo(vg, 7.563358, 7.719829, 7.738410, 8.188498, 7.738400, 8.677179);
	nvgBezierTo(vg, 7.738400, 9.165903, 7.563338, 9.634569, 7.251726, 9.980112);
	nvgBezierTo(vg, 6.940114, 10.325655, 6.517479, 10.519769, 6.076802, 10.519750);
	nvgBezierTo(vg, 5.159219, 10.519698, 4.415370, 9.694759, 4.415391, 8.677179);
	nvgBezierTo(vg, 4.415391, 7.659558, 5.159257, 6.834639, 6.076879, 6.834610);
	nvgBezierTo(vg, 6.076866, 6.834610, 6.076853, 6.834610, 6.076840, 6.834610);
	nvgClosePath(vg);
	nvgPathWinding(vg, NVG_HOLE);
	nvgMoveTo(vg, 9.353910, 1.000000); 
	nvgBezierTo(vg, 8.257845, 0.999974, 7.369288, 2.008191, 7.369260, 3.251920);
	nvgBezierTo(vg, 7.370142, 3.431143, 7.389879, 3.609628, 7.428055, 3.783565);
	nvgBezierTo(vg, 3.677499, 4.301315, 1.002539, 6.401614, 1.000000, 8.830709);
	nvgBezierTo(vg, 1.000000, 11.685596, 4.657422, 13.999968, 9.169113, 14.000000);
	nvgBezierTo(vg, 12.273875, 13.997378, 15.107895, 12.881325, 16.482780, 11.119850);
	nvgBezierTo(vg, 17.050028, 12.281209, 17.992386, 12.976240, 18.999998, 12.976410);
	nvgBezierTo(vg, 18.999998, 12.190503, 19.000000, 11.404597, 19.000000, 10.618690);
	nvgBezierTo(vg, 18.279778, 10.582579, 17.708525, 9.724345, 17.707701, 8.677180);
	nvgBezierTo(vg, 17.708055, 7.629734, 18.279377, 6.770766, 18.999895, 6.734555);
	nvgBezierTo(vg, 18.999929, 5.983167, 18.999966, 5.231779, 19.000000, 4.480391);
	nvgBezierTo(vg, 17.955334, 4.480997, 16.983759, 5.228113, 16.425869, 6.459831);
	nvgBezierTo(vg, 15.024411, 4.741649, 12.224778, 3.662023, 9.169149, 3.661411);
	nvgBezierTo(vg, 9.044622, 3.663577, 8.920227, 3.667543, 8.795987, 3.673310);
	nvgBezierTo(vg, 8.929565, 3.260565, 9.173822, 2.909245, 9.492349, 2.671691);
	nvgBezierTo(vg, 9.492349, 2.116327, 9.492349, 1.560964, 9.492349, 1.005600);
	nvgBezierTo(vg, 9.446276, 1.001906, 9.400099, 1.000038, 9.353911, 1.000000);
	nvgBezierTo(vg, 9.353911, 1.000000, 9.353910, 1.000000, 9.353910, 1.000000);
	nvgClosePath(vg);
	nvgFillColor(vg, gScheme.alternative);
	nvgPathWinding(vg, NVG_SOLID);
	nvgShapeAntiAlias(vg, true);
	nvgFill(vg);
	nvgRestore(vg);
}

void SchemeModuleWidget::drawText(NVGcontext *vg, float x, float y, int align, float size, NVGcolor col, const char *txt) {
	nvgFontFaceId(vg, gScheme.font(vg));
	nvgFontSize(vg, size * 90 / SVG_DPI);
	nvgTextAlign(vg, align);
	nvgFillColor(vg, col);
	nvgText(vg, x, y, txt, NULL);
}

void SchemeModuleWidget::drawBase(NVGcontext *vg, const char *txt) {
	drawBackground(vg);
	if (box.size.x < 90) {
		drawLogo(vg, (box.size.x - 20) / 2.0f, box.size.y - 15, 1, 0);
	}
	else {
		drawLogo(vg, box.size.x - 20, box.size.y - 15, 1, 0);
		drawText(vg, 3, 378, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE, 12, gScheme.alternative, "submarine");
	}
	if (box.size.x > 30) {
		drawText(vg, box.size.x / 2.0f, 12, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 12, gScheme.alternative, txt);
	}
	else {
		drawText(vg, box.size.x / 2.0f, 10, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE, 8, gScheme.alternative, txt);
	}
}

void SchemeModuleWidget::render(NVGcontext *vg, SchemeCanvasWidget *canvas) {
	drawBackground(vg);
}

void SchemeModuleWidget::appendContextMenu(Menu * menu) {
	menu->addChild(MenuEntry::create());
	SchemeModuleWidgetVisualMenuItem *m = MenuItem::create<SchemeModuleWidgetVisualMenuItem>("Visuals");
	m->rightText = SUBMENU;
	menu->addChild(m);
}
