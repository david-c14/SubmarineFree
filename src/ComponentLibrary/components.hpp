//////////////////
// Ports
//////////////////

struct sub_port : SVGPort {
	sub_port() {
		setSVG(SVG::load(assetPlugin(plugin, "res/Components/sub_port.svg")));
	}
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

struct sub_btn : SVGSwitch, ToggleSwitch {
	sub_btn() {
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_btn.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_btn_a.svg")));
	}
};

//////////////////
// Knobs
//////////////////

struct LightKnob : SVGKnob {
	char enabled;
	std::vector<std::shared_ptr<SVG>> frames;
	LightKnob();
	void addFrame(std::shared_ptr<SVG> svg);
	void setEnabled(char val);
};

struct sub_knob_large : LightKnob {
	sub_knob_large() {
		minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/Components/sub_knob_large.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_knob_large.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/Components/sub_knob_large_a.svg")));
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
