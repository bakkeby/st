#include <sys/wait.h>

void
plumb(const Arg *arg)
{
	char *sel = xsel.primary;
	char *cmd = (char *)arg->v;

	if (sel == NULL || cmd == NULL)
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
			if (execvp(cmd, (char *const []){cmd, sel, 0}) == -1)
				exit(1);
			exit(0);
	}
}
