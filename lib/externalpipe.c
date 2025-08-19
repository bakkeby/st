int
tlineabslen(int y)
{
	int i = term.col;
	Glyph *line = TLINEABS(y);

	if (line[i - 1].mode & ATTR_WRAP)
		return i;

	while (i > 0 && line[i - 1].u == ' ')
		--i;

	return i;
}

void
extpipe(const Arg *arg, ExternalPipeScope scope, ExternalPipeMode mode)
{
	int to[2], from[2], startline = -1, endline = -1;
	int foundprompts = 0;
	char buf[BUFSIZ];
	void (*oldsigpipe)(int);
	Glyph *bp, *end;
	int lastpos, n, x, newline;
	char **cmd = ((char **)arg->v);
	ssize_t r;

	if (pipe(to) == -1)
		return;
	if (mode == 2 && pipe(from) == -1) {
		close(to[0]); close(to[1]);
		return;
	}

	switch (fork()) {
	case -1:
		close(to[0]);
		close(to[1]);
		if (mode == EXTERNAL_PIPE_INJECT) {
			close(from[0]);
			close(from[1]);
		}
		return;
	case 0:
		dup2(to[0], STDIN_FILENO);
		if (mode == EXTERNAL_PIPE_PRINT) {
			dup2(csdfd, STDOUT_FILENO);
		} else if (mode == EXTERNAL_PIPE_INJECT) {
			dup2(from[1], STDOUT_FILENO);
			dup2(from[1], STDERR_FILENO);
		}

		close(to[0]);
		close(to[1]);

		if (mode == EXTERNAL_PIPE_PRINT || mode == EXTERNAL_PIPE_INJECT)
			close(csdfd);
		if (mode == EXTERNAL_PIPE_INJECT) {
			close(from[0]);
			close(from[1]);
		}

		execvp(cmd[0], cmd);
		fprintf(stderr, "st: execvp %s\n", cmd[0]);
		perror("failed");
		exit(0);
	}

	if (mode == EXTERNAL_PIPE_INJECT)
		close(from[1]);

	/* ignore sigpipe for now, in case child exits early */
	oldsigpipe = signal(SIGPIPE, SIG_IGN);
	newline = 0;
	scope = EXTERNAL_SCOPE_SCREEN;

	switch (scope) {
	case EXTERNAL_SCOPE_OSC133:
		/* find the last two prompts in history+screen */
		for (n = term.row - 1; n >= -term.histf; n--) {
			Line line = TLINEABS(n);
			for (x = 0; x < term.col; x++) {
				if (line[x].mode & ATTR_FTCS_PROMPT) {
					if (foundprompts == 0) {
						endline = n;   /* last (current) prompt */
						foundprompts++;
					} else if (foundprompts == 1) {
						startline = n; /* previous prompt */
						goto found;
					}
				}
			}
		}
		/* fallthrough */
	case EXTERNAL_SCOPE_ETERNAL:
		startline = -term.histf; /* dump from oldest history */
		endline = term.row;
		break;
	case EXTERNAL_SCOPE_SCREEN:
		startline = 0; /* dump from start of the screen */
		endline = term.row;
		break;
	}

found:
	if (startline == -1 || endline == -1 || startline >= endline)
		return; /* not enough prompts found, bail out */

	/* dump lines between startline and endline-1 */
	for (int n = startline; n < endline; n++) {
		int len = tlineabslen(n);
		if (len <= 0)
			continue;

		bp = TLINEABS(n);
		int lastpos = MIN(len + 1, term.col) - 1;
		end = &bp[lastpos + 1];

		for (; bp < end; ++bp) {
			if (xwrite(to[1], buf, utf8encode(bp->u, buf)) < 0)
				break;
		}

		if ((newline = TLINEABS(n)[lastpos].mode & ATTR_WRAP))
			continue;

		if (xwrite(to[1], "\n", 1) < 0)
			break;
		newline = 0;
	}

	if (newline)
		(void)xwrite(to[1], "\n", 1);
	close(to[1]);
	/* restore */
	signal(SIGPIPE, oldsigpipe);

	if (mode == 2) {
		while ((r = read(from[0], buf, sizeof buf)) > 0) {
			ttywrite(buf, r, 1);
		}
		close(from[0]);
	}
}

void
externalpipe_screen_in(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_SCREEN, EXTERNAL_PIPE_PRINT);
}

void
externalpipe_screen_out(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_SCREEN, EXTERNAL_PIPE_SENDONLY);
}

void
externalpipe_screen_inject(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_SCREEN, EXTERNAL_PIPE_INJECT);
}

void
externalpipe_hist_in(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_ETERNAL, EXTERNAL_PIPE_PRINT);
}

void
externalpipe_hist_out(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_ETERNAL, EXTERNAL_PIPE_SENDONLY);
}

void
externalpipe_hist_inject(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_ETERNAL, EXTERNAL_PIPE_INJECT);
}

void
externalpipe_osc133_in(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_OSC133, EXTERNAL_PIPE_PRINT);
}

void
externalpipe_osc133_out(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_OSC133, EXTERNAL_PIPE_SENDONLY);
}

void
externalpipe_osc133_inject(const Arg *arg)
{
	extpipe(arg, EXTERNAL_SCOPE_OSC133, EXTERNAL_PIPE_INJECT);
}
