#!/bin/bash

clear

# # # # # # # # # # # # # # # # # # #
# ./make.sh /path/to/linux-56kDroid #
# # # # # # # # # # # # # # # # # # #

ARCH="i386" PATH="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/":$PATH CC="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-gcc" CROSS_COMPILE="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-" make -j M="$(pwd)/56k/conexant/tgz/usr/lib/dgcmodem/modules/" CNXT_KERNELSRC="$1" KERNEL_DIR="$1" modules -C "$1"
ARCH="i386" PATH="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/":$PATH CC="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-gcc" CROSS_COMPILE="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-" make -j M="$(pwd)/56k/conexant/tgz/usr/lib/hcfpcimodem/modules/" CNXT_KERNELSRC="$1" KERNEL_DIR="$1" modules -C "$1"
ARCH="i386" PATH="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/":$PATH CC="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-gcc" CROSS_COMPILE="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-" make -j M="$(pwd)/56k/conexant/tgz/usr/lib/hsfmodem/modules/" CNXT_KERNELSRC="$1" KERNEL_DIR="$1" modules -C "$1"
ARCH="i386" PATH="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/":$PATH CC="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-gcc" CROSS_COMPILE="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-" make -j M="$(pwd)/56k/smlink/sl-modem-source_2.9.11~20100718-2_i386/usr/src/sl-modem-2.9.11~20100718/drivers/" CNXT_KERNELSRC="$1" KERNEL_DIR="$1" modules -C "$1"
ARCH="i386" PATH="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/":$PATH CC="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-gcc" CROSS_COMPILE="$(pwd)/i686-linux-glibc2.7-4.6-d2685281e2e54ca14d1df304867aa82c37b27162/bin/i686-linux-" make -j M="$(pwd)/56k/smlink/sl-modem-source_2.9.11~20100718-2_i386/usr/src/sl-modem-2.9.11~20100718/ungrab-winmodem/" CNXT_KERNELSRC="$1" KERNEL_DIR="$1" modules -C "$1"

find "$(pwd)/56k/" -type f -name "*.ko"
