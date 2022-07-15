#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dos.h>
#include <dir.h>

#define cmd_screen_size "MODE CON COLS=140 LINES=43"
#define cmd_menu_size   "MODE CON COLS=50 LINES=8"

#define SCREEN_MAX_X 140
#define SCREEN_MAX_Y 43
#define MAX_ROW      19
#define MAX_COL      28
#define WIDTH        3
#define GAME_SPEED   12

#define GRAY        8
#define GREEN       10
#define RED         12
#define YELLOW      14
#define WHITE       15

const char MENU[][35] = {"Play game", "Setting", "Quit"};
const int MENU_SIZE = sizeof(MENU) / sizeof(MENU[0]);
const char OPTION[][35] = {"easy   {col=10 row=10 bom=10}", "normal {col=17 row=14 bom=40}", "hard   {col=24 row=19 bom=100}", "custom"};
const int OPTION_SIZE = sizeof(OPTION) / sizeof(OPTION[0]);
int map[MAX_ROW][MAX_COL], bom[MAX_ROW][MAX_COL], choose[MAX_ROW][MAX_COL];
int row = 10;
int col = 15;
int max_bom = 20;

struct toado
{
	int x, y;
};

void textcolor(int ForgC)
{
	WORD wColor;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if(!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h,c);
}

void create_empty_arr(int arr[MAX_ROW][MAX_COL])
{
	int i, j;
	for (i = 0; i < MAX_ROW; i++)
	{
		for (j = 0; j < MAX_COL; j++)
		{
			arr[i][j] = 0;
		}
	}
}

void create_bom()
{
	int i;
	for (i = 1; i <= max_bom; i++)
	{
		int y, x;
		do
		{
			x = rand() % col;
			y = rand() % row;
		}
		while (bom[y][x] == 1);
		bom[y][x] = 1;
	}
}

void ctc(int x, int y)
{
	int i, j;
	for (i = y - 1; i <= y + 1; i++)
	{
		if (i < 0 || i >= row) continue;
		for (j = x - 1; j <= x + 1; j++)
		{
			if (j < 0 || j >= col) continue;
			map[i][j]++;
		}
	}
}

void create_map()
{
	int i, j;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if (bom[i][j] == 1)
			{
				ctc(j, i);
			}
		}
	}
}

void chuyen_toa_do(int * x, int * y)
{
	int startX = SCREEN_MAX_X/2 - (col*(WIDTH + 1) + 1)/2;
	int startY = SCREEN_MAX_Y/2 - (row*(1 + 1) + 1)/2;
	*x = startX + 4 * *x + 1;
	*y = startY + 2 * *y + 1;
}

void print_cell(int x, int y, int color, int xoa, int value) // xoa == 0 ->print value | xoa == 1 -> print o
{
	if (x < 0 || y < 0 || x > col - 1 || y > row - 1)
	{
		return;
	}

	textcolor(color);
	chuyen_toa_do(&x, &y);
	if (xoa == 0 || xoa == -1) // print o
	{
		gotoxy(x, y);
		printf("%c%c%c", 219, 219, 219);
	}
	else
	{
		gotoxy(x, y);
		printf(" %d ", value);
	}
}

void print_map()
{
	int i, j, x, y;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			print_cell(j, i, GREEN, choose[i][j], map[i][j]);
		}
	}
}

void print_border()
{
	int startX = SCREEN_MAX_X/2 - (col*(WIDTH + 1) + 1)/2;	
	int startY = SCREEN_MAX_Y/2 - (row*(1 + 1) + 1)/2;

	// 4 goc
	textcolor(WHITE);

	gotoxy(startX, startY);
	printf("%c", 218);
	gotoxy(startX + 4 * col, startY);
	printf("%c", 191);
	gotoxy(startX, startY + 2 * row);
	printf("%c", 192);
	gotoxy(startX + 4 * col, startY + 2 * row);
	printf("%c", 217);
	// tren duoi
	int x;
	for (x = 1; x <= col - 1; x++)
	{

		gotoxy(startX + 4 * x, startY);
		printf("%c", 194);
		gotoxy(startX + 4 * x, startY + 2 * row);
		printf("%c", 193);
	}
	int y;

	// trai phai
	for (y = 1; y <= row - 1; y++)
	{

		gotoxy(startX + 0, startY + 2 * y);
		printf("%c", 195);
		gotoxy(startX + 4 * col, startY + 2 * y);
		printf("%c", 180);
	}
	// ben trong
	for (y = 1; y <= row - 1; y++)
	{
		for (x = 1; x <= col - 1; x++)
		{

			gotoxy(startX + 4 * x, startY + 2 * y);
			printf("%c", 197);
		}
	}
	// --- ngang
	for (y = 0; y <= row; y++)
	{
		for (x = 0; x < col; x++)
		{

			gotoxy(startX + 4 * x + 1, startY + 2 * y);
			printf("%c%c%c", 196, 196, 196);
		}
	}
	// | doc
	for (y = 1; y <= row; y++)
	{
		for (x = 0; x <= col; x++)
		{

			gotoxy(startX + 4 * x, startY + 2 * y - 1);
			printf("%c", 179);
		}
	}
}

