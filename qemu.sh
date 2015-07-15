#!/bin/sh
set -e
. ./iso.sh

while test $# -gt 0; do
    case "$1" in
        -s|--quick)
            QEMU_DEBUG="-s -S"
            break
            ;;
	-d|--quick)
            GCC_DEBUG="-d"
            break
	    ;;
        *)
            break
            ;;
    esac
done

qemu-system-$(./target-triplet-to-arch.sh $HOST) $QEMU_DEBUG -cdrom cometos.iso -m 512M
