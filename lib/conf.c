#include <libconfig.h>
#include <strings.h>
#include <ctype.h>
#include <wchar.h>

const char *progname = "st";

/* Configuration variables */
char **fonts = NULL;
int num_fonts;
char *ascii_printable = NULL;
char **colors = NULL;
char *initial_working_directory = NULL;
char *mouseshape_text = NULL;
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
ResourcePref *resources = NULL;
Command *commands = NULL;
MouseShortcut *mouse_bindings = NULL;
Shortcut *keybindings = NULL;
int histsize = 0;
int histsize_max = 100000;
int histsize_incr = 2000;
uint64_t settings = 0;
int num_colors = 0;
int num_resources = 0;
int num_commands = 0;
int num_mouse_bindings = 0;
int num_keybindings = 0;
int reload = 0;

/* Fixed indexes for additional colors, used in the context of Xresources */
unsigned int defaultcs_idx;
unsigned int defaultrcs_idx;
unsigned int defaultbg_idx;
unsigned int defaultfg_idx;
unsigned int selectionfg_idx;
unsigned int selectionbg_idx;
unsigned int highlightfg_idx;
unsigned int highlightbg_idx;
unsigned int focusedbg_idx;
unsigned int unfocusedbg_idx;
unsigned int badattributefg_idx;

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
static void reload_config(const Arg *arg);
static void load_commands(config_t *cfg);
static void load_fallback_config(void);
static void load_misc(config_t *cfg);
static void load_colors(config_t *cfg);
static int load_additional_color(const config_t *cfg, const char *name, int *idx);
static void load_fonts(config_t *cfg);
static void load_functionality(config_t *cfg);
static void load_mouse_cursor(config_t *cfg);
static void load_mouse_bindings(config_t *cfg);
static void load_keybindings(config_t *cfg);
static void load_window_icon(config_t *cfg);

static void generate_resource_strings(void);
static void add_resource_binding(const char *string, void *ptr);

static wchar_t *char_to_wchar(const char *string);
static wchar_t *wcsdup(const wchar_t *string);

static unsigned int parse_cursor_shape(const char *string);
static int parse_byteorder(const char *string);
static unsigned int parse_modkey(const char *string);
static unsigned int parse_modifier(const char *string);
static int parse_screen(const config_setting_t *screen_t);
static int parse_understyle(const char *string);
static ArgFunc parse_function(const char *string);
static void parse_and_set_argument(const config_setting_t *arg_t, Arg *arg);
static int parse_command_reference(const char *string, void **ptr);

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
		load_functionality(&cfg);
		load_misc(&cfg);
		load_colors(&cfg);
		load_commands(&cfg);
		load_fonts(&cfg);
		load_keybindings(&cfg);
		load_mouse_bindings(&cfg);
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
	generate_resource_strings();
	config_destroy(&cfg);
}

void
reload_config(const Arg *arg)
{
	reload = 1;
	cleanup_config();
	load_config();
	if (enabled(Xresources)) {
		reload_xresources(0);
	}
	reload = 0;
}

void
load_commands(config_t *cfg)
{
	int i, j, num_cmd_elements;
	config_setting_t *commands_list, *command_entry, *command;

	commands_list = config_lookup(cfg, "commands");
	if (!commands_list || !config_setting_is_list(commands_list))
		return;

	num_commands = config_setting_length(commands_list);
	if (!num_commands)
		return;

	commands = calloc(num_commands, sizeof(Command));
	for (i = 0; i < num_commands; i++) {
		command_entry = config_setting_get_elem(commands_list, i);
		commands[i].name = NULL;
		commands[i].argv = NULL;

		config_setting_lookup_strdup(command_entry, "name", &commands[i].name);

		command = config_setting_lookup(command_entry, "command");
		num_cmd_elements = config_setting_length(command);
		commands[i].argv = calloc(num_cmd_elements + 1, sizeof(char*));

		for (j = 0; j < num_cmd_elements; j++) {
			commands[i].argv[j] = strdup(config_setting_get_string_elem(command, j));
		}
		commands[i].argv[j] = NULL;

		if (commands[i].name == NULL || commands[i].argv == NULL) {
			fprintf(stderr, "Warning: config found incomplete command at line %d\n", config_setting_source_line(command_entry));
		}
	}
}

