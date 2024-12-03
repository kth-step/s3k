#define APP0_PID 0
#define APP1_PID 1

// See plat_conf.h
#define BOOT_PMP 0
#define RAM_MEM 1
#define UART_MEM 2
#define TIME_MEM 3
#define HART0_TIME 4
#define HART1_TIME 5
#define HART2_TIME 6
#define HART3_TIME 7
#define MONITOR 8
#define CHANNEL 9

// normal caps
#define UART_CAP 10
#define UART_PMP 1
#define BUFFER_PMP 2

// CAPS for second process
#define APP_1_CAP_PMP_MEM 0
#define APP_1_CAP_PMP_UART 10
#define APP_1_TIME 1
#define APP_1_CAP_SOCKET 3

#define APP_1_PMP_SLOT_MEM 0
#define APP_1_PMP_SLOT_UART 1
#define APP_1_PMP_SLOT_BUFFER 2

// CAPS for second process
#define FREE_CAP_BEGIN 12
#define FREE_CAP_END 20

// Other constants
#define APP_1_BASE_ADDR 0x80020000
#define APP_1_SIZE 0x10000
#define SHARED_BUFFER_BASE (APP_1_BASE_ADDR+APP_1_SIZE)
#define SHARED_BUFFER_SIZE 0x10000

void setup_uart()
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, UART_CAP, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(UART_CAP, UART_PMP);
	// Synchronize PMP unit (hardware) with PMP configuration
	// false => not full synchronization.
	s3k_sync_mem();
}

void setup_uart_and_virtio()
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x2000);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, UART_CAP, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(UART_CAP, UART_PMP);
	// Synchronize PMP unit (hardware) with PMP configuration
	// false => not full synchronization.
	s3k_sync_mem();
}

void default_trap_handler(void) __attribute__((interrupt("machine")));
void default_trap_handler(void) {
    	// We enter here on illegal instructions, for example writing to
	// protected area (UART).

	// On an exception we do
	// - tf.epc = tf.pc (save program counter)
	// - tf.pc = tf.tpc (load trap handler address)
	// - tf.esp = tf.sp (save stack pointer)
	// - tf.sp = tf.tsp (load trap stack pointer)
	// - tf.ecause = mcause (see RISC-V privileged spec)
	// - tf.eval = mval (see RISC-V privileged spec)
	// tf is the trap frame, all registers of our process
	uint64_t epc = s3k_reg_read(S3K_REG_EPC);
	uint64_t esp = s3k_reg_read(S3K_REG_ESP);
	uint64_t ecause = s3k_reg_read(S3K_REG_ECAUSE);
	uint64_t eval = s3k_reg_read(S3K_REG_EVAL);

	alt_printf(
			   "error info:\n- epc: 0x%x\n- esp: 0x%x\n- ecause: 0x%x\n- eval: 0x%x\n",
			   epc, esp, ecause, eval);
	alt_printf("restoring pc and sp\n\n");
	// __attribute__((interrupt("machine"))) replaces `ret` with an `mret`.
	// When mret is executed in user-mode, the kernel catches it setting the
	// following:
	// - tf.pc = tf.epc
	// - tf.sp = tf.esp
	// Restoring pc and sp to the previous values, unless epc and esp was
	// overwritten.
} 


void setup_trap(void (*trap_handler)(void), void * trap_stack_base, uint64_t trap_stack_size)
{
	// Sets the trap handler
	s3k_reg_write(S3K_REG_TPC, (s3k_addr_t)trap_handler);
	// Set the trap stack
	s3k_reg_write(S3K_REG_TSP, ((s3k_addr_t)trap_stack_base) + trap_stack_size);
}


#define TAG_BLOCK_TO_ADDR(tag, block) ( \
					(((uint64_t) tag) << S3K_MAX_BLOCK_SIZE) + \
					(((uint64_t) block) << S3K_MIN_BLOCK_SIZE) \
					)

