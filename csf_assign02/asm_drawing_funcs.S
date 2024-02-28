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
	movq %rax, %rdx 		/* rdx = (x1 - x2)^2 */ 
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
	/* ((alpha * fg) + ((255 - alpha) * bg)) / 255 */
	movq $0, %rcx			/* going to use the lowest 8 bits for foreground */
	movq $0, %r10			/* going to use the lowest 8 bits for background */
	movq $0, %r9			/* going to use the lowest 8 bits for alpha */
	movb %dil, %cl			/* %cl (lowest 8 of %rcx) = fg */
	movb %sil, %r10b		/* %r10b (lowest 8 of %r10) = bg */
	movb %dl, %r9b			/* %r9b (lowest 8 of %r9) = alpha */
	imul %r9d, %ecx			/* %ecx (lowest 32 of %rcx) = alpha * fg */
	movq $255, %r8			/* %r8b = 255 */
	subb %r9b, %r8b			/* %r8b = 255 - alpha */
	imul %r10d, %r8d		/* %r8b = (255 - alpha) * bg */
	addl %ecx, %r8d			/* %r8b = (alpha * fg) + ((255 - alpha) * bg) */
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
							/* allocating local variables */
	pushq %rbp				/* save original value of rbp */
	movq %rsp, %rbp			/* %rbp points to top of allocated area */
	subq $16, %rsp			/* allocate 16 bytes */
	movl %edi, -16(%rbp)	/* -16(%rbp) = fg */
	movl %esi, -12(%rbp)	/* -12(%rbp) = bg */
	call get_a				/* foreground is already in %edi */
	movb %al, -8(%rbp)		/* -8(%rbp) = get_a(fg) */
	movl $0xff, -4(%rbp)	/* -4(%rbp) (result) = 0xff */

							/* blue component */
	call get_b				/* the foreground color is already in %edi */
	movb %al, %r9b			/* %r9b = get_b(fg) */
	movl -12(%rbp), %edi	/* %edi = bg */
	call get_b				/* %al = get_b(bg) */
	movb %al, %sil			/* set up get_b(bg) as second argument to blend_components */
	movb %r9b, %dil		/* set up get_b(fg) as first argument to blend_components */
	movb -8(%rbp), %dl		/* set up get_a(fg) as third argument to blend_components */
	call blend_components
	movl $0, %r15d			/* want to make all zero before we shift */
	movb %al, %r15b			/* %r15b holds the blended blue before shifting */
	shl $8, %r15d			/* shift blue component 8 bits left */
	addl %r15d, -4(%rbp)	/* adds blue components to result */

							/* green component */
	movl -16(%rbp), %edi	/* set up fg as argument */
	call get_g
	movb %al, %r9b			/* %r9b = get_g(fg) */
	movl -12(%rbp), %edi	/* set up bg as argument */
	call get_g
	movb %al, %sil			/* set up get_g(bg) as second argument */
	movb %r9b, %dil		/* set up get_g(fb) as first argument */
	movb -8(%rbp), %dl		/* set up get_a(fg) as third argument */
	call blend_components
	movl $0, %r15d
	movb %al, %r15b
	shl $16, %r15d			/* shifts blended green 16 left */
	addl %r15d, -4(%rbp)	/* adds green component to result */

							/* red component */
	movl -16(%rbp), %edi	/* set up fg as argument */
	call get_r
	movb %al, %r9b			/* %r9b = get_r(fg) */
	movl -12(%rbp), %edi	/* set up bg as argument */
	call get_r
	movb %al, %sil			/* set up get_r(bg) as second argument */
	movb %r9b, %dil		/* set up get_r(fb) as first argument */
	movb -8(%rbp), %dl		/* set up get_r(fg) as third argument */
	call blend_components
	movl $0, %r15d
	movb %al, %r15b
	shl $24, %r15d			/* shifts blended red 16 left */
	addl %r15d, -4(%rbp)	/* adds red component to result */

	movl -4(%rbp), %eax		/* set up return */
	addq $16, %rsp			/* restore */
	popq %rbp
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
	movq IMAGE_DATA_OFFSET(%rdi), %rdi		/* base address of array in %r10 */
	movl %edx, (%rdi, %rsi, 4)
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
								/* allocating local variables */
	pushq %rbp					/* save original value of rbp */
	movq %rsp, %rbp				/* %rbp points to top of allocated area */
	subq $32, %rsp				/* allocate 4 bytes (not all used) */
	movq %rdi, -32(%rbp)		/* -32(%rbp) = pointer Img */
	movl %esi, -16(%rbp)		/* -16(%rbp) = x */
	movl %edx, -12(%rbp)		/* -12(%rbp) = y */
								/* -8(%rbp) = index */
	movl %ecx, -4(%rbp)			/* -4(%rbp) = color */

	call in_bounds				/* arguments are already set up */
	cmp $0, %eax				/* if !in_bounds */
	je .Ldraw_pixel_return		/* go to return if out of bounds */
	movq -32(%rbp), %rdi		/* Img pointer as first argument */
	movl -16(%rbp), %esi		/* x as second argument */
	movl -12(%rbp), %edx		/* y as third argument */
	call compute_index
	movl %eax, -8(%rbp)			/* -8(%rbp) = index */

	movl -4(%rbp), %edi						/* move foreground (color) as first argument */
	movq -32(%rbp), %r15					/* temporarily store in %r15 because we cannot nest computations */
	movq IMAGE_DATA_OFFSET(%r15), %r15		/* put base address of array in %r15*/
	movq $0, %r10
	movl -8(%rbp), %r10d					/* temporarily put address in %r10d */
	leaq (%r15, %r10, 4), %r15
	movl (%r15), %esi						/* current color of the image */
	call blend_colors
	
	movl %eax, %edx 			/* set result_color as third parameter */
	movq -32(%rbp), %r11		/* compute addres of Image */
	movq %r11, %rdi				/* set Img pointer as first parameter */
	movl -8(%rbp), %esi			/* set index as second parameter */
	call set_pixel

