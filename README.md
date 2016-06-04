# getrandom

[getrandom](http://man7.org/linux/man-pages/man2/getrandom.2.html) binding of Emacs.

## Sample Code

```lisp
;; generate 10bytes random string
(getrandom 10)

;; Use /dev/random instead of /dev/urandom
(getrandom 10 'random)

;; Get random number(< 2^32)
(getrandom-integer)
```
