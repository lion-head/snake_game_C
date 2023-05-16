#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#define boundary_character (char)219
#define cursor_add grid[(cursor[0] - 1) * no_clm + cursor[1] - 1]
#define cursor_character (char)16
#define blank ' '
#define name_limit 10
#define snake_head_char (char)254
#define snake_tail_char (char)1
#define fruit_char '#'
/*****************************************************************************************************************************************************************************/

/*****************************************************************************************************************************************************************************/
enum constants
{
    no_clm = 197,
    no_row = 45,
    default_snake_length = 3
};
int snake_length, no_of_players, current_player_number = 0, current_score = 0, starting_snake_length, check_save_exit = 0, check_, points = 0, check_game_over = 0, check_game = 0;
int **snake = NULL;
typedef struct highscore
{
    char name[10];
    int score;
    int player_num;
} highscore;
typedef struct name
{
    char player_name[10];
    int player_num;
} name;
/*****************************************************************************************************************************************************************************/

/*Declarations*/
/***************************************************************************************************************************************************************************/
int menu(char *grid);
void get_snake_length(int player);
void create_snake(int player);
void get_players(void);
void put_data(void);
void set_grid(char *grid);
int get_cursor_input(char *grid);
void print_grid(char *grid);
int no_of_options(void);
void set_option_frame(char *grid, int menu_phase, const int *no_of_options);
int resume(char *grid);
void sort_highscore(highscore *array);
int put_highscore(char *grid);
int print_int_at_add(char *grid, int *add, int score);
int help(char *grid);
void play_game(char *grid);
char get_starting_direction(void);
int get_first_direction(char previous_derection);
int move_snake(char *grid, char current_direction);
void move_tail(char *grid, int *previous_add);
void increase_snake_length(void);
void spawn_fruit(char *grid);
char pause(char *grid, char previous_direction);
void update_game(char *grid);
void get_name(char *grid);
int check_name_duplicacy(char *grid, char *name);
int credits(char *grid);
void gameover(char *grid);
/*************************************************************************************************************************************************************************/

/*************************************************************************************************************************************************************************/
int main()
{
    extern int snake_length, no_of_players;

    char grid[no_clm * no_row];
    while (1)
    {
        check_game = check_game_over = 0;
        extern int check_save_exit;
        check_save_exit = 0;
        get_players();
        set_grid(grid);
        int player_num = menu(grid);
        extern int current_player_number;
        current_player_number = player_num;
        get_snake_length(player_num);
        free(snake);
        create_snake(player_num);
        play_game(grid);
        update_game(grid);
    }
    // printf("%d",player_num);
    return 0;
}
/*************************************************************************************************************************************************************************/

/*************************************************************************************************************************************************************************/
/*this function sets the data to defualt settings*/
void put_data(void)
{
    FILE *data;
    int length = default_snake_length;
    data = fopen("data.txt", "w");
    fprintf(data, "players=%d\n", 0);
    fprintf(data, "length>%d\n", length);
    fputc('$', data);
    for (int i = 0; i < length; i++)
        fprintf(data, " %d,%d", ((no_row + 1) / 2) + i, (no_clm + 1) / 2);
    fputc('\n', data);
    fprintf(data, "highscore-%d\n", 0);
    fclose(data);
}

/*this function updates the players(means no._of_players)*/
void get_players(void)
{
    FILE *data;
    extern int no_of_players;
    data = fopen("data.txt", "r");
    if (data == NULL)
    {
        put_data();
        data = fopen("data.txt", "r");
    }
    fseek(data, 8, SEEK_SET);
    fscanf(data, "%d", &no_of_players);
    fclose(data);
}

/*this function takes player number and updates the length of the snake to the length of that player's snake*/
void get_snake_length(int player)
{
    FILE *data;
    extern int snake_length;
    data = fopen("data.txt", "r");
    if (player != 0)
    {
        while (1)
        {
            char temp;
            temp = (char)fgetc(data);
            if (temp == '@')
            {
                int temp;
                fscanf(data, "%d", &temp);
                if (temp == player)
                    break;
            }
        }
    }
    while (1)
    {
        char temp;
        temp = (char)fgetc(data);
        if (temp == '>')
        {
            int temp;
            fscanf(data, "%d", &snake_length);
            break;
        }
    }
    return;
}

/*this function takes player number and updates/creates snake(pointer to snake) as of the player's snake in the game*/
void create_snake(int player)
{
    extern int snake_length;
    extern int **snake;
    snake = NULL;

    if (player == 0)
    { /*0 is for new game or default*/
        if (snake != NULL)
            free(snake);
        snake = (int **)calloc(snake_length, sizeof(int *));
    }
    else
        snake = (int **)realloc(snake, snake_length * sizeof(int *));
    for (int i = 0; i < snake_length; i++)
    {
        snake[i] = (int *)calloc(2, sizeof(int));
    }
    FILE *data;
    data = fopen("data.txt", "r");
    if (player != 0)
    {
        while (1)
        {
            char temp;
            temp = (char)fgetc(data);
            if (temp == '@')
            {
                int temp;
                fscanf(data, "%d", &temp);
                if (temp == player)
                    break;
            }
        }
    }
    while (1)
    {
        char temp;
        temp = (char)fgetc(data);
        if (temp == '$')
        {
            break;
        }
    }
    for (int i = 0; i < snake_length; i++)
    {
        fseek(data, 1, SEEK_CUR);
        fscanf(data, "%d", &snake[i][0]);
        fseek(data, 1, SEEK_CUR);
        fscanf(data, "%d", &snake[i][1]);
    }
    fclose(data);
    return;
}

/*sets grid to default*/
void set_grid(char *grid)
{
    int size_of_grid = no_clm * no_row;
    for (int i = 0; i < size_of_grid; i++)
    {
        if (i < (no_clm - 1) - 1 || (i > (size_of_grid - 1) - (no_clm - 1) && i < size_of_grid - 1))
            grid[i] = boundary_character;
        else if ((i + 1) % (no_clm) == 0 && i != 0)
            grid[i] = '\n';
        else if (grid[i - 1] == '\n')
            grid[i] = boundary_character;
        else if ((i + 2) % no_clm == 0)
            grid[i] = boundary_character;
        else
            grid[i] = ' ';
    }
    grid[size_of_grid - 1] = '\0';
}