#define FREENULL(A) free(A); A = NULL;
void
cleanup_config(void)
{
	int i;

	/* Cleanup fonts */
	for (i = 0; i < num_fonts; i++)
		free(fonts[i]);
	FREENULL(fonts);


	FREENULL(ascii_printable);
	FREENULL(initial_working_directory);
	FREENULL(mouseshape_text);
	FREENULL(scroll);
	FREENULL(shell);
	FREENULL(stty_args);
	FREENULL(termname);
	FREENULL(url_opener_cmd);
	FREENULL(utmp);
	FREENULL(window_icon);
	FREENULL(xdndescchar);
	FREENULL(kbds_sdelim);
	FREENULL(kbds_ldelim);
	FREENULL(worddelimiters);

	for (i = 0; i < num_colors; i++)
		free(colors[i]);
	FREENULL(colors);

	for (i = 0; i < num_resources; i++)
		free(resources[i].name);
	FREENULL(resources);

	/* Note that we do not free strings passed as arguments here; we
	 * do not have a way to track whether an argument refers to a string
	 * pointer or not */
	FREENULL(keybindings);
	FREENULL(mouse_bindings);
}

#undef FREENULL

void
load_fallback_config(void)
{
	int i, num_def;

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

	if (!colors) {
		num_colors = LEN(colorname);
		colors = calloc(num_colors, sizeof(char*));
	}

	/* Fall back to default color settings if not defined via runtime configuration */
	num_def = LEN(colorname);
	for (i = 0; i < num_colors; i++) {
		if (!colors[i]) {
			if (i >= num_def || !colorname[i]) {
				colors[i] = strdup("#FF41AE"); /* Fluorescent pink to indicate config error */
			} else {
				colors[i] = strdup(colorname[i]);
			}
		}
		/* Skip generated colors */
		if (i == 15)
			i = 255;
	}

	if (!keybindings) {
		num_keybindings = LEN(shortcuts);
		keybindings = calloc(num_keybindings, sizeof(Shortcut));
		for (i = 0; i < num_keybindings; i++) {
			keybindings[i].mod = shortcuts[i].mod;
			keybindings[i].keysym = shortcuts[i].keysym;
			keybindings[i].screen = shortcuts[i].screen;
			keybindings[i].func = shortcuts[i].func;
			keybindings[i].arg.v = shortcuts[i].arg.v;
		}
	}

	if (!mouse_bindings) {
		num_mouse_bindings = LEN(mshortcuts);
		mouse_bindings = calloc(num_mouse_bindings, sizeof(MouseShortcut));
		for (i = 0; i < num_mouse_bindings; i++) {
			mouse_bindings[i].mod = mshortcuts[i].mod;
			mouse_bindings[i].button = mshortcuts[i].button;
			mouse_bindings[i].screen = mshortcuts[i].screen;
			mouse_bindings[i].release = mshortcuts[i].release;
			mouse_bindings[i].func = mshortcuts[i].func;
			mouse_bindings[i].arg.v = mshortcuts[i].arg.v;
		}
	}

	if (!reload) {
		term.hist = calloc(histsize_incr, sizeof(Line));
		histsize = histsize_incr;
	}
}

void
generate_resource_strings(void)
{
	char resource_name[16] = {0};

	/* 16 + 11 (cs, rcs, fg, bg, sel fg, sel bg, hl fg, hl bg, foc, unfoc, def attr) */
	resources = calloc(16 + 11, sizeof(ResourcePref));

	while (num_resources < 16) {
		snprintf(resource_name, 16, "color%d", num_resources);
		add_resource_binding(resource_name, &colors[num_resources]);
	}

	/* Add additional resource strings */
	add_resource_binding("cursor", &colors[defaultcs_idx]);
	add_resource_binding("cursor.reverse", &colors[defaultrcs_idx]);
	add_resource_binding("background", &colors[defaultbg_idx]);
	add_resource_binding("foreground", &colors[defaultfg_idx]);
	add_resource_binding("selection.fg.color", &colors[selectionfg_idx]);
	add_resource_binding("selection.bg.color", &colors[selectionbg_idx]);
	add_resource_binding("highlight.fg.color", &colors[highlightfg_idx]);
	add_resource_binding("highlight.bg.color", &colors[highlightbg_idx]);
	add_resource_binding("focused.bg.color", &colors[focusedbg_idx]);
	add_resource_binding("unfocused.bg.color", &colors[unfocusedbg_idx]);
	add_resource_binding("badattribute.fg.color", &colors[badattributefg_idx]);
}

void
add_resource_binding(const char *string, void *ptr)
{
	resources[num_resources].name = strdup(string);
	resources[num_resources].type = STRING;
	resources[num_resources].dst = ptr;
	num_resources++;
}

