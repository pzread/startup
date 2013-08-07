#!/bin/sh

qemu-kvm -enable-kvm -net nic,macaddr=52:54:00:00:EE:02 -m 8 -drive file=startup.img,if=ide -smp 4 -vga std
