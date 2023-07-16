CFLAGS := -Wall -g -O0

SRC := src
SRCS := $(wildcard $(SRC)/*.c)

OBJ := obj
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

lisp: $(OBJS)
	gcc $(OBJS) -o lisp

$(OBJ)/%.o: $(SRC)/%.c
	gcc $(CFLAGS) $(EXTRAFLAGS) -c $< -o $@

clean:
	rm -r lisp $(OBJ)/*
