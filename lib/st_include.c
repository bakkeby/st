/* Patches */
#if COPYURL_PATCH || COPYURL_HIGHLIGHT_SELECTED_URLS_PATCH
#include "copyurl.c"
#endif
#if EXTERNALPIPE_PATCH
#include "externalpipe.c"
#endif
#if ISO14755_PATCH
#include "iso14755.c"
#endif
#include "keyboardselect_reflow_st.c"
#if RIGHTCLICKTOPLUMB_PATCH
#include "rightclicktoplumb_st.c"
#endif
#if NEWTERM_PATCH
#include "newterm.c"
#endif
#include "reflow.c"
#if SYNC_PATCH
#include "sync.c"
#endif
#if OSC7_PATCH
#include "osc7.c"
#endif
