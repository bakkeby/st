#include <libconfig.h>
#include <strings.h>
#include <ctype.h>
#include <wchar.h>

const char *progname = "st";

/* Configuration variables */
char **fonts = NULL;
int num_fonts;
char *ascii_printable = NULL;
char *initial_working_directory = NULL;
char *iso14755cmd = NULL;
char *mouseshape_text = NULL;
char *plumb_cmd = NULL;
char *scroll = NULL;
char *shell = NULL;
char *stty_args = NULL;
char *termname = NULL;
char *utmp = NULL;
char *url_opener_cmd = NULL;
char *window_icon = NULL;
char *xdndescchar = NULL;
wchar_t *kbds_sdelim = NULL;
wchar_t *kbds_ldelim = NULL;
wchar_t *worddelimiters = NULL;
int histsize = 2000;
uint64_t settings = 0;
static unsigned int width = 0;
static unsigned int height = 0;
static Geometry geometry = CellGeometry;

static void set_config_path(const char* filename, char *config_path, char *config_file);
static int setting_length(const config_setting_t *cfg);
static const char *setting_get_string_elem(const config_setting_t *cfg, int i);
static int setting_get_int_elem(const config_setting_t *cfg, int i);
static const config_setting_t *setting_get_elem(const config_setting_t *cfg, int i);

static int config_lookup_sloppy_bool(const config_t *cfg, const char *name, int *ptr);
static int config_setting_lookup_sloppy_bool(const config_setting_t *cfg, const char *name, int *ptr);
static int _config_setting_get_sloppy_bool(const config_setting_t *cfg, int *ptr);

static int config_lookup_wcsdup(const config_t *cfg, const char *name, wchar_t **wcsptr);
static int config_setting_lookup_wcsdup(const config_setting_t *cfg, const char *name, wchar_t **wcsptr);
static int _config_setting_wcsdup_string(const config_setting_t *cfg_item, wchar_t **wcsptr);

static int config_lookup_simple_float(const config_t *cfg, const char *name, float *floatptr);
static int config_setting_lookup_simple_float(const config_setting_t *cfg, const char *name, float *floatptr);
static int _config_setting_get_simple_float(const config_setting_t *cfg_item, float *floatptr);

static int config_lookup_strdup(const config_t *cfg, const char *name, char **strptr);
static int config_setting_lookup_strdup(const config_setting_t *cfg, const char *name, char **strptr);
static int _config_setting_strdup_string(const config_setting_t *cfg_item, char **strptr);

static int config_lookup_unsigned_int(const config_t *cfg, const char *name, unsigned int *ptr);
static int config_setting_lookup_unsigned_int(const config_setting_t *cfg, const char *name, unsigned int *ptr);
static int _config_setting_get_unsigned_int(const config_setting_t *cfg_item, unsigned int *ptr);

static void cleanup_config(void);
static void load_config(void);
static void load_fallback_config(void);
static void load_misc(config_t *cfg);
static void load_fonts(config_t *cfg);
static void load_functionality(config_t *cfg);
static void load_mouse_cursor(config_t *cfg);
static void load_window_icon(config_t *cfg);
static unsigned int parse_cursor_shape(const char *string);

static wchar_t *char_to_wchar(const char *string);
static wchar_t *wcsdup(const wchar_t *string);

static int parse_byteorder(const char *string);
static unsigned int parse_modkey(const char *string);
static int parse_understyle(const char *string);

void
set_config_path(const char* filename, char *config_path, char *config_file)
{
	const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
	const char *home = getenv("HOME");

	if (xdg_config_home && xdg_config_home[0] != '\0') {
		snprintf(config_path, PATH_MAX, "%s/%s/", xdg_config_home, progname);
	} else if (home) {
		snprintf(config_path, PATH_MAX, "%s/.config/%s/", home, progname);
	} else {
		return;
	}

	snprintf(config_file, PATH_MAX, "%s/%s.cfg", config_path, filename);
}