.Ldraw_pixel_return:
	addq $32, %rsp			/* restore */
	popq %rbp

	ret						/* FIXME */


/*
 * Draw a rectangle.
 * The rectangle has rect->x,rect->y as its upper left corner,
 * is rect->width pixels wide, and rect->height pixels high.
 *
 * Parameters:
 *   %rdi     - pointer to struct Image
 *   %rsi     - pointer to struct Rect
 *   %edx     - uint32_t color value
 * Registers:
 *	 %ebx (%rbx)    - i counter (uint32)
 *	 %ebp (%rbp)	- j counter (uint32)
 *	 %r12			- save pointer to Image
 *	 %r13			- save pointer to Rect
 *   %r14d (%r14)   - color value (uint32)
 */

 // red rect x = 2, y = 2, width = 3, height = 3
	.globl draw_rect
draw_rect:
	pushq %rbx
	pushq %rbp
	pushq %r12
	pushq %r13								/* push odd number of values for stack alignment */
	pushq %r14								/* save values before we overwrite */
	movq %rdi, %r12							/* %r12 = pointer to Image */
	movq %rsi, %r13							/* %r13 = pointer to Rect */
	movl %edx, %r14d						/* %r14d = color */
	movl $-1, %ebx							/* i = -1 (we are about to increment) - may cause issues with negative comparison */
.Loutertop:
	incl %ebx								/* i++ (starts at 0) */
	cmpl %ebx, RECT_WIDTH_OFFSET(%r13)		/* rect->width - i <= 0, exit loop */
	jbe .Ldraw_rect_return					/* if done, jump to return */
	movl $0, %ebp							/* j = 0 */
.Linnertop:
	cmpl %ebp, RECT_HEIGHT_OFFSET(%r13)	/* rect->height - j <= 0, exit loop */
	jbe .Loutertop							/* we want to exit the loop */
	movq %r12, %rdi							/* %rdi (first parameter) = address of Image */
	movl RECT_X_OFFSET(%r13), %esi			/* %esi = img->x */
	addl %ebx, %esi							/* %esi (second parameter) = img->x + i */
	movl RECT_Y_OFFSET(%r13), %edx			/* %edx = img->y */
	addl %ebp, %edx							/* %edx (third parameter) = img->y + j */
	movl %r14d, %ecx						/* %ecx (fourth parameter) = color */
	call draw_pixel
	incl %ebp								/* j++ */
	jmp .Linnertop							/* return back to check the inner loop condition */

.Ldraw_rect_return:
	popq %r14
	popq %r13
	popq %r12
	popq %rbp
	popq %rbx

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
/* x = 2, y = 2, r = 2 */

	.globl draw_circle
draw_circle:
							/* allocating local variables */
	pushq %rbp				/* save original value of rbp */
	movq %rsp, %rbp			/* %rbp points to top of allocated area */
	subq $80, %rsp			/* allocate 64 bytes */
	movl %ecx, -80(%rbp)	/* -80(%rbp) = r */
							/* could store eight bytes at -72(%rbp) */
	movq %rdi, -64(%rbp)	/* -64(%rbp) = pointer to Image */
	movl %esi, -48(%rbp)	/* -48(%rbp) = x (center of circle) */
	movl %edx, -40(%rbp)	/* -40(%rbp) = y (center of circle) */
	movl %r8d, -32(%rbp)	/* -32(%rbp) = color */
	movl $-1, -24(%rbp)		/* -24(%rbp) = i = 0 */

	movq $0, %rdi			/* clear upper 32 bits of %rdi */
	movl -80(%rbp), %edi	/* %edi = r */
	call square
	movl %eax, -80(%rbp)	/* -80(%rbp) = r ^ 2 */