void s3k_print_cap(s3k_cap_t *cap) {
	if (!cap)
		alt_printf("Capability is NULL\n");
	switch ((*cap).type) {
	case S3K_CAPTY_NONE:
		alt_printf("No Capability\n");
		break;
	case S3K_CAPTY_TIME:
		alt_printf("Time hart:%X bgn:%X mrk:%X end:%Z\n",
				   (*cap).time.hart, (*cap).time.bgn, (*cap).time.mrk, (*cap).time.end);
		break;
	case S3K_CAPTY_MEMORY:
		alt_printf("Memory rwx:%X lock:%X bgn:%X mrk:%X end:%X\n",
				   (*cap).mem.rwx, (*cap).mem.lck,
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.bgn),
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.mrk),
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.end)
				   );
		break;
	case S3K_CAPTY_PMP:
		alt_printf("PMP rwx:%X used:%X index:%X address:%Z\n",
				   (*cap).pmp.rwx, (*cap).pmp.used, (*cap).pmp.slot, (*cap).pmp.addr);
		break;
	case S3K_CAPTY_MONITOR:
		alt_printf("Monitor  bgn:%X mrk:%X end:%X\n",
				    (*cap).mon.bgn, (*cap).mon.mrk, (*cap).mon.end);
		break;
	case S3K_CAPTY_CHANNEL:
		alt_printf("Channel  bgn:%X mrk:%X end:%X\n",
				    (*cap).chan.bgn, (*cap).chan.mrk, (*cap).chan.end);
		break;
	case S3K_CAPTY_SOCKET:
		alt_printf("Socket  mode:%X perm:%X channel:%X tag:%X\n",
				    (*cap).sock.mode, (*cap).sock.perm, (*cap).sock.chan, (*cap).sock.tag);
		break;
	}
}

void debug_capability_from_idx(uint32_t cap_idx) {
	s3k_cap_t cap;
	while (s3k_cap_read(cap_idx, &cap));
	s3k_print_cap(&cap);
}

uint32_t log_sys(char * msg, uint32_t res) {
    alt_printf("%s %X\n", msg, res);
}

uint32_t find_free_cap() {
	s3k_cap_t cap;
    for (uint32_t i = FREE_CAP_BEGIN; i <= FREE_CAP_END; i++) {
    	if (s3k_cap_read(i, &cap))
            return i;
    }
    return 0;
}


void setup_app_1()
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	uint64_t app1_addr = s3k_napot_encode(APP_1_BASE_ADDR, APP_1_SIZE);

	// Derive a PMP capability for app1 main memory
	uint32_t free_cap_mem_idx = find_free_cap();
	log_sys("1",
		s3k_cap_derive(RAM_MEM, free_cap_mem_idx, s3k_mk_memory(APP_1_BASE_ADDR, APP_1_BASE_ADDR + APP_1_SIZE, S3K_MEM_RWX)));
	uint32_t free_cap_idx = find_free_cap();
	log_sys("1",
		s3k_cap_derive(free_cap_mem_idx, free_cap_idx, s3k_mk_pmp(app1_addr, S3K_MEM_RWX)));
	log_sys("2",
		s3k_mon_cap_move(MONITOR, APP0_PID, free_cap_idx, APP1_PID, APP_1_CAP_PMP_MEM));
	log_sys("3",
		s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_CAP_PMP_MEM, APP_1_PMP_SLOT_MEM));

	// Derive a PMP capability for uart
	log_sys("4",
		s3k_cap_derive(UART_MEM, free_cap_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW)));
	log_sys("5",
		s3k_mon_cap_move(MONITOR, APP0_PID, free_cap_idx, APP1_PID, APP_1_CAP_PMP_UART));
	log_sys("6",
		s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_CAP_PMP_UART, APP_1_PMP_SLOT_UART));

	// Write start PC of app1 to PC
	log_sys("7",
		s3k_mon_reg_write(MONITOR, APP1_PID, S3K_REG_PC, APP_1_BASE_ADDR));

	s3k_sync_mem();
}

#define NO_APP_1 	0
#define NO_APP_0 	1
#define ROUND_ROBIN 2
#define PARALLEL	3