void print_bom()
{
	int i, j, x, y;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			if (choose[i][j] == -1)
			{
				textcolor(YELLOW);
			}
			else
			{
				textcolor(RED);
			}
			if (bom[i][j] == 1)
			{
				y = i;
				x = j;
				chuyen_toa_do(&x, &y);
				gotoxy(x, y);
				printf(" %c ", 235);
			}
		}
	}
}

void trang_thai(int x, int y, int opened, int nghingo)
{
	textcolor(WHITE);
	gotoxy(SCREEN_MAX_X/2 - 20, SCREEN_MAX_Y/2 + (row*(1 + 1) + 2)/2);
	printf("Y=%02d  X=%02d  OPEN=%d  BOM=%d  BOOKMARK=%02d ", y, x, opened, max_bom, nghingo);
}

void init(struct toado * vitri)
{
	srand(time(NULL));
	create_empty_arr(map);
	create_empty_arr(bom);
	create_empty_arr(choose);

	create_bom();
	create_map();

	print_border();
	print_map();

	vitri->x = 0;
	vitri->y = 0;

	print_cell(0, 0, WHITE, 0, 0);

	trang_thai(0, 0, 0, 0);
}

int moveX[] = {-1, -1, -1,  0, 0,  1, 1, 1};
int moveY[] = {-1,  0,  1, -1, 1, -1, 0, 1};
void dequi_molancan(int x, int y, int * opened, int nghingo)
{
	if (y < 0 || x < 0 || y > row - 1 || x > col - 1 || choose[y][x] == 1) return;

	choose[y][x] = 1;
	(*opened)++;
	print_cell(x, y, GREEN, 1, map[y][x]);
	trang_thai(x, y, *opened, nghingo);
	Sleep(GAME_SPEED);

	if (map[y][x] != 0) return;

	int i;
	for (i = 0; i < 8; i++)
	{
		dequi_molancan(x + moveX[i], y + moveY[i], opened, nghingo);
	}
}

void xuli_mo(struct toado vitri, int * endgame, int * opened, int nghingo)
{
	int x = vitri.x;
	int y = vitri.y;

	if (choose[y][x] == 0 || choose[y][x] == -1) // chua mo --> mo
	{
		(*opened)++;
		choose[y][x] = 1;
		print_cell(x, y, WHITE, 1, map[y][x]);

		if (bom[y][x] == 1) // mo trung bom
		{
			*endgame = 1;
			(*opened)--;
		}
		else // k mo trung min
		{
			if (map[y][x] == 0)
			{
				(*opened)--;
				choose[y][x] = 0;
				dequi_molancan(x, y, opened, nghingo);
			}
		}
	}
}

void play_game()
{
	system(cmd_screen_size);
	system("cls");
	char c = ' ';
	struct toado vitri;
	int endgame = 0;
	int opened = 0;
	int nghingo = 0;
	init(&vitri);

	while (1)
	{
		c = getch();

		if (choose[vitri.y][vitri.x] == 0 || choose[vitri.y][vitri.x] == 1) // vi tri binh thuong
		{
			print_cell(vitri.x, vitri.y, GREEN, choose[vitri.y][vitri.x], map[vitri.y][vitri.x]);
		}
		else // vi tri nghi ngo
		{
			print_cell(vitri.x, vitri.y, YELLOW, choose[vitri.y][vitri.x], map[vitri.y][vitri.x]);
		}

		switch (c)
		{
			case 13: // phim enter
				xuli_mo(vitri, &endgame, &opened, nghingo);
				break;
			case '0':
				endgame = 1;
				break;
			case ' ':
				if (choose[vitri.y][vitri.x] == 0)
				{
					nghingo++;
					choose[vitri.y][vitri.x] = -1; // dat nghi van
					print_cell(vitri.x, vitri.y, YELLOW, choose[vitri.y][vitri.x], map[vitri.y][vitri.x]);
				}
				else
				{
					if (choose[vitri.y][vitri.x] == -1)
					{
						nghingo--;
						choose[vitri.y][vitri.x] = 0; // huy nghi van
						print_cell(vitri.x, vitri.y, WHITE, choose[vitri.y][vitri.x], map[vitri.y][vitri.x]);
					}
				}
				break;
			case 72: // len
				vitri.y--;
				break;
			case 75: // trai
				vitri.x--;
				break;
			case 77: // phai
				vitri.x++;
				break;
			case 80: // xuong
				vitri.y++;
				break;
		}

		if (endgame == 1)
		{
			gotoxy(SCREEN_MAX_X/2 - 4, SCREEN_MAX_Y/2 + (row * (1 + 1) + 4)/2);
			textcolor(RED);
			printf("YOU LOSE");
			break;
		}
		if (opened == row*col - max_bom)
		{
			trang_thai(vitri.x, vitri.y, opened, nghingo);
			gotoxy(SCREEN_MAX_X/2 - 4, SCREEN_MAX_Y/2 + (row * (1 + 1) + 4)/2);
			textcolor(YELLOW);
			printf("YOU WIN");
			getch();
			break;
		}

		if (vitri.x < 0) vitri.x = col - 1;
		if (vitri.y < 0) vitri.y = row - 1;
		if (vitri.x > col - 1) vitri.x = 0;
		if (vitri.y > row - 1) vitri.y = 0;

		print_cell(vitri.x, vitri.y, WHITE, choose[vitri.y][vitri.x], map[vitri.y][vitri.x]);
		trang_thai(vitri.x, vitri.y, opened, nghingo);
	}
	print_bom();
	getch();
}

