#include "SubmarineFree.hpp"


Plugin *pluginInstance;


void init(rack::Plugin *p) {
	pluginInstance = p;

	// Add all Models defined throughout the pluginInstance

	//p->addModel(modelXX219);

	p->addModel(modelAG104);
	p->addModel(modelAG106);
	p->addModel(modelAG202);
	//p->addModel(modelAO106);
	//p->addModel(modelAO112);
	//p->addModel(modelAO118);
	//p->addModel(modelAO124);
	//p->addModel(modelAO136);
	p->addModel(modelBB120);
	p->addModel(modelDN112);
	//p->addModel(modelDO105);
	//p->addModel(modelDO110);
	//p->addModel(modelDO115);
	//p->addModel(modelDO120);
	//p->addModel(modelEN104);
	//p->addModel(modelEO102);
	p->addModel(modelFF110);
	p->addModel(modelFF120);
	p->addModel(modelFF206);
	p->addModel(modelFF212);
	//p->addModel(modelHS101);
	p->addModel(modelLA108);
	p->addModel(modelLD103);
	p->addModel(modelLD106);
	//p->addModel(modelLT116);
	//p->addModel(modelMZ909);
	p->addModel(modelNG106);
	p->addModel(modelNG112);
	p->addModel(modelNG206);
	p->addModel(modelOA103);
	p->addModel(modelOA105);
	p->addModel(modelOG104);
	p->addModel(modelOG106);
	p->addModel(modelOG202);
	p->addModel(modelPG104);
	p->addModel(modelPG112);
	//p->addModel(modelPO101);
	//p->addModel(modelPO102);
	//p->addModel(modelPO204);

	//p->addModel(modelSN101);

	//p->addModel(modelSS112);
	//p->addModel(modelSS208);
	//p->addModel(modelSS212);
	//p->addModel(modelSS220);
	//p->addModel(modelSS221);

	p->addModel(modelTD116);
	p->addModel(modelTD202);
	p->addModel(modelTD316);
	p->addModel(modelTD410);
	p->addModel(modelTD510);
	//p->addModel(modelTF101);
	//p->addModel(modelTF102);
	//p->addModel(modelTM105);

	//p->addModel(modelVM101);
	//p->addModel(modelVM102);
	//p->addModel(modelVM201);
	//p->addModel(modelVM202);

	//p->addModel(modelWK101);
	//p->addModel(modelWK205);

	//p->addModel(modelWM101);
	//p->addModel(modelWM102);

	//p->addModel(modelXF101);
	//p->addModel(modelXF102);
	//p->addModel(modelXF104);
	//p->addModel(modelXF201);
	//p->addModel(modelXF202);
	//p->addModel(modelXF301);

	p->addModel(modelXG104);
	p->addModel(modelXG106);
	p->addModel(modelXG202);

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


	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