void setup_scheduling(uint32_t scheduling_type) {
	// Disable the other two cores
	s3k_cap_delete(HART2_TIME);
	s3k_cap_delete(HART3_TIME);

	if (scheduling_type == NO_APP_1) {
		s3k_cap_delete(HART1_TIME);
	} 
	else if (scheduling_type == NO_APP_0) {
		// Notice that we must be able to finish our job to setup APP 1
		s3k_cap_delete(HART1_TIME);
		uint32_t cap_idx = find_free_cap();
		s3k_cap_derive(HART0_TIME, cap_idx, s3k_mk_time(S3K_MIN_HART, 0, S3K_SLOT_CNT / 2));
		s3k_mon_cap_move(MONITOR, APP0_PID, HART0_TIME, APP1_PID, APP_1_TIME);
	}
	else if (scheduling_type == ROUND_ROBIN) {
		s3k_cap_delete(HART1_TIME);
		uint32_t cap_idx = find_free_cap();
		log_sys("Time derivation", 
			s3k_cap_derive(HART0_TIME, cap_idx, 
				s3k_mk_time(S3K_MIN_HART, 0, S3K_SLOT_CNT / 2)));
		log_sys("Time delegation", 
			s3k_mon_cap_move(MONITOR, APP0_PID, cap_idx, APP1_PID, APP_1_TIME));
	}
	else if (scheduling_type == PARALLEL) {
		s3k_mon_cap_move(MONITOR, APP0_PID, HART1_TIME, APP1_PID, APP_1_TIME);
	}
    s3k_sync();
}


uint32_t setup_socket(bool server, bool yield, bool capability)
{
	uint64_t socket_server = find_free_cap();
	uint64_t yield_mode = (yield)?S3K_IPC_YIELD:S3K_IPC_NOYIELD;

	s3k_ipc_perm_t permission = S3K_IPC_SDATA | S3K_IPC_CDATA;
	if (capability)
		permission |= S3K_IPC_CCAP | S3K_IPC_SCAP;

	s3k_cap_derive(CHANNEL, socket_server,
		       s3k_mk_socket(0, yield_mode, permission, 0));
	uint64_t socket_client = find_free_cap();
	s3k_cap_derive(socket_server, socket_client,
		       s3k_mk_socket(0, yield_mode, permission, 1));
	if (server) {
		s3k_mon_cap_move(MONITOR, APP0_PID, socket_client, APP1_PID, APP_1_CAP_SOCKET);
		return socket_server;
	}
	s3k_mon_cap_move(MONITOR, APP0_PID, socket_server, APP1_PID, APP_1_CAP_SOCKET);
	return socket_client;
}


void server_main_loop(s3k_msg_t (* handler) (s3k_reply_t, uint32_t), bool capability) {
	s3k_msg_t msg;
	s3k_reply_t reply;
	while (true)
	{
		do {
			if (capability) {
				msg.cap_idx = find_free_cap();
				msg.send_cap = 1;
			}
			alt_puts("APP1: ready to receive\n");
			reply = s3k_sock_sendrecv(APP_1_CAP_SOCKET, &msg);
			if (reply.err == S3K_ERR_TIMEOUT)
				alt_puts("APP1: timeout\n");
		} while (reply.err);
		msg = (*handler)(reply, msg.cap_idx);
	}
}

void wait_for_app1_blocked() {
	s3k_state_t state;
	while (true) {
		alt_puts("APP0: waiting loop\n");
		if (s3k_mon_state_get(MONITOR, APP1_PID, &state))
			continue;
		if (state & S3K_PSF_BLOCKED)
			break;
		alt_puts("APP0: waiting 1\n");
		s3k_mon_yield(MONITOR, APP1_PID);
		alt_puts("APP0: waiting 2\n");
	}	
}

s3k_reply_t send_receive_forever(uint64_t socket, s3k_msg_t msg) {
	s3k_reply_t reply;
	do {
		reply = s3k_sock_sendrecv(socket, &msg);
		while (reply.err == S3K_ERR_TIMEOUT) {
			alt_puts("APP0: timeout");
			reply = s3k_sock_recv(socket, msg.cap_idx);
		}
	} while (reply.err);
	return reply;
}
