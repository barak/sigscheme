;; -*- euc-jp -*-
(load "./test/unittest.scm")

;; string?
(assert-true "string? check" (string? "����������"))

;; make-string
(assert-true "alphabet make-string check" (string=? "aaa" (make-string 3 #\a)))
(assert-true "hiragana make-string check" (string=? "������" (make-string 3 #\��)))
(assert-equal? "make-string 1" "����������"   (make-string 5 #\��))

;; string-ref
(assert-equal? "hiragana string-ref check" #\�� (string-ref "����������" 4))
(assert-equal? "mixed string-ref check"    #\�� (string-ref "��iue��" 4))
(assert-equal? "alphabet string-ref 0 check" #\a  (string-ref "aiueo" 0))
(assert-equal? "hiragena string-ref 0 check" #\�� (string-ref "����������" 0))
(assert-equal? "string-ref 1" #\��  (string-ref "��hi�����" 3))


;; string-set!
(assert-true"hiragana string-set! check" (string=? "����������"
						   (begin
						     (define str (string-copy "����������"))
						     (string-set! str 2 #\��)
						     str)))
(assert-true"mixed string-set! check" (string=? "aiueo"
						(begin
						  (define str (string-copy "ai��eo"))
						  (string-set! str 2 #\u)
						  str)))
(assert-equal? "string-set! 1"     "��˶�"   (string-set!
                                               (string-copy "��ˤ�")
                                               2
                                               #\��))


;; string-length
(assert-equal? "hiragana string-length check" 5 (string-length "����������"))

;; string=?
(assert-equal? "hiragana string=? check" #t (string=? "����������" "����������"))
(assert-equal? "mixed string=? check"    #t (string=? "a������o" "a������o"))

;; substring
(assert-true"hiragana substring check" (string=? "����" (substring (string-copy "����������") 1 3)))
(assert-true"mixed substring check"    (string=? "��u"  (substring (string-copy "a��u��o") 1 3)))

;; string-append
(assert-true "hiragana 1 string-append check" (string=? "��"     (string-append "��")))
(assert-true "hiragana 2 string-append check" (string=? "����"   (string-append "��" "��")))
(assert-true "hiragana 3 string-append check" (string=? "������" (string-append "��" "��" "��")))
(assert-true "mixed 2 string-append check" (string=? "��i"   (string-append "��" "i")))
(assert-true "mixed 3 string-append check" (string=? "��i��" (string-append "��" "i" "��")))


;; string->list
(assert-true "string->list check" (equal? '(#\�� #\i #\�� #\e #\��) (string->list "��i��e��")))
(assert-equal? "string->list 1" '(#\�� #\�� #\��) (string->list "������"))

;; list->string
(assert-equal? "list->string check" "��a��" (list->string '(#\�� #\a #\��)))
(assert-equal? "list->string 1" "3����" (list->string '(#\3 #\�� #\��)))

;; string-fill!
(assert-true"hiragana string-fill! check" (string=? "����������" (begin
								   (define str (string-copy "aiueo"))
								   (string-fill! str #\��)
								   str)))
(assert-true"mixed string-fill! by alphabet check" (string=? "aaaaa" (begin
								       (define str (string-copy "a������o"))
								       (string-fill! str #\a)
								       str)))
(assert-true"mixed string-fill! by hiragana check" (string=? "����������" (begin
									    (define str (string-copy "a������o"))
									    (string-fill! str #\��)
									    str)))

;; string
(assert-equal? "string 1" "���ͤˤ�" (string #\�� #\�� #\�� #\��))

;; string-copy
(assert-equal? "string-copy 1"     "����"   (string-copy "����"))


;; The character after �� is from JIS X 0212.  The one after �� is
;; from JIS X 0213 plane 2.  This violates all known standards, but
;; souldn't be a real problem.
(define str1 "���˥�ah˽\\˽n!���������!")
(define str1-list '(#\�� #\�� #\�� #\a #\h #\˽ #\\ #\˽ #\n #\! #\�� #\��� #\�� #\��� #\!))

(assert-equal? "string 2" str1 (apply string str1-list))
(assert-equal? "list->string 2" str1-list (string->list str1))
