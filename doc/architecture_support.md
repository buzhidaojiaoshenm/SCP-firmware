# Architecture Support

## Overview

Architectural configuration comes under the `arch` directory. This area provides
not only instructions for building the architecture library, but for configuring
any architecture-dependent behaviour required by the framework.

## Structure

The structure of this directory is like so:

    .
    |-- <vendor>
    |   |-- <architecture>
    |   |   |-- arch.mk
    |   |   |-- include
    |   |   |   `-- arch_*.h
    |   |   `-- src
    |   |       |-- arch.ld.S
    |   |       |-- arch.scatter.S
    |   |       |-- arch_*.S
    |   |       |-- arch_*.c
    |   |-- include
    |   |   `-- arch_*.h
    |   |-- src
    |   |   `-- arch_*.c
    |   `-- vendor.mk

### Vendors and architectures

The _vendor_ refers to the umbrella architecture group. For Arm-based
architectures this is "arm".

The _architecture_ refers to the instruction set architecture of the target
platform.

### Build system integration

`vendor.mk` is included by the build system when building the architecture
library, and also when building the firmware. This allows you to configure
sources to compile in the architecture library, and library dependencies
(if any) when linking the final firmware binary.

`arch.ld.S` and `arch.scatter.S` also provide the linker scripts for the final
firmware. They are automatically used by the build system if not targeting the
*none* vendor (which one is chosen is based on the linker in use).

## Platform Initialization Hook

### Purpose

The platform initialization hook (`int platform_init_hook(void *params)`) is an
optional entry point for performing platform-specific initialization *before*
the framework and module initialization phase begins. It is defined as a weak
symbol that can be overridden by platform for setting up critical low-level
hardware before fwk_arch_init() is called.

### Signature

```c
int platform_init_hook(void *params);
```

- The default weak implementation simply returns FWK_SUCCESS.
- Platforms may override it to perform setup tasks and return an appropriate
status.
- On failure (return value ≠ FWK_SUCCESS), the system will panic().

### Usage Guidelines and Limitations

The `int platform_init_hook(void *params)` function executes at a very early
stage in the boot process. As such, it comes with **strict limitations**:

#### Memory

- **Do not** access memory regions that hasn't yet been enabled or configured.

#### API Usage

- **Do not** call module APIs.
- Use the framework APIs responsibly, some of the framework is not initialized
yet. For example fwk_put_event() shouldn’t be used as no event_queue has been
initialized yet.

#### Ordering

- This hook runs *before* any module or element initialization.
- This is the **first opportunity** for the platform to perform low-level setup.

#### Failure Modes

- If the hook returns any code other than **FWK_SUCCESS**, the system will call
**panic()**.
- Platforms should handle recoverable errors locally and only return non-success
for fatal conditions.

### Example – Valid Usage

```c
int platform_init_hook(void *params)
{
    /* Example: Enable SRAM region required for early stack usage */
    fwk_mmio_setbits_32(SOC_SRAM_CTRL, SOC_SRAM_CTRL_EN_MASK);

    return FWK_SUCCESS;
}
```

This example is valid because:
- It uses physical MMIO addresses to known-safe hardware regions.

### Anti-Pattern – Invalid Usage

```c
int platform_init_hook(void *params)
{
    int status;

    /* Invalid: Using framework put event before event_queue is initialized */
    status = fwk_put_event(&event);
    if (status != FWK_SUCCESS) {
        return status;
    }

    /* Invalid: Calling module API to configure GPIOs */
    return gpio_api->set_pin(3, true);
}
```

This is invalid because:
- Scheduling event requires the event queue to be initialized, which has not
happened yet.
- Calling module APIs assumes those modules have already been registered and
initialized, which is not true at this stage.
