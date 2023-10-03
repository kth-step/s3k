#!/bin/bash

if [[ -z "$TMUX" ]]; then
	echo "No tmux session"
	exit 0
fi

tmux split-window -h -d "./scripts/qemu.sh -s -S"
sleep 0.5
./scripts/gdb.sh

kill -SIGTERM $(pgrep qemu-system-*)
