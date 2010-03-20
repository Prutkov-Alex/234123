#! /bin/bash

HWSOURCE=/mnt/hgfs/hw1/src
KERNELSOURCE=/usr/src/linux-2.4.18-14custom

DIRS='arch include kernel'

cd $HWSOURCE
for DIR in $DIRS; do
    cp -rf $DIR $KERNELSOURCE
done

cd $KERNELSOURCE
