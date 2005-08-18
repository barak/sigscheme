;;;
;;; primitive syntax test
;;;

(load "./test/unittest-gauche.scm")

;;----------------------------------------------------------------
;(test-section "contitionals")

(test "if" 5 (lambda ()  (if #f 2 5)))
(test "if" 2 (lambda ()  (if (not #f) 2 5)))

(test "and" #t (lambda ()  (and)))
(test "and" 5  (lambda ()  (and 5)))
(test "and" #f (lambda ()  (and 5 #f 2)))
(test "and" #f (lambda ()  (and 5 #f unbound-var)))
(test "and" 'a (lambda ()  (and 3 4 'a)))

(test "or"  #f (lambda ()  (or)))
(test "or"  3  (lambda ()  (or 3 9)))
(test "or"  3  (lambda ()  (or #f 3 unbound-var)))

;(test "when" 4          (lambda ()  (when 3 5 4)))
;(test "when" (test-undef)    (lambda ()  (when #f 5 4)))
;(test "unless" (test-undef)  (lambda ()  (unless 3 5 4)))
;(test "unless" 4        (lambda ()  (unless #f 5 4)))

;(test "cond" (test-undef)  (lambda ()  (cond (#f 2))))
(test "cond" 5        (lambda ()  (cond (#f 2) (else 5))))
(test "cond" 2        (lambda ()  (cond (1 2) (else 5))))
(test "cond" 8        (lambda ()  (cond (#f 2) (1 8) (else 5))))
(test "cond" 3        (lambda ()  (cond (1 => (lambda (x) (+ x 2))) (else 8))))

(test "case" #t (lambda ()  (case (+ 2 3) ((1 3 5 7 9) #t) ((0 2 4 6 8) #f))))

;;----------------------------------------------------------------
;(test-section "closure and saved env")

(test "lambda" 5  (lambda ()  ((lambda (x) (car x)) '(5 6 7))))
(test "lambda" 12
      (lambda ()
        ((lambda (x y)
           ((lambda (z) (* (car z) (cdr z))) (cons x y))) 3 4)))

(define (addN n) (lambda (a) (+ a n)))
(test "lambda" 5 (lambda ()  ((addN 2) 3)))
(define add3 (addN 3))
(test "lambda" 9 (lambda ()  (add3 6)))

(define count (let ((c 0)) (lambda () (set! c (+ c 1)) c)))
(test "lambda" 1 (lambda ()  (count)))
(test "lambda" 2 (lambda ()  (count)))

;;----------------------------------------------------------------
;(test-section "application")

;(test "apply" '(1 2 3) (lambda ()  (apply list 1 '(2 3))))
;(test "apply" '(1 2 3) (lambda ()  (apply apply (list list 1 2 '(3)))))

(test "map" '()         (lambda ()  (map car '())))
(test "map" '(1 2 3)    (lambda ()  (map car '((1) (2) (3)))))
(test "map" '(() () ()) (lambda ()  (map cdr '((1) (2) (3)))))
(test "map" '((1 . 4) (2 . 5) (3 . 6))  (lambda ()  (map cons '(1 2 3) '(4 5 6))))

;;----------------------------------------------------------------
;(test-section "loop")

(define (fact-non-tail-rec n)
  (if (<= n 1) n (* n (fact-non-tail-rec (- n 1)))))
(test "loop non-tail-rec" 120 (lambda ()  (fact-non-tail-rec 5)))

(define (fact-tail-rec n r)
  (if (<= n 1) r (fact-tail-rec (- n 1) (* n r))))
(test "loop tail-rec"     120 (lambda ()  (fact-tail-rec 5 1)))

(define (fact-named-let n)
  (let loop ((n n) (r 1)) (if (<= n 1) r (loop (- n 1) (* n r)))))
(test "loop named-let"    120 (lambda ()  (fact-named-let 5)))

(define (fact-int-define n)
  (define (rec n r) (if (<= n 1) r (rec (- n 1) (* n r))))
  (rec n 1))
(test "loop int-define"   120 (lambda ()  (fact-int-define 5)))

(define (fact-do n)
  (do ((n n (- n 1)) (r 1 (* n r))) ((<= n 1) r)))
(test "loop do"           120 (lambda ()  (fact-do 5)))

;;----------------------------------------------------------------
;(test-section "quasiquote")

(test "qq" '(1 2 3)        (lambda ()  `(1 2 3)))
(test "qq" '()             (lambda ()  `()))
(test "qq," '((1 . 2))     (lambda ()  `(,(cons 1 2))))
(test "qq," '((1 . 2) 3)   (lambda ()  `(,(cons 1 2) 3)))
(test "qq@" '(1 2 3 4)     (lambda ()  `(1 ,@(list 2 3) 4)))
(test "qq@" '(1 2 3 4)     (lambda ()  `(1 2 ,@(list 3 4))))
(test "qq." '(1 2 3 4)     (lambda ()  `(1 2 . ,(list 3 4))))
(test "qq#," '#((1 . 2) 3) (lambda ()  `#(,(cons 1 2) 3)))
(test "qq#@" '#(1 2 3 4)   (lambda ()  `#(1 ,@(list 2 3) 4)))
(test "qq#@" '#(1 2 3 4)   (lambda ()  `#(1 2 ,@(list 3 4))))
(test "qq#" '#()           (lambda ()  `#()))
(test "qq#@" '#()          (lambda ()  `#(,@(list))))

(test "qq@@" '(1 2 1 2)    (lambda ()  `(,@(list 1 2) ,@(list 1 2))))
(test "qq@@" '(1 2 a 1 2)  (lambda ()  `(,@(list 1 2) a ,@(list 1 2))))
(test "qq@@" '(a 1 2 1 2)  (lambda ()  `(a ,@(list 1 2) ,@(list 1 2))))
(test "qq@@" '(1 2 1 2 a)  (lambda ()  `(,@(list 1 2) ,@(list 1 2) a)))
(test "qq@@" '(1 2 1 2 a b) (lambda ()  `(,@(list 1 2) ,@(list 1 2) a b)))
(test "qq@." '(1 2 1 2 . a)
      (lambda ()  `(,@(list 1 2) ,@(list 1 2) . a)))
(test "qq@." '(1 2 1 2 1 . 2)
      (lambda ()  `(,@(list 1 2) ,@(list 1 2) . ,(cons 1 2))))
(test "qq@." '(1 2 1 2 a 1 . 2)
      (lambda ()  `(,@(list 1 2) ,@(list 1 2) a . ,(cons 1 2))))

(test "qq#@@" '#(1 2 1 2)    (lambda ()  `#(,@(list 1 2) ,@(list 1 2))))
(test "qq#@@" '#(1 2 a 1 2)  (lambda ()  `#(,@(list 1 2) a ,@(list 1 2))))
(test "qq#@@" '#(a 1 2 1 2)  (lambda ()  `#(a ,@(list 1 2) ,@(list 1 2))))
(test "qq#@@" '#(1 2 1 2 a)  (lambda ()  `#(,@(list 1 2) ,@(list 1 2) a)))
(test "qq#@@" '#(1 2 1 2 a b) (lambda () `#(,@(list 1 2) ,@(list 1 2) a b)))

(test "qqq"   '(1 `(1 ,2 ,3) 1)  (lambda ()  `(1 `(1 ,2 ,,(+ 1 2)) 1)))
(test "qqq"   '(1 `(1 ,@2 ,@(1 2))) (lambda () `(1 `(1 ,@2 ,@,(list 1 2)))))
(test "qqq#"  '#(1 `(1 ,2 ,3) 1)  (lambda ()  `#(1 `(1 ,2 ,,(+ 1 2)) 1)))
(test "qqq#"  '#(1 `(1 ,@2 ,@(1 2))) (lambda () `#(1 `(1 ,@2 ,@,(list 1 2)))))

;;----------------------------------------------------------------
;(test-section "multiple values")
;(test "receive" '(1 2 3)
;      (lambda ()  (receive (a b c) (values 1 2 3) (list a b c))))
;(test "receive" '(1 2 3)
;      (lambda ()  (receive (a . r) (values 1 2 3) (cons a r))))
;(test "receive" '(1 2 3)
;      (lambda ()  (receive x (values 1 2 3) x)))
;(test "receive" 1
;      (lambda ()  (receive (a) 1 a)))
(test "call-with-values" '(1 2 3)
      (lambda ()  (call-with-values (lambda () (values 1 2 3)) list)))
(test "call-with-values" '()
      (lambda ()  (call-with-values (lambda () (values)) list)))

(total-report)

