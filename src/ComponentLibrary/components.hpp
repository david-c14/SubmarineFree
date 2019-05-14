/**************************************************************
*
* The graphical styles and images encoded in this source file 
* are copyright © 2018 David O'Rourke
*
**************************************************************/

#define SUBLIGHTOFF nvgRGB(0x4a,0x4a,0x4a)
#define SUBLIGHTBLUE nvgRGB(0x29,0xb2,0xef)
#define SUBLIGHTRED nvgRGB(0xed,0x2c,0x24)

#define SUBLIGHTBLUETRANS nvgRGBA(0x29,0xb2,0xef,0xc0)
#define SUBLIGHTREDTRANS nvgRGBA(0xed,0x2c,0x24,0xc0)

//////////////////
// Ports
//////////////////

struct SilverPort : Port {
	NVGcolor col = nvgRGB(0xf0, 0xf0, 0xf0);
	SilverPort() {
		box.size.x = 25;
		box.size.y = 25;
	}
	void draw(NVGcontext *vg) override;
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

struct SubSwitch2 : ToggleSwitch {
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
	void draw(NVGcontext *vg) override;
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

struct LightButton : ToggleSwitch {
	NVGcolor color = SUBLIGHTBLUE;
	LightButton() {
		//minValue = 0.0f;
		//maxValue = 1.0f;	
		box.size.x = 16.0f;
		box.size.y = 16.0f;
	}
	void draw(NVGcontext *vg) override;
};

//////////////////
// Knobs
//////////////////

struct LightKnob : Knob {
	/** Angles in radians */
	float minAngle = -0.83*M_PI;
	float maxAngle = 0.83*M_PI;
	/** Radii in standard units */
	float radius = 19.0;
	int enabled = 1;
	NVGcolor color = SUBLIGHTBLUE;
	LightKnob() {smooth = false;}
	void draw(NVGcontext *vg) override;
	void setEnabled(int val);
	void setRadius(int r);
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

struct LightSlider : LightKnob {
	void draw(NVGcontext *vg) override;
	void onDragMove(EventDragMove &e) override;
};

struct SubLogo : SVGWidget{};
//////////////////
// Lights
//////////////////

struct BlueRedLight : GrayModuleLightWidget {
	BlueRedLight() {
		addBaseColor(COLOR_BLUE);
		addBaseColor(COLOR_RED);
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
	NVGcolor background = nvgRGB(0x29, 0x4f, 0x77);
	NVGcolor alternative = nvgRGB(0x71, 0x9f, 0xcf);
	NVGcolor contrast = nvgRGB(0xff, 0xff, 0xff);
	NVGcolor highlight = nvgRGB(0x3a, 0x6e, 0xa5);
	NVGcolor shadow = nvgRGB(0x18, 0x2d, 0x44);
	std::shared_ptr<Font> _font = NULL;
	bool _fontLoaded = false;
};

extern Scheme gScheme;

struct SchemeCanvasWidget; 

struct SchemePanel : FramebufferWidget {
	bool isFlat;
	int scheme;
	SchemePanel();
	SchemePanel(Vec size);
	void step() override;
};

struct SchemeCanvasWidget : VirtualWidget {
	void draw(NVGcontext *vg) override;
};

struct SchemeModuleWidget : ModuleWidget {
	SchemeModuleWidget(Module *module): ModuleWidget(module) {}
	void appendContextMenu(Menu *menu) override;
	void drawBackground(NVGcontext *vg);
	void drawLogo(NVGcontext *vg, float left, float top, float scale, float rotate);
	void drawText(NVGcontext *vg, float x, float y, int align, float size, NVGcolor col, const char *txt);
	void drawBase(NVGcontext *vg, const char *txt);
	virtual void render(NVGcontext *vg, SchemeCanvasWidget *canvas);
};

//////////////////
// SubText
//////////////////

struct SubTextForegroundMenu;
struct SubTextBackgroundMenu;

struct SubText : LedDisplayTextField {
	NVGcolor bgColor = nvgRGB(0x00, 0x00, 0x00);
	int fontSize = 12;
	SubText() {
		color = SUBLIGHTBLUE;
	}
	int getTextPosition(Vec mousePos) override; 
	void draw(NVGcontext *vg) override;
	void appendContextMenu(Menu *menu);
	SubTextForegroundMenu *createForegroundMenuItem(std::string label, NVGcolor color);
	SubTextBackgroundMenu *createBackgroundMenuItem(std::string label, NVGcolor color);
	virtual void foregroundMenu(Menu *menu);
	virtual void backgroundMenu(Menu *menu);
};

struct SubTextForegroundParent : MenuItem {
	SubText *subText;
	Menu *createChildMenu() override;
};

struct SubTextForegroundMenu : MenuItem {
	SubText *subText;
	NVGcolor color;
	void onAction(EventAction &e) override;
	void step() override; 
};

struct SubTextBackgroundParent : MenuItem {
	SubText *subText;
	Menu *createChildMenu() override;
};

struct SubTextBackgroundMenu : MenuItem {
	SubText *subText;
	NVGcolor color;
	void onAction(EventAction &e) override;
	void step() override;
};

//////////////////
// MouseTransformWidget
//////////////////

struct MouseTransformWidget:VirtualWidget {
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
	void draw(NVGcontext *vg) override;
	void onMouseDown(EventMouseDown &e) override;
	void onMouseUp(EventMouseUp &e) override;
	void onMouseMove(EventMouseMove &e) override;
	void onHoverKey(EventHoverKey &e) override;
	void onScroll(EventScroll &e) override;
	void onPathDrop(EventPathDrop &e) override;
};
