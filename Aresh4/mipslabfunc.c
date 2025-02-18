/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
   
   Latest update 2024-12-09 by Z Arash, PJ Jacob

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

#define GRID_WIDTH 128
#define GRID_HEIGHT 29



   /* Declare a helper function which is local to this file */
static void num32asc(char* s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

extern uint8_t display_buffer[4][128];

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for (i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick(unsigned int* timep)
{
	/* Get current value, store locally */
	register unsigned int t = *timep;
	t += 1; /* Increment local copy */

	/* If result was not a valid BCD-coded time, adjust now */

	if ((t & 0x0000000f) >= 0x0000000a) t += 0x00000006;
	if ((t & 0x000000f0) >= 0x00000060) t += 0x000000a0;
	/* Seconds are now OK */

	if ((t & 0x00000f00) >= 0x00000a00) t += 0x00000600;
	if ((t & 0x0000f000) >= 0x00006000) t += 0x0000a000;
	/* Minutes are now OK */

	if ((t & 0x000f0000) >= 0x000a0000) t += 0x00060000;
	if ((t & 0x00ff0000) >= 0x00240000) t += 0x00dc0000;
	/* Hours are now OK */

	if ((t & 0x0f000000) >= 0x0a000000) t += 0x06000000;
	if ((t & 0xf0000000) >= 0xa0000000) t = 0;
	/* Days are now OK */

	*timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int* const addr)
{
	display_string(1, "Addr");
	display_string(2, "Data");
	num32asc(&textbuffer[1][6], (int)addr);
	num32asc(&textbuffer[2][6], *addr);
	display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while (!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while (!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

void display_string(int line, char* s) {
	int i;
	if (line < 0 || line >= 4)
		return;
	if (!s)
		return;

	for (i = 0; i < 16; i++)
		if (*s) {
			textbuffer[line][i] = *s;
			s++;
		}
		else
			textbuffer[line][i] = ' ';
}void clear_display()
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 128; j++)
		{
			display_buffer[i][j] = 0;
		}
	}
	display_update();
}

void set_pixel(int x, int y) {
	if (x < 0 || x >= 128 || y < 0 || y >= 32)
	{
		return;
	}
	int column = y / 8;

	display_buffer[column][x] |= (1 << (y % 8));
	display_update();

}

void display_update(void) {
	int i, j, k;
	int c;
	for (i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for (j = 0; j < 128; j++) {
			spi_send_recv(display_buffer[i][j]);
		}
	}
}

void display_string_update(void) {
	int i, j, k;
	int c;
	for (i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for (j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if (c & 0x80)
				continue;

			for (k = 0; k < 8; k++)
				spi_send_recv(font[c * 8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc(char* s, int n)
{
	int i;
	for (i = 28; i >= 0; i -= 4)
		*s++ = "0123456789ABCDEF"[(n >> i) & 15];
}


#define ITOA_BUFSIZ ( 24 )
char* itoaconv(int num)
{
	register int i, sign;
	static char itoa_buffer[ITOA_BUFSIZ];
	static const char maxneg[] = "-2147483648";

	itoa_buffer[ITOA_BUFSIZ - 1] = 0;   /* Insert the end-of-string marker. */
	sign = num;                           /* Save sign. */
	if (num < 0 && num - 1 > 0)          /* Check for most negative integer */
	{
		for (i = 0; i < sizeof(maxneg); i += 1)
			itoa_buffer[i + 1] = maxneg[i];
		i = 0;
	}
	else
	{
		if (num < 0) num = -num;           /* Make number positive. */
		i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
		do {
			itoa_buffer[i] = num % 10 + '0';/* Insert next digit. */
			num = num / 10;                   /* Remove digit from number. */
			i -= 1;                           /* Move index to next empty position. */
		} while (num > 0);
		if (sign < 0)
		{
			itoa_buffer[i] = '-';
			i -= 1;
		}
	}
	/* Since the loop always sets the index i to the next empty position,
	 * we must add 1 in order to return a pointer to the first occupied position. */
	return(&itoa_buffer[i + 1]);
}



int block_x = 0;
int block_y = GRID_HEIGHT / 2;

int last_block_x = 0;
int last_block_y = GRID_HEIGHT / 2;

uint8_t grid[GRID_WIDTH][GRID_HEIGHT] = { 0 };

typedef struct {
	int size;           // The size of the shape matrix (e.g., 4 for a 4x4 matrix)
	int data[8][8];     // The shape matrix representing the tetromino
} Shape;

// Define the tetromino shapes
Shape shapes[] = {
	// I-shape
	{
		8,
		{
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{1, 1, 1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1, 1, 1},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0}

		}
	},
	// O-shape
	{
		4,
		{
			{1, 1, 1, 1},
			{1, 1, 1, 1},
			{1, 1, 1, 1},
			{1, 1, 1, 1}
		}
	},
	// T-shape
	{
		6,
		{
			{0, 0, 1, 1, 0, 0},
			{0, 0, 1, 1, 0, 0},
			{1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		}
	},
	// S-shape
	{
		6,
		{
			{0, 0, 1, 1, 1, 1},
			{0, 0, 1, 1, 1, 1},
			{1, 1, 1, 1, 0, 0},
			{1, 1, 1, 1, 0, 0},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		}
	},
	// Z-shape
	{
		6,
		{
			{1, 1, 1, 1, 0, 0},
			{1, 1, 1, 1, 0, 0},
			{0, 0, 1, 1, 1, 1},
			{0, 0, 1, 1, 1, 1},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		}
	},
	// J-shape
	{
		6,
		{
			{1, 1, 0, 0, 0, 0},
			{1, 1, 0, 0, 0, 0},
			{1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		}
	},
	// L-shape
	{
		6,
		{
			{0, 0, 0, 0, 1, 1},
			{0, 0, 0, 0, 1, 1},
			{1, 1, 1, 1, 1, 1},
			{1, 1, 1, 1, 1, 1},
			{0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0}
		}
	}
};


Shape current_shape;
int shape_x = 14; // Start position (centered horizontally)
int shape_y = 0;              // Start at the top of the grid

/* Simple pseudo-random number generator */
int random_number(int min, int max) {
	static unsigned int seed = 5323; // You can use any seed value you prefer
	seed = (8253729 * seed + 2396403);
	return min + (seed % (max - min + 1));
}



// Function to select a random shape
void select_new_shape() {
	int rand_index = random_number(0, 6); // Assuming 7 shapes indexed from 0 to 6
	current_shape = shapes[rand_index];
	shape_x = 14;
	shape_y = 0;
}

int score = 0;
int get_score() {
	return score;
}

void clear_grid() {
	int y, x;

	for (y = 0; y < GRID_WIDTH; y++) {
		for (x = 0; x < GRID_HEIGHT; x++) {
			grid[y][x] = 0;
		}
	}
	score = 0;
}

void clear_block(int x, int y) {
	if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
		int row = y / 8; // Calculate which row in the display buffer
		display_buffer[row][x] &= ~(1 << (y % 8)); // Clear the specific bit
	}
}
void draw_shape(int x, int y, Shape* shape) {
	int i, j;
	for (i = 0; i < shape->size; i++) {
		for (j = 0; j < shape->size; j++) {
			if (shape->data[i][j]) {
				set_pixel(y + i, x + j); // Draw each block of the shape
			}
		}
	}
}

void clear_shape(int x, int y, Shape* shape) {
	int i, j;
	for (i = 0; i < shape->size; i++) {
		for (j = 0; j < shape->size; j++) {
			if (shape->data[i][j]) {
				clear_block(y + i, x + j); // Clear each block of the shape
			}
		}
	}
}

_Bool rotate_button_pressed = 0;  // Flag to track button press for rotation

void rotate_shape_90_deg() {
	int n = current_shape.size;  // Get the size of the matrix (size is nxn)

	Shape temp;
	temp.size = current_shape.size;

	int i, j;
	// Rotate the matrix 90 degrees
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			temp.data[j][n - 1 - i] = current_shape.data[i][j];  // Rotate 90 degrees clockwise
		}
	}

	if (can_rotate(shape_y, shape_x, temp) == 1) {
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (grid[i][j] == 1) {
					return;  // If there's a collision, don't rotate
				}
			}
		}

		// Copy the rotated values back to the shape's data
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				current_shape.data[i][j] = temp.data[i][j];
			}
		}

		draw_shape(shape_x, shape_y, &current_shape);
		display_update();
	}
}


int check_collision(int x, int y) {
	//add ro sides


	if (x >= GRID_WIDTH - 1) {
		return 1;
	}


	if (grid[x + 1][y] == 1) {
		return 1;
	}

	return 0;
}






/*
// Function to draw the block at its current position
void draw_block(int x, int y) {
	if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
		set_pixel(x, y); // Set the pixel for the block
	}
}
*/




void clear_completed_rows() {
	int x, y, write_y;


	// Start at the bottom of the grid and work upwards
	write_y = GRID_WIDTH - 1; // Target row for writing valid rows

	for (y = GRID_WIDTH - 1; y >= 0; y--) {
		int row_count = 0;

		// Count the number of 1's in the current row
		for (x = 0; x < GRID_HEIGHT; x++) {
			if (grid[y][x] == 1) {
				row_count++;
			}
		}

		if (row_count < GRID_HEIGHT - 1) {
			// If the row is not complete, copy it to the write_y position
			if (y != write_y) { // Avoid redundant copy to the same row
				for (x = 0; x < GRID_HEIGHT; x++) {
					grid[write_y][x] = grid[y][x];
				}
			}
			write_y--; // Move the write pointer up
		}
		else {
			score++;
		}


	}

	// Clear all rows above the write_y pointer
	for (y = write_y; y >= 0; y--) {
		for (x = 0; x < GRID_HEIGHT; x++) {
			grid[y][x] = 0;
		}
	}
}

//we need to implmenet can move right and left
int can_move_down(int x, int y, Shape* shape) {
	int i, j;
	for (i = 0; i < shape->size; i++) {
		for (j = 0; j < shape->size; j++) {
			if (shape->data[i][j]) {
				int new_x = x + j;
				int new_y = y + i + 1;
				// Check boundaries and grid occupancy
				if (new_y >= GRID_WIDTH || grid[new_y][new_x]) {
					return 0; // Collision detected
				}
			}
		}
	}
	return 1; // No collision
}

int can_move_right(int y, int x, Shape* shape) {
	int i, j;
	for (i = 0; i < shape->size; i++) {
		for (j = 0; j < shape->size; j++) {
			if (shape->data[i][j]) {
				int new_x = x + j - 1;
				int new_y = y + i;
				// Check boundaries and grid occupancy
				if (new_x <= 0 || grid[new_y][new_x]) {
					return 0; // Collision detected
				}
			}
		}
	}
	return 1; // No collision
}

int can_move_left(int y, int x, Shape* shape) {
	int i, j;
	for (i = 0; i < shape->size; i++) {
		for (j = 0; j < shape->size; j++) {
			if (shape->data[i][j]) {
				int new_x = x + j + 1;
				int new_y = y + i;
				// Check boundaries and grid occupancy
				if (new_x >= GRID_HEIGHT || grid[new_y][new_x]) {
					return 0; // Collision detected
				}
			}
		}
	}
	return 1; // No collision
}
int can_rotate(int y, int x, Shape shape) {
	int i, j;
	for (i = 0; i < shape.size; i++) {
		for (j = 0; j < shape.size; j++) {
			if (shape.data[i][j]) {
				// Check boundaries and grid occupancy
				if (x + j >= GRID_HEIGHT) {
					return 0; // Collision detected
				}
				else if (x - j <= 0) {
					return 0;
				}
				else if (y + i >= GRID_WIDTH) {
					return 0;
				}
				else if (grid[y][x]) {
					return 0;
				}
			}
		}
	}
	return 1; // No collision
}


void update_falling_block(int speed) {
	static int counter = 0;

	if (counter >= 15000 / speed) { // Adjust SPEED as needed
		clear_shape(shape_x, shape_y, &current_shape);

		if (can_move_down(shape_x, shape_y, &current_shape)) {
			shape_y++; // Move shape down
		}
		else {
			// Fix the shape in the grid
			int i, j;
			for (i = 0; i < current_shape.size; i++) {
				for (j = 0; j < current_shape.size; j++) {
					if (current_shape.data[i][j]) {
						grid[shape_y + i][shape_x + j] = 1;
					}
				}
			}

			// Check for completed rows


			// Start a new shape
			select_new_shape();

			// Check for game over condition
			if (!can_move_down(shape_x, shape_y, &current_shape)) {
				// Implement game over logic
				display_string(1, "Game Over");
				display_update();
				while (1); // Halt execution
			}

			clear_completed_rows();
			render_grid();
		}

		draw_shape(shape_x, shape_y, &current_shape);
		display_update();
		counter = 0;
	}
	counter++;
}

int game_over()
{
	int x, y;
	int t = 0;

	if (shape_y <= 2 && !can_move_down(shape_x, shape_y, &current_shape)) {
		return 1;
	}

	return 0;
}


void render_grid() {
	int x, y; // Declare loop variables for C89 compatibility

	// Clear the display buffer
	clear_display();

	draw_shape(shape_x, shape_y, &current_shape);

	for (x = 0; x < 128; x++) {
		for (y = 0; y < GRID_HEIGHT; y++) {
			if (grid[x][y] == 1) {
				set_pixel(x, y);
			}
		}
	}


	// Update the display with the new buffer
	display_update();
}



void test_grid_display() {
	int x, y;
	for (y = 0; y <= GRID_HEIGHT; y++) {     // Rows (height)
		for (x = 0; x <= 128; x++) {  // Columns (width)

			grid[x][y] = 0; // Set the pixel for the occupied cell

		}
	}
	// Manually populate the grid with some blocks
	grid[30][5] = 1;
	grid[30][9] = 1;
	grid[30][15] = 1;
	grid[80][3] = 1;
	grid[100][5] = 1;





	// Render the grid
	render_grid();
}
void 	test_display_as_cartesian() {
	display_string(0, "Starting Test");
	display_update();
	quicksleep(2000000);
	clear_display();
	quicksleep(2000000);
	set_pixel(10, 10);
	set_pixel(80, 20);
	set_pixel(30, 31);
	set_pixel(30, 20);
	set_pixel(30, 10);
	set_pixel(120, 10);
	display_update();
	quicksleep(2000000);

}

void display_game_over() {
	int x, y;
	for (x = 0; x < GRID_WIDTH; x++)
	{
		for (y = 0; y < GRID_HEIGHT; y++)
		{
			grid[x][y] = 1;
		}
	}
	render_grid();
	display_update();
}


void labworker(void) {

	select_new_shape(); // Initialize the first shape

	while (1) {
		handle_input();
		update_falling_block(1);
		quicksleep(10); // Adjust as needed for game speed
		if (game_over() == 1) {

			return;
		}
	}
}

_Bool move_left_button_pressed = 0;
_Bool move_right_button_pressed = 0;

void handle_input() {
	int buttons = (*((volatile int*)0xbf8860d0) >> 5);



	if (buttons & 0x4) {  // btn4 - move left
		if (!move_left_button_pressed) {  // Ensure action happens only once per button press
			if (can_move_right(shape_y, shape_x, &current_shape)) {
				clear_shape(shape_x, shape_y, &current_shape);
				shape_x -= 1;
				draw_shape(shape_x, shape_y, &current_shape);
				display_update();
			}
			move_left_button_pressed = 1;  // Set flag to prevent continuous action
		}
	}
	else {
		move_left_button_pressed = 0;  // Reset flag when button is released
	}

	if (buttons & 0x2) {  // btn3 - move right
		if (!move_right_button_pressed) {  // Ensure action happens only once per button press
			if (can_move_left(shape_y, shape_x, &current_shape)) {
				clear_shape(shape_x, shape_y, &current_shape);
				shape_x += 1;
				draw_shape(shape_x, shape_y, &current_shape);
				display_update();
			}
			move_right_button_pressed = 1;  // Set flag to prevent continuous action
		}
	}
	else {
		move_right_button_pressed = 0;  // Reset flag when button is released
	}

	if (buttons & 0x1) {  // btn1 - rotate shape
		if (!rotate_button_pressed) {  // Ensure rotation only happens once per button press
			clear_shape(shape_x, shape_y, &current_shape);  // Clear the shape before rotating
			rotate_shape_90_deg();  // Rotate the shape
			rotate_button_pressed = 1;  // Set flag to prevent continuous rotation
		}
	}
	else {
		rotate_button_pressed = 0;
	}

	int switchState = (PORTD >> 11) & 1;
	if (switchState == 1) {
		update_falling_block(500);
	}
}

int starts() {
	int buttons = (*((volatile int*)0xbf8860d0) >> 5);

	if (buttons & 0x4) {  // btn4 - move left
		return 1;
	}
}
