void externalpipe_screen_out(const Arg *);
void externalpipe_screen_in(const Arg *);
void externalpipe_screen_inject(const Arg *);
void externalpipe_hist_out(const Arg *);
void externalpipe_hist_in(const Arg *);
void externalpipe_hist_inject(const Arg *);
void externalpipe_osc133_out(const Arg *);
void externalpipe_osc133_in(const Arg *);
void externalpipe_osc133_inject(const Arg *);

typedef enum {
    EXTERNAL_PIPE_SENDONLY = 0,  /* send scope -> child stdin, discard child output */
    EXTERNAL_PIPE_PRINT    = 2,  /* send scope -> child stdin, print child output to terminal */
    EXTERNAL_PIPE_INJECT   = 3   /* send scope -> child stdin, inject child output into tty input */
} ExternalPipeMode;

typedef enum {
	EXTERNAL_SCOPE_SCREEN  = 0,  /* send screen content */
	EXTERNAL_SCOPE_ETERNAL = 1,  /* send entire history */
	EXTERNAL_SCOPE_OSC133  = 2   /* send everything since last OSC133 marker, e.g. command */
} ExternalPipeScope;
