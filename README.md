# lisp
Simple lisp interpreter.

# Features
- Stop-and-copy garbage collection.
- Proper tail calls. The program below runs forever and doesn't cause a stack overflow.

  ```
    (define x (lambda () (x)))
    (x)
  ```
  
  The last expression in the body of a procedure, the consequent and alternative of `if`'s and the
  last expression in `and`'s, `or`'s and `begin`'s all result in proper tail calls.
- Some primitives and special forms: `car cdr cons eq? + - = > < write quote or and if define set! lambda begin`
 
# TODO
- Macros.
- Quoting with `'` (use the `quote` built-in instead).
- Print cyclic objects.
- Error handling. Currently it just prints a message and exits.
- Comments.
- Probably should create a `UNSPECIFIED` type for expressions that evaluate to an undefined value (e.g. `(define x 2)`).
- Support for floating-point (only integers for now).
- Better REPL.

# Run it
```
  $ make
  $ ./lisp
```

# See also
- [MiniLisp](https://github.com/rui314/minilisp)
- [Lisp in 1k lines of C, explained](https://github.com/Robert-van-Engelen/lisp)
- The book "Structure and Interpretation of Computer Programs"