int
setting_length(const config_setting_t *cfg)
{
	if (!cfg)
		return 0;

	switch (config_setting_type(cfg)) {
	case CONFIG_TYPE_GROUP:
	case CONFIG_TYPE_LIST:
	case CONFIG_TYPE_ARRAY:
		return config_setting_length(cfg);
	}

	return 1;
}

const char *
setting_get_string_elem(const config_setting_t *cfg, int i)
{
	if (!cfg)
		return NULL;

	switch (config_setting_type(cfg)) {
	case CONFIG_TYPE_GROUP:
	case CONFIG_TYPE_LIST:
	case CONFIG_TYPE_ARRAY:
		return config_setting_get_string_elem(cfg, i);
	}

	return config_setting_get_string(cfg);
}

int
setting_get_int_elem(const config_setting_t *cfg, int i)
{
	if (!cfg)
		return 0;

	switch (config_setting_type(cfg)) {
	case CONFIG_TYPE_GROUP:
	case CONFIG_TYPE_LIST:
	case CONFIG_TYPE_ARRAY:
		return config_setting_get_int_elem(cfg, i);
	}

	return config_setting_get_int(cfg);
}

const config_setting_t *
setting_get_elem(const config_setting_t *cfg, int i)
{
	if (!cfg)
		return NULL;

	switch (config_setting_type(cfg)) {
	case CONFIG_TYPE_GROUP:
	case CONFIG_TYPE_LIST:
	case CONFIG_TYPE_ARRAY:
		return config_setting_get_elem(cfg, i);
	}

	return cfg;
}

int
config_lookup_sloppy_bool(const config_t *cfg, const char *name, int *ptr)
{
	return _config_setting_get_sloppy_bool(config_lookup(cfg, name), ptr);
}

int
config_setting_lookup_sloppy_bool(const config_setting_t *cfg, const char *name, int *ptr)
{
	return _config_setting_get_sloppy_bool(config_setting_lookup(cfg, name), ptr);
}

int
_config_setting_get_sloppy_bool(const config_setting_t *cfg_item, int *ptr)
{
	const char *string;

	if (!cfg_item)
		return 0;

	switch (config_setting_type(cfg_item)) {
	case CONFIG_TYPE_INT:
		*ptr = config_setting_get_int(cfg_item);
		return 1;
	case CONFIG_TYPE_STRING:
		string = config_setting_get_string(cfg_item);

		if (string && strlen(string)) {
			char a = tolower(string[0]);
			/* Match for positives like "true", "yes" and "on" */
			*ptr = (a == 't' || a == 'y' || !strcasecmp(string, "on"));
			return 1;
		}
		break;
	case CONFIG_TYPE_BOOL:
		*ptr = config_setting_get_bool(cfg_item);
		return 1;
	}

	return 0;
}

int
config_lookup_unsigned_int(const config_t *cfg, const char *name, unsigned int *ptr)
{
	return _config_setting_get_unsigned_int(config_lookup(cfg, name), ptr);
}

int
config_setting_lookup_unsigned_int(const config_setting_t *cfg, const char *name, unsigned int *ptr)
{
	return _config_setting_get_unsigned_int(config_setting_lookup(cfg, name), ptr);
}

int
_config_setting_get_unsigned_int(const config_setting_t *cfg_item, unsigned int *ptr)
{
	if (!cfg_item)
		return 0;

	int integer = config_setting_get_int(cfg_item);

	if (integer < 0)
		return 0;

	*ptr = (unsigned int)integer;
	return 1;
}

int
config_lookup_strdup(const config_t *cfg, const char *name, char **strptr)
{
	return _config_setting_strdup_string(config_lookup(cfg, name), strptr);
}


int
config_setting_lookup_strdup(const config_setting_t *cfg, const char *name, char **strptr)
{
	return _config_setting_strdup_string(config_setting_lookup(cfg, name), strptr);
}

int
_config_setting_strdup_string(const config_setting_t *cfg_item, char **strptr)
{
	if (!cfg_item)
		return 0;

	const char *string = config_setting_get_string(cfg_item);

	if (!string)
		return 0;

	free(*strptr);
	*strptr = strdup(string);
	return 1;
}

