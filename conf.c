#include <libconfig.h>

const char *progname = "st";

/* Configuration variables */
char **fonts = NULL;
int num_fonts;
char *shell = NULL;
char *utmp = NULL;
char *scroll = NULL;
char *stty_args = NULL;

void set_config_path(const char* filename, char *config_path, char *config_file);
int setting_length(const config_setting_t *cfg);
const char *setting_get_string_elem(const config_setting_t *cfg, int i);
int setting_get_int_elem(const config_setting_t *cfg, int i);
const config_setting_t *setting_get_elem(const config_setting_t *cfg, int i);
int config_lookup_strdup(const config_t *cfg, const char *name, char **strptr);
int config_setting_lookup_strdup(const config_setting_t *cfg, const char *name, char **strptr);
int config_lookup_unsigned_int(const config_t *cfg, const char *name, unsigned int *ptr);
int config_setting_lookup_unsigned_int(const config_setting_t *cfg, const char *name, unsigned int *ptr);
int config_setting_get_unsigned_int(const config_setting_t *cfg_item, unsigned int *ptr);

void cleanup_config(void);
void load_config(void);
void load_fallback_config(void);
void load_misc(config_t *cfg);
void load_fonts(config_t *cfg);

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
config_lookup_unsigned_int(const config_t *cfg, const char *name, unsigned int *ptr)
{
	return config_setting_get_unsigned_int(config_lookup(cfg, name), ptr);
}

int
config_setting_lookup_unsigned_int(const config_setting_t *cfg, const char *name, unsigned int *ptr)
{
	return config_setting_get_unsigned_int(config_setting_lookup(cfg, name), ptr);
}

int
config_setting_get_unsigned_int(const config_setting_t *cfg_item, unsigned int *ptr)
{
	if (!cfg_item)
		return 0;

	int integer = config_setting_get_int(cfg_item);

	if (integer >= 0) {
		*ptr = (unsigned int)integer;
		return 1;
	}

	return 1;
}

int
config_lookup_strdup(const config_t *cfg, const char *name, char **strptr)
{
	const char *string;
	if (config_lookup_string(cfg, name, &string)) {
		free(*strptr);
		*strptr = strdup(string);
		return 1;
	}

	return 0;
}

int
config_setting_lookup_strdup(const config_setting_t *cfg, const char *name, char **strptr)
{
	const char *string;
	if (config_setting_lookup_string(cfg, name, &string)) {
		free(*strptr);
		*strptr = strdup(string);
		return 1;
	}

	return 0;
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

}

void
load_misc(config_t *cfg)
{
	config_lookup_int(cfg, "border.percent", &borderperc);
	config_lookup_int(cfg, "border.width", &borderpx);
	config_lookup_strdup(cfg, "shell", &shell);
	config_lookup_strdup(cfg, "utmp", &utmp);
	config_lookup_strdup(cfg, "scroll", &scroll);
	config_lookup_strdup(cfg, "stty_args", &stty_args);


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
		fprintf(stderr, "set font %d to %s\n", i, fonts[i]);
	}
}




/* Not done

# background image patch
bgfile
pseudotransparency


# openurlonclick
url_opener_modkey
url_opener

*/