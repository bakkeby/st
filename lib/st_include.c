#include "util.c"
#if COPYURL_PATCH || COPYURL_HIGHLIGHT_SELECTED_URLS_PATCH
#include "copyurl.c"
#endif
#if EXTERNALPIPE_PATCH
#include "externalpipe.c"
#endif
#include "keyboardselect_reflow_st.c"
#include "rightclicktoplumb_st.c"
#include "newterm.c"
#include "reflow.c"
#if SYNC_PATCH
#include "sync.c"
#endif
#include "osc7.c"
