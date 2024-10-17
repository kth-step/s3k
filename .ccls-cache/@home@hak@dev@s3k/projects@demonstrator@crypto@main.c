#include "aes128.h"
#include "s3k/s3k.h"

#include <stddef.h>
#include <stdint.h>

typedef enum { ENCRYPT, DECRYPT } crypto_mode_t;

static uint32_t enc_key[4] = {
    0x16157e2b,
    0xa6d2ae28,
    0x8815f7ab,
    0x3c4fcf09,
};

static uint32_t enc_round_key[44];

typedef struct {
	uint8_t *data;
	size_t len;
} frame_t;

crypto_mode_t sendrecv_frame(frame_t *);

void encrypt_data(frame_t *);
void decrypt_data(frame_t *);

void main(void)
{
	aes128_keyexpansion(enc_key, enc_round_key);
	frame_t frame;
	s3k_reg_write(S3K_REG_SERVTIME, 500);
	while (1) {
		crypto_mode_t encrypt = sendrecv_frame(&frame);
		switch (encrypt) {
		case ENCRYPT:
			encrypt_data(&frame);
			break;
		case DECRYPT:
			decrypt_data(&frame);
			break;
		}
	}
}

/**
 * Receives a data frame from monitor.
 * Returns true if frame should be encrypted,
 * otherwise false if frame should be decrypted.
 */
crypto_mode_t sendrecv_frame(frame_t *frame)
{
	// Receive a frame
	s3k_msg_t msg;
	msg.send_cap = false;
	s3k_reply_t reply;
	do {
		reply = s3k_sock_sendrecv(0x3, &msg);
	} while (reply.err);
	frame->data = (uint8_t *)reply.data[0];
	frame->len = reply.data[1];
	return (reply.data[2] % 2 == 0) ? ENCRYPT : DECRYPT;
}

void encrypt_data(frame_t *frame)
{
	// Encrypt the frame
	for (int i = 0; i < frame->len; i += 16)
		aes128_enc(enc_round_key, frame->data + i);
}

void decrypt_data(frame_t *frame)
{
	// Receive the frame
	for (int i = 0; i < frame->len; i += 16)
		aes128_dec(enc_round_key, frame->data + i);
}
