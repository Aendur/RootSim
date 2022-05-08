#ifndef DRAWABLE_H
#define DRAWABLE_H

//class VTC;

class Drawable {
protected:
	Drawable (void) {}
	virtual ~Drawable(void) {}

	void Draw(int x, int y, const char * msg) const;

public:
	virtual void Draw(int x, int y) const = 0;
};


#endif
