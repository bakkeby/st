This is a version of st that is built on [st-flexipatch](https://github.com/bakkeby/st-flexipatch) and the main difference between this and other builds of st is that this one comes with runtime configuration.

That means that st can be reconfigured without having to recompile it.

After compiling copy the `st.cfg` file from the project directory to `~/.config/st/` or the XDG config home equivalent if you are using that.

Refer to [https://st.suckless.org/](https://st.suckless.org/) for details on the st terminal and how it works.

---

### Notable additions to st (beyond patches)

   - runtime configuration that can also be reloaded using USR1 signal
   - incremental scrollback history meaning less space reserved upfront
   - additional externalpipe options (full history, treat pipe output as text input)

### Patches incorporated:

   - [alpha](https://st.suckless.org/patches/alpha/)
      - adds transparency for the terminal

   - [alpha-focus-highlight](https://st.suckless.org/patches/alpha_focus_highlight/)
      - allows the user to specify two distinct opacity values or background colors in order to
        easily differentiate between focused and unfocused terminal windows

   - [anygeometry](https://st.suckless.org/patches/anygeometry/)
      - allows st to start at any pixel size using the \-G command line option (if floating)
      - can be combined with the anysize patch to resize to any pixel size

   - [anysize](https://st.suckless.org/patches/anysize/) (simple variant)
      - allows st to reize to any pixel size rather than snapping to character width / height

   - [blinking-cursor](https://st.suckless.org/patches/blinking_cursor/)
      - allows the use of a blinking cursor

   - [bold-is-not-bright](https://st.suckless.org/patches/bold-is-not-bright/)
      - by default bold text is rendered with a bold font in the bright variant of the current color
      - this patch makes bold text rendered simply as bold, leaving the color unaffected

   - [boxdraw](https://st.suckless.org/patches/boxdraw/)
      - adds dustom rendering of lines/blocks/braille characters for gapless alignment

   - [clipboard](https://st.suckless.org/patches/clipboard/)
      - by default st only sets PRIMARY on selection
      - this patch makes st set CLIPBOARD on selection

   - [copyurl](https://st.suckless.org/patches/copyurl/)
      - this patch allows you to select and copy the last URL displayed with Mod+l
      - multiple invocations cycle through the available URLs

   - [csi\_23\_23](https://st.suckless.org/patches/csi_22_23/)
      - adds support for CSI escape sequences 22 and 23, which save and restores the window title
        (for instance nvim does this when opening and closing)

   - default-cursor
      - minor change allowing escape sequences like `\e[ q` or `\e[0 q` to set the cursor back to default configuration instead of a blinking block
      - while many terminals do this the behaviour is not according to the specification

   - [delkey](https://st.suckless.org/patches/delkey/)
      - return BS on pressing backspace and DEL on pressing the delete key

   - [desktopentry](https://st.suckless.org/patches/desktopentry/)
      - adds a desktop entry for st so that it can be displayed with an icon when using a graphical launcher
      - this patch only applies to the Makefile and is enabled by default, remove if not needed

   - [disable-fonts](https://st.suckless.org/patches/disable_bold_italic_fonts/)
      - this patch adds the option of disabling bold/italic/roman fonts globally

   - [drag-n-drop](https://st.suckless.org/patches/drag-n-drop)
      - allows dragging a file into the terminal and have the path printed

   - [dynamic-cursor-color](https://st.suckless.org/patches/dynamic-cursor-color/)
      - this patch makes the cursor color the inverse of the current cell color

   - [externalpipe](https://st.suckless.org/patches/externalpipe/)
      - this patch allows for reading and writing st's screen through a pipe, e.g. to pass info to dmenu

   - [externalpipein](https://lists.suckless.org/hackers/2004/17218.html)
      - this patch prevents the reset of the signal handler set on SIGCHILD, when the forked process that executes the external process exits
      - it adds the externalpipein function to redirect the standard output of the external command to the slave size of the pty, that is, as if the external program had been manually executed on the terminal
      - this can be used to send desired escape sequences to the terminal with a shortcut (e.g. to change colors)

   - [fix-keyboard-input](https://st.suckless.org/patches/fix_keyboard_input/)
      - allows cli applications to use all the fancy key combinations that are available to GUI applications

   - [font2](https://st.suckless.org/patches/font2/)
      - allows you to add a spare font besides the default

   - [fullscreen](https://st.suckless.org/patches/fullscreen/)
      - allows the st window to go into fullscreen mode

   - [gradient](https://st.suckless.org/patches/gradient/)
      - adds gradient transparency to st
      - depends on the alpha patch

   - [hidecursor](https://st.suckless.org/patches/hidecursor/)
      - hides the X cursor whenever a key is pressed and show it back when the mouse is moved in the terminal window

   - [hide-terminal-cursor](https://www.reddit.com/r/suckless/comments/nvee8h/how_to_hide_cursor_in_st_is_there_a_patch_for_it/)
      - hides the terminal cursor when the window loses focus (as opposed to showing a hollow cursor)

   - [iso14755](https://st.suckless.org/patches/iso14755/) (via externalpipe)
      - pressing the default binding Ctrl+Shift-i will popup dmenu, asking you to enter a unicode codepoint that will be converted to a glyph and then pushed to st

   - [keyboard-select](https://st.suckless.org/patches/keyboard_select/)
      - allows you to select text on the terminal using keyboard shortcuts

   - [ligatures](https://st.suckless.org/patches/ligatures/)
      - adds support for drawing ligatures using the Harfbuzz library to transform original text of a single line to a list of glyphs with ligatures included

   - [monochrome](https://www.reddit.com/r/suckless/comments/ixbx6z/how_to_use_black_and_white_only_for_st/)
      - makes st ignore terminal color attributes to make for a monochrome look

   - [netwmicon](https://st.suckless.org/patches/netwmicon/)
      - sets the \_NET\_WM\_ICON X property with a farbfeld (.ff) file

   - [newterm](https://st.suckless.org/patches/newterm/)
      - allows you to spawn a new st terminal using Ctrl-Shift-Return
      - it will have the same CWD (current working directory) as the original st instance

   - [no-window-decorations](https://github.com/bakkeby/patches/wiki/no_window_decorations)
      - makes st show without window decorations if the WM supports it

   - [open-copied-url](https://st.suckless.org/patches/open_copied_url/)
      - open contents of the clipboard in a user-defined browser

   - [open-selected-text](https://st.suckless.org/patches/open_selected_text)
      - open the selected text using `xdg-open`

   - [openurlonclick](https://www.reddit.com/r/suckless/comments/cc83om/st_open_url/)
      - allows for URLs to be opened directly when you click on them

   - reflow
      - allows st to be resized without cutting off text when the terminal window is made larger again
      - text wraps when the terminal window is made smaller

   - [relativeborder](https://st.suckless.org/patches/relativeborder/)
      - allows you to specify a border that is relative in size to the width of a cell in the
        terminal

   - [right-click-to-plumb](https://st.suckless.org/patches/right_click_to_plumb/)
      - allows you to right-click on some selected text to send it to the plumbing program of choice

   - [scrollback](https://st.suckless.org/patches/scrollback/)
      - allows you scroll back through terminal output using keyboard shortcuts or mousewheel

   - sixel
      - this patch adds SIXEL graphics support

   - [spoiler](https://st.suckless.org/patches/spoiler/)
      - use inverted defaultbg/fg for selection when bg/fg are the same

   - [swapmouse](https://st.suckless.org/patches/swapmouse/)
      - changes the mouse shape to the global default when the running program subscribes for mouse
        events, for instance, in programs like ranger and fzf
      - it emulates the behaviour shown by vte terminals like termite

   - [sync](https://st.suckless.org/patches/sync/)
      - adds synchronized-updates/application-sync support in st
      - this has no effect except when an application uses the synchronized-update escape sequences
      - with this patch nearly all cursor flicker is eliminated in tmux, and tmux detects it
        automatically via terminfo

   - [themed-cursor](https://st.suckless.org/patches/themed_cursor/)
      - instead of a default X cursor, use the xterm cursor from your cursor theme

   - [undercurl](https://st.suckless.org/patches/undercurl/)
      - adds support for special underlines, e.g. curly / wavy underlines

   - [use-XftFontMatch](https://git.suckless.org/st/commit/528241aa3835e2f1f052abeeaf891737712955a0.html)
      - use XftFontMatch in place of FcFontMatch to allow font to scale with Xft.dpi resource
        setting

   - [vertcenter](https://st.suckless.org/patches/vertcenter/)
      - vertically center lines in the space available if you have set a larger chscale in config.h

   - [visualbell](https://st.suckless.org/patches/visualbell/)
      - adds visual indicators for the terminal bell event

   - [wide-glyphs](https://www.reddit.com/r/suckless/comments/jt90ai/update_support_for_proper_glyph_rendering_in_st/)
      - adds proper support for wide glyphs, as opposed to rendering smaller or cut glyphs

   - [workingdir](https://st.suckless.org/patches/workingdir/)
      - allows user to specify the initial path st should use as the working directory

   - [xresources](https://st.suckless.org/patches/xresources/)
      - adds the ability to override colour settings via Xresources
