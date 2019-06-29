#include <arpa/inet.h>
#include <complex.h>
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

static uint16_t colours[360*4];
static uint16_t * pixels;

static volatile uint32_t current_y = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void fill(void) {
  /*
   * Fills the colours array with values from the HSV colourspace
   */

  uint16_t rgba[4];
  rgba[3] = UINT16_MAX;
  for (int i, H = i = 0; H < 360; H++, i+=4) {
    uint16_t X = htons((uint16_t) (255.0 * (1 - fabs(fmod((double) H / 60.0, 2) - 1))) << 8);
    switch (H / 60) {
      case 0:
        rgba[0] = UINT16_MAX;
        rgba[1] = X;
        rgba[2] = 0;
        break;
      case 1:
        rgba[0] = X;
        rgba[1] = UINT16_MAX;
        rgba[2] = 0;
        break;
      case 2:
        rgba[0] = 0;
        rgba[1] = UINT16_MAX;
        rgba[2] = X;
        break;
      case 3:
        rgba[0] = 0;
        rgba[1] = X;
        rgba[2] = UINT16_MAX;
        break;
      case 4:
        rgba[0] = X;
        rgba[1] = 0;
        rgba[2] = UINT16_MAX;
        break;
      case 5:
        rgba[0] = UINT16_MAX;
        rgba[1] = 0;
        rgba[2] = X;
        break;
      default:
        rgba[0] = 0;
        rgba[1] = 0;
        rgba[2] = 0;
    }
    memcpy(&colours[i], rgba, 4*sizeof(uint16_t));
  }
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
              a2 = powl(a, 2),
              b2 = powl(b, 2),
              temp;

  while ((i < iterations) && ((a2 + b2) < 4)) {
    i++;

    // a^2 - b^2 + c
    temp = a2 - b2 + c;
    // 2ab + d
    b = ((a + a) * b) + d;
    a = temp;

    a2 = powl(a, 2),
    b2 = powl(b, 2);
  }

  if (i == iterations) {
    const uint16_t default_pixel[4] = {0,0,0,UINT16_MAX};
    memcpy(pixel, default_pixel, 4*sizeof(uint16_t));
    return;
  }

  switch (style) {
    case 1:
      // treats the number of iterations mod 360 as the hue in an
      // HSV colour where saturation and colour are 1
      memcpy(pixel, &colours[(i%360)*4], 4*sizeof(uint16_t));
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

static void * threadfunc(void * varg) {
  /*
   * colours the y'th column of the image.
   */

  /*
   *  look into using a queue to store the completed lines
   *  and having another thread which just writes the buffered lines
   *  out to the file.
   */
  uint32_t y = ((uint64_t) varg) & ((uint32_t) ~0);
  while (y < ylen) {
    uint16_t * row = pixels + (y * ylen * 4);

    // x << 2 == 4 * x
    // this is the x'th pixel in the row
    for (uint32_t x = 0; x < xlen; x++)
      colour(x,y,&row[x<<2]);

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
  fill();

  /* open the file and write the magic value / metadata */
  FILE * fp;
  fp = fopen(fname, "wb");

  fwrite("farbfeld", sizeof(char), 8, fp);
  uint32_t geom[2] = {htonl(xlen), htonl(ylen)};
  fwrite(geom, sizeof(uint32_t), 2, fp);

  pixels = malloc(sizeof(uint16_t) * xlen * ylen * 4);

  /*
   * new threading plan:
   *  create each of the threads and have each thread work on one row at a time
   *  then when each thread is finished it locks the mutex on the current row
   *  variable before incrementing it and releasing the lock before starting
   *  to work on the pixels in that row.
   */

  pthread_t tids[threads];
  for (uint32_t i = 0; i < threads; i++) {
    if (pthread_create(&tids[i], NULL, threadfunc, (void *) (uint64_t) i)) {
      printf("error creating thread %d\n", i);
      return 1;
    } else printf("created thread %d\n", i);
  } 

  for (uint32_t i = 0; i < threads; i++) {
    if (pthread_join(tids[i], NULL)) {
      printf("failed to join thread %d\n", i);
      return 2;
    } else printf("joined thread %d\n", i);
  }

  fwrite(pixels, sizeof(uint16_t), xlen * ylen * 4, fp);
  fclose(fp);
  free(pixels);
  return 0;
}
