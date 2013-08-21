#!/bin/bash

mount -t vfat /dev/loop2 /mnt
cp uefi_loader.efi /mnt/efi/boot/bootx64.efi
cp kernel.img /mnt/efi/kernel.img
sleep 0.2
umount /mnt

rm /vm/test/test.vdi
VBoxManage convertfromraw -format VDI disk.img /vm/test/test.vdi --uuid af2bf893-0c73-4543-91ea-b640dbb31abd
chmod 777 /vm/test/test.vdi
