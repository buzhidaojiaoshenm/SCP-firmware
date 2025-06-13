\ingroup GroupModules Modules
\defgroup GroupFMU Fault Management Unit

# Fault Management Unit

The Fault Management Unit is for Safety Diagnostics Monitoring. It collects
both internal faults and faults from upstream devices into a single pair of
critical (C) and non-critical (NC) outputs.

The only type of FMU currently implemented is the System FMU.

## Example topology

The toplogy below consists of 3 System FMUs connected in a tree. FMUs 1 and 2
are connected to upstream fault signals and their outputs are connected to the
root FMU.

        +---------+
    --->|         |-C---------+
    --->|  FMU 1  |           |
    --->|         |-NC-----+  |       +------------+
        +---------+        |  +----0->|            |       
                           +-------1->|            |-----> C
                                      |  Root FMU  |
                           +-------2->|            |-----> NC
        +---------+        |  +----3->|            |
    --->|         |-C------+  |       +------------+
    --->|  FMU 2  |           |
    --->|         |-NC--------+
        +---------+

## Configuration

The above topology is configured as follows. The first module element is always
the root FMU and the root FMUs IRQ numbers are defined in the base module
configuration.

```C
enum fmu_device {
    SCP_FMU_ROOT,
    SCP_FMU_1,
    SCP_FMU_2,
    SCP_FMU_COUNT,
};

static const struct fwk_element fmu_devices[SCP_FMU_COUNT + 1] = {
    [SCP_FMU_ROOT] = {
        .name = "fmu0",
        .data = &((struct mod_fmu_dev_config) {
            .base = SCP_FMU0_BASE,
            .parent = MOD_FMU_PARENT_NONE,
            .implementation = MOD_FMU_IMPL_SYSTEM,
        }),
    },
    [SCP_FMU_1] = {
        .name = "fmu1",
        .data = &((struct mod_fmu_dev_config) {
            .base = SCP_FMU1_BASE,
            .parent = SCP_FMU_ROOT,
            .parent_cr_index = 0,
            .parent_ncr_index = 1,
            .implementation = MOD_FMU_IMPL_SYSTEM,
        }),
    },
    [SCP_FMU_2] = {
        .name = "fmu2",
        .data = &((struct mod_fmu_dev_config) {
            .base = SCP_FMU2_BASE,
            .parent = SCP_FMU_ROOT,
            .parent_cr_index = 2,
            .parent_ncr_index = 3,
            .implementation = MOD_FMU_IMPL_SYSTEM,
        }),
    },
    [SCP_FMU_COUNT] = {0},
};

struct fwk_module_config config_fmu = {
    .data =
        &(struct mod_fmu_config){
            .irq_critical = SCP_FMU_CRITICAL,
            .irq_non_critical = SCP_FMU_NON_CRITICAL,
        },
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(fmu_devices),
};
```
