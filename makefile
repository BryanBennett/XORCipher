# Bryan Bennett
# Simple make file
# 10/12/2020
# CSC 250 Lab 4

SOURCE = lab4.c
EXECUTABLE = lab4
COMPILE_FLAGS = -ansi -pedantic -Wall -g

all: $(SOURCE) get_args.c get_args.h
	gcc $(COMPILE_FLAGS) -o $(EXECUTABLE) $(SOURCE) get_args.c

tidy: $(SOURCE)
	clang-tidy -checks='*' $(SOURCE) -- -std=c99

clean:
	rm -rf $(EXECUTABLE)