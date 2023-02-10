// Kernel configuration

// Start of payload
#define PAYLOAD	 0x80010000
// Number of harts
#define NHART	 4
// Number of user processes
#define NPROC	 16
// Number of capabilities per process.
#define NCAP	 64
// Number of IPC channels.
#define NCHANNEL 32
// Number of slices per period
#define NSLICE	 32
// Number of ticks per slice
#define NTICK	 1000
// Number of slack ticks per slice
#define NSLACK	 100
#define NDEBUG	 0
