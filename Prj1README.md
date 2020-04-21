# CS356 Operating System Projects - Spring 2020 - Project 1
## Personal Information
Ziqi Zhao (赵梓淇, Bugen Zhao, 518030910211), F1803302, CS, SEIEE, SJTU.
## Build and Run Tests
```bash
make emulator (AVD_NAME=xxx KERNEL_ZIMG=yyy)  # Start the emulator
make testall | tee output.txt                 # Build and run tests
```
## Directory Structure
```
.
├── BurgerBuddies               # Problem 4: Burger Buddies
│   ├── jni
│   │   ├── Android.mk
│   │   ├── bugen_bbc.h           # A header including other nessary headers
│   │   ├── BurgerBuddies.c       # Main code of problem 4
│   │   ├── BurgerBuddies.h       # Header for BurgerBuddies.c with some `random` utilities
│   │   ├── sbuf.c                # A simple thread-safe FIFO buffer library inspired by CSAPP
│   │   ├── sbuf.h                # Header for `sbuf.c`
│   │   ├── sem.c                 # A wrapper library for semaphores
│   │   └── sem.h                 # Header for `sem.c`
│   └── Makefile
├── include
│   └── ptree.h                  # Definition of `prinfo`, both for kernel and user
├── Makefile                    # Makefile of project
├── Prj1README.md               # This README file
├── ptree_prtchld               # Problem 3: Parent and child
│   ├── jni
│   │   ├── Android.mk
│   │   └── ptree_prtchld.c       # Main code of problem 3
│   └── Makefile
├── ptree_syscall               # Problem 1: `ptree` system call module
│   ├── Makefile
│   └── ptree.c                  # Main code of problem 1 - `ptree` syscall
└── ptree_test                  # Problem 2: Print the tree
    ├── jni
    │   ├── Android.mk
    │   └── ptree_test.c          # Main code of problem 2
    └── Makefile
```
## Sample Output
Please refer to [`output.txt`](output.txt)
## Reference
- *Computer Systems: A Programmer's Perspective* for `sbuf.c` in Problem 4
- *Visual Studio Code `c_cpp_properties.json` reference* [ (link) ](https://code.visualstudio.com/docs/cpp/c-cpp-properties-schema-reference)
