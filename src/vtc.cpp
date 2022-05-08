#include "vtc.h"

#include <unordered_map>
#include <stdexcept>
#include <regex>
#include <string>
#include <memory>
#include <cstdio>

VTC & VTC::vtc(void) {
	static std::unique_ptr<VTC> instance(new VTC());
	//static VTC * instance = new VTC();
	return *instance;
}

VTC::VTC(void) {
	_mode1_in  = ENABLE_VIRTUAL_TERMINAL_INPUT;
	_mode1_out = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT;
	_mode1_err = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT;
	_mode2_in  = (DWORD) ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT;
	init();
}

VTC::~VTC(void) {
	if (
		_handle_in != NULL &&
		_handle_in != INVALID_HANDLE_VALUE &&
		_mode0_in  != -1
	) { SetConsoleMode(_handle_in, _mode0_in); }
	if (
		_handle_out != NULL &&
		_handle_out != INVALID_HANDLE_VALUE &&
		_mode0_out  != -1
	) { SetConsoleMode(_handle_out, _mode0_out); }
	if (
		_handle_err != NULL &&
		_handle_err != INVALID_HANDLE_VALUE &&
		_mode0_err  != -1
	) { SetConsoleMode(_handle_err, _mode0_err); }
	printf("releasing VTC handles\n");
}

void VTC::init(void) {
	init_in();
	init_out();
}

void VTC::init_in(void) {
	_handle_in = GetStdHandle(STD_INPUT_HANDLE);
	if (_handle_in == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stdin");
	}
	
	if (!GetConsoleMode(_handle_in, &_mode0_in)) {
		throw std::runtime_error("failed to save stdin state");
	}

	if (!SetConsoleMode(_handle_in, (_mode0_in | _mode1_in) & _mode2_in)) {
		throw std::runtime_error("failed to set new stdin state");
	}
}

void VTC::init_out(void) {
	_handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (_handle_out == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stdout");
	}
	
	if (!GetConsoleMode(_handle_out, &_mode0_out)) {
		throw std::runtime_error("failed to save stdout state");
	}

	if (!SetConsoleMode(_handle_out, (_mode0_out | _mode1_out) & _mode2_out)) {
		throw std::runtime_error("failed to set new stdout state");
	}
}

void VTC::init_err(void) {
	_handle_err = GetStdHandle(STD_ERROR_HANDLE);
	if (_handle_err == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("failed to get handle to stderr");
	}
	
	if (!GetConsoleMode(_handle_err, &_mode0_err)) {
		throw std::runtime_error("failed to save stderr state");
	}

	if (!SetConsoleMode(_handle_err, (_mode0_err | _mode1_err) & _mode2_err)) {
		throw std::runtime_error("failed to set new stderr state");
	}
}

void VTC::write(const wchar_t * msg) {
	DWORD len = (DWORD) wcslen(msg);
	WriteConsoleW(_handle_out, msg, len, NULL, NULL);
}

KeyCode VTC::read(void) {
	BOOL status = ReadConsoleW(_handle_in, _input_string, sizeof(_input_string), &_input_nreads, NULL);
	if (status) {
		return handle_input();
	} else {
		return VTC_KEY_UNDEFINED;
	}
}

