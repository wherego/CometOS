#include <stdint.h>
#include <string.h>

#include "../arch/i386/log.h"
#include "../arch/i386/paging.h"
#include "../arch/i386/heap.h"
#include <kernel/portio.h>

#define INDEX_GET(i) (i / 0x1000)
#define ADDR_GET(i) (i * 0x1000)

