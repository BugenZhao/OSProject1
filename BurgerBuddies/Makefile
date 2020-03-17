build:
	cd jni && ndk-build

b:
	gcc jni/BurgerBuddies.c jni/sbuf.c jni/sem.c -o BBC_local -lpthread -g -std=gnu89 -Wall

r: b
	./BBC_local 2 4 41 10

clean:
	rm -rf libs obj
