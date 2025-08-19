int
enabled(const uint64_t functionality)
{
	return (settings & functionality) > 0;
}

int
disabled(const uint64_t functionality)
{
	return !(settings & functionality);
}

void
enablefunc(const uint64_t functionality)
{
	settings |= functionality;
}

void
disablefunc(const uint64_t functionality)
{
	settings &= ~functionality;
}

void
setenabled(const uint64_t functionality, int enabled)
{
	if (enabled) {
		enablefunc(functionality);
	} else {
		disablefunc(functionality);
	}
}

void
togglefunc(const uint64_t functionality)
{
	settings ^= functionality;
}

const uint64_t
getfuncbyname(const char *name)
{
	int i;

	if (!name)
		return 0;

	for (i = 0; function_names[i].name != NULL; i++) {
		if (strcmp(function_names[i].name, name) == 0)
			return function_names[i].value;
	}

	return 0;
}

const char *
getnamebyfunc(const uint64_t functionality)
{
	int i;

	for (i = 0; function_names[i].name != NULL; i++) {
		if (function_names[i].value == functionality)
			return function_names[i].name;
	}

	return 0;
}


