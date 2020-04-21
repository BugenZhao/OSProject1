AVD_NAME=OsPrj-518030910211
KERNEL_ZIMG=~/Android/kernel/goldfish/arch/arm/boot/zImage
KID=~/Android/kernel/goldfish
TOOLCHAIN=/home/bugenzhao/Android/android-ndk-linux/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin

MODULE_DIR=./ptree_syscall
MODULE_NAME=ptree.ko
OBJ2_DIR=./ptree_test
OBJ2_NAME=ptree_test
OBJ3_DIR=./ptree_prtchld
OBJ3_NAME=ptree_prtchld
BBC_DIR=./BurgerBuddies
BBC_NAME=BBC

MODULE=${MODULE_DIR}/${MODULE_NAME}
OBJ2=${OBJ2_DIR}/libs/armeabi/${OBJ2_NAME}
OBJ3=${OBJ3_DIR}/libs/armeabi/${OBJ3_NAME}
BBC=${BBC_DIR}/libs/armeabi/${BBC_NAME}

DEST_DIR=/data/misc

DEST_MODULE=${DEST_DIR}/${MODULE_NAME}
DEST_OBJ2=${DEST_DIR}/${OBJ2_NAME}
DEST_OBJ3=${DEST_DIR}/${OBJ3_NAME}
DEST_BBC=${DEST_DIR}/${BBC_NAME}


all: help

help:
	@echo "To run the test:"
	@echo "    1. Run 'make emulator' to start the emulator;"
	@echo "    2. Run 'make testall' to run the test."

emulator:
	emulator -avd ${AVD_NAME} -kernel ${KERNEL_ZIMG} -no-window -show-kernel

testall: clean
	make run | tee output.txt

shell:
	adb shell

build:
	@echo "\n\n\n>> Building..."
	make -C ${MODULE_DIR} KID=${KID} TOOLCHAIN=${TOOLCHAIN}
	make -C ${OBJ2_DIR}
	make -C ${OBJ3_DIR}
	make -C ${BBC_DIR}


upload:
	@echo "\n\n\n>> Uploading..."
	adb shell rmmod ${DEST_MODULE} > /dev/null
	adb shell rm -f ${DEST_MODULE}
	adb push ${MODULE} ${DEST_DIR}
	adb push ${OBJ2} ${DEST_DIR}
	adb push ${OBJ3} ${DEST_DIR}
	adb push ${BBC} ${DEST_DIR}



run: build upload
	@echo "\n\n\n>> Running..."
	@echo ">> Problem 1"
	adb shell chmod +x ${DEST_OBJ2} ${DEST_OBJ3}
	adb shell insmod ${DEST_MODULE}
	adb shell lsmod
	@echo "\n\n>> Problem 2"
	adb shell ${DEST_OBJ2}
	@echo "\n\n>> Problem 3"
	adb shell "cd ${DEST_DIR} && ./${OBJ3_NAME}"
	@echo "\n\n>> Problem 4"
	adb shell ${DEST_BBC} 2 4 41 10
	@echo "\n\n>> Cleaning..."
	adb shell rmmod ${DEST_MODULE} 

clean:
	make -C ${MODULE_DIR} clean
	make -C ${OBJ2_DIR} clean
	make -C ${OBJ3_DIR} clean
	make -C ${BBC_DIR} clean

handin:
	tar --exclude-vcs -cvf Prj1+518030910211.tar .
