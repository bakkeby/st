static const uint64_t
	Alpha = 0x1,
	AlphaFocusHighlight = 0x2,
	BoxDraw = 0x4,
	BoxDrawBoldAffectsLineThickness = 0x8,
	BoxDrawForBraille = 0x10,
	HideCursor = 0x20,
	SwapMouse = 0x40,
	FuncPlaceholderColorEmoji = 0x80,
	FuncPlaceholderStatus2DNoAlpha = 0x100, // option to not use alpha when drawing status2d status
	FuncPlaceholderSystray = 0x200, // enables systray
	FuncPlaceholderBarBorder = 0x400, // draw a border around the bar
	FuncPlaceholderNoBorders = 0x800, // as per the noborder patch, show no border when only one client in tiled mode
	FuncPlaceholderWarp = 0x1000, // warp patch
	FuncPlaceholderFocusedOnTop = 0x2000,
	FuncPlaceholderDecorationHints = 0x4000,
	FuncPlaceholderFocusOnNetActive = 0x8000,
	FuncPlaceholderAllowNoModifierButtons = 0x10000,
	FuncPlaceholderCenterSizeHintsClients = 0x20000, // center tiled clients subject to size hints within their tiled area
	FuncPlaceholderResizeHints = 0x40000, // if enabled then dusk will respect size hints in tiled resizals
	FuncPlaceholderSortScreens = 0x80000, // only applies on startup
	FuncPlaceholderViewOnWs = 0x100000, // follow a window to the workspace it is being moved to
	FuncPlaceholderXresources = 0x200000, // xrdb patch
	FuncPlaceholderSnapToWindows = 0x400000, // snap to windows when moving floating clients
	FuncPlaceholderSnapToGaps = 0x800000, // snap to outer gaps when moving floating clients
	FuncPlaceholderAltWorkspaceIcons = 0x1000000, // show the workspace name instead of the icons
	FuncPlaceholderGreedyMonitor = 0x2000000, // when viewing a workspace the monitor is greedy and gives nothing in return (i.e. disables swap of workspaces)
	FuncPlaceholderSmartLayoutConversion = 0x4000000, // when moving a workspace from one monitor to another, automatically adjust layout based on monitor orientation (i.e. vertical vs horizontal)
	FuncPlaceholderSmartLayoutConvertion = 0x4000000, // typo correction alias for the above
	FuncPlaceholderAutoHideScratchpads = 0x8000000, // automatically hide open scratchpads when moving to another workspace
	FuncPlaceholderRioDrawIncludeBorders = 0x10000000, // indicates whether the area drawn using slop includes the window borders
	FuncPlaceholderRioDrawSpawnAsync = 0x20000000, // indicates whether to spawn the application alongside or after drawing area using slop
	FuncPlaceholderBarPadding = 0x40000000, // enables bar padding
	FuncPlaceholderRestrictFocusstackToMonitor = 0x80000000, // option to restrict focusstack to only operate within the monitor
	FuncPlaceholderAutoReduceNmaster = 0x100000000, // automatically reduce nmaster if there are multiple master clients and one is killed
	FuncPlaceholderWinTitleIcons = 0x200000000, // adds application icons to window titles in the bar
	FuncPlaceholderWorkspacePreview = 0x400000000, // adds preview images when hovering workspace icons in the bar
	FuncPlaceholderSpawnCwd = 0x800000000, // spawn applications in the currently selected client's working directory
	FuncPlaceholderSystrayNoAlpha = 0x1000000000, // option to not use alpha (transparency) for the systray
	FuncPlaceholderWorkspaceLabels = 0x2000000000, // adds the class of the master client next to the workspace icon
	FuncPlaceholderFlexWinBorders = 0x4000000000, // option to use the SchemeFlex* colour schemes, if disabled then SchemeTitle* is used instead
	FuncPlaceholderFocusOnClick = 0x8000000000, // only allow focus change when the user clicks on windows (disables sloppy focus)
	FuncPlaceholderFocusedOnTopTiled = 0x10000000000, // additional toggle to also allow focused tiled clients to display on top of floating windows
	FuncPlaceholderBanishMouseCursor = 0x20000000000, // like xbanish, hides mouse cursor when using the keyboard
	FuncPlaceholderFocusFollowMouse = 0x40000000000, // window that rests under the mouse cursor will get focus when changing workspace or killing clients
	FuncPlaceholderBanishMouseCursorToCorner = 0x80000000000, // makes BanishMouseCursor also move the cursor to top right corner of the screen
	FuncPlaceholderStackerIcons = 0x100000000000, // adds a stacker icon hints in window titles
	FuncPlaceholderAltWindowTitles = 0x200000000000, // show alternate window titles, if present
	FuncPlaceholderBarBorderColBg = 0x400000000000, // optionally use the background colour of the bar for the border as well, rather than border colur
	FuncPlaceholder140737488355328 = 0x800000000000,
	FuncPlaceholder281474976710656 = 0x1000000000000,
	FuncPlaceholder562949953421312 = 0x2000000000000,
	FuncPlaceholder1125899906842624 = 0x4000000000000,
	FuncPlaceholder2251799813685248 = 0x8000000000000,
	FuncPlaceholder4503599627370496 = 0x10000000000000,
	FuncPlaceholder9007199254740992 = 0x20000000000000,
	FuncPlaceholder18014398509481984 = 0x40000000000000,
	FuncPlaceholder36028797018963968 = 0x80000000000000,
	FuncPlaceholderDebug = 0x100000000000000, // same name and value used for client flags, see flags.h
	FuncPlaceholder144115188075855872 = 0x200000000000000,
	FuncPlaceholder288230376151711744 = 0x400000000000000,
	FuncPlaceholder576460752303423488 = 0x800000000000000,
	FuncPlaceholder1152921504606846976 = 0x1000000000000000,
	FuncPlaceholder2305843009213693952 = 0x2000000000000000,
	FuncPlaceholder4611686018427387904 = 0x4000000000000000,
	FuncPlaceholder9223372036854775808 = 0x8000000000000000;

struct nv {
    const char *name;
    uint64_t value;
};

#define map(F) { #F, F }

static const struct nv function_names[] = {
    map(Alpha),
    map(AlphaFocusHighlight),
    map(BoxDraw),
    map(BoxDrawBoldAffectsLineThickness),
    map(BoxDrawForBraille),
    map(HideCursor),
    map(SwapMouse),
    map(FuncPlaceholderColorEmoji),
    { NULL, 0 }
};

#undef map


int enabled(const uint64_t functionality);
int disabled(const uint64_t functionality);
void enablefunc(const uint64_t functionality);
void disablefunc(const uint64_t functionality);
void setenabled(const uint64_t functionality, int enabled);
void togglefunc(const uint64_t functionality);
const uint64_t getfuncbyname(const char *name);
const char *getnamebyfunc(const uint64_t functionality);
