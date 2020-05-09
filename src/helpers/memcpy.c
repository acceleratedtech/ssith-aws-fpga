#include <stddef.h>
#include <stdint.h>

typedef uint64_t BLOCK;

/* Not a general memcpy; must call with dest % sizeof(BLOCK) == src % sizeof(BLOCK) */
void copy_from_bank2(char *dest, const char *src, size_t len, volatile size_t *done)
{
    char *end;
    /* Copy leading bytes */
    size_t leading = (sizeof(BLOCK) - ((size_t)src % sizeof(BLOCK))) % sizeof(BLOCK);
    if (leading > len) {
	leading = len;
    }
    for (end = dest + leading; dest != end;) {
	*dest++ = *src++;
    }
    /* Copy blocks */
    size_t blocks = (len - leading) / sizeof(BLOCK);
    for (end = dest + blocks * sizeof(BLOCK); dest != end;) {
	*(BLOCK *)dest = *(const BLOCK *)src;
	dest += sizeof(BLOCK);
	src += sizeof(BLOCK);
    }
    /* Copy trailing bytes */
    size_t trailing = (len - leading) % sizeof(BLOCK);
    for (end = dest + trailing; dest != end;) {
	*dest++ = *src++;
    }
    *done = len;
    while (1)
	;
}
