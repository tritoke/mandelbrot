// defines the style of the resulting render
// 1 -> use the map file specified by mapfile
// 2 -> bi flag alternating each pixel
// 3 -> greyscale for iterations between 100 and 1000
// 4 -> greyscale for more than 1000 iterations
#define style 1

const char mapfile[] = "colourmaps/Digiorg1.cmap";
/* dank:
damien3, Digiorg1, bud(2,4), droz(22,60)
glasses2, headache, Lindaa(02,04,06,09,10,16,17)
lkmtch(05,12),Skydye05
*/

// the x and y lengths of the image in numbers of pixels
// as well as the number of threads to use and the number
// of iterations before declaring a value to be in the set
#define xlen 10000
#define ylen 10000
#define iterations 10000

// the number of threads to use when rendering the image
#define threads 8

// the bottom left (blx, bly) and top right (trx, try) coordinates
// defining the area of the fractal to render

#if (1==1) // close

#define radius 0.0000000026
#define centre_x -1.762317839
#define centre_y 0.022931577

#else // full set

#define radius 2
#define centre_x 0
#define centre_y 0

#endif

const long double blx = centre_x - radius,
                  bly = centre_y - radius,
                  trx = centre_x + radius,
                  try = centre_y + radius;

/*
const long double x_off = 0.1,
                  y_off = 0,
                  blx = centre_x - (radius * 0.25) + x_off,
                  bly = centre_y + (radius * 0.25) + y_off,
                  trx = centre_x + (radius * 0) + x_off,
                  try = centre_y + (radius * 0.5) + y_off;
*/

// the name of the file to save the image to
#define fname "mand.ff"
