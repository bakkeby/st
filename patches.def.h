/*
 * This file contains patch control flags.
 *
 * In principle you should be able to mix and match any patches
 * you may want. In cases where patches are logically incompatible
 * one patch may take precedence over the other as noted in the
 * relevant descriptions.
 */

/* Patches */

/* Adds proper glyphs rendering in st allowing wide glyphs to be drawn as-is as opposed to
 * smaller or cut glyphs being rendered.
 * https://github.com/Dreomite/st/commit/e3b821dcb3511d60341dec35ee05a4a0abfef7f2
 * https://www.reddit.com/r/suckless/comments/jt90ai/update_support_for_proper_glyph_rendering_in_st/
 */
#define WIDE_GLYPHS_PATCH 0
