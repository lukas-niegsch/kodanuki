#include "graphics.h"
#include "board.h"
#include "tetromino.h"
#include "movement.h"
#include "engine/central/entity.h"
#include "engine/central/archetype.h"
#include <ncurses.h>
using namespace Kodanuki;

void initialize_ncurses()
{
	initscr();
	curs_set(0);
	noecho();
	timeout(50);
	start_color();

	for (int color = 0; color < 8; color++) {
		init_pair(color, color, color);
	}
}

void terminate_ncurses()
{
	endwin();
}

void draw_box(int x1, int y1, int x2, int y2)
{
	mvhline(y1, x1, 0, x2 - x1);
	mvhline(y2, x1, 0, x2 - x1);
	mvvline(y1, x1, 0, y2 - y1);
	mvvline(y1, x2, 0, y2 - y1);
	mvaddch(y1, x1, ACS_ULCORNER);
	mvaddch(y2, x1, ACS_LLCORNER);
	mvaddch(y1, x2, ACS_URCORNER);
	mvaddch(y2, x2, ACS_LRCORNER);
}

void draw_board_system()
{
	using System = Archetype<Iterator<Board>, Predicate<>>;
	for (auto[board] : ECS::iterate<System>()) {
		mvaddstr(board.offsetY - 2, 2 * board.offsetX - 1, std::string(2 * board.sizeX + 2, ' ').c_str());
		draw_box(2 * board.offsetX - 1, board.offsetY - 1, 2 * (board.offsetX + board.sizeX), board.offsetY + board.sizeY);
		for (int x = 0; x < board.sizeX; x++) {
			for (int y = 0; y < board.sizeY; y++) {
				int globalX = 2 * (board.offsetX + x);
				int globalY = board.offsetY + y;
				int index = y * board.sizeX + x;

				if (board.isblock[index]) {
					attron(COLOR_PAIR(board.isblock[index]));
					mvaddch(globalY, globalX, ' ');
					mvaddch(globalY, globalX + 1, ' ');
					attroff(COLOR_PAIR(board.isblock[index]));
				} else {
					mvaddch(globalY, globalX, ':');
					mvaddch(globalY, globalX + 1, ':');
				}
			}
		}
	}
}

void draw_tetromino_system()
{
	using System = Archetype<Iterator<Tetromino, Color, Position, Board>, Predicate<>>;
	for (auto[tetromino, color, position, board] : ECS::iterate<System>()) {
		attron(COLOR_PAIR(color.ncurses_mod8));
		execute_blockwise(tetromino, [&](int x, int y) {
			int globalX = 2 * (board.offsetX + position.x + x);
			int globalY = board.offsetY + position.y + y;
			mvaddch(globalY, globalX, ' ');
			mvaddch(globalY, globalX + 1, ' ');
		});
		attroff(COLOR_PAIR(color.ncurses_mod8));
	}
}