int
config_lookup_wcsdup(const config_t *cfg, const char *name, wchar_t **wcsptr)
{
	return _config_setting_wcsdup_string(config_lookup(cfg, name), wcsptr);
}

int
config_setting_lookup_wcsdup(const config_setting_t *cfg, const char *name, wchar_t **wcsptr)
{
	return _config_setting_wcsdup_string(config_setting_lookup(cfg, name), wcsptr);
}

int
_config_setting_wcsdup_string(const config_setting_t *cfg_item, wchar_t **wcsptr)
{
	if (!cfg_item)
		return 0;

	const char *string = config_setting_get_string(cfg_item);

	if (!string)
		return 0;

	free(*wcsptr);
	*wcsptr = NULL;
	*wcsptr = char_to_wchar(string);
	return wcsptr != NULL;
}

int
config_lookup_simple_float(const config_t *cfg, const char *name, float *floatptr)
{
	return _config_setting_get_simple_float(config_lookup(cfg, name), floatptr);
}

int
config_setting_lookup_simple_float(const config_setting_t *cfg, const char *name, float *floatptr)
{
	return _config_setting_get_simple_float(config_setting_lookup(cfg, name), floatptr);
}

int
_config_setting_get_simple_float(const config_setting_t *cfg_item, float *floatptr)
{
	if (!cfg_item)
		return 0;

	double value = config_setting_get_float(cfg_item);

	*floatptr = (float)value;
	return 1;
}

void
load_config(void)
{
	config_t cfg;
	char config_path[PATH_MAX] = {0};
	char config_file[PATH_MAX] = {0};

	set_config_path(progname, config_path, config_file);
	config_init(&cfg);
	config_set_include_dir(&cfg, config_path);

	if (config_read_file(&cfg, config_file)) {
		load_misc(&cfg);
		load_fonts(&cfg);
		load_functionality(&cfg);
		load_mouse_cursor(&cfg);
		load_window_icon(&cfg);
	} else if (strcmp(config_error_text(&cfg), "file I/O error")) {
		fprintf(stderr, "Error reading config at %s\n", config_file);
		fprintf(stderr, "%s:%d - %s\n",
			config_error_file(&cfg),
			config_error_line(&cfg),
			config_error_text(&cfg)
		);
	}

	load_fallback_config();
	config_destroy(&cfg);
}

void
cleanup_config(void)
{
	int i;

	/* Cleanup fonts */
	for (i = 0; i < num_fonts; i++)
		free(fonts[i]);
	free(fonts);

	free(ascii_printable);
	free(initial_working_directory);
	free(iso14755cmd);
	free(mouseshape_text);
	free(plumb_cmd);
	free(scroll);
	free(shell);
	free(stty_args);
	free(termname);
	free(utmp);
	free(window_icon);
	free(xdndescchar);
	free(kbds_sdelim);
	free(kbds_ldelim);
	free(worddelimiters);
	/* TODO consider what needs to be done to preserve the history if
	 * we were to introduce live reload of config during runtime. */
	free(term.hist);
}

void
load_fallback_config(void)
{
	int i;

	if (!fonts) {
		num_fonts = 1 + LEN(font2);
		fonts = calloc(num_fonts, sizeof(char*));
		fonts[0] = strdup(font);
		for (i = 1; i < num_fonts; i++) {
			fonts[i] = strdup(font2[i - 1]);
		}
	}

	if (!ascii_printable)
		ascii_printable = strdup(ascii_printable_def);
	if (!iso14755cmd)
		iso14755cmd = strdup(iso14755cmd_def);
	if (!plumb_cmd)
		plumb_cmd = strdup(plumb_cmd_def);
	if (!shell)
		shell = strdup("/bin/sh");
	if (!stty_args)
		stty_args = strdup(stty_def_args);
	if (!termname)
		termname = strdup(terminal_name);
	if (!url_opener_cmd)
		url_opener_cmd = strdup(url_opener_def);
	if (!worddelimiters)
		worddelimiters = wcsdup(worddelimiters_def);
	if (!kbds_sdelim)
		kbds_sdelim = wcsdup(kbds_sdelim_def);
	if (!kbds_ldelim)
		kbds_ldelim = wcsdup(kbds_ldelim_def);
	if (!xdndescchar)
		xdndescchar = strdup(xdndescchar_def);


	term.hist = calloc(histsize, sizeof(Line));
}

