#include <arpa/inet.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"

static uint16_t colours[256 * 4];
static uint16_t * pixels;

static volatile uint32_t current_y = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void read_map(const char * mapfile) {
  uint16_t rgba[4];
  rgba[3] = UINT16_MAX;
  FILE * fp;
  fp = fopen(mapfile, "r");
  if (fp == NULL) {
    puts("Failed to open mapfile, exiting.");
    exit(EXIT_FAILURE);
  }
  size_t n = 50;
  char * buf = malloc(n);
  for (int i = 0; i < 256; i++) {
    ssize_t chars_read = getline(&buf, &n, fp);
    if (chars_read == -1) {
      puts("Failed to read mapfile, exiting.");
      exit(EXIT_FAILURE);
    }
    sscanf(buf, "%3hu %3hu %3hu", &rgba[0], &rgba[1], &rgba[2]);
    memcpy(&colours[i*4], rgba, 4*sizeof(uint16_t));
  }
  free(buf);
}

void colour(uint32_t x, uint32_t y, uint16_t * pixel) {
  /*
   * colour the pixel with the values for the coordinate at x+iy
   *  z = a + bi, c = c + di
   */

  uint32_t i = 0;
  long double c = blx + (x / (xlen / (trx-blx))),
              d = try - (y / (ylen / (try-bly))),
              a = c,
              b = d,
              a2 = a * a,
              b2 = b * b,
              temp;

  while ((i < iterations) && ((a2 + b2) < 4)) {
    i++;

    // a^2 - b^2 + c
    temp = a2 - b2 + c;
    // 2ab + d
    b = ((a + a) * b) + d;
    a = temp;

    a2 = a * a;
    b2 = b * b;
  } 

  if (i == iterations) {
    const uint16_t default_pixel[4] = {0,0,0,UINT16_MAX};
    memcpy(pixel, default_pixel, 4*sizeof(uint16_t));
  } else {
    switch (style) {
      case 1:
        // load the colours from the specified map file
        memcpy(pixel, &colours[(i%256)*4], 4*sizeof(uint16_t));
        break;
      case 2: { // trippy pattern in the colours of the bi flag
          //#EC1C89
          const uint16_t pink[4] = {0xEC, 0x1C, 0x89, UINT16_MAX};
          //#9E49C2
          const uint16_t purple[4] = {0x9E, 0x49, 0xC2, UINT16_MAX};
          //#3F48CC
          const uint16_t blue[4] = {0x3F, 0x48, 0xCC, UINT16_MAX};
          const uint16_t * clrs[3] = {pink, purple, blue};
          memcpy(pixel, clrs[i%3], 4*sizeof(uint16_t));
          break;
        }
      case 3: {
        //good for between 100 and 1000 iterations
        uint16_t scaled_pixel[4] = {htons(i<<8), htons(i<<8), htons(i<<8), UINT16_MAX};
        memcpy(pixel, scaled_pixel, 4*sizeof(uint16_t));
        break;
      }
      case 4: {
        //good for more than 1000 iterations
        uint16_t scaled_pixel[4] = {htons(i<<7), htons(i<<7), htons(i<<7), UINT16_MAX};
        memcpy(pixel, scaled_pixel, 4*sizeof(uint16_t));
        break;
      }
      default: 
      // the default colour scheme is good for less than 100 iterations
      {
        uint16_t scaled_pixel[4] = {htons(i<<9), htons(i<<9), htons(i<<9), UINT16_MAX};
        memcpy(pixel, scaled_pixel, 4*sizeof(uint16_t));
      }
    }
  }
}

static void * threadfunc(void * varg) {
  /*
   * colours the y'th column of the image.
   */

  uint32_t y = ((uint64_t) varg) & ((uint32_t) ~0);
  while (y < ylen) {
    uint16_t * row = pixels + (y * xlen << 2);

    // x << 2 == 4 * x
    // this is the x'th pixel in the row
    for (uint32_t x = 0; x < xlen; x++) {
      //printf("[thread]\tx:%d\ty:%d\n", x, y);
      colour(x,y,&row[x<<2]); }

    // acquire lock
    if (pthread_mutex_lock(&mtx) == 0) {
      // lock acquired
      // get the next row to work on and then increment the global y
      y = current_y++;
    } else {
      // failed to acquire lock => exit from thread.
      return NULL;
    }

    // release lock
    if (pthread_mutex_unlock(&mtx) != 0) {
      // failed to release the lock exit the program.
      exit(EXIT_FAILURE);
    }
  }
  return NULL;
}

int main(void) {
  /*
   *  initial check on the ratio between the size of the
   *  area specified by the top right and bottom left coordinates
   *  and the ratio between the x and y lengths of the image to be written
   */
  long double image_ratio = (long double) xlen / (long double) ylen;
  long double area_ratio  = (trx - blx) / (try - bly);
  printf("image ratio: %.30Lf\narea ratio:  %.30Lf\n", image_ratio, area_ratio);

  /* fill the colour pallette */
  read_map(mapfile);

  /* open the file and write the magic value / metadata */
  FILE * fp;
  fp = fopen(fname, "wb");

  fwrite("farbfeld", sizeof(char), 8, fp);
  uint32_t geom[2] = {htonl(xlen), htonl(ylen)};
  fwrite(geom, sizeof(uint32_t), 2, fp);

  pixels = malloc(sizeof(uint16_t) * xlen * ylen * 4);
  pthread_t tids[threads];

  // acquire lock
  if (pthread_mutex_lock(&mtx) != 0) {
    // failed to acquire lock => exit
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < threads; i++, current_y++) {
    if (pthread_create(&tids[i], NULL, threadfunc, (void *) (uint64_t) i)) {
      printf("error creating thread %d\n", i);
      return 1;
    } else printf("[thread]\t%d\tcreated\n", i);
  } 

  // release lock
  if (pthread_mutex_unlock(&mtx) != 0) {
    // failed to release the lock exit the program.
    exit(EXIT_FAILURE);
  }

  for (uint32_t i = 0; i < threads; i++) {
    if (pthread_join(tids[i], NULL)) {
      printf("failed to join thread %d\n", i);
      return 2;
    } else printf("[thread]\t%d\tjoined\n", i);
  }

  puts("[main]\t\twriting pixels");

  fwrite(pixels, sizeof(uint16_t), xlen * ylen * 4, fp);

  puts("[main]\t\tclosing file");
  fclose(fp);

  puts("[main]\t\tfreeing memory");
  free(pixels);

  return 0;
}
