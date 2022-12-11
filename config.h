/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int gappx     = 10;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Hack Nerd Font Mono:size=10" };
static const char dmenufont[]       = "TerminessTTF Nerd Font Mono:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_red[]         = "#ff5370";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_red  },
};
/* rofi look */
static const char rofi_font[]      = "hack 10";
static const char rofi_theme[]     = "solarized";
static const char rofi_icontheme[] = "Papirus";

/* tagging */
static const char *tags[] = { "", "", "爵", "", "", "" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class               instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "st-256color",       NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "Alacritty",         NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "VSCodium",          NULL,     NULL,           1 << 1,    0,          0,           0,        -1 },
	{ "QtCreator",         NULL,     NULL,           1 << 1,    0,          0,           0,        -1 },
	{ "Firefox",           NULL,     NULL,           1 << 2,    0,          0,          -1,        -1 },
	{ "Chromium",          NULL,     NULL,           1 << 2,    0,          0,          -1,        -1 },
	{ "TelegramDesktop",   NULL,     NULL,           1 << 2,    0,          0,          -1,        -1 },
	{ "discord",           NULL,     NULL,           1 << 2,    0,          0,          -1,        -1 },
	{ "Steam",             NULL,     NULL,           1 << 3,    0,          0,          -1,        -1 },
	{ "Lutris",            NULL,     NULL,           1 << 3,    0,          0,          -1,        -1 },
	{ "Gimp",              NULL,     NULL,           1 << 4,    1,          0,           0,        -1 },
	{ NULL,                NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *roficmd[] = { "rofi", "-combi-modi", "window,drun,ssh", "-theme", rofi_theme, "-font", rofi_font, "-show", "combi", "icon-theme", rofi_icontheme, "-show-icons", NULL };
static const char *termcmd[]  = { "alacritty", NULL };
/* vol */
static const char *inc_vol[] = { "inc_vol.sh", "+2%", NULL };
static const char *dec_vol[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };
static const char *mute_spkr[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };
static const char *mute_mic[] = { "pactl", "set-source-mute", "@DEFAULT_SOURCE@", "toggle", NULL };
/* media */
static const char *stop_media[] = { "playerctl", "stop", NULL };
static const char *play_media[] = { "playerctl", "play", NULL };
static const char *pause_media[] = { "playerctl", "pause", NULL };
static const char *next_media[] = { "playerctl", "next", NULL };
static const char *prev_media[] = { "playerctl", "previous", NULL };
/* brightness */
static const char *inc_brightness[] = { "xbacklight", "-inc", "2", NULL };
static const char *dec_brightness[] = { "xbacklight", "-dec", "2", NULL };

static const Key keys[] = {
	/* modifier                     key        			function        argument */
	{ MODKEY,                       XK_p,      			spawn,          {.v = roficmd } },
	{ MODKEY|ShiftMask,             XK_Return, 			spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      			togglebar,      {0} },
	{ MODKEY,                       XK_j,      			focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      			focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      			incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      			incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      			setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      			setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, 			zoom,           {0} },
	{ MODKEY,                       XK_Tab,    			view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      			killclient,     {0} },
	{ MODKEY,                       XK_t,      			setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      			setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      			setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  			setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  			togglefloating, {0} },
	{ MODKEY,                       XK_0,      			view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      			tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  			focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, 			focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  			tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, 			tagmon,         {.i = +1 } },

	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },

	{ NULL,				XF86XK_AudioRaiseVolume,	spawn,		{.v = inc_vol} },
	{ NULL,				XF86XK_AudioLowerVolume,	spawn,		{.v = dec_vol} },
	{ NULL,				XF86XK_AudioMute,		spawn,		{.v = mute_spkr} },
	{ NULL,				XF86XK_AudioMicMute,		spawn,		{.v = mute_mic} },
	{ NULL,				XF86XK_AudioStop,		spawn,		{.v = stop_media} },
	{ NULL,				XF86XK_AudioPlay,		spawn,		{.v = play_media} },
	{ NULL,				XF86XK_AudioPause,		spawn,		{.v = pause_media} },
	{ NULL,				XF86XK_AudioNext,		spawn,		{.v = next_media} },
	{ NULL,				XF86XK_AudioPrev,		spawn,		{.v = prev_media} },
	{ NULL,				XF86XK_MonBrightnessUp,		spawn,		{.v = inc_brightness} },
	{ NULL,				XF86XK_MonBrightnessDown,	spawn,		{.v = dec_brightness} },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

