#!/bin/bash

# nice colors and formatting
red=`tput setaf 1`
green=`tput setaf 2`
yellow=`tput setaf 3`
bold=`tput bold`
reset=`tput sgr0`
fail=${red}${bold}
pass=${green}${bold}
warn=${yellow}${bold}

test()
{
    OUTPUT=`echo $2 | ./lisp`

    if [ $? != 0 ]
    then
        echo "${fail}FAIL${reset}: $1 didn't exit with status code 0"
    elif [ "$OUTPUT" = "$3" ]
    then
        echo "${pass}PASS${reset}: $1"
    else
        echo "${fail}FAIL${reset}: $1 (expected ${3}, got $OUTPUT)"
    fi
}

make clean
make EXTRAFLAGS="-DALWAYS_GC -DQUIET"

printf "\n${warn}>>> Start tests <<<${reset}\n\n"

#############
# tests
#############

test self-evaluating 42 42

test self-evaluating "\"hello\"" "\"hello\""

test cons "(cons 1 2)" "(1 . 2)"

test car "(car (cons 1 \"hello\"))" 1

test cdr "(cdr (cons 1 \"hello\"))" "\"hello\""

test write "(write \"hello\")" "\"hello\"()"

test begin "(begin (write \"hello\") 2)" "\"hello\"2"

test define "(begin (define x 2) x)" 2

test quote "(quote (foo 1 2))" "(foo . (1 . (2 . ())))"

test eq? "(eq? (quote x) (quote x))" "#t"

test eq? "(eq? (quote (f 1 2)) (quote (f 1 2)))" "#f"

test + "(+ 7 35)" 42

test = "(= (+ 7 35) (+ 6 36))" "#t"

test = "(= (+ 7 35) (+ 6 35))" "#f"

test if "(if (= 1 1) \"true\" \"false\")" "\"true\""

test if "(if (= 2 1) \"true\" \"false\")" "\"false\""

test set! "(begin (define x 42) (set! x 66) x)" 66

test lambda "(begin (define f (lambda (x y) (+ x y))) (f 20 22))" 42

test ">" "(> 2 1)" "#t"

test ">" "(> 1 2)" "#f"

test "<" "(< 2 1)" "#f"

test "<" "(< 1 2)" "#t"

test - "(- 77 35)" 42

test - "(- 10)" 10

test or "(or (> 2 3) (= 42 0) 5)" 5

test or "(or (> 2 3) (= 42 0))" "#f"

test or "(or)" "#f"

test and "(and (> 2 3) (+ 4 5))" "#f"

test and "(and (< 2 3) (+ 4 5))" 9

test and "(and)" "#t"

test list "(list)" "()"

test list "(list 1 (quote a) (+ 1 2))" "(1 . (a . (3 . ())))"

FIBOPROG="
(begin
    (define fibo
      (lambda (n)
        (if (or (= n 0) (= n 1))
            1
            (+ (fibo (- n 1))
               (fibo (- n 2))))))
    (fibo 9))
"

test fibo "$FIBOPROG" 55

MAPPROG="
(begin 
  (define map
    (lambda (f l)
      (if (eq? l (quote ()))
          l
          (cons (f (car l))
                (map f (cdr l))))))
  (map (lambda (x) (+ x 2)) (quote (1 2 3 4))))
"

test mymap "$MAPPROG" "(3 . (4 . (5 . (6 . ()))))"

COUNTERPROG="
(begin
  (define make-counter (lambda (init)
    (lambda ()
      (if (> init 0)
          (begin (set! init (- init 1)) init)
          init))))
  (define count (make-counter 10))
  (count)
  (count))
"

test counter "$COUNTERPROG" 8

FIBOITERPROG="
(begin
  (define fibo (lambda (n)
    (define iter (lambda (a b count)
      (if (= n count)
          b
          (iter (+ a b) a (+ count 1)))))
    (iter 1 1 0)))
  (fibo 9))
"

test fiboiter "$FIBOITERPROG" 55

test macro "(begin (define mymacro (macro (x) 1)) (mymacro 'whatever))" 1

MACROPROG="
(begin
  (define set2! 
    (macro (v1 v2 e)
      (list (quote begin) 
            (list (quote set!) v1 e) 
            (list (quote set!) v2 e))))
  (define v1 1)
  (define v2 2)
  (set2! v1 v2 3)
  (and (= v1 3) (= v2 3)))
"

test macro "$MACROPROG" "#t"

# program below used to cause a segfault
IFALTPROG="
(begin
  (define foo (lambda (n)
      (if (> n 0)
          (foo (- n 1)))))
  (foo 1))
"

test "if with no alternative" "$IFALTPROG" "#t"
