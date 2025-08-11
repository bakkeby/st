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
togglefunc(const uint64_t functionality)
{
	settings ^= functionality;
}

#define map(F) else if (!strcasecmp(name, #F)) return F

const uint64_t
getfuncbyname(const char *name)
{
	if (!name)
		return 0;

	map(FuncPlaceholderSmartGaps);
	map(FuncPlaceholderSmartGapsMonocle);
	map(FuncPlaceholderSwallow);
	map(FuncPlaceholderSwallowFloating);
	map(FuncPlaceholderCenteredWindowName);
	map(FuncPlaceholderBarActiveGroupBorderColor);
	map(FuncPlaceholderBarMasterGroupBorderColor);
	map(FuncPlaceholderSpawnCwd);
	map(FuncPlaceholderColorEmoji);
	map(FuncPlaceholderStatus2DNoAlpha);
	map(FuncPlaceholderSystray);
	map(FuncPlaceholderBarBorder);
	map(FuncPlaceholderNoBorders);
	map(FuncPlaceholderWarp);
	map(FuncPlaceholderFocusedOnTop);
	map(FuncPlaceholderDecorationHints);
	map(FuncPlaceholderFocusOnNetActive);
	map(FuncPlaceholderAllowNoModifierButtons);
	map(FuncPlaceholderCenterSizeHintsClients);
	map(FuncPlaceholderResizeHints);
	map(FuncPlaceholderSortScreens);
	map(FuncPlaceholderViewOnWs);
	map(FuncPlaceholderXresources);
	map(FuncPlaceholderAltWorkspaceIcons);
	map(FuncPlaceholderGreedyMonitor);
	map(FuncPlaceholderSmartLayoutConversion);
	map(FuncPlaceholderSmartLayoutConvertion);
	map(FuncPlaceholderAutoHideScratchpads);
	map(FuncPlaceholderRioDrawIncludeBorders);
	map(FuncPlaceholderRioDrawSpawnAsync);
	map(FuncPlaceholderBarPadding);
	map(FuncPlaceholderRestrictFocusstackToMonitor);
	map(FuncPlaceholderAutoReduceNmaster);
	map(FuncPlaceholderWinTitleIcons);
	map(FuncPlaceholderWorkspacePreview);
	map(FuncPlaceholderSystrayNoAlpha);
	map(FuncPlaceholderWorkspaceLabels);
	map(FuncPlaceholderSnapToWindows);
	map(FuncPlaceholderSnapToGaps);
	map(FuncPlaceholderFlexWinBorders);
	map(FuncPlaceholderFocusOnClick);
	map(FuncPlaceholderFocusedOnTopTiled);
	map(FuncPlaceholderBanishMouseCursor);
	map(FuncPlaceholderFocusFollowMouse);
	map(FuncPlaceholderBanishMouseCursorToCorner);
	map(FuncPlaceholderStackerIcons);
	map(FuncPlaceholderAltWindowTitles);
	map(FuncPlaceholderBarBorderColBg);
	map(FuncPlaceholder140737488355328);
	map(FuncPlaceholder281474976710656);
	map(FuncPlaceholder562949953421312);
	map(FuncPlaceholder1125899906842624);
	map(FuncPlaceholder2251799813685248);
	map(FuncPlaceholder4503599627370496);
	map(FuncPlaceholder9007199254740992);
	map(FuncPlaceholder18014398509481984);
	map(FuncPlaceholder36028797018963968);
	map(FuncPlaceholderDebug);
	map(FuncPlaceholder144115188075855872);
	map(FuncPlaceholder288230376151711744);
	map(FuncPlaceholder576460752303423488);
	map(FuncPlaceholder1152921504606846976);
	map(FuncPlaceholder2305843009213693952);
	map(FuncPlaceholder4611686018427387904);
	map(FuncPlaceholder9223372036854775808);

	return 0;
}

#undef map
