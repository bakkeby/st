static const uint64_t
	Alpha = 0x1,
	AlphaFocusHighlight = 0x2,
	BoxDraw = 0x4,
	BoxDrawBoldAffectsLineThickness = 0x8,
	BoxDrawForBraille = 0x10,
	HideCursor = 0x20,
	SwapMouse = 0x40,
	ThemedCursor = 0x80,
	AnySize = 0x100,
	RetainSelectionPerWindow = 0x200,
	BoldIsBright = 0x400,
	AllowItalic = 0x800,
	AllowBoldItalic = 0x1000,
	AllowItalicBold = 0x1000,
	AllowBold = 0x2000,
	FuncPlaceholder0x4000 = 0x4000,
	FuncPlaceholder0x8000 = 0x8000,
	FuncPlaceholder0x10000 = 0x10000,
	FuncPlaceholder0x20000 = 0x20000,
	FuncPlaceholder0x40000 = 0x40000,
	FuncPlaceholder0x80000 = 0x80000,
	FuncPlaceholder0x100000 = 0x100000,
	FuncPlaceholder0x200000 = 0x200000,
	FuncPlaceholder0x400000 = 0x400000,
	FuncPlaceholder0x800000 = 0x800000,
	FuncPlaceholder0x1000000 = 0x1000000,
	FuncPlaceholder0x2000000 = 0x2000000,
	FuncPlaceholder0x4000000 = 0x4000000,
	FuncPlaceholder0x8000000 = 0x8000000,
	FuncPlaceholder0x10000000 = 0x10000000,
	FuncPlaceholder0x20000000 = 0x20000000,
	FuncPlaceholder0x40000000 = 0x40000000,
	FuncPlaceholder0x80000000 = 0x80000000,
	FuncPlaceholder0x100000000 = 0x100000000,
	FuncPlaceholder0x200000000 = 0x200000000,
	FuncPlaceholder0x400000000 = 0x400000000,
	FuncPlaceholder0x800000000 = 0x800000000,
	FuncPlaceholder0x1000000000 = 0x1000000000,
	FuncPlaceholder0x2000000000 = 0x2000000000,
	FuncPlaceholder0x4000000000 = 0x4000000000,
	FuncPlaceholder0x8000000000 = 0x8000000000,
	FuncPlaceholder0x10000000000 = 0x10000000000,
	FuncPlaceholder0x20000000000 = 0x20000000000,
	FuncPlaceholder0x40000000000 = 0x40000000000,
	FuncPlaceholder0x80000000000 = 0x80000000000,
	FuncPlaceholder0x100000000000 = 0x100000000000,
	FuncPlaceholder0x200000000000 = 0x200000000000,
	FuncPlaceholder0x400000000000 = 0x400000000000,
	FuncPlaceholder0x800000000000 = 0x800000000000,
	FuncPlaceholder0x1000000000000 = 0x1000000000000,
	FuncPlaceholder0x2000000000000 = 0x2000000000000,
	FuncPlaceholder0x4000000000000 = 0x4000000000000,
	FuncPlaceholder0x8000000000000 = 0x8000000000000,
	FuncPlaceholder0x10000000000000 = 0x10000000000000,
	FuncPlaceholder0x20000000000000 = 0x20000000000000,
	FuncPlaceholder0x40000000000000 = 0x40000000000000,
	FuncPlaceholder0x80000000000000 = 0x80000000000000,
	FuncPlaceholder0x100000000000000 = 0x100000000000000,
	FuncPlaceholder0x200000000000000 = 0x200000000000000,
	FuncPlaceholder0x400000000000000 = 0x400000000000000,
	FuncPlaceholder0x800000000000000 = 0x800000000000000,
	FuncPlaceholder0x1000000000000000 = 0x1000000000000000,
	FuncPlaceholder0x2000000000000000 = 0x2000000000000000,
	FuncPlaceholder0x4000000000000000 = 0x4000000000000000,
	FuncPlaceholder0x8000000000000000 = 0x8000000000000000;

struct nv {
    const char *name;
    uint64_t value;
};

#define map(F) { #F, F }

static const struct nv function_names[] = {
	map(Alpha),
	map(AlphaFocusHighlight),
	map(AnySize),
	map(BoldIsBright),
	map(BoxDraw),
	map(BoxDrawBoldAffectsLineThickness),
	map(BoxDrawForBraille),
	map(HideCursor),
	map(RetainSelectionPerWindow),
	map(SwapMouse),
	map(ThemedCursor),
	map(AllowItalic),
	map(AllowBoldItalic),
	map(AllowItalicBold),
	map(AllowBold),
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
