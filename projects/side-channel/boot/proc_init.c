#include "s3k.h"
#include <stdio.h>

extern char __uart_base[]; // UART base address, provided by the linker or platform

/**
 * Helper function to derive and configure a memory capability for a process.
 * - mon_idx: Monitor index controlling the process.
 * - idx: Initial memory capability index.
 * - slot: Capability slot to use.
 * - cfree: Capability fuel (number of uses).
 * - perm: Permissions (e.g., RWX).
 * - base: Base address of the memory region.
 * - size: Size of the memory region.
 * Returns true on success, false on failure.
 */
static bool mem_init(s3k_word_t mon_idx, s3k_word_t idx, s3k_word_t slot, s3k_word_t cfree, s3k_word_t perm, s3k_word_t base, s3k_word_t size)
{
    // Derive a memory capability for the region
    idx = s3k_mon_mem_derive(mon_idx, idx, cfree, perm, base, size);
    if (idx < 0) {
        printf("Failed to derive memory capability %lx\n", base);
        return false;
    }

    // Set PMP (Physical Memory Protection) for the derived memory region
    s3k_word_t addr = s3k_pmp_napot_encode(base, size);
    int err = s3k_mon_mem_pmp_set(mon_idx, idx, slot, perm, addr);
    if (err < 0) {
        printf("Failed to set PMP for derived memory %lx, err=%d\n", base, err);
        return false;
    }

    return true;
}

/**
 * Initialize the process controlled by mon_idx with memory regions for RAM and UART.
 * - mon_idx: Monitor index for the process.
 * - ram_idx: Initial memory capability index for RAM.
 * - ram_base: Base address of the RAM region.
 * - ram_size: Size of the RAM region.
 */
void proc_init(s3k_word_t mon_idx, int ram_idx, s3k_word_t ram_base, s3k_word_t ram_size)
{
    // Indices for memory regions
    int uart_idx = 16;  // UART memory capability index

    // RAM configuration parameters
    s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // RAM: Read/Write/Execute permissions
    s3k_word_t ram_fuel = 1;                // RAM: Capability fuel
    s3k_word_t ram_slot = 1;                // RAM: Capability slot

    // Initialize RAM memory region for the process
    if (!mem_init(mon_idx, ram_idx, ram_slot, ram_fuel, ram_perm, ram_base, ram_size)) {
        printf("Failed to initialize RAM memory\n");
        return;
    }

    // UART configuration parameters
    s3k_word_t uart_base = (s3k_word_t)__uart_base; // UART base address
    s3k_word_t uart_size = 0x20;                    // UART region size (32 bytes)
    s3k_word_t uart_perm = S3K_MEM_PERM_RW;         // UART: Read/Write permissions
    s3k_word_t uart_fuel = 1;                       // UART: Capability fuel
    s3k_word_t uart_slot = 2;                       // UART: Capability slot

    // Initialize UART memory region for the process
    if (!mem_init(mon_idx, uart_idx, uart_slot, uart_fuel, uart_perm, uart_base, uart_size)) {
        printf("Failed to initialize UART memory\n");
        return;
    }

    // Set the program counter (PC) to the start of the RAM region
    if (s3k_mon_reg_set(mon_idx, S3K_REG_PC, ram_base) != 0) {
        printf("Failed to set program counter to RAM start\n");
        return;
    }

}
