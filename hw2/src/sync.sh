#! /bin/bash

HWSOURCE=/mnt/hgfs/src
KERNELSOURCE=/usr/src/linux-2.4.18-14custom

DIRS='arch include kernel'

cd $HWSOURCE
for DIR in $DIRS; do
    cp -rf $DIR $KERNELSOURCE
done

cd $KERNELSOURCE
make bzImage && make modules && make install

if [ $? != 0 ]; then
	cp arch/i386/boot/bzImage /boot/vmlinuz-2.4.18-14custom
	cp System.map /boot/System.map-2.4.18-14custom
	cd /boot
	rm initrd-2.4.18-14custom.img
	mkinitrd initrd-2.4.18-14custom.img 2.4.18-14custom
	reboot
else
	echo "Won't compile = won't install!"
fi
