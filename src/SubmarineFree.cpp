#include "SubmarineFree.hpp"


Plugin *plugin;


void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	// Add all Models defined throughout the plugin
	p->addModel(modelLD106);

	p->addModel(modelXF101);
	p->addModel(modelXF102);
	p->addModel(modelXF104);
	p->addModel(modelXF201);
	p->addModel(modelXF202);

	p->addModel(modelBP101);
	p->addModel(modelBP102);
	p->addModel(modelBP104);
	p->addModel(modelBP108);
	p->addModel(modelBP110);
	p->addModel(modelBP112);
	p->addModel(modelBP116);
	p->addModel(modelBP120);
	p->addModel(modelBP124);
	p->addModel(modelBP132);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
