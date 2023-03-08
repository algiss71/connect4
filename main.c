#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

int match(const char *string, const char *pattern) {
    regex_t re;
    if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB) != 0) return 0;
    int status = regexec(&re, string, 0, NULL, 0);
    regfree(&re);
    if (status != 0) return 0;
    return 1;
}

void clearScreen() {
#ifdef WIN32
    system("cls");
#else
    system("clear");
#endif // WIN32
}

#define ROWS 9
#define COLS 9
int rows = 6;
int cols = 7;
char player1[15];
char player2[15];

unsigned long long num = 0;
void initBoard(int, int);

void showBoard(char *, int, int);

int position(int, int, int, int);

void srartGame();

int checkWin(char *board);

int checkFour(char *board, int, int, int, int);

int horizontalCheck(char *board);

int verticalCheck(char *board);

int diagonalCheck(char *board);

int numberColum(char *);

int takeTurn(char *board, int player, const char *PIECES);

//=================================================================================================
void flushInputBuffer() {
    while (getchar() != '\n');
}

//=================================================================================================
char board[ROWS * COLS];
char fString[1 + COLS * 2];
char endString[1 + COLS * 2];

//=================================================================================================
int main() {
    const char *PIECES = "o*";
    int turn = 0, done = 0;
    srartGame();
    initBoard(rows, cols);

    for (turn = 0; turn < (rows * cols) && !done; ++turn)//делаем ход пока количество ходом меньше размера поля и никто не победил
    {
        showBoard(board, rows, cols);//печатаем поле

        int t;
        while ( (t = takeTurn(board, turn % 2,PIECES)) != 1)//делем ход - передаем поле, номер игрока (0 или 1) и метки для хода игроками Х и О
        {//возвращается 0 если стобец переполнен
            showBoard(board, rows, cols);   //печатаем поле
            printf("Column %d is full\n", t + 1);//
        }

        done = checkWin(board);//проверяем не выиграл ли кто
    }//цикл заканчивается, когда поле заканчивается или один из игроков выиграл
    showBoard(board, rows, cols);//печатаем поле

    if (turn == (rows * cols) && !done) {//если ходы закончились и никто не выиграл
        printf("It's a tie!");//это ничья
    } else {
        --turn;
        (turn % 2 + 1 == 1) ? printf("%s  win!\n",player1) : printf("%s  win!\n",player1);//выводим сообщение о выигрыше
    }
    return 0;
}

//===================================================================================================
int takeTurn(char *board, int player, const char *PIECES) {
    int ir, ic = 0;
    while (1)//проверяем диапазон ввода номера столбца(col)
    {
        player == 0 ? (ic = numberColum(player1)) : (ic = numberColum(player2));
        if (ic < 1 || ic > cols)  printf("The column number is out of range (1 - %d)\n", cols);
        else  break;
     }
    --ic;

    for (ir = rows - 1; ir >= 0; --ir)//с последней строки
    {
        if (board[cols * ir + ic] == ' ')//ячейка в столбце пустая
        {
            board[cols * ir + ic] = PIECES[player];//
            return 1;
        }
    }
    return ic;
}

//=================================================================================================
int numberColum(char *pl) {//проверяет введенные символы при ходе игрока
    char tbuf[4] = {0}; char turn[4] = {0};
    long int n;
    char *p;
    while (1) {
        printf("%s turn:", pl);
        fgets(tbuf, 4, stdin);
        sscanf(tbuf, "%s", turn);

        if (strcmp("end", turn) == 0) {//введено "end"
            printf("Game over\n"); exit(1);//выход из программы
        }
        n = strtol(turn, &p, 0);//р - указывает на позицию в строке которая является символом не цифрой
        if (n != 0 && *p == 0) break;//введено число
        else {
            if(strlen(turn) == 2) printf("incorect input\n");
            else{
                printf("incorect input\n");
                flushInputBuffer();
            }
        }
    }
    return (int)n;
}

