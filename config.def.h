/* See LICENSE file for copyright and license details. */

/*
 * appearance
 *
 * font: see http://freedesktop.org/software/fontconfig/fontconfig-user.html
 */
static char *font = "Liberation Mono:pixelsize=12:antialias=true:autohint=true";

/* Spare fonts */
static char *font2[] = {
/*	"Inconsolata for Powerline:pixelsize=12:antialias=true:autohint=true", */
/*	"Hack Nerd Font Mono:pixelsize=11:antialias=true:autohint=true", */
};

#if BACKGROUND_IMAGE_PATCH
/*
 * background image
 * expects farbfeld format
 * pseudo transparency fixes coordinates to the screen origin
 */
static const char *bgfile = "/path/to/image.ff";
static const int pseudotransparency = 0;
#endif // BACKGROUND_IMAGE_PATCH

/* borderperc: percentage of cell width to use as a border
 *     -1 = use fixed borderpx,
 *      0 = no border
 *    100 = border width is same as cell width
 */
static int borderperc = -1;
static int borderpx = 2;

/* modkey options: ControlMask, ShiftMask or XK_ANY_MOD */
static uint url_opener_modkey = XK_ANY_MOD;
static char *url_opener_def = "xdg-open";

char *stty_def_args = "stty raw pass8 nl -echo -iexten -cstopb 38400";

/* identification sequence returned in DA and DECID */
char *vtiden = "\033[?62;4c"; /* VT200 family (62) with sixel (4) */

/* sixel rgb byte order: LSBFirst or MSBFirst */
int sixelbyteorder = LSBFirst;

/* Kerning / character bounding-box multipliers */
static float cwscale = 1.0;
static float chscale = 1.0;

/*
 * word delimiter string
 *
 * More advanced example: L" `'\"()[]{}"
 */
wchar_t *worddelimiters_def = L" ";

/* Word delimiters for short and long jumps in the keyboard select patch */
wchar_t *kbds_sdelim_def = L"!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~ ";
wchar_t *kbds_ldelim_def = L" ";

/* selection timeouts (in milliseconds) */
static unsigned int doubleclicktimeout = 300;
static unsigned int tripleclicktimeout = 600;

/*
 * draw latency range in ms - from new content/keypress/etc until drawing.
 * within this range, st draws when content stops arriving (idle). mostly it's
 * near minlatency, but it waits longer for slow updates to avoid partial draw.
 * low minlatency will tear/flicker more, as it can "detect" idle too early.
 */
static double minlatency = 2;
static double maxlatency = 33;

/*
 * Synchronized-Update timeout in ms
 * https://gitlab.com/gnachman/iterm2/-/wikis/synchronized-updates-spec
 */
static uint su_timeout = 200;

/*
 * blinking timeout (set to 0 to disable blinking) for the terminal blinking
 * attribute.
 */
static unsigned int blinktimeout = 800;

/*
 * thickness of underline and bar cursors
 */
static unsigned int cursorthickness = 2;

/*
 * bell volume. It must be a value between -100 and 100. Use 0 for disabling
 * it
 */
static int bellvolume = 0;

/* default TERM value */
char *terminal_name = "st-256color";

/*
 * spaces per tab
 *
 * When you are changing this value, don't forget to adapt the »it« value in
 * the st.info and appropriately install the st.info in the environment where
 * you use this st version.
 *
 *	it#$tabspaces,
 *
 * Secondly make sure your kernel is not expanding tabs. When running `stty
 * -a` »tab0« should appear. You can tell the terminal to not expand tabs by
 *  running following command:
 *
 *	stty tabs
 */
unsigned int tabspaces = 8;

/* background opacity */
float alpha = 0.8;
float alpha_unfocused = 0.6;
float gradient_constant = 0.46; // constant alpha value that will get added to gradient

/*
 * drag and drop escape characters
 *
 * this will add a '\' before any characters specified in the string.
 */