/*it will return player number by prompting user to select from the menu*/
int menu(char *grid)
{
    // 0 for resume
    // 1 for new game
    // 2 for highscore
    // 3 help
    // 4 credits
    // 5 exit
    int player;
    set_grid(grid);
    int select_option = get_cursor_input(grid);
    switch (select_option)
    {
    case 0:
        player = resume(grid);
        break;
    case 1:
        return 0;
        break;
    case 2:
        player = put_highscore(grid);
        break;
    case 3:
        player = help(grid);
        break;
    case 4:
        player = credits(grid);
        break;
    case 5:
        exit(0);
        break;
    }
    return player;
}

/*this will prompt user to select from the 5/6 options and according to which it will return a value after pressing enter*/
int get_cursor_input(char *grid)
{
    int cursor[2], cursor_pos = 0, cursor_check, cursor_starting[2];
    cursor[0] = ((no_row - 8) + 1) / 2;
    cursor[1] = ((no_clm - 14) + 1) / 2;
    cursor_starting[0] = cursor[0];
    cursor_starting[1] = cursor[1];
    int options = no_of_options();
    if (options == 6)
        cursor_check = 0;
    else
        cursor_check = 1;
    cursor_pos = cursor_check;
    cursor[0] = cursor_starting[0] + cursor_pos;
    int check_enter = 0;
    cursor_add = cursor_character;
    set_option_frame(grid, 0, (const int *)cursor_starting);
    print_grid(grid);
    while (1)
    {
        switch (getch())
        {
        case 0x48:
        case 0xc8:
        case 'a':
        case 'A':
            cursor_add = blank;
            if (cursor_pos == cursor_check)
                cursor_pos = 5;
            else
                cursor_pos--;
            cursor[0] = cursor_starting[0] + cursor_pos;
            cursor_add = cursor_character;
            print_grid(grid);
            break;
        case 0x50:
        case 0xd0:
        case 's':
        case 'S':
            cursor_add = blank;
            if (cursor_pos == 5)
                cursor_pos = cursor_check;
            else
                cursor_pos++;
            cursor[0] = cursor_starting[0] + cursor_pos;
            cursor_add = cursor_character;
            print_grid(grid);
            break;
        case '\r':
            check_enter = 1;
            break;
        default:
            break;
        }
        if (check_enter)
        {
            break;
        }
    }
    fflush(stdin);
    return cursor_pos;
}

/*this will return no of options according to the no. of players*/
int no_of_options(void)
{
    extern int no_of_players;
    if (no_of_players < 1)
        return 5;
    else
        return 6;
}

/*it will print the grid*/
void print_grid(char *grid)
{
    extern int snake_length, snake_increase, current_score;
    system("cls");
    puts(grid);
    if (check_game)
    {
        printf("SNAKE LENGTH : %d", snake_length);
        printf("\nSCORE : %d", current_score);
    }
    return;
}

