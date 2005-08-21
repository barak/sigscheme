(load "./test/unittest.scm")

;; check string?
(assert "string? check" (string? "aiueo"))

;; check make-string
(assert "null make-string" (string? (make-string 6)))
(assert "alphabet make-string check" (string=? "aaa" (make-string 3 #\a)))
(assert "hiragana make-string check" (string=? "������" (make-string 3 #\��)))

;; check string-ref
(assert-equal? "alphabet string-ref check" #\o  (string-ref "aiueo" 4))
(assert-equal? "hiragena string-ref check" #\�� (string-ref "����������" 4))
(assert-equal? "mixed string-ref check"    #\�� (string-ref "��iue��" 4))
(assert-equal? "alphabet string-ref 0 check" #\a  (string-ref "aiueo" 0))
(assert-equal? "hiragena string-ref 0 check" #\�� (string-ref "����������" 0))

;; check string-set!
(assert "alphabet string-set! check" (string=? "aikeo"
					       (begin
						 (define str "aiueo")
						 (string-set! str 2 #\k)
						 str)))
(assert "hiragana string-set! check" (string=? "����������"
					       (begin
						 (define str "����������")
						 (string-set! str 2 #\��)
						 str)))
(assert "mixed string-set! check" (string=? "aiueo"
					    (begin
					      (define str "ai��eo")
					      (string-set! str 2 #\u)
					      str)))

;; check string-length
(assert-equal? "alphabet string-length check" 5 (string-length "aiueo"))
(assert-equal? "hiragana string-length check" 5 (string-length "����������"))

;; string=? check
(assert-equal? "alphabet string=? check" #t (string=? "aiueo" "aiueo"))
(assert-equal? "hiragana string=? check" #t (string=? "����������" "����������"))
(assert-equal? "mixed string=? check"    #t (string=? "a������o" "a������o"))


;; substring check
(assert "alphabet substring check" (string=? "iu"   (substring "aiueo" 1 3)))
(assert "hiragana substring check" (string=? "����" (substring "����������" 1 3)))
(assert "mixed substring check"    (string=? "��u"  (substring "a��u��o" 1 3)))


;; string-append check
(assert "alphabet 1 string-append check" (string=? "a"   (string-append "a")))
(assert "alphabet 2 string-append check" (string=? "ai"  (string-append "a" "i")))
(assert "alphabet 3 string-append check" (string=? "aiu" (string-append "a" "i" "u")))
(assert "hiragana 1 string-append check" (string=? "��"     (string-append "��")))
(assert "hiragana 2 string-append check" (string=? "����"   (string-append "��" "��")))
(assert "hiragana 3 string-append check" (string=? "������" (string-append "��" "��" "��")))
(assert "mixed 2 string-append check" (string=? "��i"   (string-append "��" "i")))
(assert "mixed 3 string-append check" (string=? "��i��" (string-append "��" "i" "��")))

;; string->list
(assert "string->list check" (equal? '(#\�� #\i #\�� #\e #\��) (string->list "��i��e��")))


;; list->string
(assert "list->string check" (string=? "��a��" (list->string '(#\�� #\a #\��))))


;; string-fill!
(assert "alphabet string-fill! check" (string=? "jjjjj" (begin
							  (define str "aiueo")
							  (string-fill! str #\j)
							  str)))
(assert "hiragana string-fill! check" (string=? "����������" (begin
							       (define str "aiueo")
							       (string-fill! str #\��)
							       str)))
(assert "mixed string-fill! by alphabet check" (string=? "aaaaa" (begin
								   (define str "a������o")
								   (string-fill! str #\a)
								   str)))
(assert "mixed string-fill! by hiragana check" (string=? "����������" (begin
									(define str "a������o")
									(string-fill! str #\��)
									str)))

(total-report)
