#include <iostream> // cout, endl
#include <conio.h> // _kbhit() 
#include <Windows.h> // system(), Sleep()
#include <vector> // vector<> cend()
#include <cstdlib> // rand()
#include <stdlib.h> // setting for terminal colors
#include <MMSystem.h> // to play some music
#include <fstream> // to keep high scores
#include <string> // to pass parameters to system commands
#include <cctype> // for string operations

//Ramazan Emre Oduncuoglu 180254023
//COM309 Course Project
//20.04.2022


using namespace::std;

//#define KEY_UP 72    //using arrow keys to control snake is under construction 
//#define KEY_RIGHT 77
//#define KEY_DOWN 80
//#define KEY_LEFT 75

const int width = 50; // size of game board
const int height = 25; // size of game board
unsigned short int  fruitX, fruitY; // to store coordinates of fruit
int  score = -1; // to keep score
int sscore; // to show score
float speed = 60; // argument for Sleep() function (waiting time between iterations)
const char wall = 219; // for board walls
const char fruit = 254; // for good lookin' fruit
int selected; // to store selection
int cause; // specifies GameOver cause
bool GameOver; // current state of game
string player;
char ans;



vector<unsigned short  int> TailX; // to store x coordinates of tail parts 
vector<unsigned short  int> TailY; // to store y coordinates of tail parts

enum sDirection { START = 0, LEFT, RIGHT, UP, DOWN }; // to store current move of the snake
sDirection state;


void set_console_size()
{
	HANDLE hOut;
	SMALL_RECT DisplayArea = { 0, 0, 0, 0 };
	//set x and y to whatever ye' want
	int x = 750;
	int y = 550;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DisplayArea.Right = x;
	DisplayArea.Bottom = y;

	SetConsoleWindowInfo(hOut, TRUE, &DisplayArea);
}
void ClearScreen() // we used instead of system("cls")
{
	HANDLE hOut;
	COORD Position;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}
