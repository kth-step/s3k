#include "s3k/s3k.h"

typedef enum {
	// Basic Info & Registers
	S3K_SYS_GET_INFO,  // Retrieve system information
	S3K_SYS_REG_READ,  // Read from a register
	S3K_SYS_REG_WRITE, // Write to a register
	S3K_SYS_SYNC,	   // Synchronize memory and time.

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
	s3k_syscall_t call;

	struct {
		uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		s3k_syscall_t call;
		int info;
	} get_info;

	struct {
		s3k_syscall_t call;
		s3k_reg_t reg;
		uint64_t val;
	} reg;

	struct {
		s3k_syscall_t call;
		bool full;
	} sync;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t idx;
		s3k_cidx_t dst_idx;
		s3k_cap_t cap;
	} cap;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t pmp_idx;
		s3k_pmp_slot_t pmp_slot;
	} pmp;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
	} mon_state;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
		s3k_reg_t reg;
		uint64_t val;
	} mon_reg;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t mon_idx;
		s3k_pid_t src_pid;
		s3k_cidx_t src_idx;
		s3k_pid_t dst_pid;
		s3k_cidx_t dst_idx;
	} mon_cap;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
		s3k_cidx_t pmp_idx;
		s3k_pmp_slot_t pmp_slot;
	} mon_pmp;

	struct {
		s3k_syscall_t call;
		s3k_cidx_t sock_idx;
		s3k_cidx_t cap_idx;
		bool send_cap;
		uint64_t data[4];
	} sock;
} sys_args_t;

typedef struct {
	s3k_err_t err;
	uint64_t val;
} s3k_ret_t;

_Static_assert(sizeof(sys_args_t) == 64, "sys_args_t has the wrong size");

static inline s3k_ret_t do_ecall(sys_args_t args, int width)
{
	register uint64_t t0 __asm__("t0");
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	switch ((width + 7) / 8) {
	case 0:
		__asm__ volatile("ecall" : "=r"(t0));
		break;
	case 1:
		__asm__ volatile("ecall" : "=r"(t0), "+r"(a0));
		break;
	case 2:
		__asm__ volatile("ecall" : "=r"(t0), "+r"(a0) : "r"(a1));
		break;
	case 3:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2));
		break;
	case 4:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2), "r"(a3));
		break;
	case 5:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
		break;
	case 6:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
		break;
	case 7:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
				   "r"(a6));
		break;
	case 8:
		__asm__ volatile("ecall"
				 : "=r"(t0), "+r"(a0)
				 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
				   "r"(a6), "r"(a7));
		break;
	}
	return (s3k_ret_t){.err = t0, .val = a0};
}

