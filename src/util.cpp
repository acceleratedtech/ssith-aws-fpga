
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

#include "util.h"


int copyFile(char *buffer, const char *filename, size_t buffer_size)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: failed to open file %s: %s\r\n", filename, strerror(errno));
        return -errno;
    }
    size_t bytes_copied = 0;
    do {
        char readbuf[4096];
        size_t readsize = sizeof(readbuf);
        if (readsize > (buffer_size - bytes_copied))
            readsize = (buffer_size - bytes_copied);
        int bytes_read = read(fd, readbuf, readsize);
        if (readsize <= 0) {
            fprintf(stderr, "Error: failed to read file %s at offset %ld: %s\r\n", filename, bytes_copied, strerror(errno));
            return -errno;
        }
        if (!bytes_read)
            break;
        memcpy(buffer + bytes_copied, readbuf, bytes_read);
        bytes_copied += bytes_read;
        debugLog("Copied %d bytes %ld bytes total readsize %ld\r\n", bytes_read, bytes_copied, readsize);
    } while (bytes_copied < buffer_size);
    close(fd);
    debugLog("Read %ld bytes from %s\r\n", bytes_copied, filename);
    return bytes_copied;
}

static int debug = 0;

void setEnableDebugLog(int enable)
{
    debug = enable;
}

int debugLog(const char * __restrict format, ...)
{
    if (!debug)
        return 0;

    va_list ap;
    va_start(ap, format);
    int ret = vfprintf(stderr, format, ap);
    va_end(ap);
    return ret;
}