KeyCode VTC::handle_esc(void) {
	static const std::unordered_map<std::wstring, KeyCode> patterns = {
		{ L"\033\\[A", VTC_KEY_ARROW_UP },
		{ L"\033\\[C", VTC_KEY_ARROW_RIGHT },
		{ L"\033\\[B", VTC_KEY_ARROW_DOWN },
		{ L"\033\\[D", VTC_KEY_ARROW_LEFT },
		{ L"\033\\[1;5A", VTC_KEY_CTRL_ARROW_UP },
		{ L"\033\\[1;5C", VTC_KEY_CTRL_ARROW_RIGHT },
		{ L"\033\\[1;5B", VTC_KEY_CTRL_ARROW_DOWN },
		{ L"\033\\[1;5D", VTC_KEY_CTRL_ARROW_LEFT },
		{ L"\033\\[2~", VTC_KEY_INSERT },
		{ L"\033\\[3~", VTC_KEY_DELETE },
		{ L"\033\\[H", VTC_KEY_HOME },
		{ L"\033\\[F", VTC_KEY_END },
		{ L"\033\\[Z", VTC_KEY_SHIFT_TAB },
		{ L"\033\\[5~", VTC_KEY_PGUP },
		{ L"\033\\[6~", VTC_KEY_PGDOWN },
		{ L"\033"  L"OP", VTC_KEY_F1 }, { L"\033\\[15~", VTC_KEY_F5 }, { L"\033\\[20~", VTC_KEY_F9 },
		{ L"\033"  L"OQ", VTC_KEY_F2 }, { L"\033\\[17~", VTC_KEY_F6 }, { L"\033\\[21~", VTC_KEY_F10 },
		{ L"\033"  L"OR", VTC_KEY_F3 }, { L"\033\\[18~", VTC_KEY_F7 }, { L"\033\\[23~", VTC_KEY_F11 },
		{ L"\033"  L"OS", VTC_KEY_F4 }, { L"\033\\[19~", VTC_KEY_F8 }, { L"\033\\[24~", VTC_KEY_F12 },
		{ L"\033\\[1;2P", VTC_KEY_SHIFT_F1 }, { L"\033\\[15;2~", VTC_KEY_SHIFT_F5 }, { L"\033\\[20;2~", VTC_KEY_SHIFT_F9 },
		{ L"\033\\[1;2Q", VTC_KEY_SHIFT_F2 }, { L"\033\\[17;2~", VTC_KEY_SHIFT_F6 }, { L"\033\\[21;2~", VTC_KEY_SHIFT_F10 },
		{ L"\033\\[1;2R", VTC_KEY_SHIFT_F3 }, { L"\033\\[18;2~", VTC_KEY_SHIFT_F7 }, { L"\033\\[23;2~", VTC_KEY_SHIFT_F11 },
		{ L"\033\\[1;2S", VTC_KEY_SHIFT_F4 }, { L"\033\\[19;2~", VTC_KEY_SHIFT_F8 }, { L"\033\\[24;2~", VTC_KEY_SHIFT_F12 },
	};

	for(const auto & [pat,ret] : patterns) {
		if (std::regex_search(_input_string, std::wregex(pat))) {
			//wprintf(L"\033[48;1HMATCH - Key pressed: ESC %ls (", &_input_string[1]);
			return ret;
		}
	}

	if (std::regex_match(_input_string, std::wregex(L"\033"))) {
		return VTC_KEY_ESC;
	}

	PUSHORT val = (PUSHORT) _input_string;
	USHORT bytes[] = {
		(USHORT)(val[0] & 0x00FF),
		(USHORT)(val[1] & 0x00FF),
		(USHORT)(val[2] & 0x00FF),
		(USHORT)(val[3] & 0x00FF),
		(USHORT)(val[4] & 0x00FF),
		(USHORT)(val[5] & 0x00FF),
		(USHORT)(val[6] & 0x00FF),
		(USHORT)(val[7] & 0x00FF),
	};
	
	wprintf(L"\033[48;1HNO MATCH - Key pressed: ESC %ls (", &_input_string[1]);
	for (int p = 0; p < 8; ++p) { wprintf(L" 0x%02X", bytes[p]); }
	wprintf(L" )\033[0K\n");
	return VTC_KEY_UNDEFINED;
}

KeyCode VTC::handle_input(void) {
	_input_string[_input_nreads] = 0;
	
	if (_input_nreads == 0) { return VTC_KEY_UNDEFINED; }
	_input_nreads = 0;

	if (' ' <= _input_string[0] && _input_string[0] <= '~') {
		return (KeyCode) _input_string[0];
	}

	short v0 = ((PUSHORT) _input_string)[0];
	short v1 = ((PUSHORT) _input_string)[1];
	short v2 = ((PUSHORT) _input_string)[1];
	short v3 = ((PUSHORT) _input_string)[1];
	//wprintf(L"\033[47;1H\n\n\n");
	//wprintf(L"Key pressed: %ls\n(0x%02X 0x%02X 0x%02X 0x%02X)\033[0K\n", _input_string, v0 & 0x00FF, v1 & 0x00FF, v2 & 0x00FF, v3 & 0x00FF);

	switch(_input_string[0]) {
	case L'\033': return handle_esc();
	case L'\n': return VTC_KEY_RETURN;
	case L'\r': return VTC_KEY_RETURN;
	case L'\t': return VTC_KEY_TAB;
	case 0x7F: return VTC_KEY_BACKSPACE;
	case 0x1A: return VTC_KEY_PAUSE;
	default:
		wprintf(L"\033[48;1HKey pressed: %ls (0x%02X 0x%02X 0x%02X 0x%02X)\033[0K\n", _input_string, v0 & 0x00FF, v1 & 0x00FF, v2 & 0x00FF, v3 & 0x00FF);
		return VTC_KEY_UNDEFINED;
	}
}


void VTC::clear_screen(void) const {
	//write(L"\033[1;1H\033[J");
	printf("\033[1;1H\033[J");
}


void VTC::clear_column(int X, int Y, int W, int H) const {
	for(int i = 0; i < H; ++i) {
		printf("\033[%d;%dH%*s", Y+i, X, W, "");
	}
	printf("\033[%d;%dH", Y, X);
}