void
load_misc(config_t *cfg)
{
	const char *string;

	config_lookup_int(cfg, "border.percent", &borderperc);
	config_lookup_int(cfg, "border.width", &borderpx);
	config_lookup_strdup(cfg, "ascii_printable", &ascii_printable);
	config_lookup_strdup(cfg, "initial_working_directory", &initial_working_directory);
	config_lookup_strdup(cfg, "iso14755_cmd", &iso14755cmd);
	config_lookup_strdup(cfg, "plumb_cmd", &plumb_cmd);
	config_lookup_strdup(cfg, "shell", &shell);
	config_lookup_strdup(cfg, "utmp", &utmp);
	config_lookup_strdup(cfg, "url_opener_cmd", &url_opener_cmd);
	config_lookup_strdup(cfg, "scroll", &scroll);
	config_lookup_strdup(cfg, "stty_args", &stty_args);
	config_lookup_strdup(cfg, "drag_and_drop_escape_characters", &xdndescchar);

	config_lookup_wcsdup(cfg, "word_delimiters", &worddelimiters);
	config_lookup_wcsdup(cfg, "keyboardselect.short_delimiter", &kbds_sdelim);
	config_lookup_wcsdup(cfg, "keyboardselect.long_delimiter", &kbds_ldelim);

	if (config_lookup_string(cfg, "sixelbyteorder", &string)) {
		sixelbyteorder = parse_byteorder(string);
	}

	if (config_lookup_string(cfg, "url_opener_modkey", &string)) {
		url_opener_modkey = parse_modkey(string);
	}

	config_lookup_simple_float(cfg, "cwscale", &cwscale);
	config_lookup_simple_float(cfg, "chscale", &chscale);

	config_lookup_unsigned_int(cfg, "double_click_timeout", &doubleclicktimeout);
	config_lookup_unsigned_int(cfg, "triple_click_timeout", &tripleclicktimeout);
	config_lookup_unsigned_int(cfg, "synchronized_update_timeout", &su_timeout);

	config_lookup_float(cfg, "minlatency", &minlatency);
	config_lookup_float(cfg, "maxlatency", &maxlatency);
	config_lookup_int(cfg, "bell_volume", &bellvolume);

	config_lookup_unsigned_int(cfg, "terminal_size.cols", &cols);
	config_lookup_unsigned_int(cfg, "terminal_size.rows", &rows);
	config_lookup_unsigned_int(cfg, "terminal_size.width", &width);
	config_lookup_unsigned_int(cfg, "terminal_size.height", &height);
	if (width && height)
		geometry = PixelGeometry;

	config_lookup_int(cfg, "scrollback_history", &histsize);

	config_lookup_unsigned_int(cfg, "terminal_cursor.blink_timeout", &blinktimeout);
	config_lookup_unsigned_int(cfg, "terminal_cursor.thickness", &cursorthickness);
	config_lookup_unsigned_int(cfg, "terminal_cursor.style", &cursorstyle);
	config_lookup_unsigned_int(cfg, "terminal_cursor.custom_cursor", &stcursor);

	config_lookup_simple_float(cfg, "alpha.focused", &alpha);
	config_lookup_simple_float(cfg, "alpha.unfocused", &alpha_unfocused);
	config_lookup_simple_float(cfg, "alpha.gradient_minimum", &gradient_constant);

	if (config_lookup_string(cfg, "undercurl_style", &string)) {
		undercurl_style = parse_understyle(string);
	}
}

void
load_fonts(config_t *cfg)
{
	int i;
	const char *string;
	config_setting_t *fonts_t, *font;

	fonts_t = config_lookup(cfg, "fonts");
	if (!fonts_t)
		return;

	num_fonts = setting_length(fonts_t);
	fonts = calloc(num_fonts, sizeof(char*));

	for (i = 0; i < num_fonts; i++) {
		fonts[i] = strdup(setting_get_string_elem(fonts_t, i));
	}
}

