CC = gcc
CFLAGS = -pedantic -Wall -Werror -Wextra -Ofast -lpthread -lm
OBJ = mandelbrot.c

all: mand run view 

mand:
	${CC} ${OBJ} ${CFLAGS} -o mand

debug:
	${CC} ${OBJ} ${CFLAGS} -g3 -oO -o mand

run: mand
	./mand

view: mand.ff
	feh mand.ff &

compress: mand.ff
	cp mand.ff compressed.ff
	pbzip2 compressed.ff &

png: mand.ff
	ff2png < mand.ff > mand.png

jpg: mand.ff
	ff2png < mand.ff > mand.jpg

jpeg: jpg

.PHONY: all mand run view compress png jpg jpeg
