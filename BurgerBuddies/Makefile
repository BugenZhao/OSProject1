build:
	cd jni && ndk-build

localbuild:
	gcc jni/BurgerBuddies.c jni/sbuf.c jni/sem.c -o BBC_local -lpthread -g -std=gnu89 -Wall

localrun: localbuild
	./BBC_local 2 4 41 10

clean:
	rm -rf libs obj BBC_local
