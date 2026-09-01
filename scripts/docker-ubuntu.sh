#!/bin/sh

set -eu

WORKSPACE="$(pwd -P)"
SCRIPT_DIR="$(CDPATH= cd -- "$(dirname "$0")" && pwd -P)"
REPO_ROOT="$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd -P)"
IMAGE_NAME="${S3K_DOCKER_UBUNTU_IMAGE:-s3k/ubuntu-riscv-picolibc-2}"

docker build \
	-t "$IMAGE_NAME" \
	-f "$REPO_ROOT/.devcontainer/Dockerfile.ubuntu" \
	"$REPO_ROOT/.devcontainer"

docker run -it --rm \
	-p "5555:5555" \
	-v "$WORKSPACE:/workspace" \
	-w /workspace \
	"$IMAGE_NAME"
