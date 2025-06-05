#!/usr/bin/env bash

set -e
set -u
set -o pipefail

sudo -v

# homepage: https://ftp.gnu.org/gnu/

ver_binutils=2.44 # TODO: always checks latest version
ver_gcc=15.1.0
ver_gdb=16.3

link_binutils=https://ftp.gnu.org/gnu/binutils/binutils-${ver_binutils}.tar.xz
link_gcc=https://ftp.gnu.org/gnu/gcc/gcc-${ver_gcc}/gcc-${ver_gcc}.tar.xz
link_gdb=https://ftp.gnu.org/gnu/gdb/gdb-${ver_gdb}.tar.xz

install_binutils=false
install_gcc=false
install_gdb=false

build_dir=$(realpath ./build)
keep_build=false

archs=()
# arch="$(uname -m)"

while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        --gcc)
            install_gcc=true
            shift
            ;;
        --gdb)
            install_gdb=true
            shift
            ;;
        --binutils)
            install_binutils=true
            shift
            ;;
        --arch)
            shift
            while [[ $# -gt 0 && ! $1 =~ ^-- ]]; do
                archs+=("$1")
                shift
            done
            ;;
        # --arch)
        #     if [[ $# -gt 1 ]]; then
        #         arch="$2"
        #         shift
        #     else
        #         echo "Error: --arch option requires an argument."
        #         exit 1
        #     fi
        #     shift
        #     ;;
        --build)
            if [[ $# -gt 1 ]]; then
                build_dir=$(realpath "$2")
                shift
            else
                echo "Error: --build option requires an argument."
                exit 1
            fi
            shift
            ;;
        --keep-build)
            keep_build=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --gcc          Install GCC"
            echo "  --gdb          Install GDB"
            echo "  --binutils     Install Binutils"
            echo "  --arch <arch>  Specify (multiple) architecture (e.g., x86_64, arm, etc.)"
            echo "  --build <dir>  Specify build directory (default: ./build)"
            echo "  --keep-build   Keep build directory after installation"
            echo "  --help         Show this help message"
            exit 0
            ;;
        *)
            echo "Error: Unknown option: $key"
            echo "Use --help for usage information."
            exit 1
            ;;
    esac
done

if [[ $install_binutils == false && $install_gcc == false && $install_gdb == false ]]; then
    echo "Error: At least one of --gcc, --gdb, or --binutils must be specified."
    exit 1
fi

if [[ -d $build_dir && "$(ls -A $build_dir)" ]]; then
    echo "Error: Build directory $build_dir is not empty."
    exit 1
elif [[ ! -d $build_dir ]]; then
    mkdir -p "$build_dir"
fi

cd "$build_dir"
if [[ $install_binutils == true ]]; then
    mkdir -p build_binutils
    cd build_binutils
    wget "$link_binutils" -O binutils.tar.xz
    
    mkdir -p build
    mkdir -p binutils
    tar Jxf binutils.tar.xz -C binutils --strip-components=1

    cd build

    for arch in "${archs[@]}"; do
        ../binutils/configure --prefix=/usr/local --target=${arch}-elf --program-prefix=${arch}-elf-
        make -j$(nproc) all
        sudo make install
        make distclean
    done
    cd ..

    cd ..
fi

if [[ $install_gcc == true ]]; then
    mkdir -p build_gcc
    cd build_gcc
    wget "$link_gcc" -O gcc.tar.xz
    
    mkdir -p build
    mkdir -p gcc
    tar Jxf gcc.tar.xz -C gcc --strip-components=1

    cd build
    for arch in "${archs[@]}"; do
        ../gcc/configure \
            --enable-languages=c \
            --enable-targets=all \
            --without-headers \
            --disable-nls \
            --disable-libssp \
            --disable-libmudflap \
            --disable-multilib \
            --disable-shared \
            --disable-threads \
            --disable-libgomp \
            --disable-libquadmath \
            --prefix=/usr/local --target=${arch}-elf --program-prefix=${arch}-elf- 
        make -j$(nproc) all-gcc all-target-libgcc
        sudo make install-gcc install-target-libgcc
        make distclean
    done
    cd ..

    cd ..
fi

if [[ $install_gdb == true ]]; then
    mkdir -p build_gdb
    cd build_gdb
    wget "$link_gdb" -O gdb.tar.xz
    
    mkdir -p build
    mkdir -p gdb
    tar Jxf gdb.tar.xz -C gdb --strip-components=1

    cd build

    for arch in "${archs[@]}"; do
        ../gdb/configure --prefix=/usr/local --target=${arch}-elf --program-prefix=${arch}-elf-
        make -j$(nproc) all
        sudo make install
        make distclean
    done
    cd ..

    cd ..
fi

cd ..
if [[ $keep_build == false ]]; then
    rm -rf $build_dir
fi

# references: https://stackoverflow.com/questions/66355305/how-do-i-install-x86-64-elf-ld-on-ubuntu-20-04
