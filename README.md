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
- Some primitives and special forms: `car cdr cons eq? + - = > < write quote or and if define set! lambda begin macro`

# Examples
## Iterative Fibonacci

```
(define fibo (lambda (n)
  (define iter (lambda (a b count)
    (if (= n count)
        b
        (iter (+ a b) a (+ count 1)))))
  (iter 1 1 0)))
```

## Simple macro
```
(define set2! 
    (macro (v1 v2 e)
      (list (quote begin) 
            (list (quote set!) v1 e) 
            (list (quote set!) v2 e))))
```

## Object with state
```
> (define make-counter (lambda (init)
    (lambda ()
      (if (> init 0)
          (begin (set! init (- init 1)) init)
          init))))
compound procedure
> (define counter (make-counter 10))
compound procedure
> (counter)
9
> (counter)
8
```

# Run it
```
  $ make
  $ ./lisp
```

# TODO
- Quoting with `'` (use the `quote` built-in instead).
- Print cyclic objects.
- Error handling. Currently it just prints a message and exits.
- Comments.
- Probably should create a `UNSPECIFIED` type for expressions that evaluate to an undefined value (e.g. `(define x 2)`).
- Support for floating-point (only integers for now).
- Better REPL.

# See also
- [MiniLisp](https://github.com/rui314/minilisp)
- [Lisp in 1k lines of C, explained](https://github.com/Robert-van-Engelen/lisp)
- The book "Structure and Interpretation of Computer Programs"

