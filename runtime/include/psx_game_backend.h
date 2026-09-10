/* psx_game_backend.h — universal multi-region game-image registry.
 *
 * Historically one executable linked exactly one recompiled game image, which
 * exported the psx_game_* dispatch interface under its own name. A universal
 * build links several regional images of the same title: each is compiled
 * under a distinct symbol prefix ([recompiler] symbol_prefix) and exports one
 * PsxGameBackend descriptor instead of the interface symbols. This registry
 * owns the unprefixed interface and forwards every call to the image selected
 * for the boot EXE currently loaded, so no runtime call site changed.
 *
 * The generated dispatcher self-registers its descriptor with a static
 * constructor, so linking the image is enough — no central list to maintain.
 */

#ifndef PSX_GAME_BACKEND_H
#define PSX_GAME_BACKEND_H

#include <stdint.h>

#include "cpu_state.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PsxGameBackend {
    const char* boot_exe_stem;   /* e.g. "SLES_017.22" — selection key */
    const char* game_id;         /* e.g. "SLES-01722" (informational) */
    uint32_t load_address;       /* guest load vaddr for THIS image */
    uint32_t entry_pc;           /* guest entry PC for THIS image (timing) */
    uint32_t text_size;          /* static text segment size for THIS image */
    int (*address_in_text)(uint32_t addr);
    int (*dispatch_compiled)(CPUState* cpu, uint32_t addr);
    int (*is_function_entry)(uint32_t addr);
    int (*text_native_ok)(uint32_t addr);
    int (*text_native_ok_full)(uint32_t addr);
} PsxGameBackend;

/* Called by each image's generated dispatcher at load (static ctor). */
void psx_game_backend_register(const PsxGameBackend* backend);

/* Select the image whose boot_exe_stem matches (case-insensitive, extension
 * and directory ignored by the caller). Returns 1 on match, 0 if none. */
int psx_game_backend_select(const char* boot_exe_stem);

/* Non-mutating lookup: the image that owns this serial/stem, or NULL. Safe to
 * call from the launcher's disc-verify pass (does not change the active image). */
const PsxGameBackend* psx_game_backend_match(const char* boot_exe_stem);

/* Active image, or NULL before selection. */
const PsxGameBackend* psx_game_backend_active(void);

#ifdef __cplusplus
}
#endif

#endif /* PSX_GAME_BACKEND_H */
