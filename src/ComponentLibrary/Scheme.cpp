#include "../SubmarineFree.hpp"

Scheme::Scheme() {
	json_error_t error;
	FILE *file = fopen(assetLocal("SubmarineFree/Settings.json").c_str(), "r");
	if (!file) {
		return;
	}
	json_t *rootJ = json_loadf(file, 0, &error);
	fclose(file);
	if (!rootJ) {
		std::string message = stringf("Submarine Free Settings: JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
		warn(message.c_str());
		return;
	}
	json_t *h1 = json_object_get(rootJ, "flat");
	if (h1) {
		isFlat = (json_number_value(h1) != 0);
	}
	json_decref(rootJ);
}

Scheme gScheme;
