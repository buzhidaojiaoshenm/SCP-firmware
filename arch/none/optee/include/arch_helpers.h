/*
 * Arm SCP/MCP Software
 * Copyright (c) 2022-2025, Linaro Limited and Contributors. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ARCH_HELPERS_H
#define ARCH_HELPERS_H

/*!
 * \brief Enables global CPU interrupts. (stub)
 *
 */
inline static void arch_interrupts_enable(unsigned int not_used)
{
}

/*!
 * \brief Disables global CPU interrupts. (stub)
 *
 */
inline static unsigned int arch_interrupts_disable(void)
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

/*!
 * \brief Initialize the architecture interrupt management component.
 *
 * \param[out] Pointer to the interrupt driver.
 *
 * \retval ::FWK_E_PANIC The operation failed.
 * \retval ::FWK_SUCCESS The operation succeeded.
 *
 * \return Status code representing the result of the operation.
 */
int arch_interrupt_init();

#endif /* ARCH_HELPERS_H */
