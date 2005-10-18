;; -*- euc-jp -*-
(load "./test/unittest.scm")

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


;; The character after �� is from JIS X 0212.  The one after �� is
;; from JIS X 0213 plane 2.  This violates all known standards, but
;; souldn't be a real problem.
(define str1 "���˥�ah˽\\˽n!���������!")
(define str1-list '(#\�� #\�� #\�� #\a #\h #\˽ #\\ #\˽ #\n #\! #\�� #\��� #\�� #\��� #\!))

(assert-equal? "string 2" str1 (apply string str1-list))
(assert-equal? "list->string 2" str1-list (string->list str1))
