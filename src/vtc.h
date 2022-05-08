#ifndef VTC_H
#define VTC_H

#include <Windows.h>
#include <utility>
#include "vtc_keycodes.h"

class VTC {
private:
	HANDLE _handle_in = NULL;
	HANDLE _handle_out = NULL;
	HANDLE _handle_err = NULL;
	DWORD _mode0_in = (unsigned) -1;
	DWORD _mode0_out = (unsigned) -1;
	DWORD _mode0_err = (unsigned) -1;
	DWORD _mode1_in;
	DWORD _mode1_out;
	DWORD _mode1_err;
	DWORD _mode2_in = (unsigned) -1;
	DWORD _mode2_out = (unsigned) -1;
	DWORD _mode2_err = (unsigned) -1;
	void init_in(void);
	void init_out(void);
	void init_err(void);
	KeyCode handle_input(void);
	KeyCode handle_esc(void);

	inline static const int BUFSIZE = 16;
	wchar_t _input_string[BUFSIZE];
	DWORD _input_nreads;

	std::pair<int,int> _position;
	void init(void);

	VTC(void);
public:
	~VTC(void);

	static VTC & vtc(void);

	KeyCode read(void);
	void write(const wchar_t * msg);
	void clear_screen(void) const;
	void clear_column(int X, int Y, int W, int H) const;

	const wchar_t * str(void) const { return _input_string; }
	size_t len(void) const { return _input_nreads; }
};

#endif
