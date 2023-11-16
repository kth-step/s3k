#include "s3k/s3k.h"

typedef enum {
	// Basic Info & Registers
	S3K_SYS_GET_INFO,  // Retrieve system information
	S3K_SYS_REG_READ,  // Read from a register
	S3K_SYS_REG_WRITE, // Write to a register
	S3K_SYS_SYNC,	   // Synchronize memory and time.
	S3K_SYS_SLEEP,

	// Capability Management
	S3K_SYS_CAP_READ,   // Read the properties of a capability.
	S3K_SYS_CAP_MOVE,   // Move a capability to a different slot.
	S3K_SYS_CAP_DELETE, // Delete a capability from the system.
	S3K_SYS_CAP_REVOKE, // Deletes derived capabilities.
	S3K_SYS_CAP_DERIVE, // Creates a new capability.

	// PMP calls
	S3K_SYS_PMP_LOAD,
	S3K_SYS_PMP_UNLOAD,

	// Monitor calls
	S3K_SYS_MON_SUSPEND,
	S3K_SYS_MON_RESUME,
	S3K_SYS_MON_STATE_GET,
	S3K_SYS_MON_YIELD,
	S3K_SYS_MON_REG_READ,
	S3K_SYS_MON_REG_WRITE,
	S3K_SYS_MON_CAP_READ,
	S3K_SYS_MON_CAP_MOVE,
	S3K_SYS_MON_PMP_LOAD,
	S3K_SYS_MON_PMP_UNLOAD,

	// Socket calls
	S3K_SYS_SOCK_SEND,
	S3K_SYS_SOCK_RECV,
	S3K_SYS_SOCK_SENDRECV,
} s3k_syscall_t;

typedef union {
	struct {
		uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		uint64_t info;
	} get_info;

	struct {
		uint64_t reg;
	} reg_read;

	struct {
		uint64_t reg;
		uint64_t val;
	} reg_write;

	struct {
		uint64_t full;
	} sync;

	struct {
		uint64_t time;
	} sleep;

	struct {
		uint64_t idx;
	} cap_read;

	struct {
		uint64_t src_idx;
		uint64_t dst_idx;
	} cap_move;

	struct {
		uint64_t idx;
	} cap_delete;

	struct {
		uint64_t idx;
	} cap_revoke;

	struct {
		uint64_t src_idx;
		uint64_t dst_idx;
		uint64_t cap_raw;
	} cap_derive;

	struct {
		uint64_t idx;
		uint64_t slot;
	} pmp_load;

	struct {
		uint64_t idx;
	} pmp_unload;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
	} mon_state;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t reg;
	} mon_reg_read;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t reg;
		uint64_t val;
	} mon_reg_write;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
	} mon_cap_read;

	struct {
		uint64_t mon_idx;
		uint64_t src_pid;
		uint64_t src_idx;
		uint64_t dst_pid;
		uint64_t dst_idx;
	} mon_cap_move;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
		uint64_t slot;
	} mon_pmp_load;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
	} mon_pmp_unload;

	struct {
		uint64_t sock_idx;
		uint64_t cap_idx;
		uint64_t send_cap;
		uint64_t data[4];
	} sock;
} sys_args_t;

typedef struct {
	s3k_err_t err;
	uint64_t val;
} s3k_ret_t;

_Static_assert(sizeof(sys_args_t) == 64, "sys_args_t has the wrong size");

