#==========================================
#    Makefile: makefile for sl 5.1
#	Copyright 1993, 1998, 2014
#                 Toyoda Masashi
#		  (mtoyoda@acm.org)
#	Last Modified: 2014/03/31
#==========================================

CC=gcc
CFLAGS=-O

sl: sl.c sl.h MiniShell.c g.h
	$(CC) $(CFLAGS) -o MiniShell MiniShell.c sl.c -lncurses
