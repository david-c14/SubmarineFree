/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/
#include <functional>

#define SUBLIGHTOFF nvgRGB(0x4a,0x4a,0x4a)
#define SUBLIGHTBLUE nvgRGB(0x29,0xb2,0xef)
#define SUBLIGHTRED nvgRGB(0xed,0x2c,0x24)

#define SUBLIGHTBLUETRANS nvgRGBA(0x29,0xb2,0xef,0xc0)
#define SUBLIGHTREDTRANS nvgRGBA(0xed,0x2c,0x24,0xc0)

#define COLOR_EQ(x,y) (((x).a == (y).a) && ((x).r == (y).r) && ((x).g == (y).g) && ((x).b == (y).b))

//////////////////
// Ports
//////////////////

struct SilverPort : PortWidget {
	NVGcolor col = nvgRGB(0xf0, 0xf0, 0xf0);
	SilverPort() {
		box.size.x = 25;
		box.size.y = 25;
	}
	void draw(const DrawArgs &args) override;
};

struct RedPort : SilverPort {
	RedPort() { col = SUBLIGHTRED; }
};

struct BluePort : SilverPort {
	BluePort() { col = SUBLIGHTBLUE; }
};

struct BlackPort : SilverPort {
	BlackPort() { col = nvgRGB(0x40, 0x40, 0x40); }
};

//////////////////
// Switches
//////////////////

struct SubSwitch2 : app::Switch {
	float length = 25.0f;
	int horz = 0;
	SubSwitch2() {
		SetSize();
	}
	void SetSize() {
		if (horz) {
			box.size.x = length;
			box.size.y = 14;
		}
		else {
			box.size.x = 14;
			box.size.y = length;
		}
	}
	void draw(const DrawArgs &args) override;
};

struct SubSwitch3 : SubSwitch2 {
	SubSwitch3() {
		length = 30.0f;
		SetSize();
	}
};

template <class k>
struct SubSwitchHorz : k {
	SubSwitchHorz() {
		k::horz = 1;
		k::SetSize();
	}
};


//////////////////
// Buttons
//////////////////

struct LightButtonLight;

struct LightButton : app::Switch {
	NVGcolor color = SUBLIGHTBLUE;
	LightButtonLight *light;
	LightButton(); 
	void draw(const DrawArgs &args) override;
};

struct LightButtonLight : LightWidget {
	LightButton *button;
	void draw(const DrawArgs &args) override;
};

//////////////////
// Knobs
//////////////////

struct LightKnob;

struct LightKnobLight : LightWidget {
	LightKnob *knob;
	void draw(const DrawArgs &args) override;
};

struct LightKnob : Knob {
	/** Angles in radians */
	float minAngle = -0.83*M_PI;
	float maxAngle = 0.83*M_PI;
	/** Radii in standard units */
	float radius = 19.0;
	int enabled = 1;
	LightKnobLight *light;
	NVGcolor color = SUBLIGHTBLUE;
	virtual void doDraw(const rack::widget::Widget::DrawArgs &args);
	void setEnabled(int val);
	void setRadius(int r);
	LightKnob() {
		smooth = false;
		light = new LightKnobLight();
		light->box.pos = Vec(0,0);
		light->box.size = Vec(radius * 2, radius * 2);
		light->knob = this;
		addChild(light);
	}
	void draw(const DrawArgs &args) override {
		doDraw(args);
		Knob::draw(args);
	}
};

template <class K>
struct TinyKnob : K {
	TinyKnob() {
		K::setRadius(9.0f);
	}
};

template <class K>
struct SmallKnob : K {
	SmallKnob() {
		K::setRadius(12.0f);
	}
};

template <class K>
struct MedKnob : K {
	MedKnob() {
		K::setRadius(19.0f);
	}
};

template <class K>
struct LargeKnob : K {
	LargeKnob() {
		K::setRadius(27.0f);
	}
};

template <class K>
struct SnapKnob : K {
	SnapKnob() {
		K::snap = true;
	}
};

template <class K>
struct NarrowKnob : K {
	NarrowKnob() {
		K::minAngle = -0.75*M_PI;
		K::maxAngle = 0.75*M_PI;	
	}
};

template <class K>
struct RedKnob : K {
	RedKnob() {
		K::color = SUBLIGHTRED;
	}
};

struct SubLightSlider : LightKnob {
	void draw(const DrawArgs &args) override;
	void onDragMove(const event::DragMove &e) override;
};


struct SubLogo : SvgWidget{};
//////////////////
// Lights
//////////////////

