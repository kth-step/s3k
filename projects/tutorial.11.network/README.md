# Tutorial 11: Networking

## What this tutorial is about

This tutorial brings up a VirtIO network device and runs a tiny telnet-based integer service. The service listens on guest port `23`; QEMU forwards host port `5555` to it, so a telnet client on the host can send an integer and receive that integer plus one.

## Lecture Notes

This final tutorial reinforces the same architectural message as the filesystem example: networking is not assumed to be a built-in kernel facility. S3K provides isolation, capability-controlled device access, and execution control. A networked service is then constructed on top of those primitives in user space.

That is why the example is deliberately small. The interesting part is not the arithmetic. The interesting part is that:

- the process obtains explicit access to the VirtIO network device,
- the network path is brought up in the guest,
- QEMU provides host-to-guest port forwarding,
- a user-space service becomes reachable from outside the guest.

The tutorial makes the separation-kernel design visible: the kernel does not need to become a full network stack in order for isolated applications to communicate externally.

## What to look at in the code

Read [app1/main.c](/Users/guancio/Sources/STEP/sw/s3k.v2/projects/tutorial.11.network/app1/main.c). Focus on:

- UART and VirtIO capability setup,
- network initialization,
- the telnet request parsing and reply logic.

You should also inspect this tutorial's QEMU configuration in its Meson build, because the host port forwarding is part of the end-to-end setup.

## How to run it

Start the containerized toolchain, build the tutorial, and launch QEMU:

```bash
./scripts/docker.sh
cd projects/tutorial.11.network
meson setup builddir --cross-file=../../cross/rv64imac.ini
ninja -C builddir
ninja -C builddir qemu-run
```

Keep `ninja -C builddir qemu-run` running. In a second terminal on the host, connect to the forwarded port:

```bash
telnet 127.0.0.1 5555
```

Then send:

```text
45
```

## Expected result

The service should respond with:

```text
46
```

This confirms the full chain from host networking, through QEMU forwarding, into the guest network device, and up to a user-space service running on top of S3K.
