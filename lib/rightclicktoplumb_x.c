#include <sys/wait.h>

void
plumb(const Arg *arg)
{
	char *sel = (arg->v ? (char*)arg->v : xsel.primary);

	if (sel == NULL || plumb_cmd == NULL)
		return;

	char cwd[PATH_MAX];
	pid_t child;

	if (subprocwd(cwd) != 0)
		return;

	switch (child = fork()) {
		case -1:
			return;
		case 0:
			if (chdir(cwd) != 0)
				exit(1);
			if (execvp(plumb_cmd, (char *const []){plumb_cmd, sel, 0}) == -1)
				exit(1);
			exit(0);
	}
}
