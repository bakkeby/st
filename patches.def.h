/*
 * This file contains patch control flags.
 *
 * In principle you should be able to mix and match any patches
 * you may want. In cases where patches are logically incompatible
 * one patch may take precedence over the other as noted in the
 * relevant descriptions.
 */

/* Patches */

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

/* Adds proper glyphs rendering in st allowing wide glyphs to be drawn as-is as opposed to
 * smaller or cut glyphs being rendered.
 * https://github.com/Dreomite/st/commit/e3b821dcb3511d60341dec35ee05a4a0abfef7f2
 * https://www.reddit.com/r/suckless/comments/jt90ai/update_support_for_proper_glyph_rendering_in_st/
 */
#define WIDE_GLYPHS_PATCH 0