/*this will set options whithin the frame in the grid acc. to the menu phase*/
// 0 for front menu
// 1 for for resume
// 2 for highscores
// 3 for help
// 4 credits
// 5 exit
void set_option_frame(char *grid, int menu_phase, const int *cursor)
{
    int lower_limit_clmn, lower_limit_row, upper_limit_clmn, upper_limit_row;
    if (menu_phase == 0)
    {
        char options[6][11] = {"RESUME", "NEW GAME", "HIGHSCORES", "HELP", "CREDITS", "EXIT"};
        int i = no_of_options() == 6 ? 0 : 1;
        for (; i < 6; i++)
        {
            for (int j = 0; j < strlen(options[i]); j++)
            {
                grid[(cursor[0] + i - 1) * (no_clm) + cursor[1] + 1 + j - 1] = options[i][j];
            }
        }
        lower_limit_row = no_of_options() == 6 ? -1 : 0;
        lower_limit_clmn = -2;
        upper_limit_row = 6;
        upper_limit_clmn = 12;
    }
    else if (menu_phase == 1)
    {
        extern int no_of_players;
        FILE *data_ptr;
        name player[no_of_players];
        data_ptr = fopen("data.txt", "r");
        for (int i = 0; i < no_of_players; i++)
        {
            while (1)
            {
                char temp;
                temp = fgetc(data_ptr);
                if (temp == '@')
                    break;
            }
            fscanf(data_ptr, "%d", &player[i].player_num);
            fseek(data_ptr, 1, SEEK_CUR);
            fscanf(data_ptr, "%s", player[i].player_name);
        }
        fclose(data_ptr);
        int max = 0;
        for (int i = 0; i < no_of_players; i++)
        {
            for (int j = 0; j < strlen(player[i].player_name); j++)
            {
                if (strlen(player[i].player_name) > max)
                    max = strlen(player[i].player_name);
                grid[(cursor[0] + i - 1) * (no_clm) + cursor[1] + 1 + j - 1] = player[i].player_name[j];
            }
        }
        int add[2];
        add[0] = no_row - 1;
        add[1] = 2;
        char return_[] = "  press esc. to return to main menu  ";
        int j, k;
        for (j = 1, k = 0; k < strlen(return_); j++, k++)
            grid[(add[0] - 1) * no_clm + (add[1] + j) - 1] = return_[k];
        int i = -1;
        lower_limit_row = -1;
        lower_limit_clmn = -2;
        upper_limit_row = no_of_players;
        upper_limit_clmn = max + 2;
    }
    else if (menu_phase == 2)
    {
        int max = 0;
        extern int no_of_players;
        FILE *data_ptr;
        highscore player[no_of_players];
        data_ptr = fopen("data.txt", "r");
        int i;
        for (i = 0; i < no_of_players; i++)
        {
            while (1)
            {
                char temp;
                temp = fgetc(data_ptr);
                if (temp == '@')
                    break;
            }
            fscanf(data_ptr, "%d", &player[i].player_num);
            fseek(data_ptr, 1, SEEK_CUR);
            fscanf(data_ptr, "%s", player[i].name);
            while (1)
            {
                char temp;
                temp = fgetc(data_ptr);
                if (temp == '-')
                    break;
            }
            fscanf(data_ptr, "%d", &player[i].score);
        }
        fclose(data_ptr);
        sort_highscore(player);
        // printf("controll line 455\n");
        for (i = 0; i < no_of_players; i++)
        {
            int j, k;
            for (j = 1, k = 0; k < strlen(player[i].name); j++, k++)
            {
                grid[(cursor[0] + i - 1) * no_clm + cursor[1] + j - 1] = player[i].name[k];
            }
            int add[2];
            add[0] = cursor[0];
            add[1] = cursor[1];
            add[0] += i;
            add[1] += 12;
            int temp = print_int_at_add(grid, add, player[i].score);
            if ((temp - cursor[1]) > max)
                max = temp - cursor[1];
        }
        int add[2];
        add[0] = no_row - 1;
        add[1] = 2;
        char return_[] = "press enter key or esc. to return to main menu";
        int j, k;
        for (j = 1, k = 0; k < strlen(return_); j++, k++)
            grid[(add[0] - 1) * no_clm + (add[1] + j) - 1] = return_[k];
        max++;
        i = -1;
        lower_limit_row = -1;
        lower_limit_clmn = -1;
        upper_limit_row = no_of_players;
        upper_limit_clmn = max;
    }
    else if (menu_phase == 3)
    {
        upper_limit_clmn = 0;
        char hlp[10][200] = {"HOW TO PLAY", "press :", "(up-arrow-key) or (alphabet key 'w') => to move snake upward", "(down-arrow-key) or (alphabet key 's') => to move snake down", "(left-arrow-key) or (alphabet key 'a') => to move snake to the left", "(right-arrow-key) or (alphabet key 'd') => to move snake to the left", " ", "INSTRUCTIONS", "If you hit snake's tail , you will be out", "snake gets one tail longer if it eats '#'(fruit)"};
        for (int i = 1; i < 10; i++)
        {
            if (strlen(hlp[i]) > upper_limit_clmn)
                upper_limit_clmn = strlen(hlp[i]);
            if (i != 7)
                for (int j = 0; j < strlen(hlp[i]); j++)
                    grid[(cursor[0] + i - 1) * no_clm + cursor[1] + j - 1] = hlp[i][j];
        }

        for (int j = (upper_limit_clmn - strlen(hlp[0])) / 2, k = 0; k < strlen(hlp[0]); j++, k++)
            grid[(cursor[0] - 1) * no_clm + cursor[1] + j - 1] = hlp[0][k];
        for (int j = (upper_limit_clmn - strlen(hlp[7])) / 2, k = 0; k < strlen(hlp[7]); j++, k++)
            grid[(cursor[0] - 1 + 7) * no_clm + cursor[1] + j - 1] = hlp[7][k];
        int add[2];
        add[0] = no_row - 1;
        add[1] = 2;
        char return_[] = "press enter key or esc. to return to main menu";
        int j, k;
        for (j = 1, k = 0; k < strlen(return_); j++, k++)
            grid[(add[0] - 1) * no_clm + (add[1] + j) - 1] = return_[k];
        //   printf("\n%d",upper_limit_clmn);
        //   getch();
        lower_limit_row = -1;
        lower_limit_clmn = -2;
        upper_limit_row = 10;
        upper_limit_clmn++;
    }
    else if (menu_phase == 4)
    {
        upper_limit_clmn = 0;
        for (int i = 0; i < 21; i++)
        {
            grid[(cursor[0] - 2) * no_clm + cursor[1] + i - 1] = blank;
            grid[(cursor[0] + 3 - 1) * no_clm + cursor[1] + i - 1] = blank;
        }
        char crdt[][50] = {"  MADE WITH LOVE BY  ", "   ATULYA KHAROTIA   ", "    IN C LANGUAGE    "};
        for (int i = 0; i < 3; i++)
        {
            if (strlen(crdt[i]) > upper_limit_clmn)
                upper_limit_clmn = strlen(crdt[i]);
            for (int j = 0; j < strlen(crdt[i]); j++)
                grid[(cursor[0] - 1 + i) * no_clm + cursor[1] + j - 1] = crdt[i][j];
        }
        char email[] = "  EMAIL : kharotiakamini12@gmail.com  ";
        for (int i = 0; i < strlen(email); i++)
            grid[(no_row - 4) * no_clm + 3 + i - 1] = email[i];
        int add[2];
        add[0] = no_row - 1;
        add[1] = 2;
        char return_[] = "  press enter key or esc. to return to main menu  ";
        int j, k;
        for (j = 1, k = 0; k < strlen(return_); j++, k++)
            grid[(add[0] - 1) * no_clm + (add[1] + j) - 1] = return_[k];
        // printf("\n%d",upper_limit_clmn);
        // getch();
        lower_limit_clmn = -1;
        lower_limit_row = -2;
        upper_limit_row = 4;
        upper_limit_clmn;
    }
    else if (menu_phase == 5)
    {
        upper_limit_clmn = 0;
        extern int current_score;
        char crdt[][50] = {"          ***GAME-OVER***", " ", "     SCORE    : ", " SNAKE LENGTH : "};
        for (int i = 0; i < 4; i++)
        {
            if (strlen(crdt[i]) > upper_limit_clmn)
                upper_limit_clmn = strlen(crdt[i]);
            int j;
            for (j = 0; j < strlen(crdt[i]); j++)
                grid[(cursor[0] - 1 + i) * no_clm + cursor[1] + j - 1] = crdt[i][j];
            int score[2];
            score[0] = cursor[0] + i;
            score[1] = cursor[1] + j;
            if (i == 2)
                print_int_at_add(grid, score, current_score);
            else if (i == 3)
                print_int_at_add(grid, score, snake_length);
        }
        int add[2];
        add[0] = no_row - 1;
        add[1] = 2;
        char return_[] = "  press enter key or esc. to return to main menu  ";
        int j, k;
        for (j = 1, k = 0; k < strlen(return_); j++, k++)
            grid[(add[0] - 1) * no_clm + (add[1] + j) - 1] = return_[k];
        // printf("\n%d", upper_limit_clmn);
        // getch();
        lower_limit_clmn = -1;
        lower_limit_row = -1;
        upper_limit_row = 4;
        upper_limit_clmn += 10;
    }

    for (int j = lower_limit_row; j < upper_limit_row + 1; j++)
    {
        for (int k = lower_limit_clmn; k < upper_limit_clmn + 1; k++)
        {
            if (j == lower_limit_row || j == upper_limit_row || k == lower_limit_clmn || k == upper_limit_clmn)
            {
                if (j == lower_limit_row && k == lower_limit_clmn)
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)201;
                else if (j == lower_limit_row && k == upper_limit_clmn)
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)187;
                else if (j == upper_limit_row && k == lower_limit_clmn)
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)200;
                else if (j == upper_limit_row && k == upper_limit_clmn)
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)188;
                else if (j == lower_limit_row || j == upper_limit_row)
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)205;
                else
                    grid[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)186;
            }
        }
    }
}

