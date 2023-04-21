#!/usr/bin/env bash

# Build U-Boot scipts for vaaman and axon

DEVICE=$1
DATE=$(date +%Y%m%d)

# vaaman and axon
if [ "$DEVICE" == "vaaman" ]; then
	DEVICE="vaaman"
	echo "Building U-Boot for vaaman"
	./make.sh rk3399-vaaman
elif [ "$DEVICE" == "axon" ]; then
	DEVICE="axon"
	echo "Building U-Boot for axon"
	./make.sh rk3588-axon
elif [ "$DEVICE" == "clean" ]; then
	make clean -j$(nproc --all) && make distclean -j$(nproc --all)
	exit 0
else
	echo "Usage: ./build-uboot.sh vaaman|axon|clean"
	exit 1
fi

# Build U-Boot
./make.sh --idblock

# Create output directory if it doesn't exist
if [ "$1" = "vaaman" ]; then
	if [ ! -d vaaman ]; then
		mkdir -p $DEVICE
	fi
elif [ "$1" = "axon" ]; then
	if [ ! -d axon ]; then
		mkdir -p $DEVICE
	fi
else
	echo "Usage: ./build-uboot.sh vaaman|axon"
	exit 1
fi

cp -v uboot.img $DEVICE/uboot-"${DATE}".img
cp -v idblock.bin $DEVICE/idblock-"${DATE}".bin
if [ "$DEVICE" == "vaaman" ]; then
	cp -v trust.img $DEVICE/trust-"${DATE}".img
fi