struct BlueRedLight : GrayModuleLightWidget {
	BlueRedLight() {
		addBaseColor(componentlibrary::SCHEME_BLUE);
		addBaseColor(componentlibrary::SCHEME_RED);
	}
};

//////////////////
// Scheme
//////////////////

struct Scheme {
	enum enumScheme {
		Blue,
		Dark,
		Light
	};
	Scheme();
	void setColors();
	void save();
	std::shared_ptr<Font> font();
	int font(NVGcontext *vg);
	bool isFlat = false;
	int scheme = Blue;
	std::shared_ptr<Font> _font = NULL;
	bool _fontLoaded = false;
	NVGcolor getBackground(Module *module) { return module?background:_background; }
	NVGcolor getAlternative(Module *module) { return module?alternative:_alternative; }
	NVGcolor getContrast(Module *module) { return module?contrast:_contrast; }
	NVGcolor getHighlight(Module *module) { return module?highlight:_highlight; }
	NVGcolor getShadow(Module *module) { return module?shadow:_shadow; }
private:
	NVGcolor background = nvgRGB(0x29, 0x4f, 0x77);
	NVGcolor alternative = nvgRGB(0x71, 0x9f, 0xcf);
	NVGcolor contrast = nvgRGB(0xff, 0xff, 0xff);
	NVGcolor highlight = nvgRGB(0x3a, 0x6e, 0xa5);
	NVGcolor shadow = nvgRGB(0x18, 0x2d, 0x44);
	NVGcolor _background = nvgRGB(0x29, 0x4f, 0x77);
	NVGcolor _alternative = nvgRGB(0x71, 0x9f, 0xcf);
	NVGcolor _contrast = nvgRGB(0xff, 0xff, 0xff);
	NVGcolor _highlight = nvgRGB(0x3a, 0x6e, 0xa5);
	NVGcolor _shadow = nvgRGB(0x18, 0x2d, 0x44);
};

extern Scheme gScheme;

struct SchemeCanvasWidget; 

//////////////////
// ResizeHandle
//////////////////

struct ResizeHandle : OpaqueWidget {
	bool right = false;
	Vec dragPos;
	Rect originalBox;

	ResizeHandle();

	void onDragStart(const event::DragStart& e) override;

	void onDragMove(const event::DragMove& e) override;

	void draw(const DrawArgs& args) override;
};

struct SchemePanel : FramebufferWidget {
	SchemeCanvasWidget *canvas;
	bool isFlat;
	ResizeHandle *leftHandle;
	ResizeHandle *rightHandle;
	float minWidth = 60.0f;
	float maxWidth = 300.0f;
	int scheme;
	SchemePanel();
	SchemePanel(Vec size);
	SchemePanel(Vec size, float minimum, float maximum);
	void step() override;
	void resize(Rect newBox, Rect oldBox);
	void resize(ModuleWidget *mw, Rect newBox);
};

struct SchemeCanvasWidget : Widget {
	void draw(const DrawArgs &args) override;
};

namespace scheme {
	void drawLogoPath(NVGcontext *vg, float left, float top, float scale, float rotate);
}

struct SchemeModuleWidget : app::ModuleWidget {
	SchemeModuleWidget() {}
	void appendContextMenu(Menu *menu) override;
	void drawBackground(NVGcontext *vg);
	void drawLogo(NVGcontext *vg, float left, float top, float scale, float rotate);
	void drawText(NVGcontext *vg, float x, float y, int align, float size, NVGcolor col, const char *txt);
	void drawBase(NVGcontext *vg, const char *txt);
	virtual void render(NVGcontext *vg, SchemeCanvasWidget *canvas);
	virtual float* getBulkParam(int id) { return NULL; }
};

//////////////////
// SubText
//////////////////

struct SubText : LedDisplayTextField {
	NVGcolor bgColor = nvgRGB(0x00, 0x00, 0x00);
	float fontSize = 12.0f;
	bool textMenu = true;
	bool colorMenu = true;
	SubText() {
		color = SUBLIGHTBLUE;
	}
	int getTextPosition(Vec mousePos) override; 
	void draw(const DrawArgs &args) override;
	void appendContextMenu(Menu *menu);
	MenuItem *createForegroundMenuItem(std::string label, NVGcolor color);
	MenuItem *createBackgroundMenuItem(std::string label, NVGcolor color);
	virtual void foregroundMenu(Menu *menu);
	virtual void backgroundMenu(Menu *menu);
};

//////////////////
// MouseTransformWidget
//////////////////

