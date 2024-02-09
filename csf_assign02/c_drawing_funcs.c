// C implementations of drawing functions (and helper functions)

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drawing_funcs.h"

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

// TODO: implement helper functions

//
// Get the red component of a color
//
// Parameters:
//   color   - a 32 bit integer
// Return:
//   uint8_t - Bits 24-31
//
uint8_t get_r(uint32_t color) {
  return color >> 24;
}

//
// Get the green component of a color
//
// Parameters:
//   color   - a 32 bit integer
// Return:
//   uint8_t - Bits 16-23
//
uint8_t get_g(uint32_t color) {
  return (color & 0x00ff0000) >> 16;
}

//
// Get the blue component of a color
//
// Parameters:
//   color   - a 32 bit integer
// Return:
//   uint8_t - Bits 8–15
//
uint8_t get_b(uint32_t color) {
  return (color & 0x0000ff00) >> 8;
}

//
// Get the opacity (alpha) of a color
//
// Parameters:
//   color   - a 32 bit integer
// Return:
//   uint8_t - Bits 0–7
//
uint8_t get_a(uint32_t color) {
  return color & 0x000000ff;
}

//
// Squares a number
//
// Parameters:
//   x   - a 64 bit integer
// Return:
//   x * x - a 64 bit integer
//
int64_t square(int64_t x) {
  return x * x;
}

//
// Returns the square of the distance between two points (x1, y1)
// and (x2, y2)
//
// Parameters:
//   x1   - a 64 bit integer representing
//   y1   - a 64 bit integer
//   x2   - a 64 bit integer
//   y2   - a 64 bit integer
// Return:
//   (x1 - x2)^2 + (y1 - y2)^2 - the square distance as a 64 bit integer
//
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
  return square(x1 - x2) + square(y1 - y2);
}

//
// Returns true if 0 <= x <= image_width and
// 0 <= y <= image_height
//
// Parameters:
//   img - 
//   x - 
//   y -
// Return:
//   0 if out of bounds
//
int32_t in_bounds(struct Image *img, int32_t x, int32_t y) {
  if (x < 0 || y < 0 || x >= img->width || y >= img->height) {
    return 0;
  } else {
    return 1;
  }
}

////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  // TODO: implement
}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color) {
  // TODO: implement
}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color) {
  // TODO: implement
}

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile) {
 // TODO: implement
}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite) {
  // TODO: implement
}
