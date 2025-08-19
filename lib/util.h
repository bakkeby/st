#define Button6 6
#define Button7 7
#define Button8 8
#define Button9 9

static int hex2int(char c);
static inline int startswith(const char *needle, const char *haystack);
static char *expandhome(const char *string);
static size_t strlcpy(char * __restrict dst, const char * __restrict src, size_t dsize);