void
load_misc(config_t *cfg)
{
	const char *string;

	config_lookup_int(cfg, "border.percent", &borderperc);
	config_lookup_int(cfg, "border.width", &borderpx);
	config_lookup_strdup(cfg, "ascii_printable", &ascii_printable);
	config_lookup_strdup(cfg, "initial_working_directory", &initial_working_directory);
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

	if (config_lookup_string(cfg, "url_opener_modifier", &string)) {
		url_opener_modkey = parse_modkey(string);
	}

	if (config_lookup_string(cfg, "modifiers_to_ignore", &string))
		ignoremod = parse_modifier(string);

	if (config_lookup_string(cfg, "rectangular_selection_modifier", &string)) {
		if (LEN(selmasks) > SEL_RECTANGULAR) {
			selmasks[SEL_RECTANGULAR] = parse_modkey(string);
		}
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

	config_lookup_int(cfg, "scrollback_history", &histsize_max);
	config_lookup_int(cfg, "scrollback_history_increment", &histsize_incr);

	config_lookup_unsigned_int(cfg, "terminal_cursor.blink_timeout", &blinktimeout);
	config_lookup_unsigned_int(cfg, "terminal_cursor.thickness", &cursorthickness);
	config_lookup_unsigned_int(cfg, "terminal_cursor.style", &cursorstyle);
	config_lookup_unsigned_int(cfg, "terminal_cursor.custom_cursor", &stcursor);

	config_lookup_simple_float(cfg, "alpha.focused", &alpha);
	config_lookup_simple_float(cfg, "alpha.unfocused", &alpha_unfocused);
	config_lookup_simple_float(cfg, "alpha.selection_background", &alpha_selection_background);
	config_lookup_simple_float(cfg, "alpha.gradient_minimum", &gradient_constant);

	if (config_lookup_string(cfg, "undercurl_style", &string)) {
		undercurl_style = parse_understyle(string);
	}
}

void
load_colors(config_t *cfg)
{
	int i;

	/* 256 color palette + cs, rcs, fg, bg, sel fg, sel bg, hl fg, hl bg, focused, unfocused, attr */
	num_colors = 256 + 11;
	colors = calloc(num_colors, sizeof(char*));

	char lookup_name[16];

	for (i = 0; i < 16; i++) {
		snprintf(lookup_name, 16, "colors.color%d", i);
		config_lookup_strdup(cfg, lookup_name, &colors[i]);
	}

	defaultcs_idx = defaultcs = 256;
	defaultrcs_idx = defaultrcs = 257;
	defaultbg_idx = defaultbg = 258;
	defaultfg_idx = defaultfg = 259;
	selectionfg_idx = selectionfg = 260;
	selectionbg_idx = selectionbg = 261;
	highlightfg_idx = highlightfg = 262;
	highlightbg_idx = highlightbg = 263;
	focusedbg_idx = focusedbg = 264;
	unfocusedbg_idx = unfocusedbg = 265;
	badattributefg_idx = badattributefg = 266;

	load_additional_color(cfg, "colors.cursor", &defaultcs);
	load_additional_color(cfg, "colors.cursor_reverse", &defaultrcs);
	load_additional_color(cfg, "colors.background", &defaultbg);
	load_additional_color(cfg, "colors.foreground", &defaultfg);
	load_additional_color(cfg, "colors.selection_foreground", &selectionfg);
	load_additional_color(cfg, "colors.selection_background", &selectionbg);
	load_additional_color(cfg, "colors.highlight_foreground", &highlightfg);
	load_additional_color(cfg, "colors.highlight_background", &highlightbg);
	load_additional_color(cfg, "colors.focused_background", &focusedbg);
	load_additional_color(cfg, "colors.unfocused_background", &unfocusedbg);
	load_additional_color(cfg, "colors.badattribute_foreground", &badattributefg);

	if (enabled(AlphaFocusHighlight)) {
		defaultbg = defaultbg_idx;
	}
}

#define map(S, I) \
	if (!strcasecmp(string, S)) { \
		colors[*idx] = strdup("#000000"); \
		*idx = I; \
		break; \
	}

int
load_additional_color(const config_t *cfg, const char *name, int *idx)
{
	const char *string;
	const config_setting_t *color_t = config_lookup(cfg, name);
	if (!color_t)
		return 0;

	switch (config_setting_type(color_t)) {
	case CONFIG_TYPE_INT:
		colors[*idx] = strdup("#000000");
		*idx = config_setting_get_int(color_t);
		break;
	case CONFIG_TYPE_STRING:
		string = config_setting_get_string(color_t);

		map("cursor", defaultcs);
		map("cursor_reverse", defaultrcs);
		map("background", defaultbg);
		map("foreground", defaultfg);
		map("selection_background", selectionbg);
		map("selection_foreground", selectionfg);
		map("highlight_background", highlightbg);
		map("highlight_foreground", highlightfg);
		map("focused_background", focusedbg);
		map("unfocused_background", unfocusedbg);
		map("badattribute_foreground", badattributefg);

		colors[*idx] = strdup(string);
		break;
	default:
		return 0;
	}

	return 1;
}

#undef map

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

	for (i = 0; functionality_names[i].name != NULL; i++) {
		if (config_setting_lookup_sloppy_bool(func_t, functionality_names[i].name, &enabled)) {
			setenabled(functionality_names[i].value, enabled);
		}
	}
}

