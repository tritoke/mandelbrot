CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Werror -O3 -lpthread -lm
OBJ = mandelbrot.c

all: mand run view

mand:
	${CC} -o mand ${CFLAGS} ${OBJ}

run: mand
	./mand

view: mand.ff
	feh mand.ff &

compress: mand.ff
	cp mand.ff compressed.ff
	bzip2 compressed.ff &

png: mand.ff
	ff2png < mand.ff > mand.png

.PHONY: all mand run view compress
