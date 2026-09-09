#ifndef PSX_CHAT_FILTER_COMPAT_H
#define PSX_CHAT_FILTER_COMPAT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The pinned recomp-net checkout used by this project predates chat_filter.h.
 * Keep the call site stable until the real shared filter is available. */
int rnet_chat_filter_apply(char *text, size_t capacity);

/* The current recomp-net pin calls the shared card-transfer channel SRAM.
 * Keep the newer runtime spelling local until the protocol grows a distinct
 * MEMCARD opcode. */
#ifndef RNET_STATE_OP_MEMCARD
#define RNET_STATE_OP_MEMCARD RNET_STATE_OP_SRAM
#endif

#ifdef __cplusplus
}
#endif

#endif
