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
//   img - the image we will compare dimensions of
//   x - target x value
//   y - target y value
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

// Computes the corresponding array index to a given point (x, y)
//
// Parameters:
//   img - the image to which we will be translating x, y coordinates
//   x - desired x position (row major order)
//   y - desired y position
// Return:
//   index - representing the appropriate pixel of the array
//
uint32_t compute_index(struct Image *img, int32_t x, int32_t y) {
  return img->width * y + x;
}

// Blends foreground and background color component
// values using a specified alpha (opacity) value.
// note this operation is meant to be performed on each component
//
// Parameters:
//   fg - an 8-bit integer that is the foreground color component
//   bg - an 8-bit integer that is the background color component
//   alpha - a number representing the opacity
// Return:
//   index - representing the appropriate pixel of the array
//
uint8_t blend_components(uint8_t fg, uint8_t bg, uint8_t alpha) {
  return ((alpha * fg) + ((255 - alpha) * bg)) / 255;
}

// Blends foreground and background colors
// using the opacity of the foreground color
//
// Parameters:
//   fg - a 32-bit integer that is the foreground color
//   bg - a 32-bit integer that is the background color
// Return:
//   unit32_t - the blended color
//
uint32_t blend_colors(uint32_t fg, uint32_t bg) {
  // alpha, blue, green, red (least -> most significant)
  uint32_t fg_alpha = get_a(fg);
  uint32_t result = 0xff; // the alpha value (least significant)
  result += blend_components(get_b(fg), get_b(bg), fg_alpha) << 8;
  result += blend_components(get_g(fg), get_g(bg), fg_alpha) << 16;
  result += blend_components(get_r(fg), get_r(bg), fg_alpha) << 24;
  return result;
}

// Sets a pixel of an image to the given color
// note: does not blend the two colors
//
// Parameters:
//   img - a point to the image to be modified
//   index - the index of the pixel
//   color - a 32-bit integer that is the color we are setting the pixel to
//
void set_pixel(struct Image *img, uint32_t index, uint32_t color) {
  img->data[index] = color;
}

// Constrains a value so it is in the range [min, max]
//
// Parameters:
//   val - a 32-bit integer that is the value to be constrained
//   min - a 32-bit integer that is the minimum allowed value
//   max - a 32-bit integer that is the maximum allowed value
// Return:
//   unit32_t - constrained value
//
int32_t clamp(int32_t val, int32_t min, int32_t max) {
  if (val < min) {
    val = min;
  } else if (val > max) {
    val = max;
  }
  return val;
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
  // TODO: implement - DONE
  if (in_bounds(img, x, y)) { // only modify the image if the pixel is within bounds
    uint32_t index = compute_index(img, x, y);
    uint32_t result_color = blend_colors(color, img->data[index]);
    set_pixel(img, index, result_color);
  }
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
  // TODO: implement - DONE
  // since draw pixel does not draw when things are out of bounds,
  // it is okay to not check that the rectange is entirely in bounds
  for (int i = 0; i < rect->width; i++) {
    for (int j = 0; j < rect->height; ++j) {
      // offset by the current pixel from start position
      draw_pixel(img, rect->x + i, rect->y + j, color);
    }
  }
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
  for (int i = 0; i < tile->width; ++i) {
    for (int j = 0; j < tile->height; ++j) {
      if (in_bounds(img, x + i, y + j)) {
        uint32_t index_dest = compute_index(img, x + i, y + j);

        // we know i, j are in bounds because they are the loop conditions
        uint32_t index_source = compute_index(tilemap, i, j);
        uint32_t copied_color = tilemap->data[index_source];
        set_pixel(img, index_dest, copied_color);
      }

    }
  }
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
