/*
 * Assembly-language implementations of the drawing functions
 */

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/* Offsets of struct Rect fields */
#define RECT_X_OFFSET        0
#define RECT_Y_OFFSET        4
#define RECT_WIDTH_OFFSET    8
#define RECT_HEIGHT_OFFSET   12

	.section .text

/***********************************************************************
   Helper functions
   You should declare these as .globl so that
   they can be called from the unit test code.
 ***********************************************************************/
/*
 * Squares a number
 *
 * Parameters:
 *   %rdi	- pointer to 64 bit integer x
 * Return:
 *   %rax 	- 64 bit integer result x^2
 */
	.globl square
square:
	imul %rdi, %rdi 	/* rdi = rdi * rdi (perform the multiplication) */
	movq %rdi, %rax 	/* rax = rdi (move value to return register) */
	ret

/*
 * Returns the square of the distance between two points (x1, y1)
 * and (x2, y2)
 *
 * Parameters:
 *  %rdi   - x1 (a 64 bit integer)
 *  %rsi   - y1 (a 64 bit integer)
 *  %rdx   - x2 (a 64 bit integer)
 *  %rcx   - y2 (a 64 bit integer)
 * Return:
 *   %rax	- 64 bit integer result
*/
	.globl square_dist
square_dist:
	subq %rdx, %rdi 		/* rdi = rdi - rdx (x1 = x1 - x2) */
	call square 			/* rdi = x1 - x2 */
	movq %rax, %rdx 		/* rdx = (x1 - x2)^2 GOOD */ 
	subq %rcx, %rsi 		/* rsi = rsi - rcx (y1 = y1 - y2) */
	movq %rsi, %rdi 		/* set up parameter */
	call square 			/* rdi = y1 - y2 */
	addq %rax, %rdx 		/* rdx = (x1 - x2)^2 + (y1 - y2) ^ 2 */
	movq %rdx, %rax			/* move result to rax */
	ret

/*
 * Get the red component of a color
 *
 * Parameters:
 *   %edi   -  r component (a 32 bit integer)
 * Return:
 *   %al 	- bits 24-31 (an 8 bit integer)
*/
	.globl get_r
get_r:
	shr $24, %edi		/* edi >> 24 */
	movb %dil, %al 		/* move 8 bits to return register */
	ret

/*
 * Get the green component of a color
 *
 * Parameters:
 *   %edi   -  g component (a 32 bit integer)
 * Return:
 *   %al 	- bits 24-31 (an 8 bit integer)
*/
	.globl get_g
get_g:
	and $0x00ff0000, %edi 	/* color & 0x00ff0000 */
	shr $16, %edi 			/* edi >> 16 */
	movb %dil, %al 			/* move 8 bits to return register */
	ret

/*
 * Get the blue component of a color
 *
 * Parameters:
 *   %edi   -  b component (a 32 bit integer)
 * Return:
 *   %al 	- bits 24-31 (an 8 bit integer)
*/
	.globl get_b
get_b:
	and $0x0000ff00, %edi 	/* color & 0x0000ff00 */
	shr $8, %edi 			/* edi >> 8 */
	movb %dil, %al 			/* move 8 bits to return register */
	ret

/*
 * Get the alpha component of a color
 *
 * Parameters:
 *   %edi   -  alpha component (a 32 bit integer)
 * Return:
 *   %al 	- bits 24-31 (an 8 bit integer)
*/
	.globl get_a
get_a:
	movb %dil, %al 			/* move lowest 8 bits to return register */
	ret						/* note: we did not have to and since we can simply retrieve last bits */

/*
 * Returns true if 0 <= x <= image_width and
 * 0 <= y <= image_height
 *
 * Parameters:
 *   %rdi	- pointer to Image
 *   %esi	- target x value (32 bit integer)
 *   %edx	- target y value (32 bit integer)
 * Return:
 *   %eax	- 0 if out of bounds (32 bit integer)
*/
	.globl in_bounds
in_bounds:
	cmpl $0, %esi							/* 0 - %esi */
	jl .Lin_bounds_false					/* x < 0, so return 0 */
	cmpl $0, %edx							/* 0 - %edx */
	jl .Lin_bounds_false					/* y < 0, so return 0 */
	cmpl %esi, IMAGE_WIDTH_OFFSET(%rdi)		/* img->width - x */
	jle .Lin_bounds_false					/* 0 >= img->width, so return 0 */
	cmpl %edx, IMAGE_HEIGHT_OFFSET(%rdi)	/* img->height - y */
	jle .Lin_bounds_false
.Lin_bounds_true:			
	movl $1, %eax							/* puts 1 in eax */
	jmp .Lin_bounds_return					/* returns to control */
.Lin_bounds_false:			
	movl $0, %eax							/* puts 0 in eax */
.Lin_bounds_return:
	ret

/*
 * Computes the corresponding array index to a given point (x, y)
 *
 * Parameters:
 *   %rdi	- pointer to Image to which we will be translating x, y coordinates
 *   %esi	- desired x position (row major order) (32 bit integer)
 *   %edx	- desired y position (32 bit integer)
 * Return:
 *   %eax	- representing the appropriate pixel of the array (32 bit integer)
*/
	.globl compute_index
compute_index:
	imul IMAGE_WIDTH_OFFSET(%rdi), %edx 	/* edx = y * img->width */
	addl %esi, %edx							/* edx += x */
	movl %edx, %eax							/* move computed index to return register */
	ret

