#!/bin/sh
set -e
. ./build.sh

while test $# -gt 0; do
    case "$1" in
	-d|--quick)
            GCC_DEBUG="-g"
            break
	    ;;
        *)
            break
            ;;
    esac
done

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/cometos.kernel isodir/boot/cometos.kernel
tar --verbose --create --file isodir/boot/cometos.initrd --directory=sysroot $(ls sysroot | grep -v boot)
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "cometos" {
	multiboot /boot/cometos.kernel
	module /boot/cometos.initrd
}
EOF
grub-mkrescue -o cometos.iso isodir