/*this will prompt user to select player from given options and return player num according to that*/
int resume(char *grid)
{
    extern int no_of_players;
    FILE *data_ptr;
    name player[no_of_players];
    data_ptr = fopen("data.txt", "r");
    for (int i = 0; i < no_of_players; i++)
    {
        while (1)
        {
            char temp;
            temp = fgetc(data_ptr);
            if (temp == '@')
                break;
        }
        fscanf(data_ptr, "%d", &player[i].player_num);
        fseek(data_ptr, 1, SEEK_CUR);
        fscanf(data_ptr, "%s", player[i].player_name);
    }
    fclose(data_ptr);
    set_grid(grid);
    int cursor[2], cursor_pos = 0, cursor_check = 0, cursor_starting[2];
    cursor[0] = ((no_row - 8) + 1) / 2;
    cursor[1] = ((no_clm - 14) + 1) / 2;
    cursor_starting[0] = cursor[0];
    cursor_starting[1] = cursor[1];
    cursor_pos = cursor_check;
    cursor[0] = cursor_starting[0] + cursor_pos;
    int check_enter = 0;
    cursor_add = cursor_character;
    set_option_frame(grid, 1, (const int *)cursor_starting);
    print_grid(grid);
    while (1)
    {
        switch (getch())
        {
        case 0x48:
        case 0xc8:
        case 'a':
        case 'A':
            cursor_add = blank;
            if (cursor_pos == cursor_check)
                cursor_pos = no_of_players - 1;
            else
                cursor_pos--;
            cursor[0] = cursor_starting[0] + cursor_pos;
            cursor_add = cursor_character;
            print_grid(grid);
            break;
        case 0x50:
        case 0xd0:
        case 's':
        case 'S':
            cursor_add = blank;
            if (cursor_pos == no_of_players - 1)
                cursor_pos = cursor_check;
            else
                cursor_pos++;
            cursor[0] = cursor_starting[0] + cursor_pos;
            cursor_add = cursor_character;
            print_grid(grid);
            break;
        case 27:
            return menu(grid);
            break;
        case '\r':
            check_enter = 1;
            break;
        default:
            break;
        }
        if (check_enter)
        {
            break;
        }
    }
    return cursor_pos + 1;
}

/*this function will sort array of daya tupe higscores*/
void sort_highscore(highscore *array)
{
    extern int no_of_players;
    highscore temp;
    for (int i = 1; i < no_of_players; i++)
    {
        temp = array[i];
        int j;
        for (j = i - 1; j >= 0 && temp.score > array[j].score; j--)
        {
            array[j + 1] = array[j];
        }
        array[j + 1] = temp;
    }
}

/*this function will return player no after displaying higscore and then calling the menu function again*/
int put_highscore(char *grid)
{
    extern int no_of_players;
    set_grid(grid);
    int cursor[2];
    cursor[0] = no_of_players % 2 == 0 ? (no_row - no_of_players + 1) / 2 : (no_row - no_of_players) / 2;
    cursor[1] = (no_clm - 23) / 2;
    set_option_frame(grid, 2, cursor);
    print_grid(grid);
    // cursor[1]+=10;
    fflush(stdin);
    while (1)
    {
        char temp = getch();
        if (temp == '\r' || (int)temp==27)
        {
            return menu(grid);
        }
    }
    return menu(grid);
}

/*this function will put integer in grid at desired adress*/
int print_int_at_add(char *grid, int *add, int score)
{
    int length = 1, i = 0, temp;
    char num;
    char *player_score = (char *)calloc(length, sizeof(char));
    if (!check_game_over)
        grid[(add[0] - 1) * no_clm + add[1] - 1] = ':';
    add[1] += 1;

    while (1)
    {
        temp = score % 10;
        switch (temp)
        {
        case 0:
            num = '0';
            break;
        case 1:
            num = '1';
            break;
        case 2:
            num = '2';
            break;
        case 3:
            num = '3';
            break;
        case 4:
            num = '4';
            break;
        case 5:
            num = '5';
            break;
        case 6:
            num = '6';
            break;
        case 7:
            num = '7';
            break;
        case 8:
            num = '8';
            break;
        case 9:
            num = '9';
            break;
        }
        score /= 10;
        player_score[i] = num;
        if (score == 0)
            break;
        else
        {
            length++;
            i++;
            player_score = (char *)realloc(player_score, length * sizeof(char));
        }
    }
    int j;
    for (i = (length - 1), j = 1; i >= 0; i--, j++)
    {
        grid[(add[0] - 1) * no_clm + add[1] + j - 1] = player_score[i];
    }
    free(player_score);
    return add[1] + length + 1;
}

/*This function will take grid as argument and then print grid for the game instructions/help*/
int help(char *grid)
{
    char grid_cpy[strlen(grid)];
    set_grid(grid_cpy);
    int cursor[2];
    cursor[0] = (no_row - 10) / 2;
    cursor[1] = (no_clm - 68 + 1) / 2;
    set_option_frame(grid_cpy, 3, cursor);
    print_grid(grid_cpy);
    fflush(stdin);
    while (1)
    {
        char temp = (char)getch();
        if (temp == '\r' || (int)temp == 27)
        {
            return menu(grid);
        }
    }
    return menu(grid);
}

