// defines the style of the resulting render
// 1 -> use the map file specified by mapfile
// 2 -> bi flag alternating each pixel
// 3 -> greyscale for iterations between 100 and 1000
// 4 -> greyscale for more than 1000 iterations
// 5 -> randomly generated colours
#define style 4

const char mapfile[] = "colourmaps/Skydye05.cmap";
/* dank:
damien3, Digiorg1, bud(2,4), droz(22,60)
glasses2, headache, Lindaa(02,04,06,09,10,16,17)
lkmtch(05,12),Skydye05
*/

// the x and y lengths of the image in numbers of pixels
// as well as the number of threads to use and the number
// of iterations before declaring a value to be in the set
#define ratio 1
const size_t xlen = 20000;
const size_t ylen = (xlen * ratio);
#define iterations 10000

// the number of threads to use when rendering the image
#define threads 8

// the bottom left (blx, bly) and top right (trx, try) coordinates
// defining the area of the fractal to render

#if (0) // close

#define radius 0.0001
#define centre_x -0.7496866
#define centre_y (radius * ratio) + 0.034334

#else // full set

#define radius 2
#define centre_x 0
#define centre_y 0

#endif

/*
const long double blx = centre_x - radius,
                  bly = centre_y - radius,
                  trx = centre_x + radius,
                  try = centre_y + radius;

*/

const long double x_off = 0,
                  y_off = 0,
                  blx = centre_x - (radius * 1) + x_off,
                  bly = centre_y - (radius * ratio) + y_off,
                  trx = centre_x + (radius * 1) + x_off,
                  try = centre_y + (radius * ratio) + y_off;

// the name of the file to save the image to
#define fname "mand.ff"
