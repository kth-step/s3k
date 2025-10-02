# S3K Artifact Evaluation

**Author: Henrik Karlsson**

Download the following files using a *private/incognito* browser window. **Do not log in to Google, as this may reveal your identity to the file owner**:

- [Paper draft](https://drive.google.com/file/d/1R_kOyQPmZw65yfOPTfEBNs2zbSc449nD/view?usp=drive_link): Latest draft of the paper.
- [S3K Eval](https://drive.google.com/file/d/1WK-CqNOLTN80gDTkJz5Z4Xbn0TDRgnj7/view?usp=sharing): VirtualBox Appliance for the evaluation. Install VirtualBox and import this VM. Password for the VM is `time2eval`.

## Connect to the FPGA

The VM image contains a wireguard configuration file, allowing you to connect to a Raspberry Pi server connected to a Genesys2 FPGA.
This FPGA runs a dual-core Cheshire SoC which you can use in the evaluation.

**Note: The Cheshire SoC on the FPGA is different from the one used in the original experiments. The previous version suffered from bugs that caused frequent crashes and made evaluation difficult. The current image uses an updated, more stable Cheshire with an improved cache system. As a result, evaluation results differ from those in the original paper, but remain within the same range.**

To connect to the FPGA, you must first ensure that Wireguard is running:
In a terminal, type the following:
```bash
# Start wireguard if it has not already started.
# Password for sudo is time2eval
sudo wg-quick up wg0

# Test connecting to the FPGA.
ssh 10.100.0.1
```

## S3K Benchmark directory

The archive `~/s3k-bench.zip` contains the kernel source code, evaluation tests, and all scripts needed for the evaluation.

To extract it, run the following in your terminal:

```bash
unzip s3k-bench.zip
```

This will create the `~/s3k-bench` directory with all necessary files.
Key contents of the `~/s3k-bench` folder:

- `data/`: Evaluation data.
- `projects/hello/`: Simple "hello, world" program.
- `projects/ipc/`: IPC performance tests.
- `projects/jitter/`: Scheduling dispatch jitter tests.
- `projects/scheduling/`: User-mode scheduling cost tests.
- `projects/side-channel/`: Side-channel tests.
- `s3k/`: Kernel source code.
- `ipc.py`: Generates IPC evaluation tables.
- `jitter.py`: Generates scheduling dispatch jitter tables.
- `scheduling.py`: Generates user-mode scheduling cost graphs.
- `side-channel.py`: Generates side-channel evaluation tables.
- `container.sh`: Starts a container with the RISC-V toolchain.
- `openocd.sh`: Connects to the FPGA server and starts OpenOCD.
- `setup.sh`: Generates evaluation projects under `build/`.
- `uart.sh`: Connects to the FPGA server and listens to the FPGA UART.

## Evaluation

We have generated data for all evaluations and placed them in the `/data` folder.

All evaluation data is pre-generated and available in the `/data` folder.

To regenerate data for a specific evaluation `<eval>`, follow these steps:

1. Enable WireGuard:
	```bash
	sudo wg-quick up wg0
	```
2. Start OpenOCD in a terminal:
	```bash
	./openocd.sh
	```
3. Start UART in another terminal:
	```bash
	./uart.sh
	```
4. In a new terminal, launch the RISC-V toolchain container:
	```bash
	./container.sh
	```
	Then, inside the container:
	```bash
	./setup.sh # Only run once time to setup all projects
	ninja -C build/<eval> gdb-run
	```

The evaluation results for `<eval>` will be printed in the UART terminal.
Note: You may need to run `ninja -C build/<eval> gdb-run` twice to get results, as the Cheshire SoC can sometimes start in the wrong privilege mode (machine/user) on the first run.

You can print the `uart.sh` results directly to a `csv` file as follows:

```bash
./uart.sh > data/<eval>.csv
# Or to print the results to stdout and the file simultanously
./uart.sh | tee data/<eval>.csv
```

Once the test is complete, kill `./uart.sh` using `<C-c>`, and clean up the `csv` file if necessary.

### IPC Evaluation (Section VII-D)

The code for the IPC Evaluation is in `projects/ipc`.
In the IPC evaluation, we have two processes, a server (`app2`), and a client (`app1`), communicating with yielding synchrous IPC.
There are no other processes interfering in the system.
The client performs `s3k_ipc_call`, sending messages to the server whom perform `s3k_ipc_replyrecv`.

We measure two metrics, `s3k_ipc_call` (Call) time, and `s3k_ipc_replyrecv` (ReplyRecv) time. Together, these form round-trip-time (RTT).
If you execute `./ipc.py`, it should print the following two table:

```
Scenario (No SMP)       Call    ReplyRecv       RTT
Data Only (hot)         473     432             905
With capability (hot)   519     503             1022
Data Only (cold)        1817    1868            3685
With capability (cold)  2168    2165            4333

Scenario (SMP)          Call    ReplyRecv       RTT
Data Only (hot)         576     553             1129
With capability (hot)   635     613             1248
Data Only (cold)        1926    1952            3878
With capability (cold)  2294    2275            4569
```

The upper table is with no SMP enabled (single-core mode), and the second table is with SMP enabled (multicore, but no inter-core interference).
The hot cases are IPC cost when the L1 cache is hot.
In the cold cases, we flush the L1 cache before executing the system call.

The following tests generate data for the upper table:
```bash
ninja -C build/ipc-1a gdb-run
ninja -C build/ipc-1b gdb-run
ninja -C build/ipc-1c gdb-run
ninja -C build/ipc-1d gdb-run
```
And the following the lower table:
```bash
ninja -C build/ipc-2a gdb-run
ninja -C build/ipc-2b gdb-run
ninja -C build/ipc-2c gdb-run
ninja -C build/ipc-2d gdb-run
```

### Scheduling Dispatch Jitter (Section VII-B)

The evaluation code is under `projects/jitter`.
An important part of a real-time kernel is time predictability and determinism. 
This evaluation checks how much jitter the user space application (`measurer`) experiences on dispatch.
This is done by measuring the jitter in the periodicity of its scheduling.

E.g., the app dispatches at time 100, then time 201, then 299. We measure the intervals between these and check how much they deviate from the expected value (100).

If you execute `./ipc.py`, it should print the following table:

```
Scenario        Dispatch        Memory  Max Jitter      Mean Jitter
a) 1core        solo            spm     0               0
b) 1core        solo            dram    91              40
c) 2core        solo            spm     0               0
d) 2core        solo            dram    115             44
e) 2core        duo             spm     0               0
f) 2core        duo             dram    102             41
```

The table depicts dispatch jitter in different scenarios.
The `solo`, means that the measurer runs alone, `duo` means that it is scheduled simultanously with another process (`dummy`).
The `spm`, means that the measurer allocated to the scratchpad memory, in this case it experience no jitter.
The `dram`, means that the measurer is allocated to DRAM memory, and thus uses L2 cache, in this case it is affected by L2 cache misses.

There are two additional cases in the paper (g,h), these were left out.

The following commands generate the data for the table:

```sh
ninja -C build/jitter-a gdb-run   # single core, app in SPM
ninja -C build/jitter-b gdb-run   # single core, app in DRAM
ninja -C build/jitter-c gdb-run   # 2core, app in SPM
ninja -C build/jitter-d gdb-run   # 2core, app in DRAM
ninja -C build/jitter-e gdb-run   # 2core, app in SPM + DRAM
ninja -C build/jitter-f gdb-run   # 2core, app in DRAM + DRAM
```

### User-mode Scheduling Cost (Section VII-E)

The evaluation code is under `projects/scheduling`.

When you are designing a hard real-time kernel, it is good if you know the expected worst-case execution time of anticipated operations.
In this evaluation, we test how quickly we can revoke and create a new scheduling for S3K.

Running `./scheduling.py` will generate a graph as a PDF file in `data/scheduling.pdf`.

The following commands generate the data for the graph:
```
ninja -C build/scheduling-a gdb-run # No temporal fence
ninja -C build/scheduling-b gdb-run # Temporal fence
ninja -C build/scheduling-c gdb-run # No temporal fence (SMP)
ninja -C build/scheduling-d gdb-run # Temporal fence (SMP)
```

### Side-Channel Evaluation (Section VII-C)

The evaluation code is under `projects/ipc`.

In this test, we have two partitioned processes `trojan` and `spy`, that secretly try to communicate.
The `trojan` is allocated to the DRAM memory, so it actively uses the L2 cache, meanwhile, the spy is either allocated to the scratchpad (case a,c,e), or the DRAM (case b,d,f).
The `trojan` attempts to send bits of information to the `spy` using L2 cache-based timing side-channels.

If you execute `./side-channel.py`, it should print the following table:
```
Scenro  Dsptch  Memory  Jitter  Syscall Cache
(a) 1c  solo    spm     0       0       0
(b) 1c  solo    dram    1000    921     1000
(c) 2c  solo    spm     0       0       0
(d) 2c  solo    dram    1000    837     1000
(e) 2c  duo     spm     0       0       0
(f) 2c  duo     dram    1000    842     1000
```
These results are expected: when the `spy` does not use the L2 cache, and core-local side-channels (L1 cache, branch predictors, etc.) are reset by the temporal fence, the `spy` can only obtain information when it is allocated to the L2 cache.

The following commands generate the data for the table:
```
ninja -C build/side-channel-a gdb-run
ninja -C build/side-channel-b gdb-run
ninja -C build/side-channel-c gdb-run
ninja -C build/side-channel-d gdb-run
ninja -C build/side-channel-e gdb-run
ninja -C build/side-channel-f gdb-run
```
