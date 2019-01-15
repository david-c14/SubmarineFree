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

struct sub_sw_2 : SVGSwitch, ToggleSwitch {
	sub_sw_2() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_2a.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_2b.svg")));
	}
};

struct sub_sw_3 : SVGSwitch, ToggleSwitch {
	sub_sw_3() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3a.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3b.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3c.svg")));
	}
};

struct sub_sw_4 : SVGSwitch, ToggleSwitch {
	sub_sw_4() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4a.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4b.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4c.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4d.svg")));
	}
};

struct sub_sw_2h : SVGSwitch, ToggleSwitch {
	sub_sw_2h() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_2ha.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_2hb.svg")));
	}
};

struct sub_sw_3h : SVGSwitch, ToggleSwitch {
	sub_sw_3h() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3ha.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3hb.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_3hc.svg")));
	}
};

struct sub_sw_4h : SVGSwitch, ToggleSwitch {
	sub_sw_4h() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4ha.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4hb.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4hc.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_sw_4hd.svg")));
	}
};

//////////////////
// Buttons
//////////////////

struct LightButton : ToggleSwitch {
	NVGcolor color = SUBLIGHTBLUE;
	LightButton() {
		minValue = 0.0f;
		maxValue = 1.0f;	
		box.size.x = 16.0f;
		box.size.y = 16.0f;
	}
	void draw(NVGcontext *vg) override;
	void step() override {
		setValue(module->params[paramId].value);
	}
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

//////////////////
// Lights
//////////////////

struct BlueRedLight : GrayModuleLightWidget {
	BlueRedLight() {
		addBaseColor(COLOR_BLUE);
		addBaseColor(COLOR_RED);
	}
};
