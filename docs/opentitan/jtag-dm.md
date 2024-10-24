# JTAG and RISC-V Debug Module support for OpenTitan

RISC-V Debug Module is implemented as an alternative to the default QEMU GDB server.

It can be used when low-level features are required, such as prototyping JTAG manufacturing and
provisioning software development, or emulating JTAG Debug Authentication.

This initial implementation of the RISC-V Debug Module and Pulp DM supports the following features:

- Ibex core debugging (RV32 only); not tested with multiple harts
- system bus memory access: read and write guest memory
- guest code debugging
- single stepping
- HW breakpoints
- HW watchpoints

## Communicating with RISC-V DM through a JTAG connection

In QEMU machines for OpenTitan, any Debug Module can register on the Debug Transport Module, which
dispatches requests to Debug Module depending on the received DMI address.

See also [JTAG mailbox](jtagmbx.md) and [Life Controller](lc_ctrl_dmi.md) for other Debug Modules.

```
+--------------------------+
| Host (OpenOCD or Python) |
+--------------------------+
      |
      | TCP connection ("bitbang mode")
      |
+-----|-----------------------------------------------------------------------------------+
|     |                                                                             QEMU  |
|     v                                              +---------+                          |
| +-------------+     +-----+     +-----------+      | PULP-DM |         +------+         |
| | JTAG server |---->| DTM |---->| RISC-V DM |<---->+ ------- +<=======>| Hart |         |
| +-------------+     +-----+     +-----------+      | ROM/RAM |    ||   +------+         |
|                                      ||            +---------+    ||                    |
|                                      ||                           ||   +--------------+ |
|                                       ================================>| I/O, RAM, ...| |
|                                                                        +--------------+ |
+-----------------------------------------------------------------------------------------+
```

### Remote bitbang protocol

The JTAG server supports the Remote Bitbang Protocol which is compatible with other tools such as
Spike and [OpenOCD](https://github.com/riscv/riscv-openocd).

QEMU should be started with an option such as:
`-chardev socket,id=taprbb,host=localhost,port=3335,server=on,wait=off` so that the JTAG server is
instantiated and listens for incoming connection on TCP port 3335.

#### Remote termination feature

The remote Remote Bitbang Protocol supports a "_quit_" feature which enables the remote client to
trigger the end of execution of the QEMU VM. This feature can be useful to run automated tests for
example.

It is nevertheless possible to disable this feature and ignore the remote exit request so that
multiple JTAG sessions can be run without terminating the QEMU VM.

To disable this feature, use the `quit` option: `-global tap-ctrl-rbb,quit=off`.

#### Implementation

* JTAG server is implemented with `jtag/jtag_bitbang.c`
* DTM is implemented with `hw/riscv/dtm.c`
* RISC-V DM is implemented with `hw/riscv/dm.c`
* Pulp-DM is implemented with `hw/misc/pulp_rv_dm.c`

See also other supported DM modules on OpenTitan:

* [JTAG mailbox DMI](jtagmbx.md)
* [LifeCycle DMI](lc_ctrl_dmi.md)

## Communicating with JTAG server using OpenOCD

OpenOCD running on a host can connect to the embedded JTAG server, using a configuration script such
as

```tcl
adapter driver remote_bitbang
remote_bitbang host localhost
remote_bitbang port 3335

transport select jtag

set _CHIPNAME riscv
# part 1, ver 0, lowRISC; actual TAP id depends on the QEMU machine
set _CPUTAPID 0x00011cdf

jtag newtap $_CHIPNAME cpu -irlen 5 -expected-id $_CPUTAPID

# Hart debug base also depends on the QEMU machine
set DM_BASE 0x00

target create $_CHIPNAME riscv -chain-position $_TARGETNAME rtos hwthread -dbgbase $DM_BASE
```

## Connecting GDB to OpenOCD

In another terminal, `riscv{32,64}-unknown-elf-gdb` can be used to connect to OpenOCD:

```
# 64-bit version support 32-bit targets
riscv64-unknown-elf-gdb
```

A basic `$HOME/.gdbinit` as the following should connect GDB to the running OpenOCD instance:
```
target remote :3333
```

## Communicating with JTAG server using Python

`scripts/opentitan/ot` directory contains Python modules that provide several APIs to test the
JTAG/DTM/DM stack.

A demo application is available from [`scripts/opentitan/dtm.py`](dtm.md) that can report basic
information about this stack and demonstrate how to use the Debug Module to access the Ibex core.

The [`scripts/opentitan/otpdm.py`](otpdm.md) also use the same stack to access the cells of the OTP
controller.

### Troubleshooting [#troubleshooting]

A common issue with initial JTAG configuration is to use the wrong Instruction Register length.
The IR length depends on the actual implementation of the TAP controller and therefore may be
different across HW implementations, here across OpenTitan instantiations.

Unfortunately, there is no reliable way to automatically detect the IR length, this needs to be
a setting that should be provided to the JTAG tool. Scripts in `scripts/opentitan` use the `-l` /
`--ir-length` option to specify the IR length. The default value may be obtained with the `-h`
option, which is IR length = 5 bits for default EarlGrey and Darjeeling machines.

Whenever the wrong IR length is specified, or the default IR length is used with a HW/VM machine
that uses a non-default length, the first instruction that is stored in the TAP instruction register
is misinterpreted, which may cause different errors with the same root cause: a wrong IR length
setting.

It is recommended to query the DMI address bits with for example [`scripts/opentitan/dtm.py`](dtm.md)
which is a basic command that needs a valid IR length setting to complete. Use `dtm.py -I` to query
some low-level information from the remote peer.

It should report something like:
````
IDCODE:    0x11cdf
DTM:       v0.13, 12 bits
````

* If the DTM bit count is stuck to 0, the IR length is likely wrong,
* If an error message such as `Invalid reported address bits` is returned, the IR length is likely
  wrong.

Another common issue is to use a machine with multiple Debug Modules but fail to specify its base
address. The default DMI base address is always `0x0`. Use `-b` / `--base` option to select the
proper Debug Module base address.
