
#include <stddef.h>
#include <stdint.h>

void copy_from_bank2(uint64_t *dest, const uint64_t *src, size_t count, uint32_t *done)
{
    for (size_t i = 0; i < count; ++i) {
	*dest++ = *src++;
    }
    *done = count;
    while (1)
	;
}