#define DO_ECALL(call, args, width)                                            \
	({                                                                     \
		register uint64_t t0 __asm__("t0") = call;                     \
		register uint64_t a0 __asm__("a0") = args.a0;                  \
		register uint64_t a1 __asm__("a1") = args.a1;                  \
		register uint64_t a2 __asm__("a2") = args.a2;                  \
		register uint64_t a3 __asm__("a3") = args.a3;                  \
		register uint64_t a4 __asm__("a4") = args.a4;                  \
		register uint64_t a5 __asm__("a5") = args.a5;                  \
		register uint64_t a6 __asm__("a6") = args.a6;                  \
		register uint64_t a7 __asm__("a7") = args.a7;                  \
		switch ((width + 7) / 8) {                                     \
		case 0:                                                        \
			__asm__ volatile("ecall" : "+r"(t0));                  \
			break;                                                 \
		case 1:                                                        \
			__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));        \
			break;                                                 \
		case 2:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1));                           \
			break;                                                 \
		case 3:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2));                  \
			break;                                                 \
		case 4:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2), "r"(a3));         \
			break;                                                 \
		case 5:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2), "r"(a3),          \
					   "r"(a4));                           \
			break;                                                 \
		case 6:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), \
					   "r"(a5));                           \
			break;                                                 \
		case 7:                                                        \
			__asm__ volatile("ecall"                               \
					 : "+r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), \
					   "r"(a5), "r"(a6));                  \
			break;                                                 \
		case 8:                                                        \
			__asm__ volatile("ecall"                               \
					 : "=r"(t0), "+r"(a0)                  \
					 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), \
					   "r"(a5), "r"(a6), "r"(a7));         \
			break;                                                 \
		}                                                              \
		(s3k_ret_t){.err = t0, .val = a0};                             \
	})

uint64_t s3k_get_pid(void)
{
	sys_args_t args = {.get_info = {0}};
	return DO_ECALL(S3K_SYS_GET_INFO, args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_time(void)
{
	sys_args_t args = {.get_info = {1}};
	return DO_ECALL(S3K_SYS_GET_INFO, args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_timeout(void)
{
	sys_args_t args = {.get_info = {2}};
	return DO_ECALL(S3K_SYS_GET_INFO, args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_wcet(bool reset)
{
	sys_args_t args = {.get_info = {reset ? 4 : 3}};
	return DO_ECALL(S3K_SYS_GET_INFO, args, sizeof(args.get_info)).val;
}

uint64_t s3k_reg_read(s3k_reg_t reg)
{
	sys_args_t args = {.reg_read = {reg}};
	return DO_ECALL(S3K_SYS_REG_READ, args, sizeof(args.reg_read)).val;
}

uint64_t s3k_reg_write(s3k_reg_t reg, uint64_t val)
{
	sys_args_t args = {
	    .reg_write = {reg, val}
	   };
	return DO_ECALL(S3K_SYS_REG_WRITE, args, sizeof(args.reg_write)).val;
}

void s3k_sync(void)
{
	sys_args_t args = {.sync = {true}};
	DO_ECALL(S3K_SYS_SYNC, args, sizeof(args.sync));
}

void s3k_sync_mem(void)
{
	sys_args_t args = {.sync = {false}};
	DO_ECALL(S3K_SYS_SYNC, args, sizeof(args.sync));
}

void s3k_sleep(uint64_t time)
{
	sys_args_t args = {.sleep = {time}};
	DO_ECALL(S3K_SYS_SLEEP, args, sizeof(args.sleep));
}

s3k_err_t s3k_cap_read(s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {.cap_read = {idx}};
	s3k_ret_t ret = DO_ECALL(S3K_SYS_CAP_READ, args, sizeof(args.cap_read));
	cap->raw = ret.val;
	return ret.err;
}

s3k_err_t s3k_cap_move(s3k_cidx_t src, s3k_cidx_t dst)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_move(src, dst);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_delete(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_delete(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_revoke(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_revoke(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t ncap)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_derive(src, dst, ncap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_load(s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_load(idx, slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_unload(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_unload(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_suspend(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_suspend(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_resume(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_resume(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_state_get(s3k_cidx_t mon_idx, s3k_pid_t pid,
			    s3k_state_t *state)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_state_get(mon_idx, pid, state);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_yield(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_yield(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			   uint64_t *val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_read(mon_idx, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_write(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			    uint64_t val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_write(mon_idx, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_cidx_t idx,
			   s3k_cap_t *cap)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_read(mon_idx, pid, idx, cap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			   s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			   s3k_cidx_t dst_idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_move(mon_idx, src_pid, src_idx, dst_pid,
					   dst_idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			   s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_load(mon_idx, pid, pmp_idx, pmp_slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
			     s3k_cidx_t pmp_idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_unload(mon_idx, pid, pmp_idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	s3k_err_t err;
	do {
		err = s3k_try_sock_send(sock_idx, msg);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_reply_t s3k_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_idx)
{
	s3k_reply_t reply;
	do {
		reply = s3k_try_sock_recv(sock_idx, cap_idx);
	} while (reply.err == S3K_ERR_PREEMPTED);
	return reply;
}

s3k_reply_t s3k_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	s3k_reply_t reply;
	do {
		reply = s3k_try_sock_sendrecv(sock_idx, msg);
	} while (reply.err == S3K_ERR_PREEMPTED);
	return reply;
}

s3k_err_t s3k_try_cap_move(s3k_cidx_t src, s3k_cidx_t dst)
{
	sys_args_t args = {
	    .cap_move = {src, dst}
	  };
	return DO_ECALL(S3K_SYS_CAP_MOVE, args, sizeof(args.cap_move)).err;
}

s3k_err_t s3k_try_cap_delete(s3k_cidx_t idx)
{
	const sys_args_t args = {.cap_delete = {idx}};
	return DO_ECALL(S3K_SYS_CAP_DELETE, args, sizeof(args.cap_delete)).err;
}

s3k_err_t s3k_try_cap_revoke(s3k_cidx_t idx)
{
	sys_args_t args = {.cap_revoke = {idx}};
	return DO_ECALL(S3K_SYS_CAP_REVOKE, args, sizeof(args.cap_revoke)).err;
}

s3k_err_t s3k_try_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t ncap)
{
	sys_args_t args = {
	    .cap_derive = {src, dst, ncap.raw}
	  };
	return DO_ECALL(S3K_SYS_CAP_DERIVE, args, sizeof(args.cap_derive)).err;
}

s3k_err_t s3k_try_pmp_load(s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	sys_args_t args = {
	    .pmp_load = {idx, slot}
	   };
	return DO_ECALL(S3K_SYS_PMP_LOAD, args, sizeof(args.pmp_load)).err;
}

s3k_err_t s3k_try_pmp_unload(s3k_cidx_t idx)
{
	sys_args_t args = {.pmp_unload = {idx}};
	return DO_ECALL(S3K_SYS_PMP_UNLOAD, args, sizeof(args.pmp_unload)).err;
}

s3k_err_t s3k_try_mon_suspend(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return DO_ECALL(S3K_SYS_MON_SUSPEND, args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_resume(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return DO_ECALL(S3K_SYS_MON_RESUME, args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_state_get(s3k_cidx_t mon, s3k_pid_t pid,
				s3k_state_t *state)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	s3k_ret_t ret
	    = DO_ECALL(S3K_SYS_MON_STATE_GET, args, sizeof(args.mon_state));
	*state = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_yield(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return DO_ECALL(S3K_SYS_MON_YIELD, args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_reg_read(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
			       uint64_t *val)
{
	sys_args_t args = {
	    .mon_reg_read = {mon, pid, reg}
	   };
	s3k_ret_t ret
	    = DO_ECALL(S3K_SYS_MON_REG_READ, args, sizeof(args.mon_reg_read));
	*val = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_reg_write(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
				uint64_t val)
{
	sys_args_t args = {
	    .mon_reg_write = {mon, pid, reg, val}
	 };
	s3k_ret_t ret
	    = DO_ECALL(S3K_SYS_MON_REG_WRITE, args, sizeof(args.mon_reg_write));
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {
	    .mon_cap_read = {mon_idx, pid, idx}
	       };
	s3k_ret_t ret
	    = DO_ECALL(S3K_SYS_MON_CAP_READ, args, sizeof(args.mon_cap_read));
	if (!ret.err)
		cap->raw = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			       s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			       s3k_cidx_t dst_idx)
{
	sys_args_t args = {
	    .mon_cap_move = {mon_idx, src_pid, src_idx, dst_pid, dst_idx}
	 };
	return DO_ECALL(S3K_SYS_MON_CAP_MOVE, args, sizeof(args.mon_cap_move))
	    .err;
}

s3k_err_t s3k_try_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	sys_args_t args = {
	    .mon_pmp_load = {mon_idx, pid, idx, slot}
	     };
	return DO_ECALL(S3K_SYS_MON_PMP_LOAD, args, sizeof(args.mon_pmp_load))
	    .err;
}

s3k_err_t s3k_try_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
				 s3k_cidx_t idx)
{
	sys_args_t args = {
	    .mon_pmp_unload = {mon_idx, pid, idx}
	 };
	return DO_ECALL(S3K_SYS_MON_PMP_UNLOAD, args,
			sizeof(args.mon_pmp_unload))
	    .err;
}

s3k_err_t s3k_try_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	return DO_ECALL(S3K_SYS_SOCK_SEND, args, sizeof(args.sock)).err;
}

s3k_reply_t s3k_try_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_idx)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx, .cap_idx = cap_idx}
	      };
	register uint64_t t0 __asm__("t0") = S3K_SYS_SOCK_SENDRECV;
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3),
			   "+r"(a4), "+r"(a5)
			 : "r"(a6), "r"(a7));
	s3k_reply_t reply;
	reply.err = t0;
	reply.tag = a0;
	reply.cap.raw = a1;
	reply.data[0] = a2;
	reply.data[1] = a3;
	reply.data[2] = a4;
	reply.data[3] = a5;
	return reply;
}

s3k_reply_t s3k_try_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	register uint64_t t0 __asm__("t0") = S3K_SYS_SOCK_SENDRECV;
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3),
			   "+r"(a4), "+r"(a5)
			 : "r"(a6), "r"(a7));
	s3k_reply_t reply;
	reply.err = t0;
	reply.tag = a0;
	reply.cap.raw = a1;
	reply.data[0] = a2;
	reply.data[1] = a3;
	reply.data[2] = a4;
	reply.data[3] = a5;
	return reply;
}
