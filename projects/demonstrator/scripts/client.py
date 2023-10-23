#!/usr/bin/env python3
import serial
import sys
import time
import os
import struct
import threading
import shlex
import socket

# Constants for PPP Encoding
PPP_ESC = b'\x7C'
PPP_BGN = b'\x7B'
PPP_END = b'\x7D'

# Constants for commands
CMD_SEEK = 0x1
CMD_WRITE = 0x2
CMD_READ = 0x3
CMD_RUN = 0x4
CMD_RESET = 0x5

# Initializing serial connection
ser = serial.serial_for_url("socket://localhost:8888")

# Function for sending data with PPP encoding
def ppp_send(data, file=sys.stdout.buffer):
    """
    Encode and send data as a PPP frame.

    :param data: The data to be sent, as bytes.
    :param file: The file-like object where data will be written. Defaults to sys.stdout.buffer.
    """
    file.write(PPP_BGN)
    for c in data:
        byte = c.to_bytes(1)
        if byte in [PPP_ESC, PPP_BGN, PPP_END]:
            file.write(PPP_ESC)
            file.write((c ^ 0x20).to_bytes(1))
        else:
            file.write(byte)
    file.write(PPP_END)
    file.flush()

# Function for receiving data with PPP encoding
def ppp_recv(file=sys.stdin.buffer):
    """
    Decode a PPP frame received from the file-like object.

    :param file: The file-like object to read from. Defaults to sys.stdin.buffer.
    :return: The decoded data as bytes.
    """
    data = bytearray()

    while True:
        c = file.read(1)
        if c == PPP_BGN:
            break

    while True:
        c = file.read(1)
        if not c:
            raise EOFError("Reached end of file/stream without finding PPP_END")
        if c == PPP_BGN:
            data = bytearray()
        elif c == PPP_END:
            break;
        elif c == PPP_ESC:
            c = file.read(1)
            data.append(ord(c) ^ 0x20)
        else:
            data.append(ord(c))

    return bytes(data)

def seek(app, val):
    header = int(app) << 28 | CMD_SEEK << 24 | (int(val) & 0xFFFF)
    data = struct.pack(">I", header)
    ppp_send(data, ser)

def write(app, val):
    if isinstance(val, str):
        val = bytes(val, 'ascii')
    header = int(app) << 28 | CMD_WRITE << 24 | len(val)
    data = struct.pack(">I", header) + val
    ppp_send(data, ser)

def writefile(app, filepath):
    if not os.path.exists(filepath):
        print(f"The file '{filepath}' does not exist.")
        return True

    with open(filepath, "rb") as file:
        total = 0
        while True:
            file_data = file.read(256)
            if not file_data:
                break
            write(app, file_data)
            total = total + len(file_data)
        print(f"File '{filepath}' ({total} bytes) sent.")

def read(app, val):
    header = int(app) << 28 | CMD_READ << 24 | int(val)
    data = struct.pack(">I", header)
    ppp_send(data, ser)


def run(app):
    header = int(app) << 28 | CMD_RUN << 24
    data = struct.pack(">I", header)
    ppp_send(data, ser)

def reset(app):
    header = int(app) << 28 | CMD_RESET << 24
    data = struct.pack(">I", header)
    ppp_send(data, ser)


def send_cmd(cmd, args):
    """
    Execute the given command with the provided arguments.

    :param cmd: The command to execute.
    :param args: The arguments for the command.
    """
    commands = {
            'writefile': writefile,
            'read': read,
            'write': write,
            'run': run,
            'seek': seek,
            'reset' : reset,
            }

    if cmd in commands:
        commands[cmd](*args)
    else:
        print("invalid command")

def read_from_uart():
    """
    Continuously read and print data received from UART.
    """
    while True:
        try:
            received_data = ppp_recv(ser)
            if received_data:
                try:
                    print(f">{received_data.decode('utf-8')}")
                except UnicodeDecodeError:
                    print(f">{received_data}")
                sys.stdout.flush()
        except Exception as e:
            print(f"Error reading from UART: {e}")
            sys.exit(0)


try:
    # Create a thread to handle reading from UART
    read_thread = threading.Thread(target=read_from_uart)
    read_thread.daemon = True  # Set as a daemon thread so it exits when the main thread exits
    read_thread.start()
    while True:
        user_input = input()  # Read user input from the command line

        if user_input.lower() == 'exit':
            break

        try:
            parts = shlex.split(user_input)
            if not parts:
                continue
            cmd = parts[0].lower()
            args = parts[1:]
            send_cmd(cmd, args)
        except Exception as e:
            print(f'command error: {e}')

except (KeyboardInterrupt, Exception) as e:  # Catching the raised exception here along with KeyboardInterrupt
    print("Closing the connection.")
finally:
    ser.close()
