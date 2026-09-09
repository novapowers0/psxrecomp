/* Framework-owned widescreen activation for Final Bout.
 * The mod controls display aspect through the trusted plugin boundary. The
 * per-title cull/FOV sites remain independently gated by game.toml. */
#include "mod_plugins.h"

#include <string.h>

#define PKG_WS "psx.enhancement.widescreen"

static void builtin_widescreen_activate(void) {
    char aspect[16] = "";
    const int have = psx_mod_option_value(PKG_WS, "widescreen", "aspect",
                                          aspect, sizeof aspect);
    if (have && strcmp(aspect, "21:9") == 0)
        (void)psx_mod_set_fixed_display_aspect(21u, 9u);
    else
        (void)psx_mod_set_fixed_display_aspect(16u, 9u);
}

PSX_MOD_CONSTRUCTOR(psx_register_builtin_widescreen_plugin) {
    (void)psx_mod_register_activation_plugin("psx.widescreen",
                                             builtin_widescreen_activate);
}
