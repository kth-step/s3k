/**
 * @file mockup.c
 * @brief Emulates hardware for unit-testing purposes.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include "csr.h"
#include "timer.h"
#include "wfi.h"

#include <stddef.h>
#include <sys/time.h>

struct proc *current;
static uint64_t _mhartid;
static uint64_t _time;
static uint64_t _timeout[8];
static uint64_t _pmpcfg0;
static uint64_t _pmpaddr[8];

void current_set(struct proc *proc)
{
	current = proc;
}

struct proc *current_get(void)
{
	return current;
}

void wfi(void)
{
	while (!csrr_mip())
		;
}

uint64_t time_get(void)
{
	_time += 10;
	return _time;
}

void time_set(uint64_t time)
{
	_time = time;
}

uint64_t timeout_get(uint64_t i)
{
	return _timeout[i];
}

void timeout_set(uint64_t i, uint64_t val)
{
	_timeout[i] = val;
}

uint64_t csrr_mhartid(void)
{
	return _mhartid;
}

uint64_t csrr_mip(void)
{
	if (time_get() < _timeout[_mhartid])
		return 0;
	return (1 << 7);
}

uint64_t csrr_pmpcfg0(void)
{
	return _pmpcfg0;
}

void csrw_pmpcfg0(uint64_t val)
{
	_pmpcfg0 = val;
}

uint64_t csrr_pmpaddr0(void)
{
	return _pmpaddr[0];
}

uint64_t csrr_pmpaddr1(void)
{
	return _pmpaddr[1];
}

uint64_t csrr_pmpaddr2(void)
{
	return _pmpaddr[2];
}

uint64_t csrr_pmpaddr3(void)
{
	return _pmpaddr[3];
}

uint64_t csrr_pmpaddr4(void)
{
	return _pmpaddr[4];
}

uint64_t csrr_pmpaddr5(void)
{
	return _pmpaddr[5];
}

uint64_t csrr_pmpaddr6(void)
{
	return _pmpaddr[6];
}

uint64_t csrr_pmpaddr7(void)
{
	return _pmpaddr[7];
}

void csrw_pmpaddr0(uint64_t val)
{
	_pmpaddr[0] = val;
}

void csrw_pmpaddr1(uint64_t val)
{
	_pmpaddr[1] = val;
}

void csrw_pmpaddr2(uint64_t val)
{
	_pmpaddr[2] = val;
}

void csrw_pmpaddr3(uint64_t val)
{
	_pmpaddr[3] = val;
}

void csrw_pmpaddr4(uint64_t val)
{
	_pmpaddr[4] = val;
}

void csrw_pmpaddr5(uint64_t val)
{
	_pmpaddr[5] = val;
}

void csrw_pmpaddr6(uint64_t val)
{
	_pmpaddr[6] = val;
}

void csrw_pmpaddr7(uint64_t val)
{
	_pmpaddr[7] = val;
}
