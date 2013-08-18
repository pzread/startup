#!/bin/sh

#qemu-system-x86_64 -enable-kvm -net nic,macaddr=52:54:00:00:EE:02 -m 2048 -drive file=startup.img,if=ide -smp 4
qemu-system-x86_64 -enable-kvm -L ~/devel/ovmf -net nic,macaddr=52:54:00:00:EE:02 -m 2048 -drive file=test.img,if=ide -smp 4
