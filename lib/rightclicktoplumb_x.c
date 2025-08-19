#include <sys/wait.h>

void
plumb(const Arg *arg)
{
	int i, len, substituted = 0;
	char *sel = xsel.primary;
	char **argv = ((char **)arg->v);

	if (sel == NULL || argv == NULL) {
		return;
	}

	/* Count the number of arguments */
	for (len = 0; argv[len]; len++);

	/* Our new command */
	char **cmd = calloc(len + 2, sizeof(char*));

	/* Subsitute %SEL% with selection */
	for (i = 0; i < len; i++) {
		if (!strcmp(argv[i], "%SEL%")) {
			cmd[i] = sel;
			substituted = 1;
		} else {
			cmd[i] = argv[i];
		}
	}

	/* or add it at the end if not specified */
	if (!substituted) {
		cmd[i] = sel;
		i++;
	}

	cmd[i] = NULL;

	char cwd[PATH_MAX];
	pid_t child;

	if (subprocwd(cwd) != 0)
		return;

	switch (child = fork()) {
		case -1:
			free(cmd);
			return;
		case 0:
			if (chdir(cwd) != 0)
				exit(1);
			if (execvp(cmd[0], cmd) == -1)
				exit(1);
			free(cmd);
			exit(0);
	}

	free(cmd);
}
