/*
 * Pixel.c
 *
 *  Created on: -----
 *      Author: -----
 */

#include "Pixel.h"

/******use these if you know how*********
//Table for pixel dots. dots[page][X][Y][COLOR]
//volatile uint8_t dots[Page_size][8][8][3]={0};
//volatile uint8_t page=0;
****************************************/

#define CONTROL (uint32_t*)(0x41220008)
#define CHANNEL (uint32_t*)(0x41220000)
#define CTRL_RST (uint8_t)0x1
#define CTRL_LAT (uint8_t)0x2
#define CTRL_SB (uint8_t)0x4
#define CTRL_SCK (uint8_t)0x8
#define CTRL_SDA (uint8_t)0x10

//Table for pixel dots.
//				 dots[X][Y][COLOR]
volatile uint8_t dots[WIDTH][HEIGHT][COLOR_BYTES] = {0};

// Holds current column coordinate.
volatile uint8_t current_x = 0;

// Advance clock once by setting and clearing it.
void advance_clock() {
	*CONTROL |= CTRL_SCK;
	*CONTROL &= ~CTRL_SCK;
}

// Setup led matrix and set black.
void setup() {
	// Reset all control flags.
	*CONTROL = 0x0;
	// Set reset bit high. Reset is active low.
	*CONTROL |= CTRL_RST;
	advance_clock();

	// Switch bank to luminance.
	*CONTROL &= ~CTRL_SB;

	// Write luminance bank full of ones.
	*CONTROL |= CTRL_SDA;
	for(uint8_t i = 0; i < WIDTH; ++i) {
		select_column(i);
		for (uint8_t j = 0; j < COLOR_BYTES * LUMINANCE_W * HEIGHT; ++j) {
			advance_clock();
		}
		latch_column();
	}

	// Switch bank back to colors.
	*CONTROL |= CTRL_SB;

	clear();
}

//Set value of one pixel at led matrix
void set_pixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
	
	// Set new pixel value. Put function paremeter values to dots array at correct places
	dots[x][y][0] = r;
	dots[x][y][1] = g;
	dots[x][y][2] = b;
}


// Shift single column data to led matrix and latch it.
void draw_column(uint8_t x) {

	// Eliminate pixel bleeding.
	*CHANNEL = 0x0;

	// Draw column
	for (uint8_t y = 0; y < HEIGHT; ++y) {
		for (int8_t c = COLOR_BYTES - 1; c >= 0; --c) {
			for (int8_t b = COLOR_W - 1; b >= 0; --b) {
				uint8_t color_bit = dots[x][y][c] & (1 << b);
				if (color_bit) {
					*CONTROL |= CTRL_SDA;
				} else {
					*CONTROL &= ~CTRL_SDA;
				}
				advance_clock();
			}
		}
	}	
	latch_column();
	select_column(x);
}

// Set screen black.
void clear() {
	for (uint8_t x = 0; x < WIDTH; ++x) {
		for (uint8_t y = 0; y < HEIGHT; ++y) {
			for (uint8_t c = 0; c < COLOR_BYTES; ++c) {
				dots[x][y][c] = 0x0;
			}
		}
	}
}

// Latch function locks shift register value as led color.
void latch_column() {
	*CONTROL |= CTRL_LAT;
	*CONTROL &= ~CTRL_LAT;
}

// Set one column as active.
void select_column(uint8_t x) {
	*CHANNEL = (1 << x);
}

// Draws current column and increments to next column.
void draw_next() {
	draw_column(current_x);
	++current_x;
	if (current_x == WIDTH)
		current_x = 0;
}
