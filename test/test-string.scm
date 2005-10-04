(load "./test/unittest.scm")

;; check string?
(assert-true"string? check" (string? "aiueo"))

;; check make-string
(assert-true"null make-string" (string? (make-string 6)))
(assert-true"alphabet make-string check" (string=? "aaa" (make-string 3 #\a)))
(assert-true"hiragana make-string check" (string=? "������" (make-string 3 #\��)))

;; check string-ref
(assert-equal? "alphabet string-ref check" #\o  (string-ref "aiueo" 4))
(assert-equal? "hiragena string-ref check" #\�� (string-ref "����������" 4))
(assert-equal? "mixed string-ref check"    #\�� (string-ref "��iue��" 4))
(assert-equal? "alphabet string-ref 0 check" #\a  (string-ref "aiueo" 0))
(assert-equal? "hiragena string-ref 0 check" #\�� (string-ref "����������" 0))

;; check string-set!
(assert-true"alphabet string-set! check" (string=? "aikeo"
					       (begin
						 (define str "aiueo")
						 (string-set! str 2 #\k)
						 str)))
(assert-true"hiragana string-set! check" (string=? "����������"
					       (begin
						 (define str "����������")
						 (string-set! str 2 #\��)
						 str)))
(assert-true"mixed string-set! check" (string=? "aiueo"
					    (begin
					      (define str "ai��eo")
					      (string-set! str 2 #\u)
					      str)))

;; check string-length
(assert-equal? "alphabet string-length check" 5 (string-length "aiueo"))
(assert-equal? "hiragana string-length check" 5 (string-length "����������"))
(assert-equal? "backslash string-length check" 1 (string-length "\\"))
(assert-equal? "backslash string-length check" 2 (string-length "\\\\"))
(assert-equal? "backslash string-length check" 3 (string-length "\\\\\\"))

;; string=? check
(assert-equal? "alphabet string=? check" #t (string=? "aiueo" "aiueo"))
(assert-equal? "hiragana string=? check" #t (string=? "����������" "����������"))
(assert-equal? "mixed string=? check"    #t (string=? "a������o" "a������o"))

;; substring check
(assert-true"alphabet substring check" (string=? "iu"   (substring "aiueo" 1 3)))
(assert-true"hiragana substring check" (string=? "����" (substring "����������" 1 3)))
(assert-true"mixed substring check"    (string=? "��u"  (substring "a��u��o" 1 3)))


;; string-append check
(assert-true"alphabet 1 string-append check" (string=? "a"   (string-append "a")))
(assert-true"alphabet 2 string-append check" (string=? "ai"  (string-append "a" "i")))
(assert-true"alphabet 3 string-append check" (string=? "aiu" (string-append "a" "i" "u")))
(assert-true"hiragana 1 string-append check" (string=? "��"     (string-append "��")))
(assert-true"hiragana 2 string-append check" (string=? "����"   (string-append "��" "��")))
(assert-true"hiragana 3 string-append check" (string=? "������" (string-append "��" "��" "��")))
(assert-true"mixed 2 string-append check" (string=? "��i"   (string-append "��" "i")))
(assert-true"mixed 3 string-append check" (string=? "��i��" (string-append "��" "i" "��")))

;; string->list
(assert-true"string->list check" (equal? '(#\�� #\i #\�� #\e #\��) (string->list "��i��e��")))
(assert-equal? "string->list check" '(#\\)             (string->list "\\"))
(assert-equal? "string->list check" '(#\\ #\\)         (string->list "\\\\"))
(assert-equal? "string->list check" '(#\\ #\\ #\\)     (string->list "\\\\\\"))
;;(assert-equal? "string->list check" '(#\tab)           (string->list "\t"))
(assert-equal? "string->list check" '(#\	)      (string->list "\t"))
;;(assert-equal? "string->list check" '(#\return)        (string->list "\r"))
(assert-equal? "string->list check" '(#\)            (string->list "\r"))
(assert-equal? "string->list check" '(#\ #\)       (string->list "\r\r"))
(assert-equal? "string->list check" '(#\newline)           (string->list "\n"))
(assert-equal? "string->list check" '(#\newline #\newline) (string->list "\n\n"))
(assert-equal? "string->list check" '(#\space)         (string->list " "))
(assert-equal? "string->list check" '(#\space #\space) (string->list "  "))
(assert-equal? "string->list check" '(#\")             (string->list "\""))
(assert-equal? "string->list check" '(#\" #\")         (string->list "\"\""))

;; list->string
(assert-equal? "list->string check" "��a��" (list->string '(#\�� #\a #\��)))
(assert-equal? "list->string check" "\\"     (list->string '(#\\)))
(assert-equal? "list->string check" "\\\\"   (list->string '(#\\ #\\)))
(assert-equal? "list->string check" "\\\\\\" (list->string '(#\\ #\\ #\\)))
(assert-equal? "list->string check" "\t" (list->string '(#\	)))
;;(assert-equal? "list->string check" "\t" (list->string '(#\tab)))
(assert-equal? "list->string check" "\r" (list->string '(#\)))
;;(assert-equal? "list->string check" "\r" (list->string '(#\return)))
(assert-equal? "list->string check" "\n" (list->string '(#\
)))
(assert-equal? "list->string check" "\n" (list->string '(#\newline)))
(assert-equal? "list->string check" " " (list->string '(#\ )))
(assert-equal? "list->string check" " " (list->string '(#\space)))
(assert-equal? "list->string check" " " (list->string '(#\ )))
(assert-equal? "list->string check" "\"" (list->string '(#\")))
(assert-equal? "list->string check" "\"a\"" (list->string '(#\" #\a #\")))

;; string-fill!
(assert-true"alphabet string-fill! check" (string=? "jjjjj" (begin
							  (define str "aiueo")
							  (string-fill! str #\j)
							  str)))
(assert-true"hiragana string-fill! check" (string=? "����������" (begin
							       (define str "aiueo")
							       (string-fill! str #\��)
							       str)))
(assert-true"mixed string-fill! by alphabet check" (string=? "aaaaa" (begin
								   (define str "a������o")
								   (string-fill! str #\a)
								   str)))
(assert-true"mixed string-fill! by hiragana check" (string=? "����������" (begin
									(define str "a������o")
									(string-fill! str #\��)
									str)))

(total-report)
