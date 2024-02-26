# Makefile for the smash program
CC = gcc 
CFLAGS = -std=c99 -g -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC)
OBJS = smash.o commands.o signals.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
commands.o: commands.c commands.h
smash.o: smash.c commands.h
signals.o: signals.c signals.h
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ smash "#"* core.*

