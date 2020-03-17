OBJ_DIR=./ptree_test
OBJ_NAME=ptree_test
MODULE_DIR=./ptree
MODULE_NAME=ptree.ko

OBJ=${OBJ_DIR}/libs/armeabi/${OBJ_NAME}
MODULE=${MODULE_DIR}/${MODULE_NAME}

DEST_DIR=/data/misc
DEST_OBJ=${DEST_DIR}/${OBJ_NAME}
DEST_MODULE=${DEST_DIR}/${MODULE_NAME}

nothing:
	@echo "Hello!"

emulator:
	./run_emulator.sh

shell:
	adb shell

build:
	make -C ${OBJ_DIR}
	make -C ${MODULE_DIR}

upload:
	adb shell rmmod ${DEST_MODULE}
	adb shell rm ${DEST_MODULE}
	adb push ${OBJ} ${DEST_DIR}
	adb push ${MODULE} ${DEST_DIR}


run: build upload
	adb shell chmod +x ${DEST_OBJ}
	adb shell insmod ${DEST_MODULE}
	adb shell lsmod
	adb shell ${DEST_OBJ}
	adb shell rmmod ${DEST_MODULE}

clean:
	make -C ${OBJ_DIR} clean
	make -C ${MODULE_DIR} clean
