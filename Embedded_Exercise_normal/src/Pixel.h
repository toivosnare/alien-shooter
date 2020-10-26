/*
 * Pixel.h
 *
 *  Created on: ------
 *      Author: ------
 */

#ifndef PIXEL_H_
#define PIXEL_H_

#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"

//size can be changed if needed
#define Page_size 10

#define WIDTH 8
#define HEIGHT 8
#define COLOR_BYTES 3
#define LUMINANCE_W 6
#define COLOR_W 8


void setup();
void set_pixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void run(uint8_t c);
void draw_next();
void latch_column();
void draw_column(uint8_t x);
void select_column(uint8_t x);
void clear();

#endif /* PIXEL_H_ */
