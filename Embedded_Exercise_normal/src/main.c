/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 *
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// Main program for exercise

//****************************************************
//By default, every outputs used in this exercise is 0
//****************************************************
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"

//***Hint: Use sleep(x)  or usleep(x) if you want some delays.****
//to call assember code found in blinker.S, call it using "blinker();".


//Comment this if you want to disable all interrupts
#define enable_interrupts

#define SHIP_R 0xff
#define SHIP_G 0x0
#define SHIP_B 0x0
#define ALIEN_R 0x0
#define ALIEN_G 0xff
#define ALIEN_B 0x0
#define PROJ_R 0x0
#define PROJ_G 0x0
#define PROJ_B 0xff
#define VICTORY_R 0x0
#define VICTORY_G 0xff
#define VICTORY_B 0x0
#define DEFEAT_R 0xff
#define DEFEAT_G 0x0
#define DEFEAT_B 0x0

#define INPUTS (uint32_t*)(0xE000A068)
#define BTN0 (uint8_t)0x1
#define BTN1 (uint8_t)0x2
#define BTN3 (uint8_t)0x8

#define MAX_MISSES 5
#define WIN_HITS 5
#define PROJ_START_Y 5

#define RUN_STATE 0
#define VICTORY_STATE 1
#define DEFEAT_STATE 2

volatile uint8_t is_proj_fired;
volatile uint8_t projectile_x;
volatile uint8_t projectile_y;

volatile uint8_t player_hits;
volatile uint8_t player_misses;

volatile uint8_t player_x;
volatile uint8_t alien_x;
volatile uint8_t alien_dir;
volatile uint8_t game_state;

void render_ship(uint8_t x);
void render_alien(uint8_t x);
void render_projectile(uint8_t x, uint8_t y);
void fire(uint8_t x);
void reset_game();

int main()
{
	//**DO NOT REMOVE THIS****
	    init_platform();
	//************************


#ifdef	enable_interrupts
	    init_interrupts();
#endif

	    //setup screen
	    setup();

		reset_game();

	    Xil_ExceptionEnable();


		//Main loop
		while(1) {

		}


		cleanup_platform();
		return 0;
}

void render_ship(uint8_t x) {
	set_pixel(x, 7, SHIP_R, SHIP_G, SHIP_B);
	set_pixel(x + 1, 7, SHIP_R, SHIP_G, SHIP_B);
	set_pixel(x + 2, 7, SHIP_R, SHIP_G, SHIP_B);
	set_pixel(x + 1, 6, SHIP_R, SHIP_G, SHIP_B);
}

void render_alien(uint8_t x) {
	set_pixel(x, 0, ALIEN_R, ALIEN_G, ALIEN_B);
}

void render_projectile(uint8_t x, uint8_t y) {
	set_pixel(x, y, PROJ_R, PROJ_G, PROJ_B);
}

void render_victory() {
	set_pixel(2, 2, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(5, 2, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(1, 5, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(2, 6, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(3, 6, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(4, 6, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(5, 6, VICTORY_R, VICTORY_G, VICTORY_B);
	set_pixel(6, 5, VICTORY_R, VICTORY_G, VICTORY_B);
}

void render_defeat() {
	set_pixel(2, 2, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(5, 2, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(1, 6, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(2, 5, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(3, 5, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(4, 5, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(5, 5, DEFEAT_R, DEFEAT_G, DEFEAT_B);
	set_pixel(6, 6, DEFEAT_R, DEFEAT_G, DEFEAT_B);
}

void fire(uint8_t x) {
	is_proj_fired = 1;
	projectile_x = x;
	projectile_y = PROJ_START_Y;
}

void reset_game() {
	is_proj_fired = 0;
	projectile_x = 0;
	projectile_y = 0;

	player_hits = 0;
 	player_misses = 0;

 	player_x = 0;
	alien_x = 0;
	alien_dir = 1;
	game_state = 0;
}

//Interrupt handler for switches and buttons. Connected buttons and switches are at bank2. Reading Status will tell which button or switch was used
void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	if (game_state == RUN_STATE) {
		if (Status == BTN0 && player_x < WIDTH - 3) {
			player_x++;
		} else if (Status == BTN1 && player_x > 0) {
			player_x--;
		} else if (Status == BTN3 && !is_proj_fired) {
			fire(player_x + 1);
		}
	} else {
		reset_game();
	}
}

//Timer interrupt handler for led matrix update. Frequency is 800Hz
void TickHandler(void *CallBackRef) {
	//Don't remove this
	uint32_t StatusEvent;

	//exceptions must be disabled when updating screen
	Xil_ExceptionDisable();

	//****Write code here ****

	clear();
	if (game_state == VICTORY_STATE) {
		render_victory();
	} else if (game_state == DEFEAT_STATE) {
		render_defeat();
	} else {
		render_alien(alien_x);
		render_ship(player_x);
		if (is_proj_fired)
			render_projectile(projectile_x, projectile_y);
	}
	draw_next();

	//****END OF OWN CODE*****************

	//*********clear timer interrupt status. DO NOT REMOVE********
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	//*************************************************************
	//enable exeptions
	Xil_ExceptionEnable();
}


//Timer interrupt for moving alien, shooting... Frequency is 10Hz by default
void TickHandler1(void *CallBackRef) {

	//Don't remove this
	uint32_t StatusEvent;

	//****Write code here ****

	if (alien_x == WIDTH - 1)
		alien_dir = -1;
	else if (alien_x == 0)
		alien_dir = 1;
	alien_x += alien_dir;

	if (is_proj_fired) {
		if (projectile_y == 0) {
			// Player point counting.
			if (alien_x == projectile_x)
				player_hits++;
			else
				player_misses++;
			
			is_proj_fired = 0;

			// Game state checking.
			if (player_hits == WIN_HITS)
				game_state = VICTORY_STATE;
			if (player_misses == MAX_MISSES)
				game_state = DEFEAT_STATE;
		}
		else
			projectile_y--;
	}

	//****END OF OWN CODE*****************
	//clear timer interrupt status. DO NOT REMOVE
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);

}
