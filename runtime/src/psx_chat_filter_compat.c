#include "psx_chat_filter_compat.h"

int rnet_chat_filter_apply(char *text, size_t capacity) {
    /* Compatibility fallback for the current pinned recomp-net API. Do not
     * claim filtering until a real shared filter implementation is linked. */
    (void)text;
    (void)capacity;
    return 0;
}
