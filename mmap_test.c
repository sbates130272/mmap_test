////////////////////////////////////////////////////////////////////////
//
// Copyright 2014 PMC-Sierra, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0 Unless required by
// applicable law or agreed to in writing, software distributed under the
// License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for
// the specific language governing permissions and limitations under the
// License.
//
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
//   Author: Logan Gunthorpe
//
//   Date:   Oct 23 2014
//
//   Description:
//     Test mmapping a file and attempting to find the physical
//     address. Also holds an mmap in a while(1) loop so the /proc/<pid>
//     filesystem can be inspected
//
////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

static size_t get_fd_size(int fd)
{
    struct stat stat;
    fstat(fd, &stat);

    if (stat.st_size)
        return stat.st_size;

    size_t ret = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return ret;
}



int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s FILE\n", argv[0]);
        return -1;
    }

    printf("PID: %d\n", getpid());

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "mmap_test: %s: %s\n", argv[1], strerror(errno));
        return -1;
    }

    size_t fd_size = get_fd_size(fd);
    uint32_t *addr = mmap((void *) 0x80000000, fd_size, PROT_WRITE | PROT_READ,
                          MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap_test: mmap failed");
        return -1;
    }

    volatile int x = addr[0];
    (void) x;

    int pm = open("/proc/self/pagemap", O_RDONLY);
    lseek(pm, (intptr_t) addr / getpagesize() * 8, SEEK_SET);
    uint64_t page;
    read(pm, &page, sizeof(page));
    printf("Physical Address: 0x%" PRIx64 "\n", page << 12);

    while (1) {
        sleep(5);
    }


    munmap(addr, fd_size);

    return 0;
}
