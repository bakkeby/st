/*
 * This file contains patch control flags.
 *
 * In principle you should be able to mix and match any patches
 * you may want. In cases where patches are logically incompatible
 * one patch may take precedence over the other as noted in the
 * relevant descriptions.
 */

/* Patches */

/* Draws a background image in farbfeld format in place of the defaultbg color allowing for pseudo
 * transparency.
 * https://st.suckless.org/patches/background_image/
 */
#define BACKGROUND_IMAGE_PATCH 0

/* This patch adds the ability to reload the background image config when a SIGUSR1 signal is
 * received, e.g.: killall -USR1 st
 * Depends on the BACKGROUND_IMAGE_PATCH.
 */
#define BACKGROUND_IMAGE_RELOAD_PATCH 0

/* By default st only sets PRIMARY on selection.
 * This patch makes st set CLIPBOARD on selection.
 * https://st.suckless.org/patches/clipboard/
 */
#define CLIPBOARD_PATCH 0

/* Select and copy the last URL displayed with Mod+l. Multiple invocations cycle through the
 * available URLs.
 * https://st.suckless.org/patches/copyurl/
 */
#define COPYURL_PATCH 0

/* Select and copy the last URL displayed with Mod+l. Multiple invocations cycle through the
 * available URLs. This variant also highlights the selected URLs.
 * https://st.suckless.org/patches/copyurl/
 */
#define COPYURL_HIGHLIGHT_SELECTED_URLS_PATCH 0

/* Reading and writing st's screen through a pipe, e.g. pass info to dmenu.
 * https://st.suckless.org/patches/externalpipe/
 */
#define EXTERNALPIPE_PATCH 0

/* This patch improves and extends the externalpipe patch in two ways:
 *    - it prevents the reset of the signal handler set on SIGCHILD, when
 *      the forked process that executes the external process exits and
 *    - it adds the externalpipein function to redirect the standard output
 *      of the external command to the slave size of the pty, that is, as if
 *      the external program had been manually executed on the terminal
 *
 * It can be used to send desired escape sequences to the terminal with a
 * keyboard shortcut. The patch was created to make use of the dynamic-colors
 * tool that uses the OSC escape sequences to change the colors of the terminal.
 *
 * This patch depends on EXTERNALPIPE_PATCH being enabled.
 *
 * https://github.com/sos4nt/dynamic-colors
 * https://lists.suckless.org/hackers/2004/17218.html
 */
#define EXTERNALPIPEIN_PATCH 0

/* Open contents of the clipboard in a user-defined browser.
 * https://st.suckless.org/patches/open_copied_url/
 */
#define OPENCOPIED_PATCH 0

/* Open the selected text using xdg-open.
 * https://st.suckless.org/patches/open_selected_text/
 */
#define OPEN_SELECTED_TEXT_PATCH 0

/* This patch allows for URLs to be opened directly when you click on them. This may not work with
 * all terminal applications.
 *
 * https://www.reddit.com/r/suckless/comments/cc83om/st_open_url/
 */
#define OPENURLONCLICK_PATCH 0

/* This patch allows you to right-click on some selected text to send it to the plumbing
 * program of choice, e.g. open a file, view an image, open a URL.
 * https://st.suckless.org/patches/right_click_to_plumb/
 */
#define RIGHTCLICKTOPLUMB_PATCH 0

/* This patch adds synchronized-updates/application-sync support in st.
 * This will have no effect except when an application uses the synchronized-update escape
 * sequences. With this patch nearly all cursor flicker is eliminated in tmux, and tmux detects
 * it automatically via terminfo.
 *
 * Note: this patch alters st.info to promote support for extra escape sequences, which can
 * potentially cause application misbehaviour if you do not use this patch. Try removing or
 * commenting out the corresponding line in st.info if this is causing issues.
 *
 * https://st.suckless.org/patches/sync/
 */
#define SYNC_PATCH 0

/* Adds support for special underlines.
 *
 * Example test command:
 *    $ echo -e "\e[4:3m\e[58:5:10munderline\e[0m"
 *                  ^ ^     ^ ^  ^- sets terminal color 10
 *                  | |     |  \- indicates that terminal colors should be used
 *                  | |      \- indicates that underline color is being set
 *                  |  \- sets underline style to curvy
 *                   \- set underline
 *
 * Note: this patch alters st.info to promote support for extra escape sequences, which can
 * potentially cause application misbehaviour if you do not use this patch. Try removing or
 * commenting out the corresponding line in st.info if this is causing issues.
 *
 * https://st.suckless.org/patches/undercurl/
 */
#define UNDERCURL_PATCH 0

/* Adds proper glyphs rendering in st allowing wide glyphs to be drawn as-is as opposed to
 * smaller or cut glyphs being rendered.
 * https://github.com/Dreomite/st/commit/e3b821dcb3511d60341dec35ee05a4a0abfef7f2
 * https://www.reddit.com/r/suckless/comments/jt90ai/update_support_for_proper_glyph_rendering_in_st/
 */
#define WIDE_GLYPHS_PATCH 0

/* This patch adds the ability to configure st via Xresources. At startup, st will read and
 * apply the resources named in the resources[] array in config.h.
 * https://st.suckless.org/patches/xresources/
 */
#define XRESOURCES_PATCH 0

/* This patch adds the ability to reload the Xresources config when a SIGUSR1 signal is received
 * e.g.: killall -USR1 st
 * Depends on the XRESOURCES_PATCH.
 */
#define XRESOURCES_RELOAD_PATCH 0
