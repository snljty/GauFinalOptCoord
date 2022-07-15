# Makefile for GauFinalOptCoord

SHELL = cmd
CC = gcc
OPTS = -s
CLINKER = $(CC)

.PHONY: all
all: GauFinalOptCoord.exe

GauFinalOptCoord.exe: GauFinalOptCoord.obj
	@echo Linking $@ against $^ ...
	$(CLINKER) -o $@ $^ -static $(OPTS)

GauFinalOptCoord.obj: GauFinalOptCoord.c
	@echo Compiling $@ ...
	$(CC) -o $@ -c $< $(CCFLAGS) $(OPTS)

.PHONY: clean
clean: clean_tmp
	-del /q GauFinalOptCoord.exe 2> NUL

.PHONY: clean_tmp
clean_tmp:
	-del /q GauFinalOptCoord.obj 2> NUL

