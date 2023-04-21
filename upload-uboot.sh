#!/usr/bin/env bash

DEVICE=$1
DATE=$(date +%Y%m%d)
ETH_ADDR="192.168.29.91"

if [ -z "$DEVICE" ]; then
	echo "Usage: $0 <device>"
	echo "<device>: vaaman|axon"
	exit 1
fi

if [ "$DEVICE" != "vaaman" ] && [ "$DEVICE" != "axon" ]; then
	echo "Error: DEVICE must be vaaman or axon"
	exit 1
fi

scp "$DEVICE"/uboot-"$DATE".img utsav@$ETH_ADDR:~/
scp "$DEVICE"/idblock-"$DATE".bin utsav@$ETH_ADDR:~/
if [ "$DEVICE" == "vaaman" ]; then
	scp "$DEVICE"/trust-"$DATE".img utsav@$ETH_ADDR:~/
fi
