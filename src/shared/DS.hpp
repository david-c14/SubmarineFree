#include "../SubmarineFree.hpp"

namespace SubmarineDS {
	extern float globalVL;
	extern float globalVH;
}

struct DS_Module : Module {
	float voltage0 = SubmarineDS::globalVL;
	float voltage1 = SubmarineDS::globalVH;
	float midpoint();
	float output(int);
	DS_Module() {}
	json_t *dataToJson() override;
	void dataFromJson(json_t *) override;
	void onReset() override;
	void appendContextMenu(Menu *);
	unsigned int getInput(int channels, unsigned int bitArray, int input);
	void setOutput(int channels, int output, unsigned int bitArray);
};

struct DS_MenuItem : MenuItem {
	DS_Module *module;
	float vh;
	float vl;
	void onAction(const event::Action &e) override;
	void step() override;
};

struct DS_Schmitt {
	int _state = 0;
	static float high(float, float);
	static float low(float, float);
	void reset();
	void set();
	void set(int);
	int state(float, float, float);
	int state(DS_Module *, float);
	int edge(float, float, float);
	int edge(DS_Module *, float);
	int edge(float, float, float, int);
	int edge(DS_Module *, float, int);
	int redge(float, float, float);
	int redge(DS_Module *, float);
	int fedge(float, float, float);
	int fedge(DS_Module *, float);
};
