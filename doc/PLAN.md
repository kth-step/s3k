# S3K - Plan

- [x] Implement the foundation of the scheduler.
- [x] Implement message passing so we can send basic messages with no capabilities.
- [x] Implement message passing so we can send time slices, allowing us to dynamically change the scheduling.
- [x] Implement the supervisor capability allowing process 0 to manage other process's capabilities.
- [x] Implement memory protection (memory slice, memory frame).
- [x] Implement exception handling.
- [x] Separate kernel and user binaries.  Implement a basic bootloader in process 0 for loading the user binaries.
- [ ] Design and build applications, benchmarks and tests.
- [ ] Start on formal verification.