char *xdndescchar_def = " !\"#$&'()*;<>?[\\]^`{|}~";

/* Terminal colors (16 first used in escape sequence) */
static const char *colorname[] = {
	/* 8 normal colors */
	"black",
	"red3",
	"green3",
	"yellow3",
	"blue2",
	"magenta3",
	"cyan3",
	"gray90",

	/* 8 bright colors */
	"gray50",
	"red",
	"green",
	"yellow",
	"#5c5cff",
	"magenta",
	"cyan",
	"white",

	[255] = 0,

	/* more colors can be added after 255 to use with DefaultXX */
	"#add8e6", /* 256 -> cursor */
	"#555555", /* 257 -> rev cursor*/
	"#000000", /* 258 -> bg */
	"#e5e5e5", /* 259 -> fg */
};


/*
 * Default colors (colorname index)
 * foreground, background, cursor, reverse cursor
 */
unsigned int bg = 17, bg_unfocused = 16;
unsigned int defaultbg = 0;
unsigned int defaultfg = 259;
unsigned int defaultcs = 256;
unsigned int defaultrcs = 257;
unsigned int selectionfg = 258;
unsigned int selectionbg = 259;

/* Foreground and background color of search results */
unsigned int highlightfg = 15;
unsigned int highlightbg = 160;

/*
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h4-Functions-using-CSI-_-ordered-by-the-final-character-lparen-s-rparen:CSI-Ps-SP-q.1D81
 * Default style of cursor
 * 0: Blinking block
 * 1: Blinking block (default)
 * 2: Steady block ("â–ˆ")
 * 3: Blinking underline
 * 4: Steady underline ("_")
 * 5: Blinking bar
 * 6: Steady bar ("|")
 * 7: Blinking st cursor
 * 8: Steady st cursor
 */
static unsigned int cursorstyle = 1;
static Rune stcursor = 0x2603; /* snowman (U+2603) */

/*
 * Default columns and rows numbers
 */
static unsigned int cols = 80;
static unsigned int rows = 24;

/*
 * Default colour and shape of the mouse cursor
 */
static unsigned int mouseshape = XC_xterm;
static unsigned int mousefg = 7;
static unsigned int mousebg = 0;

/*
 * Color used to display font attributes when fontconfig selected a font which
 * doesn't match the ones requested.
 */
static unsigned int defaultattr = 11;

#if XRESOURCES_PATCH
/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",         STRING,  &font },
		{ "color0",       STRING,  &colorname[0] },
		{ "color1",       STRING,  &colorname[1] },
		{ "color2",       STRING,  &colorname[2] },
		{ "color3",       STRING,  &colorname[3] },
		{ "color4",       STRING,  &colorname[4] },
		{ "color5",       STRING,  &colorname[5] },
		{ "color6",       STRING,  &colorname[6] },
		{ "color7",       STRING,  &colorname[7] },
		{ "color8",       STRING,  &colorname[8] },
		{ "color9",       STRING,  &colorname[9] },
		{ "color10",      STRING,  &colorname[10] },
		{ "color11",      STRING,  &colorname[11] },
		{ "color12",      STRING,  &colorname[12] },
		{ "color13",      STRING,  &colorname[13] },
		{ "color14",      STRING,  &colorname[14] },
		{ "color15",      STRING,  &colorname[15] },
		{ "background",   STRING,  &colorname[258] },
		{ "foreground",   STRING,  &colorname[259] },
		{ "cursorColor",  STRING,  &colorname[256] },
		{ "termname",     STRING,  &termname },
		{ "shell",        STRING,  &shell },
		{ "minlatency",   INTEGER, &minlatency },
		{ "maxlatency",   INTEGER, &maxlatency },
		{ "blinktimeout", INTEGER, &blinktimeout },
		{ "bellvolume",   INTEGER, &bellvolume },
		{ "tabspaces",    INTEGER, &tabspaces },
		#if RELATIVEBORDER_PATCH
		{ "borderperc",   INTEGER, &borderperc },
		#else
		{ "borderpx",     INTEGER, &borderpx },
		#endif // RELATIVEBORDER_PATCH
		{ "cwscale",      FLOAT,   &cwscale },
		{ "chscale",      FLOAT,   &chscale },
		{ "highlightfg",  INTEGER, &highlightfg },
		{ "highlightbg",  INTEGER, &highlightbg },
};
#endif // XRESOURCES_PATCH

