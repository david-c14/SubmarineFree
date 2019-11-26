//SubTag W1

/**************************************************************************
//
// DO NOT TRY THIS AT HOME
//
// Seriously, this stuff is a really messed up cheesed-induced fever dream.
// There's a lot of stuff in here that's a really bad idea, it could mess
// up the stability of Rack at a moment's notice, and I can't make any
// guarantees that it plays nicely with any other plugins.
//
// This module is intended as a bit of fun, and strictly a limited edition
// It will be available in a single version of the plugin, and then it 
// will be removed at the next version. It serves no useful purpose 
// whatsoever, so I feel no guilt at all at making it obsolete.
//
**************************************************************************/

#include <algorithm>
#include <settings.hpp>
#include <chrono>
#include "SubmarineFree.hpp"

typedef Widget *pWidget;

struct CandyCanleWidget : CableWidget {
	CandyCanleWidget(CableWidget *c) {
		outputPort = c->outputPort;
		inputPort = c->inputPort;
		hoveredOutputPort = c->hoveredOutputPort;
		hoveredInputPort = c->hoveredInputPort;
		cable = c->cable;
		color = c->color;
		box = c->box;
		parent = c->parent;
		visible = c->visible;
		requestedDelete = c->requestedDelete;
		c->cable = NULL;
		c->parent = NULL;
	}
	CableWidget *restore() {
		CableWidget *c = new CableWidget;
		c->outputPort = outputPort;
		c->inputPort = inputPort;
		c->hoveredOutputPort = hoveredOutputPort;
		c->hoveredInputPort = hoveredInputPort;
		c->cable = cable;
		c->color = color;
		c->box = box;
		c->parent = parent;
		c->visible = visible;
		c->requestedDelete = requestedDelete;
		parent = NULL;
		cable = NULL;
		return c;
	}
	static void drawCableBody(NVGcontext *vg, Vec p0, Vec p1, Vec p2, NVGcolor color, float thickness) {
		NVGcolor colorOutline = nvgLerpRGBA(color, nvgRGBf(0.0, 0.0, 0.0), 0.5);
		NVGcolor white = nvgRGBAf(1, 1, 1, color.a);
		NVGcolor whiteOutline = nvgLerpRGBA(white, nvgRGBf(0.0, 0.0, 0.0), 0.5);
		
		// Deep breath...This is the scary bit...
		// We are going to try to split the Quadratic bezier curve for the cable
		// into sections and render them in alternating colors.
		// de Casteljau's algorithm will serve us here.

		float inv_distance = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ps1(p0.minus(p1).square())));
		if (inv_distance == 0.0f)
			inv_distance = 1.0f;

		float deltaT = 25.0f * inv_distance;
		float startT = 0.0f;

		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		float endT = deltaT * (ms.count() % 1000) / 1000.0f;
		
		bool colorState = (ms.count() % 2000) < 1000;

		while (startT < 1.0f) {
			if (endT > 1.0f)
				endT = 1.0f;
			float fraction = (endT - startT) / (1.0f - startT);
			Vec p01 = p0.crossfade(p1, fraction);
			Vec p12 = p1.crossfade(p2, fraction);
			Vec p012 = p01.crossfade(p12, fraction);

			// Cable outline
			nvgBeginPath(vg);
			nvgMoveTo(vg, p0.x, p0.y);
			nvgQuadTo(vg, p01.x, p01.y, p012.x, p012.y);
			nvgStrokeColor(vg, colorState?colorOutline:whiteOutline);
			nvgStrokeWidth(vg, thickness);
			nvgStroke(vg);
	
			// Cable solid
			nvgStrokeColor(vg, colorState?color:white);
			nvgStrokeWidth(vg, thickness - 2);
			nvgStroke(vg);

			p0 = p012;
			p1 = p12;
			
			startT = endT;
			endT += deltaT;
			colorState = !colorState;
		}
	}
	static void drawCable(NVGcontext *vg, Vec pos1, Vec pos2, NVGcolor color, float thickness, float tension, float opacity) {
		NVGcolor colorShadow = nvgRGBAf(0, 0, 0, 0.10);
	
		// Cable
		if (opacity > 0.0) {
			nvgSave(vg);
			// This power scaling looks more linear than actual linear scaling
			nvgGlobalAlpha(vg, std::pow(opacity, 1.5));
	
			float dist = pos1.minus(pos2).norm();
			math::Vec slump;
			slump.y = (1.0 - tension) * (150.0 + 1.0*dist);
			Vec pos3 = pos1.plus(pos2).div(2).plus(slump);
	
			// Adjust pos1 and pos2 to not draw over the plug
			pos1 = pos1.plus(pos3.minus(pos1).normalize().mult(9));
			pos2 = pos2.plus(pos3.minus(pos2).normalize().mult(9));
	
			nvgLineJoin(vg, NVG_ROUND);
	
			// Shadow
			math::Vec pos4 = pos3.plus(slump.mult(0.08));
			nvgBeginPath(vg);
			nvgMoveTo(vg, pos1.x, pos1.y);
			nvgQuadTo(vg, pos4.x, pos4.y, pos2.x, pos2.y);
			nvgStrokeColor(vg, colorShadow);
			nvgStrokeWidth(vg, thickness);
			nvgStroke(vg);
		
			// Cable body
			drawCableBody(vg, pos1, pos3, pos2, color, thickness);
	
			nvgRestore(vg);
		}
	}
	void draw(const DrawArgs &args) override {
		float opacity = settings::cableOpacity;
		float tension = settings::cableTension;
		float thickness = 5;
	
		if (isComplete()) {
			engine::Output *output = &cable->outputModule->outputs[cable->outputId];
			// Draw opaque if mouse is hovering over a connected port
			if (output->channels > 1) {
				// Increase thickness if output port is polyphonic
				thickness = 9;
			}
	
			if (outputPort->hovered || inputPort->hovered) {
				opacity = 1.0;
			}
			else if (output->channels == 0) {
				// Draw translucent cable if not active (i.e. 0 channels)
				opacity *= 0.5;
			}
		}
		else {
			// Draw opaque if the cable is incomplete
			opacity = 1.0;
		}
	
		math::Vec outputPos = getOutputPos();
		math::Vec inputPos = getInputPos();
		drawCable(args.vg, outputPos, inputPos, color, thickness, tension, opacity);
	}
};

