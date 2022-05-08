#include "drawable.h"
// #include "vtc.h"
#include <cstdio>

// Drawable::Drawable(void) {
// 	if (!initialized) {
// 		vtc = new VTC();
// 		initialized = true;
// 	}
// }

// Drawable::~Drawable(void) {
// 	if (vtc != nullptr) {
// 		delete vtc;
// 	}
// }

void Drawable::Draw(int x, int y, const char * msg) const {
	printf("\033[%d;%dH%s", y, x, msg);
}
