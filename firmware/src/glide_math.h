/* SPDX-License-Identifier: GPL-3.0-only
 * Experimental high-resolution glide; NOT hardware-validated.
 * Q16 of the firmware's 1/16-semitone units. */
#ifndef KRIECHSTROM_GLIDE_MATH_H
#define KRIECHSTROM_GLIDE_MATH_H
#include <stdint.h>
typedef struct {
    int32_t pos, step, remainder, error;
    uint32_t duration, remaining;
} kr_glide_state_t;
static inline uint32_t kr_glide_time_ticks(uint32_t ms_x10, uint32_t sample_rate, uint32_t ctl)
{
    uint64_t d = (uint64_t)10000u * ctl;
    uint64_t n = (uint64_t)ms_x10 * sample_rate + d / 2u;
    uint32_t ticks = (uint32_t)(n / d);
    return ticks ? ticks : 1u;
}
static inline void kr_glide_begin(kr_glide_state_t *g, int32_t start, int32_t target, uint32_t ticks)
{
    int32_t delta = target - start;
    g->pos = start;
    g->error = 0;
    if (!delta || !ticks) {
        g->pos = target;
        g->step = g->remainder = 0;
        g->duration = g->remaining = 0;
        return;
    }
    g->duration = g->remaining = ticks;
    g->step = delta / (int32_t)ticks;
    g->remainder = delta % (int32_t)ticks;
}
static inline void kr_glide_tick(kr_glide_state_t *g, int32_t target)
{
    if (!g->remaining) return;
    g->pos += g->step;
    g->error += g->remainder;
    if (g->error >= (int32_t)g->duration) {
        ++g->pos;
        g->error -= (int32_t)g->duration;
    } else if (g->error <= -(int32_t)g->duration) {
        --g->pos;
        g->error += (int32_t)g->duration;
    }
    if (!--g->remaining) g->pos = target;
}
static inline uint32_t kr_glide_inc(uint32_t inc, uint32_t next, uint32_t frac_q16)
{
    if (next <= inc || !frac_q16) return inc;
    return inc + (uint32_t)(((uint64_t)(next - inc) * frac_q16) >> 16);
}
#endif
