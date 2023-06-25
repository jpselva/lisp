CFLAGS := -Wall -g -O0

SRC := src
SRCS := $(wildcard $(SRC)/*.c)

OBJ := obj
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BIN := lisp

lisp: $(OBJS)
	gcc $(OBJS) -o lisp

$(OBJ)/%.o: $(SRC)/%.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -r $(BIN) $(OBJ)/*
