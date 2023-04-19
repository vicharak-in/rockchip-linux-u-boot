#!/usr/bin/env bash

# Build U-Boot scipts for rk3399 and rk3588

DEVICE=""
DATE=$(date +%Y%m%d)

# rk3399 and rk3588
if [ "$1" = "rk3399" ]; then
	DEVICE="vaaman"
	./make.sh rk3399
elif [ "$1" = "rk3588" ]; then
	DEVICE="axon"
	echo "Building U-Boot for RK3588"
	./make.sh rk3588-axon
elif [ "$1" = "clean" ]; then
	make clean -j$(nproc --all) && make distclean -j$(nproc --all)
	exit 0
else
	echo "Usage: ./build-uboot.sh rk3399|rk3588"
	exit 1
fi

# Build U-Boot
./make.sh --idblock

# Create output directory if it doesn't exist
if [ "$1" = "rk3399" ]; then
	if [ ! -d vaaman ]; then
		mkdir -p $DEVICE
	fi
elif [ "$1" = "rk3588" ]; then
	if [ ! -d axon ]; then
		mkdir -p $DEVICE
	fi
else
	echo "Usage: ./build-uboot.sh rk3399|rk3588"
	exit 1
fi

cp -v uboot.img $DEVICE/uboot-"${DATE}".img
cp -v idblock.bin $DEVICE/idblock-"${DATE}".bin
if [ "$1" = "rk3399" ]; then
	cp -v trust.img $DEVICE/trust-"${DATE}".img
fi
