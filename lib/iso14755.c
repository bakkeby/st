void
iso14755(const Arg *arg)
{
	FILE *p;
	char *us, *e, codepoint[9], uc[4];
	unsigned long utf32;

	if (!iso14755cmd)
		return;

	if (!(p = popen(iso14755cmd, "r")))
		return;

	us = fgets(codepoint, sizeof(codepoint), p);
	pclose(p);

	if (!us || *us == '\0' || *us == '-' || strlen(us) > 7)
		return;
	if ((utf32 = strtoul(us, &e, 16)) == ULONG_MAX ||
	    (*e != '\n' && *e != '\0'))
		return;

	ttywrite(uc, utf8encode(utf32, uc), 1);
}
