/*
 * Renesas SCP/MCP Software
 * Copyright (c) 2020-2025, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

    .globl _vector_table
    .extern _entrypoint

.macro ventry label
.align	7
b \label
.endm


.section .vectors, "ax"
.balign 2048
_vector_table:

.set VBAR, _vector_table
    /* -----------------------------------------------------
     * Current EL with SP0 : 0x0 - 0x200 (at EL3)
     * -----------------------------------------------------
     */
.org VBAR
    ventry _entrypoint
.org (VBAR + 0x80)
    ventry err_exception
.org (VBAR + 0x100)
    ventry fiq_handler
.org (VBAR + 0x180)
    ventry err_exception

    /* -----------------------------------------------------
     * Current EL with SPx: 0x200 - 0x400 (at EL3)
     * -----------------------------------------------------
     */
.org (VBAR + 0x200)
    ventry err_exception
.org (VBAR + 0x280)
    ventry err_exception
.org (VBAR + 0x300)
    ventry fiq_handler
.org (VBAR + 0x380)
    ventry err_exception

    /* -----------------------------------------------------
     * Lower EL using AArch64 : 0x400 - 0x600 (at EL3)
     * -----------------------------------------------------
     */
.org (VBAR + 0x400)
    ventry err_exception
.org (VBAR + 0x480)
    ventry err_exception
.org (VBAR + 0x500)
    ventry err_exception
.org (VBAR + 0x580)
    ventry err_exception

    /* -----------------------------------------------------
     * Lower EL using AArch32 : 0x600 - 0x800 (at EL3)
     * -----------------------------------------------------
     */
.org (VBAR + 0x600)
    ventry err_exception
.org (VBAR + 0x680)
    ventry err_exception
.org (VBAR + 0x700)
    ventry err_exception
.org (VBAR + 0x780)
    ventry err_exception


err_exception:
    wfe
    b err_exception

fiq_handler:
    /* Save registers onto stack */
    str x30, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x0, x1, [sp, #-16]!

.extern irq_global
    bl irq_global

    /* Restore registers from stack */
    ldp x0, x1, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x28, x29, [sp], #16
    ldr x30, [sp], #16

    eret
