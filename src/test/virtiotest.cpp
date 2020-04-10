#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "virtiodevices.h"

int main(int argc, const char **argv)
{

    VirtioDevices virtio_devices;

    PhysMemoryRange *pr = virtio_devices.get_phys_mem_range(0x40000000);
    fprintf(stderr, "pr = %p pr->opaque %p\n", pr, pr->opaque);
    uint32_t val = pr->read_func(pr->opaque, 0, 2);
    fprintf(stderr, "pr.magic    = %08x\n", pr->read_func(pr->opaque, 0, 2));
    fprintf(stderr, "pr.version  = %08x\n", pr->read_func(pr->opaque, 4, 2));
    fprintf(stderr, "pr.deviceid = %08x\n", pr->read_func(pr->opaque, 8, 2));
    return 0;
}