/*this function will put developers credits in the grid*/
int credits(char *grid)
{
    char grid_cpy[strlen(grid)];
    set_grid(grid_cpy);
    int cursor[2];
    for (int i = 0; i < strlen(grid_cpy); i++)
    {
        if (grid_cpy[i] == boundary_character || grid_cpy[i] == '\n' || grid_cpy[i] == '\0')
            continue;
        else
            grid_cpy[i] = '-';
    }
    cursor[0] = (no_row - 3) / 2;
    cursor[1] = (no_clm - 21 + 1) / 2;
    set_option_frame(grid_cpy, 4, cursor);
    print_grid(grid_cpy);
    fflush(stdin);
    while (1)
    {
        char temp = (char)getch();
        if (temp == '\r' || (int)temp == 27)
        {
            return menu(grid);
        }
    }
    return menu(grid);
}

void play_game(char *grid)
{
    check_game = 1;
    extern int snake_length, starting_snake_length;
    extern int **snake;
    starting_snake_length = default_snake_length;
    set_grid(grid);
    grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] = snake_head_char;
    for (int i = 1; i < snake_length; i++)
        grid[(snake[i][0] - 1) * no_clm + snake[i][1] - 1] = snake_tail_char;
    spawn_fruit(grid);
    print_grid(grid);
    // above i put snake in the grid
    char previous_derection, current_direction;
    previous_derection = get_starting_direction();
    // printf("previous_derection = %c\ncurrent_direction = %c",previous_derection,current_direction);
    // getch();
    int direction;
    direction = get_first_direction(previous_derection);
    // printf("%c",(char)direction);
    // getch();
    int check_pause = 0;
    while (1)
    {
        int check = 0, check_return = 0, check_out = 0;
        check = 0;
        switch (direction)
        {
        case 224:
            check = 1;
            break;
        case 0x48:
        case (int)'w':
        case (int)'W':
            if (previous_derection == 'd')
                current_direction = 'd';
            else
                current_direction = 'u';
            previous_derection = current_direction;
            break;
        case 0x50:
        case (int)'s':
        case (int)'S':
            if (previous_derection == 'u')
                current_direction = 'u';
            else
                current_direction = 'd';
            previous_derection = current_direction;
            break;
        case 0x4b:
        case (int)'a':
        case (int)'A':
            if (previous_derection == 'r')
                current_direction = 'r';
            else
                current_direction = 'l';
            previous_derection = current_direction;
            break;
        case 0x4d:
        case (int)'d':
        case (int)'D':
            if (previous_derection == 'l')
                current_direction = 'l';
            else
                current_direction = 'r';
            previous_derection = current_direction;
            break;
        case 27:
        case 32:
        case (int)'p':
            current_direction = pause(grid, previous_derection);
            previous_derection = current_direction;
            check_pause = 0;
            break;
        case (int)'e':
            return;
            break;
        case (int)'o':
            // print_grid(grid);
            // printf("\n**********************************************************************************GAME OVER**********************************************************************************\n");
            check_game_over = 1;
            check_game = 0;
            gameover(grid);
            points = current_score;
            current_score = 0;
            direction = (int)'e';
            break;
        default:
            current_direction = previous_derection;
        }
        if (direction == (int)'e' || current_direction == (int)'e')
        {
            if (current_direction == (int)'e')
            {
                extern int check_save_exit;
                direction = (int)'e';
                check_save_exit = 1;
            }
            continue;
        }
        if (check)
        {
            direction = (int)getch();
            continue;
        }
        // printf("\ncurrent_direction = %c",current_direction);
        // getch();
        direction = move_snake(grid, current_direction);
        // getch();
        // if(check)
        // continue;
        // direction=move_snake(grid,snake,current_direction);
    }
}

/*this function returns the starting direction of the snake menas when snake is not moving*/
char get_starting_direction(void)
{
    extern int **snake;
    if (snake[1][0] == snake[0][0] + 1 || (snake[0][0] == no_row - 1 && snake[1][0] == 2))
        return 'u';
    else if (snake[1][0] == snake[0][0] - 1 || (snake[0][0] == 2 && snake[1][0] == no_row - 1))
        return 'd';
    else if (snake[1][1] == snake[0][1] + 1 || (snake[0][0] == no_clm - 2 && snake[1][0] == 2))
        return 'l';
    else
        return 'r';
}

/*this function will take right input for the dirction of the snake for the first time*/
int get_first_direction(char previous_derection)
{
    int direction;
    if (previous_derection == 'u')
    {
        while (1)
        {
            direction = (int)getch();
            if (direction == 0x48 || direction == (int)'W' || direction == (int)'w')
                return direction;
            else if (direction == 0x4d || direction == (int)'d' || direction == (int)'D')
                return direction;
            else if (direction == 0x4b || direction == 'a' || direction == 'A')
                return direction;
        }
    }
    else if (previous_derection == 'd')
    {
        while (1)
        {
            direction = (int)getch();
            if (direction == 0x4d || direction == (int)'d' || direction == (int)'D')
                return direction;
            else if (direction == 0x4b || direction == 'a' || direction == 'A')
                return direction;
            else if (direction == 0x50 || direction == 's' || direction == 'S')
                return direction;
        }
    }
    else if (previous_derection == 'l')
    {
        while (1)
        {
            direction = (int)getch();
            if (direction == 0x4b || direction == 'a' || direction == 'A')
                return direction;
            else if (direction == 0x50 || direction == 's' || direction == 'S')
                return direction;
            else if (direction == 0x48 || direction == (int)'W' || direction == (int)'w')
                return direction;
        }
    }
    else if (previous_derection == (int)'r')
        while (1)
        {
            direction = (int)getch();
            if (direction == 0x50 || direction == 's' || direction == 'S')
                return direction;
            else if (direction == 0x48 || direction == (int)'W' || direction == (int)'w')
                return direction;
            else if (direction == 0x4d || direction == (int)'d' || direction == (int)'D')
                return direction;
        }
}

