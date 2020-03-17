OBJ2_DIR=./ptree_test
OBJ2_NAME=ptree_test
OBJ3_DIR=./ptree_prtchld
OBJ3_NAME=ptree_prtchld
MODULE_DIR=./ptree
MODULE_NAME=ptree.ko

OBJ2=${OBJ2_DIR}/libs/armeabi/${OBJ2_NAME}
OBJ3=${OBJ3_DIR}/libs/armeabi/${OBJ3_NAME}
MODULE=${MODULE_DIR}/${MODULE_NAME}

DEST_DIR=/data/misc
DEST_OBJ2=${DEST_DIR}/${OBJ2_NAME}
DEST_OBJ3=${DEST_DIR}/${OBJ3_NAME}
DEST_MODULE=${DEST_DIR}/${MODULE_NAME}

nothing:
	@echo "Hello!"

emulator:
	./run_emulator.sh

shell:
	adb shell

build:
	make -C ${OBJ2_DIR}
	make -C ${OBJ3_DIR}
	make -C ${MODULE_DIR}

upload:
	adb shell rmmod ${DEST_MODULE}
	adb shell rm ${DEST_MODULE}
	adb push ${OBJ2} ${DEST_DIR}
	adb push ${OBJ3} ${DEST_DIR}
	adb push ${MODULE} ${DEST_DIR}


run: build upload
	adb shell chmod +x ${DEST_OBJ2} ${DEST_OBJ3}
	@echo "\n\n>> Problem 1"
	adb shell insmod ${DEST_MODULE}
	adb shell lsmod
	@echo "\n\n>> Problem 2"
	adb shell ${DEST_OBJ2}
	@echo "\n\n>> Problem 3"
	adb shell "cd ${DEST_DIR} && ./${OBJ3_NAME}"
	@echo "\n\nCleaning..."
	adb shell rmmod ${DEST_MODULE}

clean:
	make -C ${OBJ2_DIR} clean
	make -C ${OBJ3_DIR} clean
	make -C ${MODULE_DIR} clean
