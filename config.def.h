#define ROOTCOLOR 0x222222ff
#define BORDERCOLOR 0x444444ff
#define FOCUSCOLOR 0x005577ff
#define URGENTCOLOR 0xff0000ff

/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 0;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartgaps                 = 0;  /* 1 means no outer gap when there is only one window */
static int gaps                            = 1;  /* 1 means gaps between windows are added */
static const unsigned int gappx            = 10; /* gap pixel between windows */
static const unsigned int borderpx         = 2;  /* border pixel of windows */
static const float rootcolor[]             = COLOR(ROOTCOLOR);
static const float bordercolor[]           = COLOR(BORDERCOLOR);
static const float focuscolor[]            = COLOR(FOCUSCOLOR);
static const float urgentcolor[]           = COLOR(URGENTCOLOR);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (9)

/* logging */
static int log_level = WLR_ERROR;

/* NOTE: ALWAYS keep a rule declared even if you don't use rules (e.g leave at least one example) */
static const Rule rules[] = {
	/* app_id             title       tags mask     isfloating   monitor */
	{ "clipse",           NULL,       0,            1,           -1 },
	/* examples: */
	{ "Gimp_EXAMPLE",     NULL,       0,            1,           -1 }, /* Start on currently visible tags floating, not tiled */
	{ "firefox_EXAMPLE",  NULL,       1 << 8,       0,           -1 }, /* Start on ONLY tag "9" */
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899
*/
/* NOTE: ALWAYS add a fallback rule, even if you are completely sure it won't be used */
static const MonitorRule monrules[] = {
	/* name       mfact  nmaster scale  layout       rotate/reflect                x    y */
	{ "eDP-1",    0.55f, 1,      1.25f, &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   0,  0 },
	/* defaults */
	{ NULL,       0.55f, 1,      1,     &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL,   -1,  -1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	/* example:
	.options = "ctrl:nocaps",
	*/
	.options = NULL,
};

static const int repeat_rate = 25;
static const int repeat_delay = 400;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 0;
static const int natural_scrolling = 1;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }

static const char *clipboard_cmd[] = { "foot", "-T", "clipse", "-a", "clipse", "clipse", NULL };
static const char *term_cmd[] = { "foot", NULL };
static const char *browser_cmd[] = { "firefox", NULL };
static const char *brightness_down_cmd[] = { "brightnessctl", "set", "-n", "15%-", NULL };
static const char *brightness_up_cmd[] = { "brightnessctl", "set", "-n", "15%+", NULL };
static const char *mic_mute_cmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SOURCE@", "toggle", NULL };
static const char *mute_cmd[] = { "wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle", NULL };
static const char *volume_down_cmd[] = { "wpctl", "set-volume", "-l", "1.0", "@DEFAULT_AUDIO_SINK@", "2%-", NULL };
static const char *volume_up_cmd[] = { "wpctl", "set-volume", "-l", "1.0", "@DEFAULT_AUDIO_SINK@", "2%+", NULL };

static const Key keys[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* modifier                  key                   function        argument */
	{ MODKEY,                    XKB_KEY_q,            spawn,          {.v = term_cmd} },
	{ MODKEY,                    XKB_KEY_a,            spawn,          {.v = browser_cmd} },
	{ MODKEY,                    XKB_KEY_v,            spawn,          {.v = clipboard_cmd} },
	{ MODKEY,                    XKB_KEY_j,            focusstack,     {.i = +1} },
	{ MODKEY,                    XKB_KEY_k,            focusstack,     {.i = -1} },
	{ MODKEY|WLR_MODIFIER_ALT,   XKB_KEY_j,            focusmon,       {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_ALT,   XKB_KEY_k,            focusmon,       {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_J,            tagmon,         {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_K,            tagmon,         {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY,                    XKB_KEY_bracketright, incnmaster,     {.i = +1} },
	{ MODKEY,                    XKB_KEY_bracketleft,  incnmaster,     {.i = -1} },
	{ MODKEY,                    XKB_KEY_h,            setmfact,       {.f = -0.05f} },
	{ MODKEY,                    XKB_KEY_l,            setmfact,       {.f = +0.05f} },
	{ MODKEY,                    XKB_KEY_Return,       zoom,           {0} },
	{ MODKEY,                    XKB_KEY_Tab,          view,           {0} },
	{ MODKEY,                    XKB_KEY_g,            togglegaps,     {0} },
	{ MODKEY,                    XKB_KEY_c,            killclient,     {0} },
	{ MODKEY,                    XKB_KEY_t,            setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                    XKB_KEY_f,            setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                    XKB_KEY_m,            setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                    XKB_KEY_space,        setlayout,      {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_space,        togglefloating, {0} },
	{ MODKEY,                    XKB_KEY_e,            togglefullscreen, {0} },
	{ MODKEY,                    XKB_KEY_0,            view,           {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_parenright,   tag,            {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT, XKB_KEY_M,            quit,           {0} },
	{ 0,                         XKB_KEY_XF86MonBrightnessDown, spawn, {.v = brightness_down_cmd} },
	{ 0,                         XKB_KEY_XF86MonBrightnessUp,   spawn, {.v = brightness_up_cmd} },
	{ 0,                         XKB_KEY_XF86MonBrightnessUp,   spawn, {.v = brightness_up_cmd} },
	{ 0,                         XKB_KEY_XF86AudioMicMute,      spawn, {.v = mic_mute_cmd} },
	{ 0,                         XKB_KEY_XF86AudioMute,         spawn, {.v = mute_cmd} },
	{ 0,                         XKB_KEY_XF86AudioLowerVolume,  spawn, {.v = volume_down_cmd} },
	{ 0,                         XKB_KEY_XF86AudioRaiseVolume,  spawn, {.v = volume_up_cmd} },

	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                     0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_at,                         1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                 2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_dollar,                     3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                    4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                  6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                   7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                  8),
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
};
