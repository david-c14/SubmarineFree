#include "SubmarineFree.hpp"


Plugin *plugin;


void init(rack::Plugin *p) {
	plugin = p;
	p->slug = TOSTRING(SLUG);
	p->version = TOSTRING(VERSION);

	// Add all Models defined throughout the plugin
	p->addModel(modelAG106);
	p->addModel(modelAO106);
	p->addModel(modelAO112);
	p->addModel(modelBB120);
	p->addModel(modelFF110);
	p->addModel(modelFF120);
	p->addModel(modelFF212);
	p->addModel(modelLA108);
	p->addModel(modelLD106);
	p->addModel(modelNG112);
	p->addModel(modelOG106);
	p->addModel(modelPG112);
	p->addModel(modelPO101);
	p->addModel(modelPO102);
	p->addModel(modelPO204);

	p->addModel(modelSS112);
	p->addModel(modelSS208);
	p->addModel(modelSS212);
	p->addModel(modelSS220);
	p->addModel(modelSS221);

	p->addModel(modelTD116);
	p->addModel(modelTF101);
	p->addModel(modelTM105);

	p->addModel(modelWK101);
	p->addModel(modelWK205);

	p->addModel(modelXF101);
	p->addModel(modelXF102);
	p->addModel(modelXF104);
	p->addModel(modelXF201);
	p->addModel(modelXF202);

	p->addModel(modelXG106);

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
