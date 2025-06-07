#!/usr/bin/env bash

set -e
set -u
set -o pipefail

sudo -v

sudo apt update
sudo apt install -y build-essential make gcc g++
sudo apt install -y wget
sudo apt install -y qemu-system
sudo apt install -y libgmp-dev libmpfr-dev libmpc-dev libexpat1-dev