void
load_functionality(config_t *cfg)
{
	int i, enabled;

	config_setting_t *func_t = config_lookup(cfg, "functionality");
	if (!func_t)
		return;

	for (i = 0; function_names[i].name != NULL; i++) {
		if (config_setting_lookup_sloppy_bool(func_t, function_names[i].name, &enabled)) {
			setenabled(function_names[i].value, enabled);
		}
	}
}

void
load_mouse_cursor(config_t *cfg)
{
	const char *string;
	config_setting_t *shape_t = config_lookup(cfg, "mouse_cursor.shape");
	if (!shape_t)
		return;

	switch (config_setting_type(shape_t)) {
	case CONFIG_TYPE_INT:
		_config_setting_get_unsigned_int(shape_t, &mouseshape);
		break;
	case CONFIG_TYPE_STRING:
		string = config_setting_get_string(shape_t);
		if (startswith("XC_", string))
			string += 3;

		if (strlen(string)) {
			mouseshape = parse_cursor_shape(string);
			mouseshape_text = strdup(string);
		}

		break;
	}
}

void
load_window_icon(config_t *cfg)
{
	const char *string;

	if (!config_lookup_string(cfg, "window_icon", &string))
		return;

	window_icon = expandhome(string);
}

wchar_t *
char_to_wchar(const char *string)
{
	size_t len;
	wchar_t *wchar_array;

	setlocale(LC_ALL, "");

	len = mbstowcs(NULL, string, 0);
	if (len == (size_t)-1) {
		fprintf(stderr, "char_to_wchar: conversion length error for string \"%s\"\n", string);
		return NULL;
	}

	wchar_array = calloc(len + 1, sizeof(wchar_t));
	if (!wchar_array) {
		fprintf(stderr, "char_to_wchar: failed to allocate memory for conversion of \"%s\"\n", string);
		return NULL;
	}

	if (mbstowcs(wchar_array, string, len + 1) == (size_t)-1) {
		fprintf(stderr, "char_to_wchar: failed to convert string \"%s\"\n", string);
		free(wchar_array);
		return NULL;
	}

	return wchar_array;
}

wchar_t *
wcsdup(const wchar_t *string)
{
	size_t len = wcslen(string);
	wchar_t *dup = calloc(len + 1, sizeof(wchar_t));
	if (dup) {
		wcscpy(dup, string);
	}
	return dup;
}

#define map(S, I) if (!strcasecmp(string, S)) return I;

int
parse_byteorder(const char *string)
{
	map("LSBFirst", LSBFirst);
	map("MSBFirst", MSBFirst);

	fprintf(stderr, "Warning: config could not find byte order with name %s\n", string);
	return LSBFirst;
}