/*this will move snake untill kbhit or snake touches his head*/
int move_snake(char *grid, char current_direction)
{
    int direction;
    extern int **snake;
    int check_kbhit = 0;
    extern int starting_snake_length, current_score;

    while (!kbhit())
    {
        int current_add[2]; // this  contains snakes current adress which is before updation accordinf to the direction given
        current_add[0] = snake[0][0];
        current_add[1] = snake[0][1];
        check_kbhit = 1;
        if (current_direction == 'u')
        {
            if (snake[0][0] == 2)
                snake[0][0] = no_row - 1;
            else
                snake[0][0]--;
        }
        else if (current_direction == 'd')
        {
            if (snake[0][0] == no_row - 1)
                snake[0][0] = 2;
            else
                snake[0][0]++;
        }
        else if (current_direction == 'l')
        {
            if (snake[0][1] == 2)
                snake[0][1] = no_clm - 2;
            else
                snake[0][1]--;
        }
        else
        {
            if (snake[0][1] == no_clm - 2)
                snake[0][1] = 2;
            else
                snake[0][1]++;
        }
        if (grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] == snake_tail_char)
        {
            // snake[0][0] = current_add[0];
            // snake[0][1] = current_add[1];

            move_tail(grid, current_add);
            grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] = snake_head_char;
            return (int)'o';
        }
        if (grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] != fruit_char)
        {
            grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] = snake_head_char;
            grid[(current_add[0] - 1) * no_clm + current_add[1] - 1] = blank;
            move_tail(grid, current_add);
        }
        if (grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] == fruit_char)
        {

            increase_snake_length();
            grid[(snake[0][0] - 1) * no_clm + snake[0][1] - 1] = snake_head_char;
            move_tail(grid, current_add);
            spawn_fruit(grid);
        }

        current_score = (snake_length - starting_snake_length) * 10;

        print_grid(grid);
    }
    // char a='a';
    // increase_snake_length();
    if (check_kbhit == 0)
    {
        getch();
        getch();
        //    getch();
    }
    // if(check_kbhit==1)
    direction = (int)getch();

    // grid[(3-1)*no_clm + 6-1]=fruit_char;
    return direction;
}

/*this function will move tail along with the head*/
void move_tail(char *grid, int *previous_add)
{
    extern int snake_length;
    extern int **snake;
    int current_add[2], new_add[2];
    for (int i = 1; i < snake_length; i++)
    { // if we call this function two times in a row then thin this will wipe out the last row cuse in that scenario current_add==previous_add=snake[i][0]; or the the first time therefore it will move snake_length-2 tales to the first tales position
        current_add[0] = snake[i][0];
        current_add[1] = snake[i][1];
        new_add[0] = previous_add[0];
        new_add[1] = previous_add[1];
        if (grid[(current_add[0] - 1) * no_clm + current_add[1] - 1] != snake_head_char)
            grid[(current_add[0] - 1) * no_clm + current_add[1] - 1] = blank;
        snake[i][0] = new_add[0];
        snake[i][1] = new_add[1];
        grid[(new_add[0] - 1) * no_clm + new_add[1] - 1] = snake_tail_char;
        previous_add[0] = current_add[0];
        previous_add[1] = current_add[1];
    }
}

/*this function will make snake one steo longer*/
void increase_snake_length(void)
{
    extern int snake_length;
    extern int **snake;
    //    char direction;//direction of last two tails
    //     if(snake[snake_length-1][1]==snake[snake_length-2][1]-1)
    snake_length++;
    snake = (int **)realloc(snake, sizeof(int *) * snake_length);
    if (snake == NULL)
        printf("Erorr mememory full");
    snake[snake_length - 1] = (int *)calloc(2, sizeof(int));
    if (snake[snake_length - 1] == NULL)
        printf("Erorr mememory full");
}

/*this function will spawn fruits randomly on the grid*/
void spawn_fruit(char *grid)
{
    int rand_clmn, rand_row;
    while (1)
    {
        srand(time(NULL));
        rand_row = rand() % (no_row - 1);
        srand(time(NULL));
        rand_clmn = rand() % (no_clm - 2);
        if (rand_row == 0 || rand_clmn == 0)
            continue;
        char temp = grid[(rand_row - 1) * no_clm + rand_clmn - 1];
        if (temp == snake_head_char || temp == snake_tail_char || temp == boundary_character)
            continue;
        else
        {
            grid[(rand_row - 1) * no_clm + rand_clmn - 1] = fruit_char;
            break;
        }
    }
    return;
}