//=================================================================================================
void srartGame() {
    char pbuf[15] = {0};
    const char *re = "[0-9 ][xX][ 0-9]";
    printf("Connect Four\n");

    puts("First player's name:"); fgets(pbuf, 15, stdin);sscanf(pbuf, "%s", player1);//fflush(stdin);
    puts("Second player's name:");fgets(pbuf, 15, stdin); sscanf(pbuf, "%s", player2); //fflush(stdin);

    while (1) {
        puts("Set the board dimensions (Rows x Columns)");
        puts("Press Enter for default (6 x 7)");
        char buf[20] = {0}; char board[20] = {0};

        fgets(buf, 19, stdin); sscanf(buf, "%[0-9xX ]", board);
        if (strlen(board) == 0) break;

        if (match(board, re)) {
            char boardSize[6];
            char *token = strtok(board, "xX");
            int i = 0;
            while (token != NULL) {
                boardSize[i++] = *token;
                token = strtok(NULL, " xX ");
            }
            boardSize[i] = '\0';

            rows = boardSize[0] - 48;
            cols = boardSize[1] - 48;
            if (rows < 5 || rows > 9) {
                puts("Board rows should be from 5 to 9\n");
                continue;
            }
            if (cols < 5 || cols > 9) {
                puts("Board columns should be from 5 to 9\n");
                continue;
            }
            break;
        } else {
            puts("Invalid input\n");
            continue;
        }
    }
    printf("%s VS %s\n", player1, player2);
    printf("%d x %d board\n", rows, cols);
    putchar('\n');
}

//===================================================================================================
void initBoard(int rw, int cl) {
    memset(endString, '=', 1 + cl * 2);
    memset(board, ' ', rw * cl);
    for (int ic = 0; ic < cl * 2 + 1; ++ic) {
        if (ic % 2 == 0) fString[ic] = ' ';
        else fString[ic] = 49 + ic / 2;
    }
}

//===================================================================================================
void showBoard(char *board, int rw, int cl) {
    clearScreen();

    printf("%s\n", fString);
    for (int ir = 0; ir < rw; ++ir) {
        for (int ic = 0; ic < cl; ++ic) {
            printf("|%c", board[cl * ir + ic]);
        }
        printf("|\n");
    }
    printf("%s\n", endString);
}

//-------------------------------------------------------------------------------------------------
int checkWin(char *board) {
    return (horizontalCheck(board) || verticalCheck(board) || diagonalCheck(board));
}

//-------------------------------------------------------------------------------------------------
int checkFour(char *board, int a, int b, int c, int d) {
    return (board[a] == board[b] && board[b] == board[c] && board[c] == board[d] && board[a] != ' ');
}

//-------------------------------------------------------------------------------------------------
int horizontalCheck(char *board) {
    int index = 0;
    //const int WIDTH = 1;

    for (int ir = 0; ir < rows; ++ir) {
        for (int ic = 0; ic < cols - 3; ++ic) {
            index = cols * ir + ic;
            if (checkFour(board, index, index + 1, index + 2, index + 3)) {
                return 1;
            }
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
int verticalCheck(char *board) {
    int index = 0;
    const int HEIGHT = cols;
    for (int ir = 0; ir < rows - 3; ++ir) {

        for (int ic = 0; ic < cols; ++ic) {
            index = cols * ir + ic;
            if (checkFour(board, index, index + HEIGHT, index + HEIGHT * 2, index + HEIGHT * 3)) {
                return 1;
            }
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
int diagonalCheck(char *board) {
    int index = 0, count = 0;
    const int DIAG_RIGHT = cols - 1, DIAG_LEFT = cols + 1;

    for (int ir = 0; ir < rows - 3; ++ir) {
        for (int ic = 0; ic < cols; ++ic) {
            index = cols * ir + ic;
            if (((count < cols - 3) &&
               checkFour(board, index, index + DIAG_LEFT, 
                                       index + DIAG_LEFT * 2,
                                       index + DIAG_LEFT * 3)) ||
                ((count > 2) &&
                checkFour(board, index, index + DIAG_RIGHT, 
                                        index + DIAG_RIGHT * 2, 
                                        index + DIAG_RIGHT * 3))) 
            { return 1; }
            count++;
        }
        count = 0;
    }
    return 0;
}
