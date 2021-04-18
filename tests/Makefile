# paths
LIB = ../lib
INCLUDE = ../include
MODULES = ../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -g -Werror -I$(INCLUDE)   
LDFLAGS = -lm

# Αρχεία .o
# OBJS = $(MODULES)/game.o $(MODULES)/interface.o  $(MODULES)/state.o $(LIB)/k08.a $(LIB)/libraylib.a
OBJS = $(MODULES)/game.o $(MODULES)/interface.o  $(MODULES)/state_alt.o $(LIB)/k08.a $(LIB)/libraylib.a
# OBJS = set_utils_test.o $(LIB)/k08.a $(LIB)/libraylib.a
# OBJS = state_alt_test.o $(MODULES)/state_alt.o $(LIB)/k08.a $(LIB)/libraylib.a
# OBJS = state_test.o $(MODULES)/state.o $(LIB)/k08.a $(LIB)/libraylib.a


# Το εκτελέσιμο πρόγραμμα
EXEC = Game

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a
	
valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)