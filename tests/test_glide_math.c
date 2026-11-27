/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "../firmware/src/glide_math.h"
static void run(int32_t steps, uint32_t ms10, uint32_t fs, uint32_t ctl)
{
    kr_glide_state_t g = {0};
    const int32_t start = 65 * 16 * 65536;
    const int32_t dest = start + steps * 65536;
    uint32_t n = kr_glide_time_ticks(ms10, fs, ctl);
    int32_t prev = start;
    kr_glide_begin(&g, start, dest, n);
    for (uint32_t i=0; i<n; ++i) {
        kr_glide_tick(&g, dest);
        if (steps>0) assert(g.pos>=prev && g.pos<=dest);
        if (steps<0) assert(g.pos<=prev && g.pos>=dest);
        if (i+1<n) assert(g.pos!=dest);
        prev = g.pos;
    }
    assert(g.pos==dest && g.remaining==0);
    printf("delta %d/16, time %.3fs\n",steps,(double)n*ctl/fs);
}
int main(void)
{
    run(16,100000,44100,32);
    run(-16,100000,44100,32);
    run(192,100000,44100,32);
    run(16,50000,48000,32);
    assert(kr_glide_inc(100000u,101000u,32768u)==100500u);
    puts("PASS");
    return 0;
}
