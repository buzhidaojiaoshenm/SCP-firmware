/*
 * Arm SCP/MCP Software
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ARCH_INTERRUPT_H
#define ARCH_INTERRUPT_H

/*!
 * \brief Enables global CPU interrupts. (stub)
 *
 */
inline static void arch_interrupt_global_enable(unsigned int not_used)
{
}

/*!
 * \brief Disables global CPU interrupts. (stub)
 *
 */
inline static unsigned int arch_interrupt_global_disable(void)
{
    return 0;
}

/*!
 * \brief Suspend execution of current CPU.
 *
 */
inline static void arch_suspend(void)
{
}

int arch_interrupt_init();

#endif /* ARCH_INTERRUPT_H */