/*this functions pauses the game and returns e if save and exit or for resume nothing*/
char pause(char *grid, char previous_direction)
{
    int grid_length = strlen(grid);
    char grid_cpy[grid_length];
    set_grid(grid_cpy);
    char resume[][14] = {"RESUME", "SAVE AND EXIT"};
    int cursor_Starting_add[2];
    cursor_Starting_add[1] = (no_clm - 14 + 1) / 2;
    cursor_Starting_add[0] = (no_row - 2 + 1) / 2;
    for (int k = 0, i = 0; i < 3; i += 2, k++)
    {
        for (int j = 0; j < strlen(resume[k]); j++)
        {
            if (i == 0)
                grid_cpy[(cursor_Starting_add[0] - 1 + i) * no_clm + (cursor_Starting_add[1] + j + 3) - 1 + 1] = resume[k][j];
            else
                grid_cpy[(cursor_Starting_add[0] - 1 + i) * no_clm + (cursor_Starting_add[1] + j) - 1 + 1] = resume[k][j];
        }
    }
    int lower_limit_clmn, lower_limit_row, upper_limit_clmn, upper_limit_row;
    int cursor[2];
    cursor[0] = cursor_Starting_add[0];
    cursor[1] = cursor_Starting_add[1];
    lower_limit_row = -1;
    lower_limit_clmn = -1;
    upper_limit_clmn = 14;
    upper_limit_row = 3;
    for (int j = lower_limit_row; j < upper_limit_row + 1; j++)
    {
        for (int k = lower_limit_clmn; k < upper_limit_clmn + 1; k++)
        {
            if (j == lower_limit_row || j == upper_limit_row || k == lower_limit_clmn || k == upper_limit_clmn)
            {
                if (j == lower_limit_row && k == lower_limit_clmn)
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)201;
                else if (j == lower_limit_row && k == upper_limit_clmn)
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)187;
                else if (j == upper_limit_row && k == lower_limit_clmn)
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)200;
                else if (j == upper_limit_row && k == upper_limit_clmn)
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)188;
                else if (j == lower_limit_row || j == upper_limit_row)
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)205;
                else
                    grid_cpy[(cursor[0] + j - 1) * no_clm + cursor[1] + k - 1] = (char)186;
            }
        }
    }
    grid_cpy[(cursor_Starting_add[0] - 1) * no_clm + cursor_Starting_add[1] - 1] = (char)16;
    print_grid(grid_cpy);
    int menu_point = 0;
    int check_enter = 0;
    int check = 0;
    while (1)
    {
        check = 0;
        switch ((int)getch())
        {
        case 224:
            check = 1;
            break;
        case 0x48:
        case (int)'w':
        case (int)'W':
            grid_cpy[(cursor_Starting_add[0] - 1 + menu_point) * no_clm + cursor_Starting_add[1] - 1] = blank;
            if (menu_point == 0)
                menu_point = 2;
            else
                menu_point = 0;
            break;
        case 0x50:
        case (int)'s':
        case (int)'S':
            grid_cpy[(cursor_Starting_add[0] - 1 + menu_point) * no_clm + cursor_Starting_add[1] - 1] = blank;
            if (menu_point == 2)
                menu_point = 0;
            else
                menu_point = 2;
            break;
        case 27:
        case 32:
            menu_point = 0;
            check_enter = 1;
            break;
            break;
        case (int)'\r':
            check_enter = 1;
            break;
        default:
            check = 1;
            break;
        }
        if (check)
            continue;
        if (check_enter)
            break;
        else
        {
            grid_cpy[(cursor_Starting_add[0] - 1 + menu_point) * no_clm + cursor_Starting_add[1] - 1] = (char)16;
            print_grid(grid_cpy);
        }
    }
    print_grid(grid_cpy);

    if (menu_point == 0)
    {
        print_grid(grid);
        int direction = get_first_direction(previous_direction);
        switch (direction)
        {
        case 0x48:
        case (int)'w':
        case (int)'W':
            return 'u';
            break;
        case 0x50:
        case (int)'s':
        case (int)'S':
            return 'd';
            break;
        case 0x4b:
        case (int)'a':
        case (int)'A':
            return 'l';
            break;
        case 0x4d:
        case (int)'d':
        case (int)'D':
            return 'r';
            break;
        }
    }
    else
    {
        points = current_score;
        return 'e';
    }
}

/*this function updates data in data.txt file after the game played*/
void update_game(char *grid)
{
    extern int snake_length, current_player_number, current_score, no_of_players, **snake;
    check_game = 0;
    if (check_game_over)
    {
        get_snake_length(0);
        create_snake(0);
        current_score = 0;
    }
    if (current_player_number == 0)
    {
        get_name(grid);
        FILE *data_ptr = fopen("data.txt", "a");
        fprintf(data_ptr, "length>");
        for (int i = 1; i <= 10; i++)
            fprintf(data_ptr, " ");
        fputc('\n', data_ptr);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "r+");
        fseek(data_ptr, -15, SEEK_END);
        fseek(data_ptr, 0, SEEK_CUR);
        while (1)
        {
            char temp = fgetc(data_ptr);
            if (temp == '>')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        fprintf(data_ptr, "%d", snake_length);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "a");
        fprintf(data_ptr, "$");
        for (int i = 1; i <= 1000; i++)
            fprintf(data_ptr, " ");
        fputc('\n', data_ptr);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "r+");
        fseek(data_ptr, 0, SEEK_END);
        fseek(data_ptr, -1010, SEEK_CUR);
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '$')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        for (int i = 0; i < snake_length; i++)
            fprintf(data_ptr, " %d,%d", snake[i][0], snake[i][1]);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "a");
        fprintf(data_ptr, "highscore-");
        for (int i = 1; i <= 10; i++)
            fprintf(data_ptr, " ");
        fputc('\n', data_ptr);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "r+");
        fseek(data_ptr, 0, SEEK_END);
        fseek(data_ptr, -15, SEEK_CUR);
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '-')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        fprintf(data_ptr, "%d", points);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "a");
        fprintf(data_ptr, "score : ");
        for (int i = 1; i <= 10; i++)
            fprintf(data_ptr, " ");
        fputc('\n', data_ptr);
        fclose(data_ptr);
        data_ptr = fopen("data.txt", "r+");
        fseek(data_ptr, 0, SEEK_END);
        fseek(data_ptr, -17, SEEK_CUR);
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == ':')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        fseek(data_ptr, 1, SEEK_CUR);
        fprintf(data_ptr, "%d", current_score);
        fclose(data_ptr);
    }
    else
    {

        FILE *data_ptr = fopen("data.txt", "r+");
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '@')
            {
                int num;
                fscanf(data_ptr, "%d", &num);
                if (num == current_player_number)
                    break;
            }
        }
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '>')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        for (int i = 1; i <= 10; i++)
            fprintf(data_ptr, " ");
        fseek(data_ptr, -10, SEEK_CUR);
        fprintf(data_ptr, "%d", snake_length);
        fseek(data_ptr, 0, SEEK_CUR);
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '$')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        for (int i = 1; i <= 1000; i++)
            fprintf(data_ptr, " ");
        fseek(data_ptr, -1000, SEEK_CUR);

        for (int i = 0; i < snake_length; i++)
            fprintf(data_ptr, " %d,%d", snake[i][0], snake[i][1]);
        fseek(data_ptr, 0, SEEK_CUR);
        while (1)
        {
            char temp = fgetc(data_ptr);
            if (temp == '-')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        int high;
        fscanf(data_ptr, "%d", &high);
        fseek(data_ptr, 0, SEEK_CUR);
        if (points > high)
        {
            fseek(data_ptr, -12, SEEK_CUR);
            while (1)
            {
                char temp = fgetc(data_ptr);
                if (temp == '-')
                    break;
            }
            fseek(data_ptr, 0, SEEK_CUR);
            for (int i = 1; i <= 10; i++)
                fprintf(data_ptr, " ");
            fseek(data_ptr, 0, SEEK_CUR);
            fseek(data_ptr, -10, SEEK_CUR);
            fprintf(data_ptr, "%d", points);
        }
        fseek(data_ptr, 0, SEEK_CUR);
        while (1)
        {
            char temp = fgetc(data_ptr);
            if (temp == ':')
                break;
        }
        fseek(data_ptr, 0, SEEK_CUR);
        fseek(data_ptr, 1, SEEK_CUR);
        for (int i = 1; i <= 10; i++)
            fprintf(data_ptr, " ");
        fseek(data_ptr, -10, SEEK_CUR);
        fprintf(data_ptr, "%d", current_score);

        fclose(data_ptr);
    }
}

