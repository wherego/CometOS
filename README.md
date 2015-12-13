<img src="documentation/images/cometos-logo.png" alt="Logo" height="203" width="700">
=======

CometOS is an operating system for x86 PCs, written in assembly language and C. The os supports interrupts and dynamic memory allocation along with other features. CometOS started on Oct 2, 2014 and has been growing ever since.

<img src="https://raw.githubusercontent.com/JohannMassyn/CometOS/master/documentation/images/%20CometOS-kernel-2015-08-06.png" alt="Kernel" height="400" width="720">

## Features

Currently supported elements:

- grub BootLoader
- Printing text to screen
- C support
- keybord input
- IRQ's
- Paging
- Timer
- Time and date
- VFS
- Heap
- Arrays

Working on
- File system
Todo elements:

- User space
- Multitasking
- Tasking
- System calls
- VGA drivers
- Shell

## Plugins used

- GCC
- G++
- GNU Make
- GNU Bison
- Flex
- GNU GMP
- GNU MPFR
- GNU MPC
- QUEM
- Virtualbox
- GRUB
- genext2fs

## Compatibility with computers

At the moment I'm using grub which is a free open source bootloader. CometOS can run on most x86 computers but sometimes it can have trouble on some systems due to its lack of testing.

## How to build/install

Navagate to the project folder:
```
cd CometOS
```

You can clean the source tree by invoking:
```
./clean.sh
```

You can build a bootable cdrom image of the operating system by invoking:
```
./iso.sh
```

You can run quem emulator quickly by invoking:
```
./qemu.sh
```

If you do not have permition to run the following files use the *chmod +x* command to alow access for one file or chmod -R 755 * for all.