unsigned int
parse_cursor_shape(const char *string)
{
	map("arrow", XC_arrow);
	map("based_arrow_down", XC_based_arrow_down);
	map("based_arrow_up", XC_based_arrow_up);
	map("boat", XC_boat);
	map("bogosity", XC_bogosity);
	map("bottom_left_corner", XC_bottom_left_corner);
	map("bottom_right_corner", XC_bottom_right_corner);
	map("bottom_side", XC_bottom_side);
	map("bottom_tee", XC_bottom_tee);
	map("box_spiral", XC_box_spiral);
	map("center_ptr", XC_center_ptr);
	map("circle", XC_circle);
	map("clock", XC_clock);
	map("coffee_mug", XC_coffee_mug);
	map("cross", XC_cross);
	map("cross_reverse", XC_cross_reverse);
	map("crosshair", XC_crosshair);
	map("diamond_cross", XC_diamond_cross);
	map("dot", XC_dot);
	map("dotbox", XC_dotbox);
	map("double_arrow", XC_double_arrow);
	map("draft_large", XC_draft_large);
	map("draft_small", XC_draft_small);
	map("draped_box", XC_draped_box);
	map("exchange", XC_exchange);
	map("fleur", XC_fleur);
	map("gobbler", XC_gobbler);
	map("gumby", XC_gumby);
	map("hand1", XC_hand1);
	map("hand2", XC_hand2);
	map("heart", XC_heart);
	map("icon", XC_icon);
	map("iron_cross", XC_iron_cross);
	map("left_ptr", XC_left_ptr);
	map("left_side", XC_left_side);
	map("left_tee", XC_left_tee);
	map("leftbutton", XC_leftbutton);
	map("ll_angle", XC_ll_angle);
	map("lr_angle", XC_lr_angle);
	map("man", XC_man);
	map("middlebutton", XC_middlebutton);
	map("mouse", XC_mouse);
	map("pencil", XC_pencil);
	map("pirate", XC_pirate);
	map("plus", XC_plus);
	map("question_arrow", XC_question_arrow);
	map("right_ptr", XC_right_ptr);
	map("right_side", XC_right_side);
	map("right_tee", XC_right_tee);
	map("rightbutton", XC_rightbutton);
	map("rtl_logo", XC_rtl_logo);
	map("sailboat", XC_sailboat);
	map("sb_down_arrow", XC_sb_down_arrow);
	map("sb_h_double_arrow", XC_sb_h_double_arrow);
	map("sb_left_arrow", XC_sb_left_arrow);
	map("sb_right_arrow", XC_sb_right_arrow);
	map("sb_up_arrow", XC_sb_up_arrow);
	map("sb_v_double_arrow", XC_sb_v_double_arrow);
	map("shuttle", XC_shuttle);
	map("sizing", XC_sizing);
	map("spider", XC_spider);
	map("spraycan", XC_spraycan);
	map("star", XC_star);
	map("target", XC_target);
	map("tcross", XC_tcross);
	map("top_left_arrow", XC_top_left_arrow);
	map("top_left_corner", XC_top_left_corner);
	map("top_right_corner", XC_top_right_corner);
	map("top_side", XC_top_side);
	map("top_tee", XC_top_tee);
	map("trek", XC_trek);
	map("ul_angle", XC_ul_angle);
	map("umbrella", XC_umbrella);
	map("ur_angle", XC_ur_angle);
	map("watch", XC_watch);
	map("xterm", XC_xterm);

	return XC_xterm;
}

unsigned int
parse_modkey(const char *string)
{
	map("Any", XK_ANY_MOD);
	map("ANY_MOD", XK_ANY_MOD);
	map("XK_ANY_MOD", XK_ANY_MOD);
	map("Ctrl", ControlMask);
	map("ControlMask", ControlMask);
	map("Shift", ShiftMask);
	map("ShiftMask", ShiftMask);
	map("Alt", Mod1Mask);
	map("Mod1Mask", Mod1Mask);
	map("Super", Mod4Mask);
	map("Mod4Mask", Mod4Mask);

	fprintf(stderr, "Warning: config could not find modkey with name %s\n", string);
	return XK_ANY_MOD;
}

int
parse_understyle(const char *string)
{
	if (!strncasecmp(string, "UNDERCURL_", 10))
		string += 10;

	map("CURLY", UNDERCURL_CURLY);
	map("SPIKY", UNDERCURL_SPIKY);
	map("CAPPED", UNDERCURL_CAPPED);

	fprintf(stderr, "Warning: config could not find understyle with name %s\n", string);
	return UNDERCURL_CURLY;
}

#undef map



/* Not done

# background image patch
bgfile
pseudotransparency

vtiden - because of trouble with the octal \033, st was very slow to parse UTF-8-demo.txt

url_opener_modkey



# sync patch
su_timeout

tabspaces - because you need to change st.info as well, doesn't make sense as a runtime config

# colorname (have to decide how to handle colors)


defaultbg
bg
bgUnfocused
defaultfg
defaultcs
defaultrcs

# selection colors
selectionfg
selectionbg


# keyboardselect
highlightfg
highlightbg

mousefg
mousebg


defaultattr

# xresources

forcemousemod

mshortcuts <-- will be a big one
shortcuts

# externalpipe
openurlcmd

#externalpipein
setbgcolorcmd


ignoremod


selmasks

*/