void CursorDestroyer() // a code snippet to hide cursor
{ // you won't see a line spawning around anymore
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);
}
void ScrollbarDestroyer() 
{
	CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

	// Get console handle and get screen buffer information from that handle.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &screenBufferInfo);

	// Get rid of the scrollbar by setting the screen buffer size the same as 
	// the console window size.
	COORD new_screen_buffer_size;

	// screenBufferInfo.srWindow allows us to obtain the width and height info 
	// of the visible console in character cells.
	// That visible portion is what we want to set the screen buffer to, so that 
	// no scroll bars are needed to view the entire buffer.
	new_screen_buffer_size.X = screenBufferInfo.srWindow.Right -
		screenBufferInfo.srWindow.Left + 1; // Columns
	new_screen_buffer_size.Y = screenBufferInfo.srWindow.Bottom -
		screenBufferInfo.srWindow.Top + 1; // Rows

		// Set new buffer size
	SetConsoleScreenBufferSize(hConsole, new_screen_buffer_size);
}
void gotoxy(int x, int y) // a code snippet for adjusting cursor position
{
	COORD coord;
	coord.X = x; // to given position
	coord.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void quit();
void theGame();
void credits();

inline void fruitSpawner() // generates new location for the fruit (why inline?)
{
	for (unsigned short int i = 0; i < size(TailX); i++)
	{
		fruitX = rand() % 45 + 2; // returns a random x coordinates in range of map (0 - 47)
		fruitY = rand() % 20 + 2; // returns a random y coordinates in range of map (0 - 22)

		if (fruitX != TailX[i] || fruitY != TailY[i]) // the fruit is on the snake? if not you made it.
			break;
	}
}

void Init() // game initials
{
	GameOver = false;
	state = START;
	srand((unsigned int)time(NULL)); // inital random location for the fruit
	TailX.push_back(width / 2); // the snake always start from the middle of the board
	TailY.push_back(height / 2); // the snake always start from the middle of the board

	fruitSpawner();
}

void HighScore()
{
	ofstream HSF;
	HSF.open("highScores.txt", ios::app | ios::ate);
	HSF << sscore <<" , " << player << "\n";
	HSF.close();
}

void Draw() // draws the visuals like game board, the snake, directives and scoreboard
{
	ClearScreen();
	register unsigned int i; // to draw vertical visuals
	register unsigned int j; // to draw horizontal visuals
	register unsigned int k; // to draw snake's visuals
	for (i = 0; i < height; i++) // iterate rows 
	{
		for (j = 0; j < width; j++) // iterate columns
		{
			bool print = false;
			if (i == 0 || i == height - 1)		// fill horizontal walls with '='
				cout << wall;					// ceiling and floor
			else if (j == 0 || j == width - 1) // fill vertical lines with 'H'
				cout << wall;
			else if (i == fruitY && j == fruitX) // fruit = '@'. coordinates of fruit created in FruitSpawner()
				cout << fruit;
			else if (TailX[0] == j && TailY[0] == i) // tailX[0], tailY[0] means snake's head = 'O'
				cout << "O";
			else
			{
				for (k = 1; k < TailX.size(); k++) // iterate snake parts
				{
					if (TailX[k] == j && TailY[k] == i) // draw snake parts
					{
						print = true;
						cout << "o"; // for tail parts = 'o'
					}
				}
				if (!print)
					cout << " ";
			}
		}
		cout << endl;
	}
	sscore = score - 1;
	cout << "Score: " << sscore << endl;
	cout << "To guide the snake use WASD keys" << endl;
	cout << "Use X to pause, use SPACE to end the game" << endl;
	cout << "Guide the snake wisely " << endl;
}

void Input() // to catch and process keyboard hits
{
	if (_kbhit()) // is there any keyboard hit
	{
		switch (_getch())
		{
		case 'a':
		case 'A':
			state = LEFT; // match input with the movement
			break;
		case 'w':
		case 'W':
			state = UP;
			break;
		case 'd':
		case 'D':
			state = RIGHT;
			break;
		case 's':
		case 'S':
			state = DOWN;
			break;
		case 'x':
		case 'X':
			system("PAUSE"); // to pause the game
			break;
		case (char)32: //ascii code of space
			system("cls");
			GameOver = true; // to end the game 
			break;
		}
	}
}


void Basics()
{
	int prevX = *(TailX.cend() - 1); // previous x position 
	int prevY = *(TailY.cend() - 1); // previous y position
	int i;


	for (i = TailX.size() - 1; i != 0; i--) // for moving each tail part to new position for x coordinate
		TailX[i] = TailX[i - 1];

	for (i = TailY.size() - 1; i != 0; i--) // for moving each tail part to new position for y coordinate
		TailY[i] = TailY[i - 1];

	switch (state)	// tailX[0], tailY[0] means snake's head
	{
	case LEFT:		// move snake's head according to Input()
		--TailX[0];
		break;

	case RIGHT:
		++TailX[0];
		break;

	case UP:
		--TailY[0];
		break;

	case DOWN:
		++TailY[0];
		break;

	default:
		break;
	}

	if (TailX[0] > width || TailX[0] < 0 || TailY[0]<0 || TailY[0]>height) // check wall collision
	{																	  // if the coordinates of the snake's head are greater than the game board's size, TryAgain
		system("cls"); // clear terminal for gameover screen
		//cout << "                        Y O U  H I T  T H E  W A L L" << endl;
		//cout << "Your score: " << score << endl;
		//cout << "Try Again" << endl;
		cause = 1;
		GameOver = true; // change flag condition 
	}

	for (i = 1; i < TailX.size(); i++)									// check tail collision
		if (TailX[0] == TailX[i] && TailY[0] == TailY[i])				//  if the coordinates of the snake's head are same the any tail part of snake, TryAgain
		{
			system("cls"); // clear terminal for gameover screen
			cout << " " << endl << endl;
			cout << " " << endl << endl;
			//cout << "                   G A M E O V E R  Y O U  A T E  Y O U R S E L F" << endl;
			//cout << "Your score: " << score << endl;
			//cout << "Try Again" << endl;
			cause = 2;
			GameOver = true; // change flag condition
		}
}
void Score() // Did you just eat a fruit?
{
	int prevX = *(TailX.cend() - 1);
	int prevY = *(TailY.cend() - 1);

	if (TailX[0] == fruitX && TailY[0] == fruitY)
	{
		score += 10;			// there is your 10 points
		fruitSpawner();			// new fruit has been spawned
		TailX.push_back(prevX); // add a new tail part
		TailY.push_back(prevY);
		if (speed > 10)
			speed -= 3;			// decreasing speed variable increases the speed of snake // 3e dusur -reo
	}
}

void SetColor()
{
	string background, foreground;
	cout << " Make your Theme: " << endl;
	cout << " Background: ";
	cin >> background;
	cout << " Foreground: ";
	cin >> foreground;
	string CColor = "color " + background + foreground;
	system(CColor.c_str());
	system("cls");
	theGame();
}

void menu()
{
	ClearScreen();
	cout << "" << std::endl;
	cout << " -----------------------------------------------  " <<endl;
	cout << "|                                               | " <<endl;
	cout << "|  ******   *     *      *      *****  *    *   | " <<endl;
	cout << "| *         * *   *     * *     *      *  *     | " <<endl;
	cout << "|  ******   *  *  *    *   *    ****   * *      | " <<endl;
	cout << "|        *  *   * *   *******   *      *   *    | " <<endl;
	cout << "|  ******   *     *  *       *  *****  *     *  | " <<endl;
	cout << "|                                               | " <<endl;
	cout << " -----------------------------------------------  " <<endl;
	cout << "" << endl << std::endl;
	cout << "                   M E N U:    " << endl << std::endl;
	cout << "                1: Start Game  " << endl << std::endl;
	cout << "                2: Theme Maker   " << endl << std::endl;
	cout << "                3: Credits    " << endl << std::endl;
	cout << "                9: Exit     " << endl << std::endl;
	cout << "Select operation: ";
	scanf_s("%d", &selected);

	if (selected == 1)
	{
		ClearScreen();
		theGame();
		selected = 0;
	}
	else if (selected == 2)
	{
		system("cls");
		cout << " Color attributes are specified by TWO hex digits -- the first" << endl;
		cout << " corresponds to the background; the second the foreground." << endl;
		cout << " Each digit can be any of the following values:" << endl;

		cout << " 0 = Black       8 = Gray" << endl;
		cout << " 1 = Blue        9 = Light Blue" << endl;
		cout << " 2 = Green       A = Light Green" << endl;
		cout << " 3 = Aqua        B = Light Aqua" << endl;
		cout << " 4 = Red         C = Light Red" << endl;
		cout << " 5 = Purple      D = Light Purple" << endl;
		cout << " 6 = Yellow      E = Light Yellow" << endl;
		cout << " 7 = Light Gray  F = White" << endl;
		SetColor();

	}
	else if (selected == 3)
	{
		credits();
	}
	if (selected == 9)
	{
		exit(0);
	}
} // selam ben emre 2021 yılının noelinde buradaydım

void credits()
{
	char sel;
	system("cls");
		cout << "" << endl << endl;
		cout << "    ALKU Computer Engineering Department " << endl << endl;
		cout << "    COM309 ALGORITHMS AND ANALYSIS OF ALGORITHMS " << endl << endl;
		cout << "    Ramazan Emre Oduncuoglu " << endl << endl;
		cout << "    Sound effects: https://www.fesliyanstudios.com " << endl << endl << endl;
		cout << "Go to menu[y/n]:  ";
		cin >> sel;
		std::to_string(sel);
		if (sel == 'y') 
		{
			ClearScreen();
			system("cls");
			menu();
		}
		else
		{
			exit(0);
		}
}

void endgame() //when gameover = true this screen will come 
{
	cout << "" << endl << endl;
	cout << "  ---------------------------------------------------------------------- " << endl;
	cout << " |   *****      *     *       * ******       ****  *       ****** ****  |" << endl;
	cout << " |  *          * *    * *   * * *           *    *  *     * *     *   * |" << endl;
	cout << " |  *  ****   *   *   *  * *  * *****       *    *   *   *  ****  ****  |" << endl;
	cout << " |  *  *  *  *******  *   *   * *           *    *    * *   *     * *   |" << endl;
	cout << " |   *****  *       * *       * ******       ****      *    ***** *   * |" << endl;
	cout << "  ---------------------------------------------------------------------- " << endl;
	cout << "" << endl << endl;
	if (cause == 2) {
		cout << "                        Y O U  A T E  Y O U R S E L F" << endl;
	}
	else if (cause == 1) {
		cout << "                        Y O U  H I T  T H E  W A L L" << endl;
	}
	cout << "                        Y O U R   S C O R E : " << sscore << endl << endl;
	//cout << "                        H I G H   S C O R E : " << highscore << endl; // eger txt dosyasi uzerinde sort yapabilirsem burasi calisacak
	cout << "" << endl << endl;
}

void theGame()
{
	Init();
	while (GameOver == false) // to prevent crashes
	{

		Draw(); // draw visuals
		if ((state == UP) || (state == DOWN))
		{
			Sleep(speed * 2); // reduces the speed effect caused by rectangular cells in the console
		}
		else
			Sleep(speed); // refresh rate if we increase or decrease the speed of the snake will change
		Score(); //when snake eats a fruits score increases 10 points
		Input(); // catch keyboard inputs
		Basics(); // movement basics of the snake. decides gameover conditions.
		if (score <= 0)
		{
			score++;
		}
		if (score == 0)
		{
			PlaySound(L"C:\\Users\\odunc\\Desktop\\gamer.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		if (GameOver == true)
		{
			ClearScreen();
			PlaySound(NULL, 0, 0); // to stop music
			quit();
		}
	}
}

void quit()
{
	if (GameOver == true)
	{
		PlaySound(L"C:\\Users\\odunc\\Desktop\\gameover.wav", NULL, SND_FILENAME | SND_ASYNC);
		endgame();
		cout << "What a shame, who R U: ";
		cin >> player;
		HighScore();
	}
}

int main()
{
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 572, 520, TRUE); // to resize console form

	PlaySound(L"C:\\Users\\odunc\\Desktop\\opener.wav", NULL, SND_FILENAME | SND_ASYNC); // to play entry effect
	system("Color 0A"); // base theme of game 
	SetConsoleTitle(L"sneak"); // to rename console bar title
	CursorDestroyer();
	ScrollbarDestroyer();
	menu();
	
	return 0;
}