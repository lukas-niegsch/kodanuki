#pragma once

/**
 * The color for some tetromino.
 */
struct Color
{
	// The ncurses color value.
	int ncurses_mod8;
};

/**
 * Initializes the ncurses library for the application.
 */
void initialize_ncurses();

/**
 * Terminates the ncurses library for the application.
 */
void terminate_ncurses();

/**
 * Renders each board using ncurses.
 */
void draw_board_system();

/**
 * Renders each tetromino using ncurses.
 */
void draw_tetromino_system();