void print_menu(int key, const char menu[][35], int menu_size)
{
	int i;
//	system("cls");
	gotoxy(0, 0);
	for (i = 0; i < menu_size; i++)
	{
		if (i == key)
		{
			textcolor(YELLOW);
			printf(">> ");
		}
		else
		{
			textcolor(WHITE);
			printf("   ");
		}
		puts(menu[i]);
	}
}

int menu_process(const char menu[][35], int menu_size) // return key
{
	system(cmd_menu_size);
	system("cls");
	int key = 0;
	char c = ' ';
	print_menu(0, menu, menu_size);
	while (c != 13)
	{
		c = getch();
		switch (c)
		{
			case 72:
				key--;
				break;
			case 80:
				key++;
				break;
		}
		if (key < 0) key = menu_size - 1;
		if (key > menu_size - 1) key = 0;
		print_menu(key, menu, menu_size);
	}
	return key;
}

int play_again()
{
	const char ASK[][35] = {"Play again", "Back to menu", "Quit"};
	return menu_process(ASK, sizeof(ASK) / sizeof(ASK[0]));
}

void print_custom(int key, const char CUSTOM[][35], int CUSTOM_SIZE)
{
	system("cls");
	int i;
	for (i = 0; i < CUSTOM_SIZE; i++)
	{
		if (key == i)
		{
			textcolor(YELLOW);
			printf(">> ");
		}
		else
		{
			textcolor(WHITE);
			printf("   ");
		}
		printf("%s     ", CUSTOM[i]);
		switch (i)
		{
			case 0:
				printf("%c %d %c", 174, row, 175);
				break;
			case 1:
				printf("%c %d %c", 174, col, 175);
				break;
			case 2:
				printf("%c %d %c", 174, max_bom, 175);
				break;
		}
		printf("\n");
	}
}

void custom()
{
	const char CUSTOM[][35] = {"row", "col", "bom", "back to menu"};
	const int CUSTOM_SIZE = sizeof(CUSTOM) / sizeof(CUSTOM[0]);
	int key = 0;
	char c = ' ';
	print_custom(0, CUSTOM, CUSTOM_SIZE);

	while (1)
	{
		c = getch();
		switch (c)
		{
			case 13:
				if (key == 3) goto exit_loop;
			case 72:
				key--;
				break;
			case 75:
				if (key == 0) row--;
				if (key == 1) col--;
				if (key == 2) max_bom--;
				break;
			case 77:
				if (key == 0) row++;
				if (key == 1) col++;
				if (key == 2) max_bom++;
				break;
			case 80:
				key++;
				break;
		}

		if (key < 0) key = CUSTOM_SIZE - 1;
		if (key > CUSTOM_SIZE - 1) key = 0;
		if (row < 3) row = 3;
		if (row > MAX_ROW) row = MAX_ROW;
		if (col < 3) col = 3;
		if (col > MAX_COL) col = MAX_COL;
		if (max_bom < 1) max_bom = 1;
		if (max_bom > col*row/3) max_bom = col*row/3;
		print_custom(key, CUSTOM, CUSTOM_SIZE);
	}
	exit_loop:;
}

void option()
{
	int k = menu_process(OPTION, OPTION_SIZE);
	switch (k)
	{
		case 0:
			col = 10;
			row = 10;
			max_bom = 10;
			break;
		case 1:
			col = 17;
			row = 14;
			max_bom = 40;
			break;
		case 2:
			col = MAX_COL;
			row = MAX_ROW;
			max_bom = 100;
			break;
		case 3:
			custom();
			break;
	}
}

int main()
{
	home:;
	int k = menu_process(MENU, MENU_SIZE);
	switch (k)
	{
		case 0: // play game
			again:;
			play_game();
			switch (play_again())
			{
				case 0: // play again
					goto again;
					break;
				case 1: // menu
					goto home;
					break;
				case 2: // quit
					break;
			}
			break;
		case 1: // option
			option();
			goto home;
			break;
		case 2: // quit
			break;
	}
	return 0;
}
