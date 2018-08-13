#define COLOR_BLACK         0x0
#define COLOR_WHITE         0xFFFFFF
#define COLOR_GRAY          0xAAAAAA
#define COLOR_CYAN          0x00FFFF

void lfb_init();
void lfb_showpicture();
void lfb_print(int x, int y, unsigned int color, char *s);
void lfb_blank();
void lfb_blank_color(unsigned int color);
void lfb_blankline(int line);
void lfb_testpattern();
void printf(char *fmt, ...);