uint64_t s3k_get_pid(void)
{
	sys_args_t args = {
	    .get_info = {S3K_SYS_GET_INFO, 0}
	     };
	return do_ecall(args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_time(void)
{
	sys_args_t args = {
	    .get_info = {S3K_SYS_GET_INFO, 1}
	     };
	return do_ecall(args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_timeout(void)
{
	sys_args_t args = {
	    .get_info = {S3K_SYS_GET_INFO, 2}
	     };
	return do_ecall(args, sizeof(args.get_info)).val;
}

uint64_t s3k_get_wcet(bool reset)
{
	sys_args_t args = {
	    .get_info = {S3K_SYS_GET_INFO, reset ? 4 : 3}
	 };
	return do_ecall(args, sizeof(args.get_info)).val;
}

uint64_t s3k_reg_read(s3k_reg_t reg)
{
	sys_args_t args = {
	    .reg = {S3K_SYS_REG_READ, reg}
	  };
	return do_ecall(args, sizeof(args.reg)).val;
}

uint64_t s3k_reg_write(s3k_reg_t reg, uint64_t val)
{
	sys_args_t args = {
	    .reg = {S3K_SYS_REG_WRITE, reg, val}
	};
	return do_ecall(args, sizeof(args.reg)).val;
}

void s3k_sync(void)
{
	sys_args_t args = {
	    .sync = {S3K_SYS_SYNC, true}
	};
	do_ecall(args, sizeof(args.sync));
}

void s3k_sync_mem(void)
{
	sys_args_t args = {
	    .sync = {S3K_SYS_SYNC, false}
	 };
	do_ecall(args, sizeof(args.sync));
}

s3k_err_t s3k_cap_read(s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {
	    .cap = {S3K_SYS_CAP_READ, idx}
	  };
	s3k_ret_t ret = do_ecall(args, sizeof(args.cap));
	if (!ret.err)
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
	    .cap = {S3K_SYS_CAP_MOVE, src, dst}
	       };
	return do_ecall(args, sizeof(args.cap)).err;
}

s3k_err_t s3k_try_cap_delete(s3k_cidx_t idx)
{
	sys_args_t args = {
	    .cap = {S3K_SYS_CAP_DELETE, idx}
	    };
	return do_ecall(args, sizeof(args.cap)).err;
}

s3k_err_t s3k_try_cap_revoke(s3k_cidx_t idx)
{
	sys_args_t args = {
	    .cap = {S3K_SYS_CAP_REVOKE, idx}
	    };
	return do_ecall(args, sizeof(args.cap)).err;
}

s3k_err_t s3k_try_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t ncap)
{
	sys_args_t args = {
	    .cap = {S3K_SYS_CAP_DERIVE, src, dst, ncap}
	       };
	return do_ecall(args, sizeof(args.cap)).err;
}

s3k_err_t s3k_try_pmp_load(s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	sys_args_t args = {
	    .pmp = {S3K_SYS_PMP_LOAD, idx, slot}
	};
	return do_ecall(args, sizeof(args.pmp)).err;
}

s3k_err_t s3k_try_pmp_unload(s3k_cidx_t idx)
{
	sys_args_t args = {
	    .pmp = {S3K_SYS_PMP_UNLOAD, idx}
	    };
	return do_ecall(args, sizeof(args.pmp)).err;
}

s3k_err_t s3k_try_mon_suspend(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {S3K_SYS_MON_SUSPEND, mon, pid}
	};
	return do_ecall(args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_resume(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {S3K_SYS_MON_RESUME, mon, pid}
	       };
	return do_ecall(args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_state_get(s3k_cidx_t mon, s3k_pid_t pid,
				s3k_state_t *state)
{
	sys_args_t args = {
	    .mon_state = {S3K_SYS_MON_STATE_GET, mon, pid}
	  };
	s3k_ret_t ret = do_ecall(args, sizeof(args.mon_state));
	*state = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_yield(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {S3K_SYS_MON_YIELD, mon, pid}
	      };
	return do_ecall(args, sizeof(args.mon_state)).err;
}

s3k_err_t s3k_try_mon_reg_read(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
			       uint64_t *val)
{
	sys_args_t args = {
	    .mon_reg = {S3K_SYS_MON_REG_READ, mon, pid, reg}
	    };
	s3k_ret_t ret = do_ecall(args, sizeof(args.mon_reg));
	*val = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_reg_write(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
				uint64_t val)
{
	sys_args_t args = {
	    .mon_reg = {S3K_SYS_MON_REG_WRITE, mon, pid, reg, val}
	  };
	s3k_ret_t ret = do_ecall(args, sizeof(args.mon_reg));
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {
	    .mon_cap = {S3K_SYS_MON_CAP_READ, mon_idx, pid, idx}
	};
	s3k_ret_t ret = do_ecall(args, sizeof(args.mon_cap));
	if (!ret.err)
		cap->raw = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			       s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			       s3k_cidx_t dst_idx)
{
	sys_args_t args = {
	    .mon_cap = {S3K_SYS_MON_CAP_MOVE, mon_idx, src_pid, src_idx,
			dst_pid, dst_idx}
	 };
	return do_ecall(args, sizeof(args.mon_cap)).err;
}

s3k_err_t s3k_try_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot)
{
	sys_args_t args = {
	    .mon_pmp = {S3K_SYS_MON_PMP_LOAD, mon_idx, pid, pmp_idx, pmp_slot}
	      };
	return do_ecall(args, sizeof(args.mon_pmp)).err;
}

s3k_err_t s3k_try_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
				 s3k_cidx_t pmp_idx)
{
	sys_args_t args = {
	    .mon_pmp = {S3K_SYS_MON_PMP_UNLOAD, mon_idx, pid, pmp_idx}
	      };
	return do_ecall(args, sizeof(args.mon_pmp)).err;
}

s3k_err_t s3k_try_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	sys_args_t args = {
	    .sock = {S3K_SYS_SOCK_SEND,
		     .sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	return do_ecall(args, sizeof(args.sock)).err;
}

s3k_reply_t s3k_try_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_idx)
{
	sys_args_t args = {
	    .sock = {S3K_SYS_SOCK_SENDRECV, .sock_idx = sock_idx,
		     .cap_idx = cap_idx}
	};
	register uint64_t t0 __asm__("t0");
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
	    .sock = {S3K_SYS_SOCK_SENDRECV,
		     .sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	register uint64_t t0 __asm__("t0");
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
