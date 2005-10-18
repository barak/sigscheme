(load "./test/unittest.scm")

;; This file provides a fallback test unit for all EUC systems.  It's
;; just a copy of test-enc-eucjp.scm with EUCJP-specific character
;; sequences removed, so some characters may be undefined in other EUC
;; systems.
(assert-equal? "string 1" "���ͤˤ�" (string #\�� #\�� #\�� #\��))
(assert-equal? "list->string 1" "3����" (list->string '(#\3 #\�� #\��)))
(assert-equal? "string->list 1" '(#\�� #\�� #\��) (string->list "������"))

(assert-equal? "string-ref 1" #\��  (string-ref "��hi�����" 3))
(assert-equal? "make-string 1" "����������"   (make-string 5 #\��))
(assert-equal? "string-copy 1"     "����"   (string-copy "����"))
(assert-equal? "string-set! 1"     "��˶�"   (string-set!
                                               (string-copy "��ˤ�")
                                               2
                                               #\��))

(define str1 "����ah˽\\˽n!����!")
(define str1-list '(#\�� #\�� #\a #\h #\˽ #\\ #\˽ #\n #\! #\�� #\�� #\!))

(assert-equal? "string 2" str1 (apply string str1-list))
(assert-equal? "list->string 2" str1-list (string->list str1))
