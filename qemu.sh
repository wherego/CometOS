#!/bin/sh
set -e
. ./iso.sh

DEBUG=""

while test $# -gt 0; do
    case "$1" in
        -d|--quick)
            DEBUG="-s -S"
            break
            ;;
        *)
            break
            ;;
    esac
done

qemu-system-$(./target-triplet-to-arch.sh $HOST) $DEBUG -cdrom cometos.iso
