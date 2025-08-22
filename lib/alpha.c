float
clamp(float value, float lower, float upper) {
	if (value < lower)
		return lower;
	if (value > upper)
		return upper;
	return value;
}

void
changealpha(const Arg *arg)
{
	enablefunc(Alpha);

	if ((alpha > 0 && arg->f < 0) || (alpha < 1 && arg->f > 0))
		alpha += arg->f;
	alpha = clamp(alpha, 0.0, 1.0);
	xloadcols();
	redraw();
}

void
changealphaunfocused(const Arg *arg)
{
	enablefunc(AlphaFocusHighlight);

	if ((alpha_unfocused > 0 && arg->f < 0) || (alpha_unfocused < 1 && arg->f > 0))
		alpha_unfocused += arg->f;
	alpha_unfocused = clamp(alpha_unfocused, 0.0, 1.0);
	xloadcols();
	redraw();
}
