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

