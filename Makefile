CC = gcc
CFLAGS = -pedantic -Wall -Werror -Wextra -Ofast -lpthread -lm
OBJ = mandelbrot.c

all: mand run view 

mand:
	${CC} ${OBJ} ${CFLAGS} -o mand

debug:
	${CC} ${OBJ} ${CFLAGS} -g3 -OO -o mand

run: mand
	./mand

view: out.ff
	feh out.ff &

compress: out.ff
	cp out.ff compressed.ff
	pbzip2 compressed.ff &

png: out.ff
	ff2png < out.ff > out.png

jpg: out.ff
	ff2jpg < out.ff > out.jpg

jpeg: jpg

.PHONY: all mand run view compress png jpg jpeg