/*
 * Force mouse select/shortcuts while mask is active (when MODE_MOUSE is set).
 * Note that if you want to use ShiftMask with selmasks, set this to an other
 * modifier, set to 0 to not use it.
 */
static uint forcemousemod = ShiftMask;

/*
 * Internal mouse shortcuts.
 * Beware that overloading Button1 will disable the selection.
 */
static MouseShortcut mshortcuts[] = {
	/* mask                 button   function        argument       release  screen */
	{ ControlMask,          Button2, selopen,        {.i = 0},      1 },
	{ XK_ANY_MOD,           Button2, paste,          {.i = 0},      1 },
	{ XK_ANY_MOD,           Button3, plumb,          {.i = 0},      1 },
	{ XK_ANY_MOD,           Button4, kscrollup,      {.i = 1},      0, S_PRI },
	{ XK_ANY_MOD,           Button5, kscrolldown,    {.i = 1},      0, S_PRI },
	{ XK_ANY_MOD,           Button4, ttysend,        {.s = "\031"}, 0, S_ALT },
	{ XK_ANY_MOD,           Button5, ttysend,        {.s = "\005"}, 0, S_ALT },
};

/* Internal keyboard shortcuts. */
#define MODKEY Mod1Mask
#define TERMMOD (ControlMask|ShiftMask)

#if EXTERNALPIPE_PATCH // example command
static char *openurlcmd[] = { "/bin/sh", "-c",
	"xurls | dmenu -l 10 -w $WINDOWID | xargs -r open",
	"externalpipe", NULL };

#if EXTERNALPIPEIN_PATCH // example command
static char *setbgcolorcmd[] = { "/bin/sh", "-c",
	"printf '\033]11;#008000\007'",
	"externalpipein", NULL };
#endif // EXTERNALPIPEIN_PATCH
#endif // EXTERNALPIPE_PATCH

static Shortcut shortcuts[] = {
	/* mask                 keysym          function         argument   screen */
	{ XK_ANY_MOD,           XK_Break,       sendbreak,       {.i =  0} },
	{ ControlMask,          XK_Print,       toggleprinter,   {.i =  0} },
	{ ShiftMask,            XK_Print,       printscreen,     {.i =  0} },
	{ XK_ANY_MOD,           XK_Print,       printsel,        {.i =  0} },
	{ TERMMOD,              XK_Prior,       zoom,            {.f = +1} },
	{ TERMMOD,              XK_Next,        zoom,            {.f = -1} },
	{ TERMMOD,              XK_Home,        zoomreset,       {.f =  0} },
	{ TERMMOD,              XK_C,           clipcopy,        {.i =  0} },
	{ TERMMOD,              XK_V,           clippaste,       {.i =  0} },
	{ TERMMOD,              XK_O,           changealpha,     {.f = +0.05} },
	{ TERMMOD,              XK_P,           changealpha,     {.f = -0.05} },
//	{ TERMMOD,              XK_,            changealphaunfocused, {.f = +0.05} },
//	{ TERMMOD,              XK_,            changealphaunfocused, {.f = -0.05} },
	{ XK_NO_MOD,            XK_F11,         fullscreen,      {.i =  0} },
	{ MODKEY,               XK_Return,      fullscreen,      {.i =  0} },
	{ ShiftMask,            XK_Page_Up,     kscrollup,       {.i = -1}, S_PRI },
	{ ShiftMask,            XK_Page_Down,   kscrolldown,     {.i = -1}, S_PRI },
	{ TERMMOD,              XK_Y,           paste,           {.i =  0} },
	{ ShiftMask,            XK_Insert,      paste,           {.i =  0} },
	{ TERMMOD,              XK_Num_Lock,    numlock,         {.i =  0} },
	#if COPYURL_PATCH || COPYURL_HIGHLIGHT_SELECTED_URLS_PATCH
	{ MODKEY,               XK_l,           copyurl,         {.i =  0} },
	#endif // COPYURL_PATCH
	{ MODKEY,               XK_o,           opencopied,      {.v = "xdg-open"} },
	{ TERMMOD,              XK_Return,      newterm,         {.i =  0} },
	#if EXTERNALPIPE_PATCH
	{ TERMMOD,              XK_U,           externalpipe,    { .v = openurlcmd } },
	#if EXTERNALPIPEIN_PATCH
	{ TERMMOD,              XK_M,           externalpipein,  { .v = setbgcolorcmd } },
	#endif // EXTERNALPIPEIN_PATCH
	#endif // EXTERNALPIPE_PATCH
	{ TERMMOD,              XK_Escape,      keyboard_select, { 0 } },
	{ TERMMOD,              XK_F,           searchforward,   { 0 } },
	{ TERMMOD,              XK_B,           searchbackward,  { 0 } },
	{ TERMMOD,              XK_I,           iso14755,        {.i =  0} },
	{ ControlMask,          XK_Page_Up,     scrolltoprompt,  {.i = -1}, S_PRI },
	{ ControlMask,          XK_Page_Down,   scrolltoprompt,  {.i =  1}, S_PRI },
};

