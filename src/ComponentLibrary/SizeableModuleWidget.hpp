struct SizeableModuleWidget : SchemeModuleWidget {
	bool stabilized = false;
	float fullSize = 0;
	SchemePanel *panel;
	SizeableModuleWidget(Module *module, float size);
	void Resize();
	void Minimize(bool minimize);
	void ShiftOthers(float delta);
	json_t *toJson() override;
	void fromJson(json_t *rootJ) override;
	virtual void onResize();
};

struct MinButton : EventWidgetButtonBase {
	SizeableModuleWidget *mw;
	MinButton();
	void draw(const DrawArgs &args) override;
};