.Loutertop_draw_circle:								/* QUESTION: DO LABELS EXIST LOCALLY? Can I have two identical labels within different functions? */
	incl -24(%rbp)									/* i++ (starts at 0) */
	movl -24(%rbp), %r9d							/* %r9d = i */
	movq -64(%rbp), %r8								/* temporarily store Image pointer so it can be offset */
	movl IMAGE_WIDTH_OFFSET(%r8), %r10d
	cmpl %r9d, IMAGE_WIDTH_OFFSET(%r8)				/* img->width - i <= 0, exit loop */
	jbe .Ldraw_circle_return						/* if done, jump to return */
	movl $0, -16(%rbp)								/* -16(%rbp) = j = 0 */

.Linnertop_draw_circle:
	movl -16(%rbp), %r9d						/* %r9d = j */
	movq -64(%rbp), %r8							/* temporarily store Image pointer so it may be offset */
	cmpl %r9d, IMAGE_HEIGHT_OFFSET(%r8)			/* img->height - j <= 0, exit loop */
	jbe .Loutertop_draw_circle

	movl $0, -8(%rbp)						/* -8(%rbp) = dist_squared (set to zero) */
	movq $0, %rdi							/* %rdi = 0 (clear upper bits) */
	movl -48(%rbp), %edi					/* %edi (lower 32 of %rdi) = x */
	subl -24(%rbp), %edi					/* %rdi = x - i */
	call square
	movl %eax, -8(%rbp)						/* -8(%rbp)	= (x - i) ^ 2 - only lower 32 bits - SEEMED CORRECT */
	movq $0, %rdi							/* %rdi = 0 (clear upper bits) */
	movl -40(%rbp), %edi						/* %edi (lower 32 bits of %rdi) = y */
	subl -16(%rbp), %edi					/* %edi = y - j */
	call square
	addl %eax, -8(%rbp)						/* -8(%rbp) = (x - i) ^ 2 + (y - j) ^ 2 */
	movl -8(%rbp), %r9d						/* %r9d = dist_squared */
	cmpl %r9d, -80(%rbp)					/* r^2 - [(x - i) ^ 2 + (y - j) ^ 2] */
	jl .Ldraw_circle_increment				/* r^2 - dist_square < 0 - FIXME: is sign incorrect? should this be a unsigned comparison? */
											/* set up arguments for drawing a pixel */
	movq -64(%rbp), %rdi					/* %rdi = pointer to Image (first argument) */
	movl -24(%rbp), %esi					/* %esi = i (second argument) */
	movl -16(%rbp), %edx					/* %edx = j (third argument) */
	movl -32(%rbp), %ecx					/* %ecx = color (fourth argument) */
	call draw_pixel

.Ldraw_circle_increment:
	incl -16(%rbp)							/* j++ */
	jmp .Linnertop_draw_circle				/* return back to check the inner loop condition */

.Ldraw_circle_return:
	addq $80, %rsp			/* restore */
	popq %rbp
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
 * Registeres:
  *	 %ebx (%rbx)    - i counter (uint32)
 *	 %ebp (%rbp)	- j counter (uint32)
 *	 %r12			- save pointer to Image
 *	 %r13d (%r13)	- x (uint32)
 *   %r14d (%r14)   - y (uint32)
 *   %r15d (%r15)	- color (uint32)
 */
	.globl draw_tile
draw_tile:

	pushq %r12  /* i-pointer */
	pushq %r13  /* j-pointer */

	movq $0, %r12 /* assign the pointer 0 */
	movq $0, %r13 /* assign the pointer 0 */

	pushq %rbp      /* allocating local variables */
	movq %rsp, %rbp  /* %rbp points to top of allocated area */
	subq $96, %rsp   /* allocate 96 bytes */

	movq %rdi, -96(%rbp) /* -96(%rbp) = pointer to Image (dest image) */
						/* Could store bytes here lol at -72 */
	movl %esi, -80(%rbp) /* -80(%rbp) = x coordinate of location where tile should be copied */
	movl %edx, -72(%rbp) /* -72(%rbp) = y coordinate of location where tile should be copied */
	movq %rcx, -64(%rbp) /* -64(%rbp) = pointer to Image (the tilemap) */
	movq %r8, -48(%rbp) /* -48(%rbp) = pointer to Rect (the tile) */
	
    movl RECT_X_OFFSET(%r8), %r12d
    movl %r12d, -32(%rbp) /* -32(%rbp) = stores tile->x */

    movl RECT_Y_OFFSET(%r8), %r13d
    movl %r13d, -24(%rbp) /* -32(%rbp) = stores tile->y */


	movl RECT_WIDTH_OFFSET(%r8), %r12d /* starts as tile->x - stores the upper most bound of the outer loop  */
	addl -32(%rbp), %r12d /* becomes this after tile->x + tile->width adding tile->width */
	movl %r12d, -16(%rbp) /* save val to -16(%ebp) */

	movl RECT_HEIGHT_OFFSET(%r8), %r12d /* starts as tile->y - stores the upper most bound of the inner loop  */
	addl -24(%rbp), %r12d /* becomes this after tile->y + tile->height adding tile->height */
	movl %r12d, -8(%rbp) /* save val to -8(%ebp) */

    
    movl -32(%rbp), %r12d /* move over start pointer tile->x */
    decl %r12d /* decriment by 1 */

	movl -24(%rbp), %r13d /* move over start pointer tile->y */
	decl %r13d /* decriment by 1 */
    
	jmp .Louterloop_invalid


