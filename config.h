/* See LICENSE file for copyright and license details. */

/* appearance */
static const char *fonts[] = {
	"Ubuntu mono:size=14"
};
static const char dmenufont[]       = "Ubuntu mono:size=14";
static const char normbordercolor[] = "#444444";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#bbbbbb";
static const char selbordercolor[]  = "#005577";
static const char selbgcolorlang0[] = "#005577";
static const char selbgcolorlang1[] = "#007733";
static const char selfgcolor[]      = "#eeeeee";
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, 0: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */

static char *bat_chart_colors[] = {"#FF3388", "#49CC35"};
static char *mem_chart_colors[] = {"#007799", "#FFFFFF", "#005577"};
static char *cpu_chart_colors[] = {"#FF6E00", "#CB0C29", "#49CC35", "#0077FF"};
static Chart charts[] = {
	/* width  rows  max value  rows colors        backgr color  some inner data (will be set at init) */
	{ 50,     2,    10000,     bat_chart_colors,  "#000000",    NULL, NULL, NULL },
	{ 50,     3,    1024,      mem_chart_colors,  "#000000",    NULL, NULL, NULL },
	{ 50,     4,    400,       cpu_chart_colors,  "#000000",    NULL, NULL, NULL },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	//{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 1,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

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

//#include "xkbcommon/xkbcommon-keysyms.h"
#define XKB_KEY_XF86Sleep    0x1008FF2F
#define XKB_KEY_XF86PowerOff 0x1008FF2A

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolorlang0, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "gnome-terminal", NULL };
static const char *sleepcmd[] = { "systemctl", "suspend", NULL };

void spawn_dmenu(const Arg *arg) {
	const char *bgcolor = kbdlayout == 0 ? selbgcolorlang0 : selbgcolorlang1;
	dmenucmd[10] = bgcolor;
	spawn(arg);
}

void lcd_light_inc();
void lcd_light_dec();
void kbd_light_inc();
void kbd_light_dec();

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_bracketleft,spawn_dmenu,{.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	//{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	//{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
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
	// backlight
	{ MODKEY,                       XK_0,      lcd_light_dec,  {0} },
	{ MODKEY|ShiftMask,             XK_0,      lcd_light_inc,  {0} },
	{ MODKEY,                       XK_minus,  kbd_light_dec,  {0} },
	{ MODKEY|ShiftMask,             XK_minus,  kbd_light_inc,  {0} },
	// screenshots
	{ 0,                            XKB_KEY_XF86PowerOff,  spawn,  {.v = (char*[]){"gnome-screenshot",        NULL}} },
	{ ControlMask,                  XKB_KEY_XF86PowerOff,  spawn,  {.v = (char*[]){"gnome-screenshot", "-c",  NULL}} },
	{ ShiftMask,                    XKB_KEY_XF86PowerOff,  spawn,  {.v = (char*[]){"gnome-screenshot", "-a",  NULL}} },
	{ ControlMask|ShiftMask,        XKB_KEY_XF86PowerOff,  spawn,  {.v = (char*[]){"gnome-screenshot", "-ca", NULL}} },
	// smth
	{ 0,                            XKB_KEY_XF86Sleep, spawn,  {.v = sleepcmd} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
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


static const char LCD_BACKLIGHT_PATH[] = "/sys/devices/pci0000:00/0000:00:02.0/drm/card0/card0-eDP-1/intel_backlight/brightness";
static const char KBD_BACKLIGHT_PATH[] = "/sys/devices/platform/asus-nb-wmi/leds/asus::kbd_backlight/brightness";
int lcd_inc_func(int v) { return MAX(1, (int)(v * 1.5f + 0.99)); }
int lcd_dec_func(int v) { return MAX(1, (int)(v / 1.5f       )); }
int kbd_inc_func(int v) { return v + 1; }
int kbd_dec_func(int v) { return v - 1; }

void
update_file(const char *path, int (*func)(int)) {
	FILE* fd = fopen(path, "r+");
	int i;
	fscanf(fd, "%d", &i);
	i = func(i);
	
	// works with just fopen(..., "r+"), but this may be useful one day
	//fd = freopen(NULL, "w", fd);
	//if(!fd) {
	//	fprintf(stderr, "Failed to reopen %s\n", path);
	//	return;
	//}
	fprintf(fd, "%d\n", i);
	fclose(fd);
}

void
lcd_light_inc() {
	update_file(LCD_BACKLIGHT_PATH, lcd_inc_func);
}

void
lcd_light_dec() {
	update_file(LCD_BACKLIGHT_PATH, lcd_dec_func);
}

void
kbd_light_inc() {
	update_file(KBD_BACKLIGHT_PATH, kbd_inc_func);
}

void
kbd_light_dec() {
	update_file(KBD_BACKLIGHT_PATH, kbd_dec_func);
}
