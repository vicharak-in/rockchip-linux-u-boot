#!/usr/bin/env bash
# shellcheck source=/dev/null
#
# SPDX-License-Identifier: MIT
# Copyright (C) 2023 Utsav Balar (utsavbalar1231@gmail.com)
# Version: 1.1
#
# U-Boot build srcipt for vicharak vaaman and axon

DEVICE=$1
JOBS=$(nproc --all)
DATE=$(date +%Y%m%d)
# patch to modify make.sh to use system compiler
PATCH=$(cat << EOF
diff --git a/make.sh b/make.sh
index 6abf059c5d..fc5d3b2704 100755
--- a/make.sh
+++ b/make.sh
@@ -12,8 +12,8 @@ CMD_ARGS=$1

 ########################################### User can modify #############################################
 RKBIN_TOOLS=rkbin/tools
-CROSS_COMPILE_ARM32=../prebuilts/gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
-CROSS_COMPILE_ARM64=../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
+CROSS_COMPILE_ARM32=/usr/bin/arm-linux-gnueabihf-
+CROSS_COMPILE_ARM64=/usr/bin/aarch64-linux-gnu-
 ########################################### User not touch #############################################
 # Declare global INI file searching index name for every chip, update in select_chip_info()
 RKCHIP=
EOF
)

# Prints the string in bold yellow color if the line is a header
# if the line is not a header then it prints the string in bold green color
function print() {
	# check if the line is a header or not
	if [[ $1 == *"---"* ]]; then
		echo -e "\e[1;33m${1}\e[0m"
		return
	fi
	echo -e "\e[1;32m${1}\e[0m"
}

# Usage function for this script which will be called when -h/help option is passed
# or when an invalid option is passed
function usage() {
	print "--------------------------------------------------------------------------------"
	print "Build script for Vicharak u-boot"
	print "Usage: $0 [OPTIONS]"
	print "Options:"
	print "  android | -a\t\tBuild u-boot for android"
	print "  axon    | -A\t\tBuild u-boot for axon"
	print "  clean   | -C\t\tClean build files"
	print "  help    | -h\t\tShow this help"
	print "  vaaman  | -V\t\tBuild u-boot for vaaman"
	print ""
	print "Example: $0 -A"
	print "Above command will build u-boot for axon"
	print "--------------------------------------------------------------------------------"

	exit 0
}

# Check build command for help
if echo "$@" | grep -wqE "help|-h"; then
	if [ -n "$2" ] && [ "$(type -t usage "$2")" == function ]; then
		print "----------------------------------------------------------------"
		print "--- $2 Build Command ---"
		print "----------------------------------------------------------------"
		eval usage "$2"
	else
		usage
	fi
fi

# Check if build finished successfully or not
function finish_build() {
	case "$1" in
	*rk3399*)
		if [ -f "trust.img" ] && [ -f "uboot.img" ] && [ -f "idbloader.img" ]; then
			if [ ! -d "$DEVICE" ]; then
				mkdir -p "$DEVICE"
			fi
			rsync -au trust.img "${DEVICE}"/trust-"${DATE}".img
			rsync -au uboot.img "${DEVICE}"/uboot-"${DATE}".img
			rsync -au idbloader.img "${DEVICE}"/idbloader-"${DATE}".img
		else
			print "Error: trust.img, u-boot.img or idbloader.img not found"
			exit 1
		fi
		;;
	*rk3588*)
		if [ -f "uboot.img" ] && [ -f "idbloader.img" ]; then
			if [ ! -d "$DEVICE" ]; then
				mkdir -p "$DEVICE"
			fi
			rsync -au uboot.img "${DEVICE}"/uboot-"${DATE}".img
			rsync -au idbloader.img "${DEVICE}"/idbloader-"${DATE}".img
		else
			print "Error: u-boot.img or idbloader.img not found"
			exit 1
		fi
		;;
	*)
		print "Error: Unknown device"
		exit 1
		;;
	esac
	print "Copied u-boot images to $DEVICE directory"
}

# Build u-boot for given device
function build_uboot() {
	print "Building u-boot for $1"

	# apply patch
	echo "$PATCH" | git apply

	# Build U-Boot
	./make.sh "$1"
	./make.sh --idblock

	finish_build "$1"

	# remove patch
	echo "$PATCH" | git apply -R
}

OPTIONS=$(echo "$@" | sed -e 's/ /\n/g' | tr '\n' ' ')
print "----------------------------------------------------------------"
print "                   Build Commands = ${OPTIONS}"
print "----------------------------------------------------------------"
for OPT in ${OPTIONS}; do
	case ${OPT} in
	"clean" | "-C")
		make clean -j "${JOBS}" && make distclean -j "${JOBS}"
		;;
	"android" | "-a")
		ANDROID="true"
		;;
	"axon" | "-A")
		DEVICE="axon"
		build_uboot rk3588-${DEVICE}
		;;
	"vaaman" | "-V")
		DEVICE="vaaman"

		if [ "$ANDROID" == "true" ]; then
			build_uboot rk3399-${DEVICE}-android
		else
			build_uboot rk3399-${DEVICE}
		fi
		;;
	*)
		print "Invalid option: ${OPT}"
		usage
		;;
	esac
done
