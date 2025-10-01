
#include "DharaFTL.hpp"
#include "NandExample.hpp"
#include <cstdio>
#define debug(fmt, ...) \
    fprintf(stderr, "[%s:%s:%d] " fmt "\n", __FILE_NAME__, __func__, __LINE__, ##__VA_ARGS__)

#define GC_RATIO 4


char write_buf[512] = "Hello world!!!";
char read_buf[512];

constexpr uint32_t LOG2_PAGE_SIZE = 9; // 2^9 = 512 bytes
constexpr uint32_t LOG2_PAGES_PER_BLOCK = 3; // 2^3 = 8 (8 * 512 = 4096 - Erase block size, see NandExample.hpp)

// NandExample instance
static NandExample FlashMemory(LOG2_PAGE_SIZE, LOG2_PAGES_PER_BLOCK, NandExample::BLOCKS_NUMBER);
// Dhare flash layer translation instance
static DharaFTL Map;


// Основная программа
int main(void)
{
    size_t page_size = FlashMemory.GetPageSize();
    uint8_t page_buf[page_size];
    dhara_error_t err;

    debug("calling Map.Init() with GC_RATIO %u", GC_RATIO);
    Map.Init(&FlashMemory, page_buf, GC_RATIO);

    debug("Map.GetBlockNum(): %u", Map.GetBlockNum());
    debug("Map.GetBlockSize(): %u", Map.GetBlockSize());
    debug("Map.GetMemSize(): %u", Map.GetMemSize());
    printf("\n");

    const int iters = 5;
    debug("will perform %u writes & reads", iters);
    for (int i = 0; i < iters; i++)
    {

        debug("starting write %u/%u", i+1, iters);
        if (Map.Write(0, (const uint8_t *) write_buf, &err) < 0)
            printf("Write aborted!\r\n");

        debug("write %u/%u finished", i+1, iters);

        debug("starting read %u/%u", i+1, iters);
        if (Map.Read(0, (uint8_t *) read_buf, &err) < 0)
            printf("Read aborted!\r\n");

        debug("read %u/%u finished, result \"%s\"", i+1, iters, read_buf);
    }

    return 0;
}