#!/bin/bash
# set -x
# set -e

arch=$(uname -m)
gcc_ver=14.2.0
binutils_ver=16.2
gdb_ver=2.44

valid_archs=("x86_64" "aarch64" "riscv64" "i386" "i686" "arm" "arm64")
color_begin="\e[1;32m"
color_end="\e[0m"
echo -e "${color_begin}Installing cross-compilation toolchain for ${arch}${color_end}"  

if [ $# -lt 2 ]; then
    echo "Usage: $0 --arch <arch> [--gcc_ver <version>] [--binutils_ver <version>] [--gdb_ver <version>]"
    echo "Supported architectures: ${valid_archs[*]}"
    exit 1
fi

while [[ $# -gt 0 ]]; do
    case $1 in
        --arch)
            if [[ -n $2 ]]; then
                arch=$2
                shift
            else
                echo "Error: --arch requires an argument."
                exit 1
            fi
            ;;
        --gcc_ver)
            gcc_ver=$2
            shift
            ;;
        --binutils_ver)
            binutils_ver=$2
            shift
            ;;
        --gdb_ver)
            gdb_ver=$2
            shift
            ;;
        *)
            echo "Error: Invalid option $1"
            exit 1
            ;;
    esac
    shift
done

install_gcc() {
    
    # Add your installation commands here
}

install_gcc
