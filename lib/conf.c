#include <libconfig.h>
#include <strings.h>
#include <ctype.h>
#include <wchar.h>

const char *progname = "st";

/* Configuration variables */
char **fonts = NULL;
int num_fonts;
char *shell = NULL;
char *utmp = NULL;
char *scroll = NULL;
char *stty_args = NULL;
char *termname = NULL;
wchar_t *worddelimiters = NULL;
wchar_t *kbds_sdelim = NULL;
wchar_t *kbds_ldelim = NULL;
int histsize = 2000;
uint64_t settings = 0;

void set_config_path(const char* filename, char *config_path, char *config_file);
int setting_length(const config_setting_t *cfg);
const char *setting_get_string_elem(const config_setting_t *cfg, int i);
int setting_get_int_elem(const config_setting_t *cfg, int i);
const config_setting_t *setting_get_elem(const config_setting_t *cfg, int i);

int config_lookup_sloppy_bool(const config_t *cfg, const char *name, int *ptr);
int config_setting_lookup_sloppy_bool(const config_setting_t *cfg, const char *name, int *ptr);
int _config_setting_get_sloppy_bool(const config_setting_t *cfg, int *ptr);

int config_lookup_wcsdup(const config_t *cfg, const char *name, wchar_t **wcsptr);
int config_setting_lookup_wcsdup(const config_setting_t *cfg, const char *name, wchar_t **wcsptr);
int _config_setting_wcsdup_string(const config_setting_t *cfg_item, wchar_t **wcsptr);

int config_lookup_simple_float(const config_t *cfg, const char *name, float *floatptr);
int config_setting_lookup_simple_float(const config_setting_t *cfg, const char *name, float *floatptr);
int _config_setting_get_simple_float(const config_setting_t *cfg_item, float *floatptr);

int config_lookup_strdup(const config_t *cfg, const char *name, char **strptr);
int config_setting_lookup_strdup(const config_setting_t *cfg, const char *name, char **strptr);
int _config_setting_strdup_string(const config_setting_t *cfg_item, char **strptr);

int config_lookup_unsigned_int(const config_t *cfg, const char *name, unsigned int *ptr);
int config_setting_lookup_unsigned_int(const config_setting_t *cfg, const char *name, unsigned int *ptr);
int _config_setting_get_unsigned_int(const config_setting_t *cfg_item, unsigned int *ptr);

void cleanup_config(void);
void load_config(void);
void load_fallback_config(void);
void load_misc(config_t *cfg);
void load_fonts(config_t *cfg);
void load_functionality(config_t *cfg);

wchar_t *char_to_wchar(const char *string);
wchar_t *wcsdup(const wchar_t *string);

int parse_byteorder(const char *string);

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

	free(shell);
	free(utmp);
	free(scroll);
	free(stty_args);
	free(worddelimiters);
	free(kbds_sdelim);
	free(kbds_ldelim);
	free(termname);
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

	if (!shell)
		shell = strdup("/bin/sh");
	if (!stty_args)
		stty_args = strdup(stty_def_args);
	if (!worddelimiters)
		worddelimiters = wcsdup(worddelimiters_def);
	if (!kbds_sdelim)
		kbds_sdelim = wcsdup(kbds_sdelim_def);
	if (!kbds_ldelim)
		kbds_ldelim = wcsdup(kbds_ldelim_def);

	if (!termname)
		termname = strdup(terminal_name);

	term.hist = calloc(histsize, sizeof(Line));
}

void
load_misc(config_t *cfg)
{
	const char *string;

	config_lookup_int(cfg, "border.percent", &borderperc);
	config_lookup_int(cfg, "border.width", &borderpx);
	config_lookup_strdup(cfg, "shell", &shell);
	config_lookup_strdup(cfg, "utmp", &utmp);
	config_lookup_strdup(cfg, "scroll", &scroll);
	config_lookup_strdup(cfg, "stty_args", &stty_args);
	config_lookup_wcsdup(cfg, "word_delimiters", &worddelimiters);
	config_lookup_wcsdup(cfg, "keyboardselect.short_delimiter", &kbds_sdelim);
	config_lookup_wcsdup(cfg, "keyboardselect.long_delimiter", &kbds_ldelim);

	if (config_lookup_string(cfg, "sixelbyteorder", &string)) {
		sixelbyteorder = parse_byteorder(string);
	}

	config_lookup_simple_float(cfg, "cwscale", &cwscale);
	config_lookup_simple_float(cfg, "chscale", &chscale);

	config_lookup_unsigned_int(cfg, "double_click_timeout", &doubleclicktimeout);
	config_lookup_unsigned_int(cfg, "triple_click_timeout", &tripleclicktimeout);

	config_lookup_sloppy_bool(cfg, "allowaltscreen", &allowaltscreen);
	config_lookup_sloppy_bool(cfg, "allowwindowops", &allowwindowops);
	config_lookup_float(cfg, "minlatency", &minlatency);
	config_lookup_float(cfg, "maxlatency", &maxlatency);
	config_lookup_unsigned_int(cfg, "blink_timeout", &blinktimeout);
	config_lookup_unsigned_int(cfg, "cursor_thickness", &cursorthickness);
//	config_lookup_sloppy_bool(cfg, "hide_cursor", &hidecursor);
	config_lookup_int(cfg, "bell_volume", &bellvolume);
	config_lookup_unsigned_int(cfg, "cols", &cols);
	config_lookup_unsigned_int(cfg, "rows", &rows);
	config_lookup_int(cfg, "scrollback_history", &histsize);



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
		config_setting_lookup_sloppy_bool(func_t, function_names[i].name, &enabled);
		if (enabled) {
			enablefunc(function_names[i].value);
		} else {
			disablefunc(function_names[i].value);
		}
	}
}

#undef map

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

#undef map



/* Not done

# background image patch
bgfile
pseudotransparency

vtiden - because of trouble with the octal \033, st was very slow to parse UTF-8-demo.txt

# openurlonclick
url_opener_modkey
url_opener


# sync patch
su_timeout

# hidecursor (decide between this and swapmouse?)
hidecursor

tabspaces - because you need to change st.info as well, doesn't make sense as a runtime config

#alpha
alpha
grad_alpha
stat_alpha
alphaUnfocused

#drag and drop
xdndescchar

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
ignoreselfg


# keyboardselect
highlightfg
highlightbg

# blinking cursor
cursorstyle
stcursor <--- Rune, will be interesting

# Default shape of cursor
# 2: Block ("█")
# 4: Underline ("_")
# 6: Bar ("|")
# 7: Snowman ("☃")

cursorshape - integers, may consider string parsing?


# anygeometry
geometry - CellGeometry or PixelGeometry
width
height

# themed cursor
mouseshape

mouseshape <-- name conflict
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

#fixkeyboardinput
mappedkeys

ignoremod
key <--- will be a big one


selmasks
ascii_printable

# right click to plumb
plumb_cmd

# undercurl
#define UNDERCURL_CURLY 0
#define UNDERCURL_SPIKY 1
#define UNDERCURL_CAPPED 2
// Active style
#define UNDERCURL_STYLE UNDERCURL_SPIKY


*/