struct MouseTransformWidget:Widget {
	float transform[6];
	float inverse[6];
	float invLinear[6];
	int hasInverse;
	MouseTransformWidget();
	Rect getChildrenBoundingBox() override;
	void identity(void);
	void translate(Vec delta);
	void rotate(float angle);
	void scale(Vec s);
	void draw(const DrawArgs &args) override;
	void onButton(const event::Button &e) override;
	void onHover(const event::Hover &e) override;
	void onHoverKey(const event::HoverKey &e) override;
	void onHoverText(const event::HoverText &e) override;
	void onHoverScroll(const event::HoverScroll &e) override;
	void onDragHover(const event::DragHover &e) override;
};

//////////////////
// EventWidgets
//////////////////

struct EventWidgetAction : history::Action {
	std::function<void ()> undoHandler;
	std::function<void ()> redoHandler;
	void undo() override;
	void redo() override;
	EventWidgetAction(std::string name, std::function<void()> uHandler, std::function<void()> rHandler);
};

struct EventWidgetButtonBase : OpaqueWidget {
	std::function<void ()> clickHandler;
	std::function<void ()> rightClickHandler;
	std::function<void ()> doubleClickHandler;
	void onButton(const event::Button &e) override;
	void onDoubleClick(const event::DoubleClick &e) override;
};

struct EventWidgetSlider;

struct EventWidgetSliderTooltip : ui::Tooltip {
	EventWidgetSlider *slider;
	void step() override; 
};

struct EventWidgetSlider : OpaqueWidget {
	float startingValue = 0.5f;
	int transparent = false;
	float value = 0.5f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float defaultValue = 0.5f;
	std::string label;
	EventWidgetSliderTooltip *tooltip = NULL;
	std::function<void(float, float)> changedHandler;
	std::function<void(float, float)> changingHandler;
	std::function<std::string(float)> textHandler;
	void draw(const DrawArgs &args) override;
	void onDragStart(const event::DragStart &e) override;
	void onDragMove(const event::DragMove &e) override;
	void onDragEnd(const event::DragEnd &e) override;
	void onDoubleClick(const event::DoubleClick &e) override;
	void onEnter(const event::Enter &e) override;
	void onLeave(const event::Leave &e) override;
};

struct EventWidgetCheckBox : EventWidgetButtonBase {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventWidgetCheckBox();
	void draw (const DrawArgs &args) override;
};

struct EventWidgetRadioButton : EventWidgetButtonBase {
	std::string label;
	bool selected = false;
	std::function<void()> changeHandler;
	EventWidgetRadioButton();
	void draw (const DrawArgs &args) override;
};

struct EventWidgetLabel : Widget {
	std::string label;
	void draw(const DrawArgs &args) override;
};

struct EventWidgetButton : EventWidgetButtonBase {
	std::string label;
	void draw(const DrawArgs &args) override;
};

struct EventWidgetMenuItem : MenuItem {
	std::function<void()> clickHandler;
	std::function<Menu *()> childMenuHandler;
	std::function<void()> stepHandler;
	void onAction(const event::Action &e) override;
	Menu *createChildMenu() override;
	void step() override;
};

struct EventParamField : ui::TextField {
	std::function<void(std::string)> changeHandler;
	void step() override;
	void setText(std::string text);
	void onSelectKey(const event::SelectKey &e) override;
};

//////////////////
// SizeableModuleWidget
//////////////////

struct SizeableModule : Module {
	float size = 0;
	json_t *dataToJson() override;
	void dataFromJson(json_t *rootJ) override;
};

struct SizeableModuleWidget : SchemeModuleWidget {
	SizeableModule *sizeableModule = NULL;
	float fullSize = 0.0f;
	bool stabilized = false;
	SchemePanel *panel;
	SizeableModuleWidget(SizeableModule *module, float size);
	void Resize();
	void Minimize(bool minimize);
	void ShiftOthers(float delta);
//	void dataFromJson(json_t *rootJ) override;
	virtual void onResized();
};

struct MinButton : EventWidgetButtonBase {
	SizeableModuleWidget *mw;
	MinButton();
	void draw(const DrawArgs &args) override;
};

//////////////////
// TooltipKnob
//////////////////

struct SubTooltip : ui::Tooltip {
	std::function<void()> stepLambda;
	void step() override; 
};

struct TooltipKnob : Knob
{
	std::function<std::string()> getText;
	void onEnter(const event::Enter &e) override;
	void onLeave(const event::Leave &e) override;
};