void
load_keybindings(config_t *cfg)
{
	int i, keysym;
	const char *string;

	const config_setting_t *bindings_t, *binding_t;

	bindings_t = config_lookup(cfg, "keybindings");
	if (!bindings_t)
		return;

	num_keybindings = config_setting_length(bindings_t);
	keybindings = calloc(num_keybindings, sizeof(Shortcut));

	for (i = 0; i < num_keybindings; i++) {
		binding_t = config_setting_get_elem(bindings_t, i);
		if (config_setting_lookup_string(binding_t, "modifier", &string))
			keybindings[i].mod = parse_modifier(string);

		if (config_setting_lookup_string(binding_t, "key", &string)) {
			keysym = XStringToKeysym(string);

			/* Allow lower case keybindings to automatically be converted to upper
			 * case if the shift modifier is involved. An alternative would be to
			 * match on both in kpress, but this is simpler. */
			if (isalpha(keysym) && (keybindings[i].mod & ShiftMask))
				keysym = toupper(keysym);

			if (keysym == NoSymbol)
				fprintf(stderr, "Warning: config could not look up keysym for key %s\n", string);
			keybindings[i].keysym = keysym;
		}

		if (config_setting_lookup_string(binding_t, "function", &string))
			keybindings[i].func = parse_function(string);
		keybindings[i].screen = parse_screen(config_setting_lookup(binding_t, "screen"));
		parse_and_set_argument(config_setting_lookup(binding_t, "argument"), &keybindings[i].arg);
	}
}

void
load_mouse_bindings(config_t *cfg)
{
	int i;
	const char *string;

	const config_setting_t *bindings_t, *binding_t;

	bindings_t = config_lookup(cfg, "mouse_bindings");
	if (!bindings_t)
		return;

	num_mouse_bindings = config_setting_length(bindings_t);
	mouse_bindings = calloc(num_mouse_bindings, sizeof(MouseShortcut));

	for (i = 0; i < num_mouse_bindings; i++) {
		binding_t = config_setting_get_elem(bindings_t, i);
		if (config_setting_lookup_string(binding_t, "modifier", &string))
			mouse_bindings[i].mod = parse_modifier(string);
		config_setting_lookup_unsigned_int(binding_t, "button", &mouse_bindings[i].button);
		config_setting_lookup_sloppy_bool(binding_t, "release", &mouse_bindings[i].release);
		if (config_setting_lookup_string(binding_t, "function", &string))
			mouse_bindings[i].func = parse_function(string);
		mouse_bindings[i].screen = parse_screen(config_setting_lookup(binding_t, "screen"));
		parse_and_set_argument(config_setting_lookup(binding_t, "argument"), &mouse_bindings[i].arg);
	}
}