/*if new game then this function will take name of the player*/
void get_name(char *grid)
{
    char alphabets[11];
    while (1)
    {
        set_grid(grid);
        int take_name_length = 21;
        char name[11] = {"ENTER NAME"};
        int name_add[2];
        name_add[0] = ((no_row - 2 + 1) / 2) - 2;
        name_add[1] = (no_clm - 21) / 2 + 1;
        for (int i = 0; i < strlen(name); i++)
            grid[(name_add[0] - 1) * no_clm + name_add[1] + ((20 - strlen(name) + 1) / 2) + i - 1] = name[i];
        for (int i = 0; i < take_name_length; i += 2)
        {
            grid[(name_add[0] + 2 - 1 - 1) * no_clm + name_add[1] + i - 1] = (char)194;
            grid[(name_add[0] + 2 - 1) * no_clm + name_add[1] + i - 1] = (char)179;
            grid[(name_add[0] + 2 - 1 + 1) * no_clm + name_add[1] + i - 1] = (char)193;
        }
        for (int i = 0; i < take_name_length; i++)
        {
            if (i == 0)
            {
                grid[(name_add[0] + 2 - 1 - 1) * no_clm + name_add[1] + i - 1] = (char)218;
                grid[(name_add[0] + 2 - 1 + 1) * no_clm + name_add[1] + i - 1] = (char)192;
            }
            else if (i == take_name_length - 1)
            {
                grid[(name_add[0] + 2 - 1 - 1) * no_clm + name_add[1] + i - 1] = (char)191;
                grid[(name_add[0] + 2 - 1 + 1) * no_clm + name_add[1] + i - 1] = (char)217;
            }
            else if (i % 2 != 0)
            {
                grid[(name_add[0] + 2 - 1 - 1) * no_clm + name_add[1] + i - 1] = (char)196;
                grid[(name_add[0] + 2 - 1 + 1) * no_clm + name_add[1] + i - 1] = (char)196;
            }
        }
        print_grid(grid);
        int alphabet_pointer = 1, i = 0;
#define increase_alpha         \
    {                          \
        alphabet_pointer += 2; \
        i++;                   \
    }
        int check_duplicacy = 0;

        while (1)
        {
            char alpha = (int)getch();

            if (alpha >= 65 && alpha <= 122 && i < 10)
            {
                if (alpha <= 122 && alpha >= 97 && i == 0)
                    alpha -= 32;
                grid[(name_add[0] + 2 - 1) * no_clm + name_add[1] + alphabet_pointer - 1] = (char)alpha;
                print_grid(grid);

                alphabets[i] = (char)alpha;
                increase_alpha
            }
            else if (alpha == (char)'\b' && i > 0)
            {
                alphabet_pointer -= 2;
                i--;
                grid[(name_add[0] + 2 - 1) * no_clm + name_add[1] + alphabet_pointer - 1] = blank;
                alphabets[i] = blank;
                print_grid(grid);
            }
            else if (alpha == '\r' && i > 0)
            {
                break;
            }
        }
        alphabets[i] = '\0';
        check_duplicacy = check_name_duplicacy(grid, alphabets);
        if (check_duplicacy)
            continue;
        else
            break;
    }
    FILE *data_ptr = fopen("data.txt", "r+");
    extern int no_of_players, current_player_number;
    no_of_players++;
    fseek(data_ptr, 8, SEEK_SET);
    fprintf(data_ptr, "%d", no_of_players);
    fseek(data_ptr, 0, SEEK_END);
    fprintf(data_ptr, "@%d %s\n", no_of_players, alphabets); // alphabets is just name and i named it like this beacuase i dint came up with something else
    current_player_number = no_of_players;
    fclose(data_ptr);
}

/*this function will check all the names if there any duplicate name in there*/
int check_name_duplicacy(char *grid, char *name)
{
    extern int no_of_players;

    FILE *data_ptr = fopen("data.txt", "r");
    for (int i = 1; i <= no_of_players; i++)
    {
        while (1)
        {
            char temp = (char)fgetc(data_ptr);
            if (temp == '@')
            {
                int temp;
                fscanf(data_ptr, "%d", &temp);
                fseek(data_ptr, 1, SEEK_CUR);
                break;
            }
        }
        char name_array[11];
        fscanf(data_ptr, "%s", name_array);
        int check = strcmp(name_array, name);
        if (check == 0)
        {
            int name_add[2];
            name_add[0] = ((no_row - 2 + 1) / 2) - 9;
            name_add[1] = (no_clm - 21) / 2 + 1 - 16;

            char error[] = "THIS NAME EXISTS BEFORE";
            for (int i = 0; i < strlen(error); i++)
                grid[(name_add[0] - 1) * no_clm + name_add[1] + i + 15 + -1] = error[i];
            char error2[] = "PRESS ENTER TO TRY AGAIN!";
            for (int i = 0; i < strlen(error2); i++)
                grid[(name_add[0] - 1 + 1 + 1) * no_clm + name_add[1] + 14 + i - 1] = error2[i];
            print_grid(grid);
            while (1)
            {
                if (getch() == '\r')
                    break;
            }
            return 1;
            break;
        }
    }
    return 0;
}

/*This function will print score and snake length on the grid after game over*/
void gameover(char *grid)
{
    char grid_cpy[strlen(grid)];
    set_grid(grid_cpy);
    int cursor[2];
    cursor[0] = (no_row - 4) / 2;
    cursor[1] = (no_clm - 26 + 1) / 2;
    set_option_frame(grid_cpy, 5, cursor);
    print_grid(grid_cpy);
    fflush(stdin);
    while (1)
    {
        char temp = (char)getch();
        if (temp == '\r' || (int)temp == 27)
        {
            break;
        }
    }
}
