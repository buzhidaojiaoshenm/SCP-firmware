# AArch64 Architecture Support

SCP-firmware has extended support for the AArch64 architecture, including both
Armv8-R64 and Armv8-A machines. This includes support for switching from EL3 to
EL2 for platforms that boot in EL3 mode, such as Cortex-A53 cores on certain
products.

The AArch64 architecture support follows the structure and interfaces defined on
the [Architecture Support](architecture_support.md) page.

Both the Armv8-R64 PMSA and Armv8-A MMU are supported. An initial memory map is
installed in `arch_crt0.S`, based on the firmware layout, which should be
customized using the `armv8r_mpu` module configuration for PMSA or the MMU
configuration for Armv8-A.

Note that AArch64 support now includes multiple exception levels. For Armv8-R64,
the firmware runs exclusively at secure EL2, while for Armv8-A, the firmware can
transition from EL3 to EL2 as needed. The EL1/0 environment is not initialized
by default.

Newlib is required to use certain SCP-firmware features on AArch64 that require
early initialization, such as notifications.

Both GCC and Clang are supported, but not Arm Clang.

## Interrupt handling

A minimal interrupt controller is provided with the following limitations:

 * All interrupts have the same priority.
 * NMIs (non-maskable interrupts) are not supported.
 * The extended SPI and PPI ranges are not supported.

AArch64 products must provide a `fmw_gic.h` header which defines:

 * `GICD_BASE` - the base address of the GIC distributor
 * `GICR_BASE` - the base address of the GIC redstributor corresponding to the
   core on which SCP-firmware is running

## Usage

To use the AArch64 architecture in a product, including the following in
Firmware.cmake:

    set(SCP_ARCHITECTURE "aarch64")

The fvp-baser-aemv8r product can be used as a reference for implementing an
AArch64 product.
