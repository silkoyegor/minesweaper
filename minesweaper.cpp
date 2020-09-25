#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <ctype.h> 
#include <math.h>
#include <unistd.h>
using namespace std;


// Game Symbols
const char BOMB = 'x'; 
const char EMPTY = '.'; // не открытое поле (для отображения поля игрока)
const char PREDICT = 'p'; // игрок ожидает, что там мина
const char FAIL_PREDICT = 'f'; // поставлен флажок, но там нет мины
const char OPEN = 'o';

// GameField Options
const int board_size = 11; // действительный размер поля (нужен для предотвращения переноса строк)
const int field_size = board_size - 1; // рабочее поле
char field[board_size][board_size];
bool opened[board_size][board_size];


// Bomb Options
int bombs_quantity = 10;


// Debug Options
const bool SHOW_BOMBS = false;
const bool DELAY = false;
const int DELAY_TIME = 1;


// Game variables
bool GAME_OVER = false;
int OPENED_CELLS_COUNT = (field_size * field_size) - bombs_quantity;

//Functions
//--------------------------------------------------------------
void clear_screen() {
#ifdef WINDOWS
	system("cls");
#else
	//Assume POSIX
	system("clear");
#endif
}

void init_field() {
	for (int row = 0; row < field_size; ++row) {
		for (int col = 0; col < field_size; ++col) {
			field[row][col] = '-';
			opened[row][col] = false;
		}
	}
}


void bomb_generator(int quantity) {

	while (quantity > 0) {
		int rand_x = rand() % (field_size - 1);
		int rand_y = rand() % (field_size - 1);

		
		if (field[rand_x][rand_y] != BOMB) {
			field[rand_x][rand_y] = BOMB;
			quantity--;
		}
	}
}


// search bombs in near cells algorythm 
void bombs_near(int x, int y) {
	
		    if (field[x][y] != BOMB) {
			int bomb_counter = 0;
	
			for (int row = x - 1; row <= x + 1; ++row) {
				for (int col = y - 1; col <= y + 1; ++col) {
					if ((row >= 0 && row < field_size) && (col >= 0 && col < field_size)){
						if (field[row][col] == BOMB || field[row][col] == PREDICT) ++bomb_counter;
					}
				}
			}

			field[x][y] = bomb_counter + '0';
		    }
}

// заполнить всё поле числами количества бомб в соседних клетках
void bomb_count() {
	for (int x = 0; x < field_size; ++x) {
		for (int y = 0; y < field_size; ++y) {
			bombs_near(x, y);
		}
	}
}


// отобразить игровое поле (работает от 0 до 9);
void display() {
	clear_screen();
	// отображаем координатную сетку 
	for (int row = 0; row < 2; ++row) {
		cout << "   ";
		for (int col = 0; col < field_size; ++col) {
			if (row == 0) {
				cout << col << " ";
			} else {
				cout << "=" << " ";
			}
		}
		cout << "\n";
	}

	// отображаем координатную сетку + поле
	for (int row = 0; row < field_size; ++row) {
		cout << row << "| ";
		for (int col = 0; col < field_size; ++col) {
			if ((opened[row][col] == true) || (SHOW_BOMBS && field[row][col] == BOMB)) {	
				if (field[row][col] == FAIL_PREDICT) {
					cout << PREDICT << " ";
				} else {
					cout << field[row][col] << " ";
				}
			} else {
				cout << EMPTY << " ";
			}
		}
		cout << "\n";
	}
	cout << "You need to open: " << OPENED_CELLS_COUNT << " cells" << endl;
	cout << endl;
}


void game_over() {
	for (int row = 0; row < field_size; ++row) {
		for (int col = 0; col < field_size; ++col) {
			if (field[row][col] == BOMB) opened[row][col] = true;
		}
	}
	display();
	cout << "Game Over!" << endl;
}

void game_win() {
	display();
	cout << "You Win! Hope you play it again :)" << endl;

}


void open(int x, int y) {
	// проверка на выход за границы
	if (x < 0 || y < 0 || x >= field_size || y >= field_size) return;
	
	// если открыта мина, то игра кончается
	if (field [x][y] == BOMB) {
		GAME_OVER = true;

		return;
	}

	// предотвращение бесконечной рекурсии, если клетка открыта, то ничего не делаем
	if (opened[x][y]) return;
	
	// открываем клетку и уменьшаем количество открытых клеток на одну
	opened[x][y] = true;
	--OPENED_CELLS_COUNT;


	// делвем пошаговое отображение процесса
	if (DELAY) {
		display();
		sleep(DELAY_TIME);
	}

	// если в клетке нет мин, то проверяем все соседние клетки
	if (field[x][y] == '0') {
		open(x-1, y-1); open(x, y-1); open(x+1, y-1);
		open(x-1, y  );               open(x+1, y  );
		open(x-1, y+1); open(x, y+1); open(x+1, y+1);
	}
}

// функция, запускающая действия, в зависимости от введеного оператора
void mark (char symb, int x, int y) {
	switch (symb) {
		case PREDICT:
			if (opened[x][y] == true) {
				if (field[x][y] == PREDICT) {
					field[x][y] = BOMB;
					opened[x][y] = false;
				} if (field[x][y] == FAIL_PREDICT) {
					bombs_near(x, y);
					opened[x][y] = false;
				}
				//opened[x][y] = false;	
			} else {
				if (field[x][y] == BOMB) {
					field[x][y] = PREDICT;
				} else {
					field[x][y] = FAIL_PREDICT;
				}
				opened[x][y] = true;
			}
			break;
		case OPEN:
			open(x, y);
			break;
		default:
			cout << "Write the right operator" << endl;
			break;
	}
}


void play_game() {

	char symb;
	int x, y; // reverse coordinates. x is horizontal now
	

	do {
		do {
			display();
			cout << OPEN << " - open cell" << endl;
			cout << PREDICT << " - prediction the bomb" << endl;
			cout << "Enter operator and coords [x, y]. For example \""
				<< OPEN << " 2 5\"" << endl;		
			cin >> symb;
		
		} while ( !(symb == OPEN || symb == PREDICT) );
			cin >> x;
			if (cin.fail()) {
				cin.clear();
				//cin.ignore(32767, '\n');
			}
			cin >> y;
			if (cin.fail()) {
				cin.clear();
				//cin.ignore(32767, '\n');
			}
	} while ( !(x >= 0 && x < field_size && y >= 0 && y < field_size) );
//	} while ( !(symb == OPEN || symb == PREDICT) || !(isdigit(x) && isdigit(y)) || !(x >= 0 && x < field_size && y >= 0 && y < field_size) ); 

	mark(symb, y, x);

	if (!GAME_OVER) {
		if (OPENED_CELLS_COUNT > 0) {
		       	play_game(); 
		} else {
			game_win();
		}
	} else {
		game_over();
	}
}

int main () {
	
	init_field();
	srand(time(0));
	bomb_generator(bombs_quantity);
	bomb_count();
	display();
	
	play_game();


	return 0;		
}
