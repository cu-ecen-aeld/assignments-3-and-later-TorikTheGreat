#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

#
export PATH=$PATH:/home/student/arm-cross-compiler/arm-gnu-toolchain-13.3.rel1-x86_64-aarch64-none-linux-gnu/bin
SYSROOT=$(${CROSS_COMPILE}gcc --print-sysroot)
ORIGPATH=$(pwd)
#
if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
	# Deep clean build
	echo "Deep cleaning build"
	make ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" mrproper

	# Create defconfig
	echo "Creating defconfig"
	make ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" defconfig # virt

	# KERNEL COMPILATION STEP OMG WOWIE ZOWIE
	echo "Compiling kernel"
	make -j$(nproc) ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" all

	# Build modules
	echo "Building modules"
	make ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" modules

	# Build devicetree
	echo "Building device trees"
	make ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" dtbs

	#
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

mkdir -p "${OUTDIR}"/rootfs/{bin,dev,etc,home,lib,lib64,proc,sbin,sys,tmp,usr,var}
mkdir -p "${OUTDIR}"/rootfs/usr/{bin,lib,sbin}
mkdir -p "${OUTDIR}"/rootfs/var/log

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}"
make CONFIG_PREFIX="${OUTDIR}"/rootfs ARCH="${ARCH}" CROSS_COMPILE="${CROSS_COMPILE}" install


echo "Library dependencies"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a "${OUTDIR}"/rootfs/bin/busybox | grep "program interpreter"
#${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"
${CROSS_COMPILE}readelf -a  "${OUTDIR}"/rootfs/bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
cp "${SYSROOT}"/lib/ld-linux-aarch64.so.1 "${OUTDIR}"/rootfs/lib
cp "${SYSROOT}"/lib64/libm.so.6 "${OUTDIR}"/rootfs/lib64
cp "${SYSROOT}"/lib64/libresolv.so.2 "${OUTDIR}"/rootfs/lib64
cp "${SYSROOT}"/lib64/libc.so.6 "${OUTDIR}"/rootfs/lib64

# TODO: Make device nodes
sudo mknod -m 666 "${OUTDIR}"/rootfs/dev/null c 1 3
sudo mknod -m 666 "${OUTDIR}"/rootfs/dev/console c 5 1

# TODO: Clean and build the writer utility
make -C "${ORIGPATH}" CROSS_COMPILE=aarch64-none-linux-gnu- #cross compile writer
cp  "${ORIGPATH}"/writer "${OUTDIR}"/rootfs/home # copy writer to rootfs

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp  "${ORIGPATH}"/finder.sh "${OUTDIR}"/rootfs/home

cp  "${ORIGPATH}"/finder-test.sh "${OUTDIR}"/rootfs/home

cp  "${ORIGPATH}"/autorun-qemu.sh "${OUTDIR}"/rootfs/home

mkdir -p "${OUTDIR}"/rootfs/home/conf
cp  "${ORIGPATH}"/../conf/username.txt "${OUTDIR}"/rootfs/home/conf
cp  "${ORIGPATH}"/../conf/assignment.txt "${OUTDIR}"/rootfs/home/conf

# Replace references to ../conf with /conf in finder-test.sh
sed -i 's/..\/conf/conf/g' "${OUTDIR}"/rootfs/home/finder-test.sh

# TODO: Chown the root directory
sudo chown -R root:root "${OUTDIR}"/rootfs

# TODO: Create initramfs.cpio.gz
cd "${OUTDIR}"/rootfs
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f "${OUTDIR}"/initramfs.cpio

# Experiment: move image from arch/arm64/boot to ${OUTDIR}
cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}
