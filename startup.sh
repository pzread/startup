#!/bin/sh

qemu-system-x86_64 -enable-kvm -net nic,macaddr=52:54:00:00:EE:02 -m 2048 -drive file=startup.img,if=ide -smp 4 -monitor stdio
