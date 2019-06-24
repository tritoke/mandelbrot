#include <arpa/inet.h>
#include <complex.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

uint16_t colours[360*4];

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
   */

  long complex double c = (blx + (x / (xlen / (trx-blx))))
                     + ((try - (y / (ylen / (try-bly)))) * I);

  uint16_t * val,
           default_pixel[4] = {0,0,0,UINT16_MAX};
  uint32_t i = 0;
  if (cabs(c) < 2.0) {
    long complex double z = 0;
    for (;(i < iterations) && (cabs(z) < 2.0); i++, z = cpow(z,2) + c);
  }
  switch (style) {
    case 1:
      // treats the number of iterations mod 360 as the hue in an
      // HSV colour where saturation and colour are 1
      val = (i == iterations) ? default_pixel : &colours[(i%360)*4];
      break;
    case 2:
      // trippy pattern in the colours of the bi flag
      {
        //#EC1C89
        uint16_t pink[4] = {htons(0xEC << 8), htons(0x1C << 8), htons(0x89 << 8), UINT16_MAX};
        //#9E49C2
        uint16_t purple[4] = {htons(0x9E << 8), htons(0x49 << 8), htons(0xC2 << 8), UINT16_MAX};
        //#3F48CC
        uint16_t blue[4] = {htons(0x3F << 8), htons(0x48 << 8), htons(0xCC << 8), UINT16_MAX};
        uint16_t * clrs[3] = {pink, purple, blue};
        val = (i == iterations) ? default_pixel : clrs[i%3];
      }
      break;
    case 3:
    //good for between 100 and 1000 iterations
    {
      uint16_t scaled_pixel[4] = {htons(i<<8), htons(i<<8), htons(i<<8), UINT16_MAX};
      val = (i == iterations) ? default_pixel : scaled_pixel;
    }
    break;
    case 4: //good for more than 1000 iterations
    {
      uint16_t scaled_pixel[4] = {htons(i<<7), htons(i<<7), htons(i<<7), UINT16_MAX};
      val = (i == iterations) ? default_pixel : scaled_pixel;
    }
    break;
    default: 
    // the default colour scheme is good for less than 100 iterations
    {
      uint16_t scaled_pixel[4] = {htons(i<<9), htons(i<<9), htons(i<<9), UINT16_MAX};
      val = (i == iterations) ? default_pixel : scaled_pixel;
    }
  }
  memcpy(pixel, val, 4*sizeof(uint16_t));
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

  FILE * fp;
  fp = fopen(fname, "wb");

  /* write the magic value, width and length of the image */
  fwrite("farbfeld", sizeof(char), 8, fp);
  uint32_t geom[2] = {htonl(xlen), htonl(ylen)};
  fwrite(geom, sizeof(uint32_t), 2, fp);

  uint16_t * pixels = malloc(sizeof(uint16_t) * xlen * ylen * 4);

  /* 
   * farbfeld is a row major image system so we
   * need to iterate over rows on the inner loop
   */

  for (int y = 0, i = 0; y < ylen; y++)
    for (int x = 0; x < xlen; x++, i+=4)
      colour(x,y,&pixels[i]);

  fwrite(pixels, sizeof(uint16_t), xlen * ylen * 4, fp);
  fclose(fp);
  free(pixels);
  return 0;
}
