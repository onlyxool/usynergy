
int keyTranslation [65535];

static void build_key_translation_table () {
	int i = 0;
	for (i = 0; i < 65535; i++) {
		keyTranslation [i] = -1;
	}

	keyTranslation ['a'] = KEY_A;
	keyTranslation ['b'] = KEY_B;
	keyTranslation ['c'] = KEY_C;
	keyTranslation ['d'] = KEY_D;
	keyTranslation ['e'] = KEY_E;
	keyTranslation ['f'] = KEY_F;
	keyTranslation ['g'] = KEY_G;
	keyTranslation ['h'] = KEY_H;
	keyTranslation ['i'] = KEY_I;
	keyTranslation ['j'] = KEY_J;
	keyTranslation ['k'] = KEY_K;
	keyTranslation ['l'] = KEY_L;
	keyTranslation ['m'] = KEY_M;
	keyTranslation ['n'] = KEY_N;
	keyTranslation ['o'] = KEY_O;
	keyTranslation ['p'] = KEY_P;
	keyTranslation ['q'] = KEY_Q;
	keyTranslation ['r'] = KEY_R;
	keyTranslation ['s'] = KEY_S;
	keyTranslation ['t'] = KEY_T;
	keyTranslation ['u'] = KEY_U;
	keyTranslation ['v'] = KEY_V;
	keyTranslation ['w'] = KEY_W;
	keyTranslation ['x'] = KEY_X;
	keyTranslation ['y'] = KEY_Y;
	keyTranslation ['z'] = KEY_Z;

	keyTranslation ['A'] = KEY_A;
	keyTranslation ['B'] = KEY_B;
	keyTranslation ['C'] = KEY_C;
	keyTranslation ['D'] = KEY_D;
	keyTranslation ['E'] = KEY_E;
	keyTranslation ['F'] = KEY_F;
	keyTranslation ['G'] = KEY_G;
	keyTranslation ['H'] = KEY_H;
	keyTranslation ['I'] = KEY_I;
	keyTranslation ['J'] = KEY_J;
	keyTranslation ['K'] = KEY_K;
	keyTranslation ['L'] = KEY_L;
	keyTranslation ['M'] = KEY_M;
	keyTranslation ['N'] = KEY_N;
	keyTranslation ['O'] = KEY_O;
	keyTranslation ['P'] = KEY_P;
	keyTranslation ['Q'] = KEY_Q;
	keyTranslation ['R'] = KEY_R;
	keyTranslation ['S'] = KEY_S;
	keyTranslation ['T'] = KEY_T;
	keyTranslation ['U'] = KEY_U;
	keyTranslation ['V'] = KEY_V;
	keyTranslation ['W'] = KEY_W;
	keyTranslation ['X'] = KEY_X;
	keyTranslation ['Y'] = KEY_Y;
	keyTranslation ['Z'] = KEY_Z;

	keyTranslation ['`'] = KEY_GRAVE;
	keyTranslation ['0'] = KEY_0;
	keyTranslation ['1'] = KEY_1;
	keyTranslation ['2'] = KEY_2;
	keyTranslation ['3'] = KEY_3;
	keyTranslation ['4'] = KEY_4;
	keyTranslation ['5'] = KEY_5;
	keyTranslation ['6'] = KEY_6;
	keyTranslation ['7'] = KEY_7;
	keyTranslation ['8'] = KEY_8;
	keyTranslation ['9'] = KEY_9;

	/*
	 * this is definitely not the correct way to handle
	 *  the shift key... this should really be done with
	 *  the mask...
	 */

	keyTranslation ['~'] = KEY_GRAVE;
	keyTranslation ['!'] = KEY_1;
	keyTranslation ['@'] = KEY_2;
	keyTranslation ['#'] = KEY_3;
	keyTranslation ['$'] = KEY_4;
	keyTranslation ['%'] = KEY_5;
	keyTranslation ['^'] = KEY_6;
	keyTranslation ['&'] = KEY_7;
	keyTranslation ['*'] = KEY_8;
	keyTranslation ['('] = KEY_9;
	keyTranslation [')'] = KEY_0;
	keyTranslation ['-'] = KEY_MINUS;
	keyTranslation ['_'] = KEY_MINUS;
	keyTranslation ['='] = KEY_EQUAL;
	keyTranslation ['+'] = KEY_EQUAL;
	keyTranslation ['?'] = KEY_SLASH;
	keyTranslation ['/'] = KEY_SLASH;
	keyTranslation ['.'] = KEY_DOT;
	keyTranslation ['>'] = KEY_DOT;
	keyTranslation [','] = KEY_COMMA;
	keyTranslation ['<'] = KEY_COMMA;
	keyTranslation [';'] = KEY_SEMICOLON;
	keyTranslation [':'] = KEY_SEMICOLON;
	keyTranslation ['\''] = KEY_APOSTROPHE;
	keyTranslation ['"'] = KEY_APOSTROPHE;
	keyTranslation ['\\'] = KEY_BACKSLASH;
	keyTranslation ['|'] = KEY_BACKSLASH;
	keyTranslation ['['] = KEY_LEFTBRACE;
	keyTranslation [']'] = KEY_RIGHTBRACE;
	keyTranslation ['{'] = KEY_LEFTBRACE;
	keyTranslation ['}'] = KEY_RIGHTBRACE;
	keyTranslation [61419] = KEY_MENU;
	keyTranslation [61420] = KEY_MENU;


	keyTranslation [61192] = KEY_BACKSPACE;
	keyTranslation [8] = KEY_BACKSPACE;

	keyTranslation [61197] = KEY_ENTER;
	keyTranslation [10] = KEY_ENTER;
	keyTranslation [13] = KEY_ENTER;
	keyTranslation [32] = KEY_SPACE;

	keyTranslation [61283] = KEY_INSERT;
	keyTranslation [61439] = KEY_DELETE;
	keyTranslation [61264] = KEY_HOME;
	keyTranslation [61271] = KEY_END;
	keyTranslation [61269] = KEY_PAGEUP;
	keyTranslation [61270] = KEY_PAGEDOWN;

	//Keypad
	keyTranslation [61361] = KEY_1;
	keyTranslation [61362] = KEY_2;
	keyTranslation [61363] = KEY_3;
	keyTranslation [61364] = KEY_4;
	keyTranslation [61365] = KEY_5;
	keyTranslation [61366] = KEY_6;
	keyTranslation [61367] = KEY_7;
	keyTranslation [61368] = KEY_8;
	keyTranslation [61369] = KEY_9;
	keyTranslation [61360] = KEY_0;
	keyTranslation [61358] = KEY_DOT;

	keyTranslation [61354] = KEY_KPASTERISK;
	keyTranslation [61359] = KEY_SLASH;
	keyTranslation [61355] = KEY_KPPLUS;
	keyTranslation [61357] = KEY_MINUS;
	keyTranslation [61325] = KEY_ENTER;
	keyTranslation [61343] = KEY_DELETE;

	keyTranslation [61340] = KEY_END;
	keyTranslation [61339] = KEY_PAGEDOWN;
	keyTranslation [61333] = KEY_HOME;
	keyTranslation [61338] = KEY_PAGEUP;
	keyTranslation [61342] = KEY_INSERT;

    // Arrows
	keyTranslation [61265] = KEY_LEFT;
	keyTranslation [61266] = KEY_UP;
	keyTranslation [61267] = KEY_RIGHT;
	keyTranslation [61268] = KEY_DOWN;

	keyTranslation [61193] = KEY_TAB;
	keyTranslation [61413] = KEY_CAPSLOCK;
	keyTranslation [61410] = KEY_RIGHTSHIFT;
	keyTranslation [61409] = KEY_LEFTSHIFT;

	keyTranslation [27] = KEY_BACK;      // ESC to BACK
	keyTranslation [61211] = KEY_HOME;   // ESC to BACK

	keyTranslation [63236] = KEY_HOME;   // F1 to HOME
	keyTranslation [63237] = KEY_MENU;   // F2 to MENU
	keyTranslation [63238] = KEY_BACK;   // F3 to BACK
	keyTranslation [63239] = KEY_SEARCH; // F4 to SEARCH
	keyTranslation [63240] = KEY_POWER;  // F5 to POWER

	keyTranslation [61374] = KEY_HOME;   // F1 to HOME
	keyTranslation [61375] = KEY_MENU;   // F2 to MENU
	keyTranslation [61376] = KEY_BACK;   // F3 to BACK
	keyTranslation [61377] = KEY_SEARCH; // F4 to SEARCH
	keyTranslation [61378] = KEY_POWER;  // F5 to POWER
}