struct SS2019 : SchemeModuleWidget {

	SS2019(Module *module) : SchemeModuleWidget(module) {
		this->box.size = Vec(15, 380);
	}
	~SS2019() {
		this->_delete();
	}
	void step() override {
		if (!module) {
			return;
		}
		std::list<Widget *>::iterator iterator;
		for (iterator = APP->scene->rack->cableContainer->children.begin(); iterator != APP->scene->rack->cableContainer->children.end(); iterator++) {
			CandyCanleWidget *canle = dynamic_cast<CandyCanleWidget *>(*iterator);
			if (canle)
				continue;
			CableWidget *cable = dynamic_cast<CableWidget *>(*iterator);
			if (!cable)
				continue;
			canle = new CandyCanleWidget(cable);
			if (APP->scene->rack->incompleteCable == cable) {
				APP->scene->rack->incompleteCable = canle;
			}
			pWidget & c(*iterator);
			c = canle;
			delete cable;
		}
		SchemeModuleWidget::step();
	}
	void _delete() {
		if (!module) {
			return;
		}
		std::list<Widget *>::iterator iterator;
		for (iterator = APP->scene->rack->cableContainer->children.begin(); iterator != APP->scene->rack->cableContainer->children.end(); iterator++) {
			CandyCanleWidget *canle = dynamic_cast<CandyCanleWidget *>(*iterator);
			if (!canle)
				continue;
			CableWidget *cable = canle->restore();
			if (APP->scene->rack->incompleteCable == canle) {
				APP->scene->rack->incompleteCable = cable;
			}
			pWidget & c(*iterator);
			c = cable;
			delete canle;
		}		
	}
	void render(NVGcontext *vg, SchemeCanvasWidget *canvas) override {
	drawBackground(vg);
		drawLogo(vg, 0, 20, 1, -M_PI / 2.0f);
		nvgSave(vg);
		nvgTranslate(vg, 1, 377);
		nvgRotate(vg, -M_PI / 2.0f);
		drawText(vg, 0, 0, NVG_ALIGN_LEFT | NVG_ALIGN_TOP, 12, gScheme.getAlternative(module), "submarine");
		nvgRestore(vg);	
  }
	void addMenuItem(Menu *menu, const char *label) {
		MenuLabel *ml = new MenuLabel();
		ml->text = label;
		menu->addChild(ml);
	}
	void appendContextMenu(Menu *menu) override {
		ModuleWidget::appendContextMenu(menu);
		addMenuItem(menu, "");
		addMenuItem(menu, "Secret Santa 2019");
		addMenuItem(menu, "");
		addMenuItem(menu, "Season's Greetings to all.");
		addMenuItem(menu, "Secret Santa 2019 will be");
		addMenuItem(menu, "available ONLY in v1.1.0.");
	}
};

Model *modelSS2019 = createModel<Module, SS2019>("SecretSanta2019");
