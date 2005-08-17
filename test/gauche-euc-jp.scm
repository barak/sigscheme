;; this test only works when the core system is compiled with euc-jp.

;; $Id: euc-jp.scm,v 1.6 2001/05/03 19:05:39 shirok Exp $

(load "./test/unittest-gauche.scm")

;;-------------------------------------------------------------------

(test "string" "����h�ˤۤ�t"
      (lambda () (string #\�� #\�� #\h #\�� #\�� #\�� #\t)))
(test "list->string" "����h�ˤۤ�t"
      (lambda () (list->string '(#\�� #\�� #\h #\�� #\�� #\�� #\t))))
(test "make-string" "�ؤؤؤؤ�" (lambda () (make-string 5 #\��)))
(test "make-string" "" (lambda () (make-string 0 #\��)))

(test "string->list" '(#\�� #\�� #\h #\�� #\�� #\�� #\t)
      (lambda () (string->list "����h�ˤۤ�t")))
(test "string->list" '(#\�� #\h #\�� #\�� #\�� #\t)
      (lambda () (string->list "����h�ˤۤ�t" 1)))
(test "string->list" '(#\�� #\h #\��)
      (lambda () (string->list "����h�ˤۤ�t" 1 4)))

(test "string-copy" '("����ͤ�" #f)
      (lambda () (let* ((x "����ͤ�") (y (string-copy x)))
                   (list y (eq? x y)))))
(test "string-copy" "��ͤ�" (lambda () (string-copy "����ͤ�" 1)))
(test "string-copy" "���"  (lambda () (string-copy "����ͤ�" 1 3)))

(test "string-ref" #\�� (lambda () (string-ref "�����" 1)))
(define x (string-copy "����Ϥˤ�"))
(test "string-set!" "����Z�ˤ�" (lambda () (string-set! x 2 #\Z) x))

(test "string-fill!" "�ΤΤΤΤΤ�"
      (lambda () (string-fill! (string-copy "000000") #\��)))
(test "string-fill!" "000�ΤΤ�"
      (lambda () (string-fill! (string-copy "000000") #\�� 3)))
(test "string-fill!" "000�Τ�0"
      (lambda () (string-fill! (string-copy "000000") #\�� 3 5)))

;(test "string-join" "�դ� �Ф� �Ф�"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�"))))
;(test "string-join" "�դ����Ф����Ф�"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�") "��")))
;(test "string-join" "�դ������Ф������Ф�"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�") "����" 'infix)))
;(test "string-join" ""
;      (lambda () (string-join '() "����")))
;(test "string-join" "�դ����Ф����Ф���"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�") "��" 'suffix)))
;(test "string-join" "���դ����Ф����Ф�"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�") "��" 'prefix)))
;(test "string-join" "�դ����Ф����Ф�"
;      (lambda () (string-join '("�դ�" "�Ф�" "�Ф�") "��" 'strict-infix)))

;(test "string-substitute!" "������defghi"
;      (lambda ()
;        (let ((s (string-copy "abcdefghi")))
;          (string-substitute! s 0 "������")
;          s)))
;(test "string-substitute!" "abc������ghi"
;      (lambda ()
;        (let ((s (string-copy "abcdefghi")))
;          (string-substitute! s 3 "������")
;          s)))

;;-------------------------------------------------------------------
;(test-section "string-pointer")
;(define sp #f)
;(test "make-string-pointer" #t
;      (lambda ()
;        (set! sp (make-string-pointer "����Ϥ�ho�ؤ�"))
;        (string-pointer? sp)))
;(test "string-pointer-next!" #\��
;      (lambda () (string-pointer-next! sp)))
;(test "string-pointer-next!" #\��
;      (lambda () (string-pointer-next! sp)))
;(test "string-pointer-prev!" #\��
;      (lambda () (string-pointer-prev! sp)))
;(test "string-pointer-prev!" #\��
;      (lambda () (string-pointer-prev! sp)))
;(test "string-pointer-prev!" #t
;      (lambda () (eof-object? (string-pointer-prev! sp))))
;(test "string-pointer-index" 0
;      (lambda () (string-pointer-index sp)))
;(test "string-pointer-index" 8
;      (lambda () (do ((x (string-pointer-next! sp) (string-pointer-next! sp)))
;                     ((eof-object? x) (string-pointer-index sp)))))
;(test "string-pointer-substring" '("����Ϥ�ho�ؤ�" "")
;      (lambda () (list (string-pointer-substring sp)
;                       (string-pointer-substring sp :after #t))))
;(test "string-pointer-substring" '("����Ϥ�h" "o�ؤ�")
;      (lambda ()
;        (string-pointer-set! sp 5)
;        (list (string-pointer-substring sp)
;              (string-pointer-substring sp :after #t))))
;(test "string-pointer-substring" '("" "����Ϥ�ho�ؤ�")
;      (lambda ()
;        (string-pointer-set! sp 0)
;        (list (string-pointer-substring sp)
;              (string-pointer-substring sp :after #t))))

;;-------------------------------------------------------------------
;(use srfi-13)

;(test "string-every" #t (lambda () (string-every #\�� "")))
;(test "string-every" #t (lambda () (string-every #\�� "��������")))
;(test "string-every" #f (lambda () (string-every #\�� "������a")))
;(test "string-every" #t (lambda () (string-every #[��-��] "��������")))
;(test "string-every" #f (lambda () (string-every #[��-��] "����a��")))
;(test "string-every" #t (lambda () (string-every #[��-��] "")))
;(test "string-every" #t (lambda () (string-every (lambda (x) (char-ci=? x #\��)) "��������")))
;(test "string-every" #f (lambda () (string-every (lambda (x) (char-ci=? x #\��)) "��������")))

;(test "string-any" #t (lambda () (string-any #\�� "��������")))
;(test "string-any" #f (lambda () (string-any #\�� "��������")))
;(test "string-any" #f (lambda () (string-any #\�� "")))
;(test "string-any" #t (lambda () (string-any #[��-��] "��������")))
;(test "string-any" #f (lambda () (string-any #[��-��] "��������")))
;(test "string-any" #f (lambda () (string-any #[��-��] "")))
;(test "string-any" #t (lambda () (string-any (lambda (x) (char-ci=? x #\��)) "���餢")))
;(test "string-any" #f (lambda () (string-any (lambda (x) (char-ci=? x #\��)) "���饢")))
;(test "string-tabulate" "����������"
;      (lambda ()
;        (string-tabulate (lambda (code)
;                           (integer->char (+ code
;                                             (char->integer #\��))))
;                         5)))
;(test "reverse-list->string" "����"
;      (lambda () (reverse-list->string '(#\�� #\�� #\��))))
;(test "string-copy!" "ab������fg"
;      (lambda () (let ((x (string-copy "abcdefg")))
;                   (string-copy! x 2 "������������" 2 5)
;                   x)))
;(test "string-take" "��������"  (lambda () (string-take "������������" 4)))
;(test "string-drop" "����"  (lambda () (string-drop "������������" 4)))
;(test "string-take-right" "��������"  (lambda () (string-take-right "������������" 4)))
;(test "string-drop-right" "����"  (lambda () (string-drop-right "������������" 4)))
;(test "string-pad" "�����ѥå�" (lambda () (string-pad "�ѥå�" 5 #\��)))
;(test "string-pad" "�ѥǥ���" (lambda () (string-pad "�ѥǥ���" 5 #\��)))
;(test "string-pad" "�ǥ��󥰥�" (lambda () (string-pad "�ѥǥ��󥰥�" 5 #\��)))
;(test "string-pad-right" "�ѥåɢ���" (lambda () (string-pad-right "�ѥå�" 5 #\��)))
;(test "string-pad" "�ѥǥ���" (lambda () (string-pad-right "�ѥǥ��󥰥�" 5 #\��)))

;;-------------------------------------------------------------------
;(use srfi-14)

;(test "char-set" #t
;      (lambda () (char-set= (char-set #\�� #\�� #\�� #\�� #\��)
;                            (string->char-set "����������"))))
;(test "char-set" #t
;      (lambda () (char-set= (list->char-set '(#\�� #\�� #\�� #\��))
;                            (string->char-set "��󤤤���������"))))
;(test "char-set" #t
;      (lambda () (char-set<= (list->char-set '(#\�� #\��))
;                             char-set:full)))
;(test "char-set" #t
;      (lambda ()
;        (char-set= (->char-set "������������������")
;                   (integer-range->char-set (char->integer #\��)
;                                            (char->integer #\��)))))

(total-report)