.Louterloop_invalid:
	incl %r12d /* increments i */

	cmpl %r12d, -16(%rbp) /* compares i and tile->x + tile->width */
	jbe .Lreset_values /* if i >= tile->x + tile->width jump */
	movl RECT_Y_OFFSET(%r8), %r13d /* set the value for j */

	//decl -24(%rbp) - unnessisary decriment

.Linnerloop_invalid:
	cmpl %r13d, -8(%rbp)
	jbe .Louterloop_invalid

	movq -64(%rbp), %rdi /* assigns the image */

	movl %r12d, %esi /* assigns the i value */
	movl %r13d, %edx /* assigns the j value */

	movl $0, %eax /* set rax to 0 */

	call in_bounds /* call the function with params */

	cmpl $0, %eax /* compare if %eax is equal to zero */
	jz .Lend /* jump if the values are equal */

	incl %r13d /* increment the inner loop j */

	jmp .Linnerloop_invalid

.Lreset_values:
	
	movl $0, %r12d /* reset the values for i to -1 */
	decl %r12d

	movl $0, %r13d /* reset the values for j to -1 */
	/* decl %r13d */

	movl RECT_WIDTH_OFFSET(%r8), %r10d
	movl %r10d, -16(%rbp) /* seting the outer loop to width */
	movl RECT_HEIGHT_OFFSET(%r8), %r10d
	movl %r10d, -8(%rbp) /* seting the outer loop to height */

.Louterloop_draw:
	incl %r12d /* incriments i */
	cmpl %r12d, -16(%rbp) /* compares i and tile->width */
	jbe .Lend /* if i >= tile->width jump */
	movl $0, %r13d /* reset the values for j */

	/*decl %r13d decriment j by 1 */

.Linnerloop_draw:

	cmpl %r13d, -8(%rbp)
	jbe .Louterloop_draw

	// before stuff
	
	movl $0, %eax

	movq -96(%rbp), %rdi /* set rdi to img */
	movl RECT_X_OFFSET(%r8), %esi /* set rsi to x + i */
	addl -32(%rbp), %esi
	movl RECT_Y_OFFSET(%r8), %edx /* set rds to y + i */
	addl -24(%rbp), %esi

	call in_bounds

	cmpl $1, %eax
	jz .Lset_pixel

.Ljump_back:
	// after stuff

	incl %r13d
	jmp .Linnerloop_draw

.Lset_pixel:

	movl $0, %eax

	movq -96(%rbp), %rdi /* set rdi to img */
	movl -80(%rbp), %esi /* set rsi to x + i */
	addl %r12d, %esi
	movl -72(%rbp), %edx /* set rds to y + i */
	addl %r13d, %esi

	call compute_index

	movl %eax, %r10d /* store index_dest in r10d*/

	movl $0, %eax

	movq -64(%rbp), %rdi /* set rdi to tilemap */
	movl RECT_X_OFFSET(%r8), %esi /* set rsi to tile->x + i */
	addl %r12d, %esi  /*CHECK D vs no d*/
	movl RECT_Y_OFFSET(%r8), %edx /* set rds to tile->y + i */
	addl %r13d, %esi

	call compute_index

	movl %eax, %r11d /* store index_dest in r11d*/
	
	movq -64(%rbp), %rdx 
	movl IMAGE_DATA_OFFSET(%rdx, %r11, 4), %r11d

	movq -96(%rbp), %rdi /* set rdi to img */
	movl %r10d, %esi
	movl %r11d, %edx
	
	call set_pixel

	jmp .Ljump_back
	
.Lend:

	addq $96, %rsp
	popq %rbp

	popq %r13
	popq %r12
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
