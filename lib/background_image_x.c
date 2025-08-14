void
updatexy()
{
	Window child;
	XTranslateCoordinates(xw.dpy, xw.win, DefaultRootWindow(xw.dpy), 0, 0, &win.x, &win.y, &child);
}

/*
 * load farbfeld file to XImage
 */
XImage*
loadff(const char *filename)
{
	uint32_t i, hdr[4], w, h, size;
	uint64_t *data;
	FILE *f = fopen(filename, "rb");

	if (f == NULL) {
		fprintf(stderr, "could not load background image.\n");
		return NULL;
	}

	if (fread(hdr, sizeof(*hdr), LEN(hdr), f) != LEN(hdr)) {
		fprintf(stderr, "fread: %s\n", ferror(f) ? "" : "Unexpected end of file reading header");
		fclose(f);
		return NULL;
	}

	if (memcmp("farbfeld", hdr, sizeof("farbfeld") - 1)) {
		fprintf(stderr, "Invalid magic value\n");
		fclose(f);
		return NULL;
	}

	w = ntohl(hdr[2]);
	h = ntohl(hdr[3]);
	size = w * h;
	data = xmalloc(size * sizeof(uint64_t));

	if (fread(data, sizeof(uint64_t), size, f) != size) {
		fprintf(stderr, "fread: %s\n", ferror(f) ? "" : "Unexpected end of file reading data");
		fclose(f);
		return NULL;
	}

	fclose(f);

	for (i = 0; i < size; i++)
		 data[i] = (data[i] & 0x00000000000000FF) << 16 |
		           (data[i] & 0x0000000000FF0000) >> 8  |
		           (data[i] & 0x000000FF00000000) >> 32 |
		           (data[i] & 0x00FF000000000000) >> 24;

	XImage *xi = XCreateImage(xw.dpy, xw.vis, xw.depth, ZPixmap, 0,
		(char *)data, w, h, 32, w * 8);
	xi->bits_per_pixel = 64;
	return xi;
}

/*
 * initialize background image
 */
void
bginit()
{
	XGCValues gcvalues;
	Drawable bgimg;
	XImage *bgxi = loadff(bgfile);

	memset(&gcvalues, 0, sizeof(gcvalues));
	xw.bggc = XCreateGC(xw.dpy, xw.win, 0, &gcvalues);
	if (!bgxi)
		return;
	bgimg = XCreatePixmap(xw.dpy, xw.win, bgxi->width, bgxi->height,
		xw.depth);
	XPutImage(xw.dpy, bgimg, dc.gc, bgxi, 0, 0, 0, 0, bgxi->width, bgxi->height);
	XDestroyImage(bgxi);
	XSetTile(xw.dpy, xw.bggc, bgimg);
	XSetFillStyle(xw.dpy, xw.bggc, FillTiled);
	if (pseudotransparency) {
		updatexy();
		MODBIT(xw.attrs.event_mask, 1, PropertyChangeMask);
		XChangeWindowAttributes(xw.dpy, xw.win, CWEventMask, &xw.attrs);
	}
}

#if BACKGROUND_IMAGE_RELOAD_PATCH
void
reload_image()
{
	XFreeGC(xw.dpy, xw.bggc);
	bginit();
	redraw();
}
#endif // XRESOURCES_RELOAD_PATCH
