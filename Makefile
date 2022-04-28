# Makefile for GauFinalOptCoord

SHELL = cmd
CC = gcc
CCFLAGS = -O3
CLINKER = $(CC)

.PHONY: all
all: GauFinalOptCoord.exe

GauFinalOptCoord.exe: GauFinalOptCoord.obj
	@echo Linking $@ against $^ ...
	$(CLINKER) -o $@ $^ -static -s

GauFinalOptCoord.obj: GauFinalOptCoord.c
	@echo Compiling $@ ...
	$(CC) -o $@ -c $< $(CCFLAGS) -s

.PHONY: clean
clean: clean_tmp
	-del /q GauFinalOptCoord.exe 1> NUL 2> NUL

.PHONY: clean_tmp
clean_tmp:
	-del /q GauFinalOptCoord.obj 1> NUL 2> NUL