void
load_mouse_cursor(config_t *cfg)
{
	const char *string;
	config_setting_t *shape_t, *url_shape_t;

	shape_t = config_lookup(cfg, "mouse_cursor.shape");
	if (shape_t) {
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

	url_shape_t = config_lookup(cfg, "mouse_cursor.url_shape");
	if (url_shape_t) {
		switch (config_setting_type(url_shape_t)) {
		case CONFIG_TYPE_INT:
			_config_setting_get_unsigned_int(url_shape_t, &mouseshape_url);
			break;
		case CONFIG_TYPE_STRING:
			string = config_setting_get_string(url_shape_t);
			if (startswith("XC_", string))
				string += 3;

			if (strlen(string)) {
				mouseshape_url = parse_cursor_shape(string);
			}
			break;
		}
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
	map("No", XK_NO_MOD)
	map("None", XK_NO_MOD);
	map("XK_NO_MOD", XK_NO_MOD);
	map("XK_SWITCH_MOD", XK_SWITCH_MOD);
	map("Ctrl", ControlMask);
	map("Shift", ShiftMask);
	map("Alt", Mod1Mask);
	map("Super", Mod4Mask);
	map("Control", ControlMask);
	map("ControlMask", ControlMask);
	map("ShiftMask", ShiftMask);
	map("Mod1Mask", Mod1Mask);
	map("Mod2Mask", Mod2Mask);
	map("Mod3Mask", Mod3Mask);
	map("Mod4Mask", Mod4Mask);
	map("Mod5Mask", Mod5Mask);

	fprintf(stderr, "Warning: config could not find modkey with name \"%s\"\n", string);
	return XK_NO_MOD;
}

unsigned int
parse_modifier(const char *string)
{
	int i, len;
	unsigned int mask = 0;
	len = strlen(string) + 1;
	char buffer[len];
	strlcpy(buffer, string, len);

	const char *delims = "+-|:;, ";

	char *token = strtok(buffer, delims);
	while (token) {
		mask |= parse_modkey(token);
		token = strtok(NULL, delims);
	}

	return mask;
}

int
parse_screen(const config_setting_t *screen_t)
{
	const char *string;

	if (screen_t) {
		switch (config_setting_type(screen_t)) {
		case CONFIG_TYPE_INT:
			return config_setting_get_int(screen_t);
		case CONFIG_TYPE_STRING:
			string = config_setting_get_string(screen_t);
			if (startswith("S_", string))
				string += 2;

			if (!strncasecmp(string, "PRI", 3))
				return S_PRI;
			if (!strncasecmp(string, "ALT", 3))
				return S_ALT;
			if (!strncasecmp(string, "ALL", 3))
				return S_ALL;
			if (!strcasecmp(string, "BOTH"))
				return S_ALL;

			break;
		}
	}

	return S_ALL;
}

void
parse_and_set_argument(const config_setting_t *arg_t, Arg *arg)
{
	const char *string;

	if (arg_t) {
		switch (config_setting_type(arg_t)) {
		case CONFIG_TYPE_INT:
			arg->i = config_setting_get_int(arg_t);
			break;
		case CONFIG_TYPE_FLOAT:
			_config_setting_get_simple_float(arg_t, &arg->f);
			break;
		case CONFIG_TYPE_STRING:
			string = config_setting_get_string(arg_t);
			if (parse_command_reference(string, &arg->v))
				break;

			arg->s = strdup(string);
			break;
		}
	}
}

int
parse_command_reference(const char *string, void **ptr)
{
	int i;

	for (i = 0; i < num_commands; i++) {
		if (!strcasecmp(string, commands[i].name)) {
			*ptr = commands[i].argv;
			return 1;
		}
	}

	return 0;
}

ArgFunc
parse_function(const char *string)
{
	map("changealpha", changealpha);
	map("changealphaunfocused", changealphaunfocused);
	map("clipcopy", clipcopy);
	map("clippaste", clippaste);
	map("copyurl", copyurl);
	map("externalpipe_screen_out", externalpipe_screen_out);
	map("externalpipe_screen_in", externalpipe_screen_in);
	map("externalpipe_screen_inject", externalpipe_screen_inject);
	map("externalpipe_hist_out", externalpipe_hist_out);
	map("externalpipe_hist_in", externalpipe_hist_in);
	map("externalpipe_hist_inject", externalpipe_hist_inject);
	map("externalpipe_osc133_out", externalpipe_osc133_out);
	map("externalpipe_osc133_in", externalpipe_osc133_in);
	map("externalpipe_osc133_inject", externalpipe_osc133_inject);
	map("fullscreen", fullscreen);
	map("keyboard_select", keyboard_select);
	map("kscrolldown", kscrolldown);
	map("kscrollup", kscrollup);
	map("newterm", newterm);
	map("numlock", numlock);
	map("opencopied", opencopied);
	map("paste", paste);
	map("plumb", plumb);
	map("printscreen", printscreen);
	map("printsel", printsel);
	map("reload_config", reload_config);
	map("scrolltoprompt", scrolltoprompt);
	map("searchbackward", searchbackward);
	map("searchforward", searchforward);
	map("selopen", selopen);
	map("selpaste", selpaste);
	map("sendbreak", sendbreak);
	map("toggleprinter", toggleprinter);
	map("ttysend", ttysend);
	map("zoom", zoom);
	map("zoomabs", zoomabs);
	map("zoomreset", zoomreset);

	fprintf(stderr, "Warning: config could not find function with name \"%s\"\n", string);
	return NULL;
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

/* Configuration options that are not covered:
 *
 *    vtiden    - this is not something that general users should play around with
 *    tabspaces - because you need to change st.info as well, as such it doesn't make
 *                sense as a runtime config
 */
