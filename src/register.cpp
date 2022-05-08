#include "register.h"
#include <stdexcept>
#include <string>
#include <cstdio>

static const int REGISTER_SIZE = 32;

int Register::GetBit(int i) const {
	if (0 <= i && i < REGISTER_SIZE) {
		return (int)((value.dword >> i) & 1UL);
	} else {
		throw std::range_error("must be between 0 and " + std::to_string(REGISTER_SIZE - 1));
	}
}

#define FULL_WIDTH_0 "０"
#define FULL_WIDTH_1 "１"
static const char * UPPER_BAR = "┌──┬──┬──┬──┬──┬──┬──┬──┐┌──┬──┬──┬──┬──┬──┬──┬──┐┌──┬──┬──┬──┬──┬──┬──┬──┐┌──┬──┬──┬──┬──┬──┬──┬──┐";
static const char * LOWER_BAR = "├──┴──┴──┴──┴──┴──┴──┴──┤└──┴──┴──┴──┴──┴──┴──┴──┘└──┴──┴──┴──┴──┴──┴──┴──┘└──┴──┴──┴──┴──┴──┴──┴──┘";
static const char * VALUE_BAR = "└───────────────────────┘";
static const char * LFIELD = "┤%s";
static const char * MFIELD = "│%s";
static const char * RFIELD = "├";
static const char * VFIELD = "│     %10.5f        │";
//┼

void Register::Draw(int x, int y) const {
	static const char values[2][4] = { FULL_WIDTH_0, FULL_WIDTH_1 };
	static const int SIZE = 512;
	char buffer[SIZE];
	char * pos = buffer;

	int index = REGISTER_SIZE;
	for (int i = 0; i < REGISTER_SIZE / 8; ++i){
		pos += snprintf(pos, SIZE-(pos-buffer), LFIELD, values[GetBit(--index)]);
		for (int j = 1; j < 8; ++j) {
			pos += snprintf(pos, SIZE-(pos-buffer), MFIELD, values[GetBit(--index)]);
		}
		pos += snprintf(pos, SIZE-(pos-buffer), RFIELD);
	}

	this->Drawable::Draw(x, y++, UPPER_BAR);
	this->Drawable::Draw(x, y++, buffer);
	this->Drawable::Draw(x, y++, LOWER_BAR);
	
	snprintf(buffer, SIZE, VFIELD, value.fsingle);
	this->Drawable::Draw(x, y++, buffer);
	this->Drawable::Draw(x, y++, VALUE_BAR);
}

const Register & Register::operator+=(const Register & other) {
	this->value.fsingle += other.value.fsingle;
	return *this;
}

const Register & Register::operator*=(const Register & other) {
	this->value.fsingle *= other.value.fsingle;
	return *this;
}

const Register operator/(unsigned long i, const Register & reg) {
	Register result = reg;
	result.value.fsingle = i / result.value.fsingle;
	return result;
}
