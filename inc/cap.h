/**
 * @file cap.h
 * @brief Functions for handling capabilities.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "consts.h"

#define CAP_RWX 0x7
#define CAP_RW	0x3
#define CAP_RX	0x5
#define CAP_R	0x1

enum capty {
	CAPTY_NONE,	///< No capability
	CAPTY_TIME,	///< Time Slice capability
	CAPTY_MEMORY,	///< Memory Slice capability
	CAPTY_PMP,	///< PMP Frame capability
	CAPTY_MONITOR,	///< Monitor capability
	CAPTY_CHANNEL,	///< IPC Channel capability
	CAPTY_SOCKET,	///< IPC Socket capability
};

struct time {
	uint64_t type : 4;
	uint64_t _padd : 4;
	uint64_t hartid : 8;
	uint64_t begin : 16;
	uint64_t free : 16;
	uint64_t end : 16;
};

struct memory {
	uint64_t type : 4;
	uint64_t lock : 1;
	uint64_t rwx : 3;
	uint64_t offset : 8;
	uint64_t begin : 16;
	uint64_t free : 16;
	uint64_t end : 16;
};

struct pmp {
	uint64_t type : 4;
	uint64_t addr : 52;
	uint64_t cfg : 8;
};

struct monitor {
	uint64_t type : 4;
	uint64_t _padd : 12;
	uint64_t begin : 16;
	uint64_t free : 16;
	uint64_t end : 16;
};

struct channel {
	uint64_t type : 4;
	uint64_t _padd : 12;
	uint64_t begin : 16;
	uint64_t free : 16;
	uint64_t end : 16;
};

struct socket {
	uint64_t type : 4;
	uint64_t _padd : 28;
	uint64_t channel : 16;
	uint64_t tag : 16;
};

/// Capability description
union cap {
	uint64_t type : 4;
	uint64_t raw;
	struct time time;
	struct memory memory;
	struct pmp pmp;
	struct monitor monitor;
	struct channel channel;
	struct socket socket;
};

_Static_assert(sizeof(union cap) == 8, "union cap size != 8 bytes");

// Constructors
union cap cap_time(uint64_t hartid, uint64_t begin, uint64_t end);
union cap cap_memory(uint64_t begin, uint64_t end, uint64_t offset, uint64_t rwx);
union cap cap_pmp(uint64_t cfg, uint64_t addr);
union cap cap_monitor(uint64_t begin, uint64_t end);
union cap cap_channel(uint64_t begin, uint64_t end);
union cap cap_socket(uint64_t channel, uint64_t tag);

// Derivation check
bool cap_time_derive(union cap parent, union cap child);
bool cap_memory_derive(union cap parent, union cap child);
bool cap_monitor_derive(union cap parent, union cap child);
bool cap_channel_derive(union cap parent, union cap child);
bool cap_socket_derive(union cap parent, union cap child);

// Check for parent (revocation)
bool cap_time_parent(union cap parent, union cap child);
bool cap_memory_parent(union cap parent, union cap child);
bool cap_monitor_parent(union cap parent, union cap child);
bool cap_channel_parent(union cap parent, union cap child);
bool cap_socket_parent(union cap parent, union cap child);

// PMP utils
uint64_t pmp_napot_addr(uint64_t begin, uint64_t end);
uint64_t pmp_napot_begin(uint64_t addr);
uint64_t pmp_napot_end(uint64_t addr);
