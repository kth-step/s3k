#include "../config.h"
#include "s3k/s3k.h"
#include "string.h"

#define MONITOR_CIDX 0xa

typedef enum {
	CMD_INVALID = 0,
	CMD_SEEK = 0x1,
	CMD_WRITE = 0x2,
	CMD_READ = 0x3,
	CMD_RUN = 0x4,
	CMD_RESET = 0x5,
} cmd_t;

typedef struct {
	s3k_pid_t pid;
	char *memory;
	size_t seek;
	size_t size;
} app_data_t;

s3k_reply_t sendrecv(s3k_msg_t *msg);
void decrypt(void);
void encrypt(void);
cmd_t get_command(size_t len);

void run_app(s3k_msg_t *msg, app_data_t *app);
void seek_app(s3k_msg_t *msg, app_data_t *app);
void write_app(s3k_msg_t *msg, app_data_t *app);
void read_app(s3k_msg_t *msg, app_data_t *app);
void reset_app(s3k_msg_t *msg, app_data_t *app);
void invalid(s3k_msg_t *msg);

char *const uart_buffer = SHARED0_MEM;
char *const crypto_buffer = SHARED1_MEM;

void setup_memory(void)
{
	s3k_cap_t pmp;
	pmp = s3k_mk_pmp(s3k_napot_encode((uint64_t)APP0_MEM, APP0_MEM_LEN),
			 S3K_MEM_RW);
	s3k_cap_derive(0x5, 0x10, pmp);
	s3k_pmp_load(0x10, 4);
	s3k_sync_mem();

	pmp = s3k_mk_pmp(s3k_napot_encode((uint64_t)APP0_MEM, APP0_MEM_LEN),
			 S3K_MEM_RWX);
	s3k_cap_derive(0x5, 0x12, pmp);
	s3k_mon_cap_move(MONITOR_CIDX, MONITOR_PID, 0x12, APP0_PID, 0x0);
	s3k_mon_pmp_load(MONITOR_CIDX, APP0_PID, 0x0, 0x0);

	pmp = s3k_mk_pmp(s3k_napot_encode((uint64_t)APP1_MEM, APP1_MEM_LEN),
			 S3K_MEM_RW);
	s3k_cap_derive(0x6, 0x11, pmp);
	s3k_pmp_load(0x11, 5);
	s3k_sync_mem();

	pmp = s3k_mk_pmp(s3k_napot_encode((uint64_t)APP1_MEM, APP1_MEM_LEN),
			 S3K_MEM_RWX);
	s3k_cap_derive(0x6, 0x12, pmp);
	s3k_mon_cap_move(MONITOR_CIDX, MONITOR_PID, 0x12, APP1_PID, 0x0);
	s3k_mon_pmp_load(MONITOR_CIDX, APP1_PID, 0x0, 0x0);
}

void main(void)
{
	// Revoke on time slice
	s3k_cap_revoke(0x2);
	setup_memory();

	app_data_t apps[2];
	apps[0].pid = APP0_PID;
	apps[0].memory = APP0_MEM;
	apps[0].size = APP1_MEM_LEN;
	apps[0].seek = 0;
	apps[1].pid = APP0_PID;
	apps[1].memory = APP1_MEM;
	apps[1].size = APP1_MEM_LEN;
	apps[1].seek = 0;

	s3k_reply_t reply;
	s3k_msg_t msg;
	while (1) {
		reply = sendrecv(&msg);
		cmd_t cmd = get_command(reply.data[0]);
		int app = (uart_buffer[0] >> 4) % 0x2;
		switch (cmd) {
		case CMD_SEEK:
			seek_app(&msg, &apps[app]);
			break;
		case CMD_RUN:
			run_app(&msg, &apps[app]);
			break;
		case CMD_WRITE:
			write_app(&msg, &apps[app]);
			break;
		case CMD_READ:
			read_app(&msg, &apps[app]);
			break;
		case CMD_RESET:
			reset_app(&msg, &apps[app]);
			break;
		default:
			invalid(&msg);
		}
	}
}

cmd_t get_command(size_t len)
{
	if (len < 4)
		return CMD_INVALID;
	switch (uart_buffer[0] & 0xF) {
	case CMD_SEEK:
	case CMD_RUN:
	case CMD_WRITE:
	case CMD_READ:
	case CMD_RESET:
		return uart_buffer[0] & 0xF;
	default:
		return CMD_INVALID;
	}
}

s3k_reply_t sendrecv(s3k_msg_t *msg)
{
	s3k_reply_t reply;
	do {
		reply = s3k_sock_sendrecv(0x7, msg);
	} while (reply.err);
	return reply;
}

void decrypt(void)
{
	s3k_msg_t msg;
	msg.send_cap = false;
	msg.data[0] = 0;
	msg.data[1] = 0;
	msg.data[2] = 0x1;
	while (s3k_sock_sendrecv(0x4, &msg).err)
		;
}

void encrypt(void)
{
	s3k_msg_t msg;
	msg.send_cap = false;
	msg.data[0] = 0;
	msg.data[1] = 0;
	msg.data[2] = 0x0;
	while (s3k_sock_sendrecv(0x4, &msg).err)
		;
}

void run_app(s3k_msg_t *msg, app_data_t *app)
{
	s3k_mon_resume(MONITOR_CIDX, app->pid);
	s3k_mon_yield(MONITOR_CIDX, app->pid);
	s3k_mon_suspend(MONITOR_CIDX, app->pid);
	msg->data[0] = 2;
	memcpy(uart_buffer, "ok", 2);
}

void seek_app(s3k_msg_t *msg, app_data_t *app)
{
	uint16_t val = uart_buffer[2] << 8 | uart_buffer[3];
	val = (val > app->size) ? app->size : val;
	app->seek = val;
	msg->data[0] = 2;
	memcpy(uart_buffer, "ok", 2);
}

void write_app(s3k_msg_t *msg, app_data_t *app)
{
	uint16_t len = uart_buffer[2] << 8 | uart_buffer[3];
	int i = 0;
	while (i < len && i < SHARED0_MEM_LEN && app->seek < app->size) {
		app->memory[app->seek++] = uart_buffer[(i++) + 4];
	}
	msg->data[0] = 2;
	memcpy(uart_buffer, "ok", 2);
}

void read_app(s3k_msg_t *msg, app_data_t *app)
{
	uint16_t len = uart_buffer[2] << 8 | uart_buffer[3];
	int i = 0;
	while (i < len && app->seek < app->size) {
		uart_buffer[i++] = app->memory[app->seek++];
	}
	msg->data[0] = i;
}

void reset_app(s3k_msg_t *msg, app_data_t *app)
{
	s3k_mon_reg_write(MONITOR_CIDX, app->pid, S3K_REG_PC,
			  (uint64_t)app->memory);
	msg->data[0] = 2;
	memcpy(uart_buffer, "ok", 2);
}

void invalid(s3k_msg_t *msg)
{
	msg->data[0] = 3;
	memcpy(uart_buffer, "inv", 3);
}
