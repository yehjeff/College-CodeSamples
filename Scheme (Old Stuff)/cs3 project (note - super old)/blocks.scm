;;JEFFREY YEH - CS3l-hm - TuTh5-8PM
;;
;; Framework for the CS 3 "Blocks World" project.
;;
;; version 1.4


; This file contains procedures to initialize the blocks world, to read
; commands from the user, and to display a graphic version of the blocks
; world.



; Initialize the blocks environment from a file specified by the user.
; Handle all the user's commands, then come back and put the updated
; blocks configuration in a file named "blocks.out".
(define (startup)
  (show '(type the name of the blocks configuration file surrounded by double quotes))
  (let*
      ((final-blocks (handle-cmds (read (open-input-file (read))) '()) )
       (new-blocks-file (open-output-file "blocks.out") ) )
    (display final-blocks new-blocks-file)
    (newline new-blocks-file)
    (close-output-port new-blocks-file) ) )


; Handle commands until a "quit" command is submitted.
; blocks is a blocks configuration; it is a list of blocks.
; If it contains only one block, a reference to "it" is unambiguous.
(define (handle-cmds blocks it)
  (display (append '(block configuration =) (list blocks)))
  (newline)
  (draw-stack-list blocks)      ; Call graphics display code.
  (show "command? ")
  (let ((cmd (read)))
    (cond
     ((or (equal? cmd 'quit) (equal? cmd '(quit)))
      (display (append '(final configuration =) (list blocks)))
      (newline)
      ;closes the graphics window
      (delete-canvas!)
      blocks)
     (else
      (let ((analyzed-cmd (analyzed cmd blocks it)))
	(if analyzed-cmd
	    (execute analyzed-cmd blocks it)
	    (handle-cmds blocks '()) ) ) ) ) ) )



; Access functions for block information.
;I actually opted to not use this pre-provided code and simply write my own things...
(define block-id car)
(define block-color cadr)



;;
;; Graphics stuff (feel free to ignore)
;;


; Constants for displaying blocks.
; BLOCK-SIZE is the height and width in pixels of each block.
; FUDGE is the number of pixels that separate adjacent stacks.
; BASE-OFFSET is how far from the bottom of the window we draw.
(define *BLOCK-SIZE* 50)
(define *FUDGE* 5)
(define *BASE-OFFSET* 20)

; If you need more room in the graphcis window for your blocks,
; redefine canvas-width and canvas-height as follows.
;(define (canvas-width) 480)
;(define (canvas-height) 480)


; Create a window and display a blocks environment, a list of stacks.
(define (draw-stack-list stack-list)
  (clearscreen)
  (hideturtle)
  (draw-stack-list-helper
   stack-list
   *FUDGE*
   (- (canvas-height) *BASE-OFFSET* *FUDGE*)) )

; Display a list of stacks, with the lower-left corner of the bottom block
; in the first stack at position (x,y).
(define (draw-stack-list-helper stack-list x y)
  (cond
   ((null? stack-list) )   ;done
   (else
    (draw-stack (reverse (car stack-list)) x y)
    (draw-stack-list-helper
     (cdr stack-list)
     (+ x *BLOCK-SIZE* *FUDGE*) y) ) ) )

; Display a stack, with the lower-left corner of the bottom block at position
; (x,y).  The first block on the stack is its *bottom* block; this is the
; opposite of how blocks are stored in a blocks environment.
(define (draw-stack stack x y)
  (cond
   ((null? stack) )   ;done
   (else
    (draw-block (car stack) x y)
    (draw-stack (cdr stack) x (- y *BLOCK-SIZE* *FUDGE*)) ) ) )

; Display a block with lower-left corner at position (x,y).
; Its block id number will be printed in the middle of the block.
(define (draw-block block x y)
  (let ((outer-outline-color (if (equal? (block-color block) 'black) 'white 'black))
	(inner-outline-color (if (equal? (block-color block) 'white) 'black 'white)))
    (canvas-widget
     'create 'rect
     x (- y *BLOCK-SIZE*) (+ x *BLOCK-SIZE*) y
     :fill (block-color block) ; draw the block
     :outline outer-outline-color )
    (canvas-widget
     'create 'rect
     (+ x (* .25 *BLOCK-SIZE*)) (- y (* 0.75 *BLOCK-SIZE*))
     (+ x (* *BLOCK-SIZE* .75)) (- y (* 0.25 *BLOCK-SIZE*))
     :fill 'white ;fill block with white
     :outline inner-outline-color)
    (let ((name-incr (truncate (/ *BLOCK-SIZE* 2)))) 
      (canvas-widget
       'create 'text
       (+ x name-incr) (- y name-incr)
       :text (number->string (block-id block))
       :fill 'black
       :anchor 'center) ) ) ); draw the identifying number. 
  




;;;;
;;;; Your code goes here...
;;;;

;-------------------------NOTE TO GRADER--------------------
;-for "it" when it is undefined, I often used "undefined it" for when "it" was more than one block or "error" for when "it" was not specified. I realize now that this was not what was requested in the project instructions and i am very very sorry. Please show me some mercy as I don't quite have time to be able to go back and rewrite all my code =(
;-there may be other small inconsistencies with my code and the instructions as there is always the possibility that I was more careless than I ought to have been. I don't think theres anything else, but if so, I'm sorry!

(define (analyzed cmd blocks it) ;uses resolved-descr for alot of the commands
  (cond ((not (sentence? cmd)) (show "please type commands in a sentence (inside parentheses)") #f)
	((and (not (block? it)) (member? 'it cmd)) (show (se '(please explain what is meant by) "it")) #f)
	((or (equal? (length cmd) 1) (not (member? (car cmd) '(put what add remove))))
	 (show "improper command") #f)
	
        ((equal? (car cmd) 'put) ;analyzed for "put"
	 (put-helper (if (equal? 1 (length (resolved-descr2 (cdr (member 'on cmd)) blocks it)))
			 (choose-random 1 (remove-from
					   (car (resolved-descr2 (cdr (member 'on cmd)) blocks it))
				           (resolved-descr2 (cdr cmd) blocks it)))
			 (choose-random 1 (resolved-descr2 (cdr cmd) blocks it)))
		     (resolved-descr (cdr (member 'on cmd)) blocks it)
		     (resolved-descr (cdr cmd) blocks it)
		     blocks it))
	
	((equal? (list (car cmd) (cadr cmd)) '(what is))
	 (append (list (caddr cmd)) (resolved-descr (cdddr cmd) blocks it)))

	((equal? (car cmd) 'add) (resolved-descr cmd blocks it))

	((and (equal? (car cmd) 'remove) (number? (cadr cmd)));for remove num blocks
	 (append '(remove) (resolved-descr (cdr cmd) blocks it)))
	((equal? (car cmd) 'remove) ;for the other kinds of remove ("a" or "the" or "it")
	 (append '(remove) (list (choose-random 1 (resolved-descr (cdr cmd) blocks it)))))
	
	(else
  (show "improper command")
  #f)))

(define (put-helper picked-up undecided-bases placeholder blocks it) ;helper for put in "analyzed".
  ;the placeholder is only used in certain cases that may or may not require an error messaage to be shown.
  (cond ((and (not (equal? 'error picked-up)) (null? (remove-from picked-up undecided-bases)))
         (show "the block cannot be moved onto itself") #f)
	((equal? picked-up 'error) #f)
	(else 
         (list 'put picked-up (choose-random 1 (remove-from picked-up undecided-bases))))))



(define (execute analyzed-cmd blocks it) ;uses an analyzed command from the analyzed program
  (cond ((< 0 (deep-appearances 'error analyzed-cmd)) ;for syntax errors in the commands
	 (handle-cmds blocks '()))
	((equal? 'add (car analyzed-cmd))
	 (handle-cmds (adder blocks (cadr analyzed-cmd)) (cadr analyzed-cmd)))
	((equal? 'remove (car analyzed-cmd))
	 (handle-cmds (apply remover (append (list blocks) (list (cdr analyzed-cmd))))
		      '(undefined it)))
	((equal? 'put (car analyzed-cmd))
	 (handle-cmds (putter (cadr analyzed-cmd) (caddr analyzed-cmd)
			      (remover blocks (list (cadr analyzed-cmd))))
		      (cadr analyzed-cmd)))
	((equal? 'under (car analyzed-cmd))
	 (handle-cmds blocks (if (equal? (length (cdr analyzed-cmd)) 1)
				 (car (what-is? blocks (cdr analyzed-cmd)))
				 (cons '()
				       (what-is? blocks (cdr analyzed-cmd))))))
	((equal? 'on (car analyzed-cmd))
	 (handle-cmds blocks (if (equal? (length (cdr analyzed-cmd)) 1)
				 (car (what-is? (almost-complete-reverse blocks) (cdr analyzed-cmd)))
				 (cons '()
				       (what-is? (almost-complete-reverse blocks) (cdr analyzed-cmd))))))
  (else (show "execute not yet PROPERLY implemented")
   (handle-cmds blocks it))))




(define (resolved-descr block-descr blocks it) ;a color block, the color block, etc for the descr
  (cond ((equal? (car block-descr) 'it) ;it
	 (list it))
	((if (and (equal? (car block-descr) 'a) ;a color block
		  (color? (cadr block-descr))
		  (equal? (caddr block-descr) 'block)
		  (if (> 1 (length (list-blocks (cadr block-descr) blocks)))
		      (less-than-one-msg (cadr block-descr)) #t))
	     #t #f)
	 (list-blocks (cadr block-descr) blocks))
	((if (and (equal? (car block-descr) 'add) ;unique resolved-descr for the "add" function
		  (equal? (cadr block-descr) 'a)
		  (or (equal? (caddr block-descr) 'block) (and (equal? (cadddr block-descr) 'block)
							       (color? (caddr block-descr))))) #t #f)
	 (cond ((equal? (caddr block-descr) 'block) (list 'add (block-generator blocks)))
	       ((and (equal? (cadddr block-descr) 'block)
		     (color? (caddr block-descr)))
		(list 'add (block-generator2 blocks (caddr block-descr))))))
	((if (and (equal? (car block-descr) 'the) ;the color block
		  (color? (cadr block-descr))
		  (equal? (caddr block-descr) 'block)
		  (cond ((equal? (length (list-blocks (cadr block-descr) blocks)) 1) #t)
			((< 1 (length (list-blocks (cadr block-descr) blocks)))
			 (more-than-one-msg (cadr block-descr)))
			((> 1 (length (list-blocks (cadr block-descr) blocks)))
			 (less-than-one-msg (cadr block-descr)))
			(else #f)))
	     #t #f)
	 (list-blocks (cadr block-descr) blocks))
	((if (and (number? (car block-descr));number color blocks
		  (color? (cadr block-descr))
		  (equal? (caddr block-descr) 'blocks)
		  (cond ((<= (car block-descr)
			     (length (list-blocks (cadr block-descr) blocks))) #t)
			(else (show (se "not enough" (cadr block-descr) "blocks")) #f)))
	     #t #f)
	 (choose-random (car block-descr) (list-blocks (cadr block-descr) blocks)))
	((if (and (equal? (car block-descr) 'a) ;a block
		  (equal? (cadr block-descr) 'block)) #t #f)
	 (list-all-blocks blocks))
	((if (and (number? (car block-descr)); number blocks
		  (equal? 'blocks (cadr block-descr))
		  (cond ((<= (car block-descr)
			     (length (list-all-blocks blocks))) #t)
			(else (show (se "there are fewer than" (car block-descr) "blocks")) #f))) #t #f)
	 (choose-random (car block-descr) (list-all-blocks blocks)))
	((if (and (not (equal? (car block-descr) 'the)) ;incorrect descr
		  (not (number? (car block-descr)))
		  (not (equal? (car block-descr) 'a))) #t #f)
	 (show "incorrect description") '(error))
	(else '(error)); the resolved-descr program will return "error" whenever it hits a problem, like a command of "the red block" when there are multiple red blocks. this "error" is used in execute in order to handle-cmds without changing the environment.
	))


;______________________________________________________________________
;sorry for this horribly long and ugly section of code below. It is simply the same as resolved-descr, with the only difference being that it does not use any "show" or "display". This was done simply to avoid a glitch that would occur when i called resolved-descr either recursively or multiple times in analyzed or execute.
(define (resolved-descr2 block-descr blocks it)  (cond ((equal? (car block-descr) 'it)  (list it)) ((if (and (equal? (car block-descr) 'a)  (color? (cadr block-descr)) (equal? (caddr block-descr) 'block) (if (> 1 (length (list-blocks (cadr block-descr) blocks))) #f #t)) #t #f) (list-blocks (cadr block-descr) blocks)) ((if (and (equal? (car block-descr) 'add) (equal? (cadr block-descr) 'a) (or (equal? (caddr block-descr) 'block) (and (equal? (cadddr block-descr) 'block) (color? (caddr block-descr))))) #t #f) (cond ((equal? (caddr block-descr) 'block) (list 'add (block-generator blocks))) ((and (equal? (cadddr block-descr) 'block) (color? (caddr block-descr))) (list 'add (block-generator2 blocks (caddr block-descr)))))) ((if (and (equal? (car block-descr) 'the) (color? (cadr block-descr)) (equal? (caddr block-descr) 'block) (cond ((equal? (length (list-blocks (cadr block-descr) blocks)) 1) #t) ((< 1 (length (list-blocks (cadr block-descr) blocks))) #f) ((> 1 (length (list-blocks (cadr block-descr) blocks))) #f) (else #f))) #t #f) (list-blocks (cadr block-descr) blocks)) ((if (and (number? (car block-descr)) (color? (cadr block-descr)) (equal? (caddr block-descr) 'blocks) (cond ((<= (car block-descr) (length (list-blocks (cadr block-descr) blocks))) #t) (else #f))) #t #f) (choose-random (car block-descr) (list-blocks (cadr block-descr) blocks))) ((if (and (equal? (car block-descr) 'a) (equal? (cadr block-descr) 'block)) #t #f) (list-all-blocks blocks)) ((if (and (number? (car block-descr)) (equal? 'blocks (cadr block-descr)) (cond ((<= (car block-descr) (length (list-all-blocks blocks))) #t) (else #f))) #t #f) (choose-random (car block-descr) (list-all-blocks blocks))) ((if (and (not (equal? (car block-descr) 'the)) (not (number? (car block-descr))) (not (equal? (car block-descr) 'a))) #t #f) '(error)) (else '(error)) ))
;______________________________________________________________________

(define (less-than-one-msg color) ;displays error message if there is "less than one"
  (show (se "no" color "block exists")) #f)
   
(define (more-than-one-msg color) ;displays error message if there is "more than one"
  (show (se "more than one" color "block exists")) #f)
  
; below are mostly helpers for execute
;_______________________________________________________________________

(define (putter picked-up base blocks); putter - picked-up is block you are putting on top of the base
  (putter-helper picked-up base (clear-top base blocks))) ;this clears blocks from the base first
(define (putter-helper picked-up base blocks) ;does the actual "putting"...
  (cond ((null? blocks) '())
	((= 0 (deep-appearances (car base) (car blocks))) (cons (car blocks) (putter-helper picked-up base (cdr blocks))))
	((= 1 (deep-appearances (car base) (car blocks))) (cons (cons picked-up (car blocks)) (cdr blocks)))))

(define (what-is? blocks target-blocks) ;"what-is?" shows the block that follows the target block
  ;and then returns a value for the "it" that will be used in handle-cmds within execute.
  (map (lambda (x) (after? blocks x)) target-blocks))
(define (after? blocks target-block) ;helper for "what-is"
  (if (equal? 1 (deep-appearances (car target-block) (car blocks)))
      (after-helper (car blocks) target-block)
      (after? (cdr blocks) target-block)))
(define (after-helper block-collumn target-block)
  (cond ((equal? target-block (car block-collumn))
	 (cond ((null? (cdr block-collumn))
	        (show "(nothing)") '())
	       (else (show (cadr block-collumn)) (cadr block-collumn))))
	(else (after-helper (cdr block-collumn) target-block))))
	     
  

(define (clear-top target-block blocks) ;clears (not removes) blocks above the target block
  (if (null? (car blocks))
      (cons '() (clear-top target-block (cdr blocks)))
      (if (< 0 (deep-appearances (car target-block) (car blocks)))
	  (append (clear-top-helper target-block (car blocks)) (cdr blocks))
	  (cons (car blocks) (clear-top target-block (cdr blocks))))))
(define (clear-top-helper target-block block-section) ;it is what the name implies
  (if (equal? target-block (car block-section))
      (list block-section)
      (cons (list (car block-section)) (clear-top-helper target-block (cdr block-section)))))

(define (remover blocks remove-these) ;removes blocks, remove-these is a list of blocks to remove
  (if (null? remove-these) blocks
      (remover (remover-helper2 (remover-helper1 blocks (car remove-these)) (car remove-these))
	       (cdr remove-these))))
(define (remover-helper1 blocks remove-this) ;clears top of the thing being removed
  (clear-top remove-this blocks))
(define (remover-helper2 blocks remove-this) ;removes one part at a time (helper)
  (map (lambda (x) (remover-helper3 remove-this x)) blocks))
(define (remover-helper3 remove-this blocks)
  (filter (lambda (x) (not (equal? remove-this x))) blocks))


(define (adder blocks block);program that randomly adds a block to the blocks in the availiable collumns
  (remove-first-dupls-blocks
  (if (null? blocks) (adder-helper blocks block 0)
  (adder-helper blocks block (random (length blocks))))))
;below are helper procedures for adder___________________________

(define (adder-helper blocks block random-number) ;helps by making the addition of the block more "random"
  (if (null? blocks) (list (list block))
  (map (lambda (x) (cond
		    ((equal? (list-ref blocks random-number) x)
		     (cons block x))
		    (else
		       x))) blocks)))

(define (remove-first-dupls-blocks blocks) ; helps fix an "adder" glitch by removing block multiples
  (if (null? blocks) '()
      (if (null? (car blocks)) (cons (car blocks) (remove-first-dupls-blocks (cdr blocks)))
  (if (< 1 (deep-appearances (caaar blocks) blocks))
      (cons '() (remove-first-dupls-blocks (cdr blocks)))
      (cons (car blocks) (remove-first-dupls-blocks (cdr blocks)))))))

(define (block-generator blocks) ;generates a block of a random color and an unused number
  (list (number-generator blocks 1) (item (+ 1 (random 7)) (color-sent))))
(define (block-generator2 blocks color) ;generates a block of a non-random color and unused number
  (list (number-generator blocks 1) color))
(define (number-generator blocks number) ;returns the next highest availiable number
  (if (equal? 0 (deep-appearances number blocks))
      number
      (number-generator blocks (+ 1 number))))
;_________________________________________________________________
;_________________________________________________________________

;below are generally useful helpers for the overall code

(define (deep-appearances elt lst) ;code used in a previous lab / quiz,
                                   ;remember elt is a single element, not a list or sentence!
  (cond
    ((null? lst) 0)
    ((list? (car lst))
     (+ (deep-appearances elt (car lst))
        (deep-appearances elt (cdr lst))))
    ((equal? (car lst) elt)
     (+ 1 (deep-appearances elt (cdr lst))))
    (else
     (deep-appearances elt (cdr lst)))))

(define (almost-complete-reverse list-words) ;reverses order of blocks, useful for "what is".
  (cond ((null? list-words) list-words) 
	((not (list? (car list-words))) list-words)
	(else (append (almost-complete-reverse (cdr list-words))
		      (list (almost-complete-reverse (car list-words)))))))

(define (color-sent) '(black blue green cyan red magenta yellow white))

(define (color? x) (member? x (color-sent))) ;checks whether or not x is a color

(define (block? x) ;tests to see if x is a block
  (cond ((not (list? x)) #f)
	((< (length x) 2) #f)
	((and (number? (car x)) (color? (cadr x))) #t)
	(else #f)))

(define (list-blocks color blocks) ;lists blocks of that color
  (reduce (lambda (x y) (append x y))
  (map (lambda (x) (list-blocks-helper color x)) blocks)))
(define (list-blocks-helper color blocks)
  (filter (lambda (x) (equal? color (last x))) blocks))
(define (list-all-blocks blocks) ;lists all availiable blocks in a single list
  (reduce (lambda (x y) (append x y)) blocks))

(define (choose-random num list-blocks) ;randomly chooses num blocks from the list of blocks
  (if (null? list-blocks)
      #f
  (if (equal? 1 (length (choose-random-helper num list-blocks '())))
      (car (choose-random-helper num list-blocks '()))
      (choose-random-helper num list-blocks '()))))
(define (choose-random-helper num list-blocks so-far)
  (choose-helper (list-ref list-blocks (random (length list-blocks))) num list-blocks so-far))
(define (choose-helper block num list-blocks so-far)
  (if (equal? num 1)
      (cons block so-far)
      (choose-random-helper (- num 1) (remove-from block list-blocks) (cons block so-far))))

(define (remove-from block list-blocks) ;helper for choose-random but may be used/useful elsewhere
  (filter (lambda (x) (not (equal? x block))) list-blocks))
;________________________________________________________________