/*
 * Blends foreground and background color component
 * values using a specified alpha (opacity) value.
 * note this operation is meant to be performed on each component
 *
 * Parameters:
 *   %dil	- fg an 8-bit integer that is the foreground color component
 *   %sil 	- bg an 8-bit integer that is the background color component
 *   %dl 	- alpha a number representing the opacity
 * Return:
 *   %al 	- resulting blended color
 */
	.globl blend_components
blend_components:
	movl $0, %r10d			/* going to use the lowest 8 bits for foreground */
	movl $0, %r11d			/* going to use the lowest 8 bits for background */
	movl $0, %r9d			/* going to use the lowest 8 bits for alpha */
	movb %dil, %r10b		/* %r10b = fg */
	movb %sil, %r11b		/* %r11b = bg */
	movb %dl, %r9b			/* %r9b = alpha */
	imul %r9d, %r10d		/* %r10d = alpha * fg */
	movq $255, %r8			/* %r8b = 255 */
	subb %r9b, %r8b			/* %r8b = 255 - alpha */
	imul %r11d, %r8d		/* %r8b = (255 - alpha) * bg */
	addl %r10d, %r8d		/* %r8b = (alpha * fg) + ((255 - alpha) * bg) */
	movq $0, %rdx
	movq %r8, %rax			/* %rdx:%rax is the dividend */
	movq $255, %r9			/* put divisor in %r9 */
	idiv %r9				/* result automatically stored in %rax (return register) */
	ret

/*
 * Blends foreground and background colors
 * using the opacity of the foreground color
 *
 * Parameters:
 *   %edi 	- fg a 32-bit integer that is the foreground color
 *   %esi 	- bg a 32-bit integer that is the background color
 * Return:
 *   %eax 	- the blended color (32 bits)
*/
	.globl blend_colors
blend_colors:
	movl %edi, %r12d		/* %r12d = fg */
	movl %esi, %r13d		/* %r13d = bg */
	movl %r12d, %edi
	call get_a				/* the foreground color is already in %edi */
	movb %al, %r8b			/* %r8b = get_a(fg) - NOTE: gdb says void when trying to print, but works for %r8 */
	movl $0xff, %ebx		/* %ebx (result) = 0xff */
	call get_b				/* the foreground color is already in %edi */
	movb %al, %r9b			/* %r9b = get_b(fg) */
	movl %esi, %edi			/* %edi = bg */
	call get_b				/* %al = get_b(bg) */
	movb %al, %sil			/* set up get_b(bg) as second argument to blend_components */
	movb %r9b, %dil			/* set up get_b(fg) as first argument to blend_components */
	movb %r8b, %dl			/* set up get_a(fg) as third argument to blend_components */
	call blend_components
	movl $0, %r15d			/* want to make all zero before we shift */
	movb %al, %r15b			/* %r15b holds the blended blue before shifting */
	shl $8, %r15d				/* shift blue component 8 bits left */
	addl %r15d, %ebx			/* adds blue components to result */
	movl %ebx, %eax			/* set up return */
	ret

/*
 * Sets a pixel of an image to the given color
 * note: does not blend the two colors
 *
 * Parameters:
 *   %rdi 	- pointer to Image
 *   %esi	- index the index of the pixel (32 bits)
 *   %edx 	- color (32-bit integer) that is the color we are setting the pixel to
*/
	.globl set_pixel
set_pixel:
	ret

/***********************************************************************
   Public API functions
 ***********************************************************************/

/*
 * Draw a pixel.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate (pixel column)
 *   %edx     - y coordinate (pixel row)
 *   %ecx     - uint32_t color value
 */
	.globl draw_pixel
draw_pixel:
	/* TODO: implement */
	ret


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 */
	.globl draw_rect
draw_rect:
	/* TODO: implement */
	ret

/*
 * Draw a circle.
 * The circle has x,y as its center and has r as its radius.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %esi     - x coordinate of circle's center
 *   %edx     - y coordinate of circle's center
 *   %ecx     - radius of circle
 *   %r8d     - uint32_t color value
 */
	.globl draw_circle
draw_circle:
	/* TODO: implement */
	ret

/*
 * Draw a tile by copying all pixels in the region
 * enclosed by the tile parameter in the tilemap image
 * to the specified x/y coordinates of the destination image.
 * No blending of the tile pixel colors with the background
 * colors should be done.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where tile should be copied
 *   %edx - y coordinate of location where tile should be copied
 *   %rcx - pointer to Image (the tilemap)
 *   %r8  - pointer to Rect (the tile)
 */
	.globl draw_tile
draw_tile:
	/* TODO: implement */
	ret

/*
 * Draw a sprite by copying all pixels in the region
 * enclosed by the sprite parameter in the spritemap image
 * to the specified x/y coordinates of the destination image.
 * The alpha values of the sprite pixels should be used to
 * blend the sprite pixel colors with the background
 * pixel colors.
 *
 * Parameters:
 *   %rdi - pointer to Image (dest image)
 *   %esi - x coordinate of location where sprite should be copied
 *   %edx - y coordinate of location where sprite should be copied
 *   %rcx - pointer to Image (the spritemap)
 *   %r8  - pointer to Rect (the sprite)
 */
	.globl draw_sprite
draw_sprite:
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
