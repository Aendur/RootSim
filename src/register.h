#ifndef REGISTER_H
#define REGISTER_H

#include "drawable.h"

class Register : public Drawable {
public:
	union _reg_val {
		unsigned char  byte[4];
		unsigned short word[2];
		unsigned long  dword;
		float fsingle;
	} value;

	int GetBit(int) const;
	Register(void) : Drawable() { value.fsingle = 0.0f; }
	//Register(unsigned long v) : Drawable() { value.fsingle = (float) v; }
	Register(float v) : Drawable() { value.fsingle = v; }
	const Register & operator+=(const Register & other);
	const Register & operator*=(const Register & other);

	void Draw(int x, int y) const;
};

const Register operator/(unsigned long i, const Register & reg);


#endif