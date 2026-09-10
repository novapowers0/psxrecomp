/* psx_game_backend.c — universal multi-region game-image registry.
 *
 * Compiled for every target (harmless): when PSX_GAME_BACKEND_REGISTRY is not
 * defined the file is empty and the generated dispatcher's own strong
 * definitions are used, exactly as before. When it IS defined the registry
 * owns the unprefixed psx_game_* interface and forwards to the selected
 * image, and game_dispatch_compat.c opts out so it cannot double-define them.
 */

#include "psx_game_backend.h"

#include <ctype.h>

#if defined(PSX_GAME_BACKEND_REGISTRY)

#define PSX_GAME_BACKEND_MAX 8

static const PsxGameBackend* g_backends[PSX_GAME_BACKEND_MAX];
static int                   g_backend_count = 0;
static const PsxGameBackend* g_active = 0;

void psx_game_backend_register(const PsxGameBackend* backend) {
    if (!backend) return;
    if (g_backend_count < PSX_GAME_BACKEND_MAX)
        g_backends[g_backend_count++] = backend;
}

const PsxGameBackend* psx_game_backend_active(void) {
    /* A single-image build (the isolated per-region targets, or a dev run with
     * no mounted disc) has exactly one candidate, so use it without requiring
     * an explicit serial select. A universal build with several images fails
     * closed until a serial picks one. */
    if (g_active) return g_active;
    if (g_backend_count == 1) return g_backends[0];
    return 0;
}

/* Identity comparison that ignores separators and case, so "SLES-01722",
 * "SLES_017.22" and "sles01722" all name the same image. */
static int key_equal(const char* a, const char* b) {
    if (!a || !b) return 0;
    for (;;) {
        while (*a && !isalnum((unsigned char)*a)) ++a;
        while (*b && !isalnum((unsigned char)*b)) ++b;
        if (*a == 0 || *b == 0) return (*a == 0 && *b == 0);
        int ca = toupper((unsigned char)*a++);
        int cb = toupper((unsigned char)*b++);
        if (ca != cb) return 0;
    }
}

const PsxGameBackend* psx_game_backend_match(const char* serial) {
    for (int i = 0; i < g_backend_count; ++i) {
        if (key_equal(g_backends[i]->game_id, serial) ||
            key_equal(g_backends[i]->boot_exe_stem, serial)) {
            return g_backends[i];
        }
    }
    return 0;
}

int psx_game_backend_select(const char* serial) {
    const PsxGameBackend* be = psx_game_backend_match(serial);
    if (be) {
        g_active = be;
        return 1;
    }
    return 0;
}

/* Unprefixed interface: forwarded to the selected image. Returning the
 * fail-closed default before selection guarantees no regional code can run
 * against the wrong image during early boot. */
int psx_dispatch_game_compiled(CPUState* cpu, uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->dispatch_compiled(cpu, addr) : 0;
}

int psx_game_is_function_entry(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->is_function_entry(addr) : 0;
}

int psx_game_text_native_ok(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->text_native_ok(addr) : 0;
}

int psx_game_text_native_ok_full(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->text_native_ok_full(addr) : 0;
}

int psx_game_address_in_text(uint32_t addr) {
    const PsxGameBackend* be = psx_game_backend_active();
    return be ? be->address_in_text(addr) : 0;
}

#endif /* PSX_GAME_BACKEND_REGISTRY */