/* Refer to lib/keyboardinput.c for mapped keys and the key array. */

/*
 * State bits to ignore when matching key or button events.  By default,
 * numlock (Mod2Mask) and keyboard layout (XK_SWITCH_MOD) are ignored.
 */
static uint ignoremod = Mod2Mask|XK_SWITCH_MOD;

/*
 * Selection types' masks.
 * Use the same masks as usual.
 * Button1Mask is always unset, to make masks match between ButtonPress.
 * ButtonRelease and MotionNotify.
 * If no match is found, regular selection is used.
 */
static uint selmasks[] = {
	[SEL_RECTANGULAR] = Mod1Mask,
};

/*
 * Printable characters in ASCII, used to estimate the advance width
 * of single wide characters.
 */
static char ascii_printable_def[] =
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
	"`abcdefghijklmnopqrstuvwxyz{|}~";

/* Triggering the iso14755 command will popup dmenu asking you to
 * enter a unicode codepoint that will be converted to a glyph and
 * then pushed to st.
 *
 * Below is the command that is executed in case you wish to
 * repurpose it.
 */
static char *iso14755cmd_def = "dmenu -w \"$WINDOWID\" -p codepoint: </dev/null";

/*
 * This command is run via the plumb function, typically bound to mouse
 * right click. The argument will be set to the current selection and with
 * the current working directory being set to that of the active shell.
 *
 * It may be an idea to pass the selection to a shell script that can
 * further decide what to do with it depending on the content.
 */
static char *plumb_cmd_def = "plumb";

#if UNDERCURL_PATCH
/**
 * Undercurl style. Set UNDERCURL_STYLE to one of the available styles.
 *
 * Curly: Dunno how to draw it *shrug*
 *  _   _   _   _
 * ( ) ( ) ( ) ( )
 *	 (_) (_) (_) (_)
 *
 * Spiky:
 * /\  /\   /\	/\
 *   \/  \/	  \/
 *
 * Capped:
 *	 _     _     _
 * / \   / \   / \
 *    \_/   \_/
 */
// Available styles
#define UNDERCURL_CURLY 0
#define UNDERCURL_SPIKY 1
#define UNDERCURL_CAPPED 2
// Active style
#define UNDERCURL_STYLE UNDERCURL_SPIKY
#endif // UNDERCURL_PATCH
