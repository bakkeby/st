#include "util.c"
#if COPYURL_PATCH || COPYURL_HIGHLIGHT_SELECTED_URLS_PATCH
#include "copyurl.c"
#endif
#if EXTERNALPIPE_PATCH
#include "externalpipe.c"
#endif
#include "keyboardselect_reflow_st.c"
#if RIGHTCLICKTOPLUMB_PATCH
#include "rightclicktoplumb_st.c"
#endif
#include "newterm.c"
#include "reflow.c"
#if SYNC_PATCH
#include "sync.c"
#endif
#include "osc7.c"
