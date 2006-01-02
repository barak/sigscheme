#! /usr/bin/env sscm -C EUC-JP
;; -*- buffer-file-coding-system: euc-jisx0213 -*-

;;  FileName : test-enc-eucjp.scm
;;  About    : unit test for EUC-JP string
;;
;;  Copyright (C) 2005 Kazuki Ohta <mover AT hct.zaq.ne.jp>
;;
;;  All rights reserved.
;;
;;  Redistribution and use in source and binary forms, with or without
;;  modification, are permitted provided that the following conditions
;;  are met:
;;
;;  1. Redistributions of source code must retain the above copyright
;;     notice, this list of conditions and the following disclaimer.
;;  2. Redistributions in binary form must reproduce the above copyright
;;     notice, this list of conditions and the following disclaimer in the
;;     documentation and/or other materials provided with the distribution.
;;  3. Neither the name of authors nor the names of its contributors
;;     may be used to endorse or promote products derived from this software
;;     without specific prior written permission.
;;
;;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
;;  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;;  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;;  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
;;  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
;;  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
;;  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
;;  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
;;  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
;;  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;;  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

(load "./test/unittest.scm")

(define tn test-name)

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

;; SRFI-75
(tn "SRFI-75")
(assert-parseable   (tn) "#\\x63")
(assert-parse-error (tn) "#\\u0063")
(assert-parse-error (tn) "#\\U00000063")

(assert-parseable   (tn) "\"\\x63\"")
(assert-parse-error (tn) "\"\\u0063\"")
(assert-parse-error (tn) "\"\\U00000063\"")

(assert-parseable   (tn) "'a")
(assert-parse-error (tn) "'��")

(total-report)
