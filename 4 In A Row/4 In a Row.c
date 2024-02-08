#include <stdio.h>
#include <stdbool.h>

#define BOARD_MAX_SIDE 9
#define BOARD_MIN_SIDE 3
#define MIN_TOKENS 3

#define RED_SLOT_SYMBOL ('R')
#define YELLOW_SLOT_SYMBOL ('Y')
#define EMPTY_SLOT_SYMBOL (' ')


#define ROWS_IDX  0
#define COLS_IDX  1
#define TOKENS_IDX 2
#define NUM_OF_PARAMS 3

#define P1_IDX 1
#define P2_IDX 2
#define NUM_OF_PLAYERS 2

//sending numbers to there and bringing them back
#define ISOLATED_AREA 20
// a number that probably is enough
#define MAX_MOVES 1000

//negative number for tie printing
#define TIE_PLAYER_NUM -1
#define ERROR_NUM 500

#define REDO_IDX -2
#define UNDO_IDX -1

#define WIN_IDX 0
#define TIE_IDX 1
#define WIN_OR_TIE 2

//victories indexes:
#define ROW_V_IDX  0
#define COL_V_IDX  1
#define R_DIAGONAL_V_IDX 2
#define L_DIAGONAL_V_IDX 3
#define NUM_OF_POSSIBLE_VICTORIES 4
//--------------------------- Board Edges ------------------------------//

#define BOARD_VERT_SEP  '|'
#define BOARD_LEFT_ANG '\\'
#define BOARD_RIGHT_ANG '/'
#define BOARD_BOTTOM  '-'
#define BOARD_BOTTOM_SEP '-'



//----------------------------------------------------------------------//
//-------------------------setup functions:-----------------------------//

//user choosing parameters, inserting them to array and returning if scanf succeeded
int welcome_choose_params(char players_color[NUM_OF_PLAYERS+1], int possible_params[NUM_OF_PARAMS]);
//checking if the parameters that the player chose are valid and returning false if one of them is invalid
bool Is_Valid(int possible_params[NUM_OF_PARAMS], int param);
//returning the color of player 2, based on player 1 choice.
char apply_p2_color(char player1_color);
//returning which player's turn it is, based on the turn number.
int find_player(int turn);
// setting up the board based on the user chosen rows and columns
void set_board(char board[][BOARD_MAX_SIDE], int rows, int cols);


//-------------------------mid game functions:-----------------------------//

/*asking the user to choose a move based on if it's a "new" move or a move after an illegal move.
  putting it inside player choice and returning whether or not scanf succeeded*/
int choose_move(int player,int* player_choice,bool is_conventional_choice);
// applying the player's choice and returning if its a conventional choice or not
bool apply_players_choice(int moves[MAX_MOVES],int turn_idx, int player_choice, bool* vc_ptr, char board[][BOARD_MAX_SIDE],
                          int rows, int cols, char players_color[NUM_OF_PLAYERS+1]);
//checking if player's choice is in valid range and returning if it is
bool player_choice_is_valid(int player_choice, int cols);
//function starts if player have played a valid move
void do_valid_scenerio(char board[][BOARD_MAX_SIDE],int player_choice,bool* vc_ptr,int rows,char players_color[NUM_OF_PLAYERS],int player);
void insert_token(char board[][BOARD_MAX_SIDE], int rows, int player_choice, char players_color);
//returning if the chosen col is full
bool col_is_full(char board[][BOARD_MAX_SIDE], int player_choice);

// function starts if player asked to undo last move
void do_undo_scenario(char board[][BOARD_MAX_SIDE],bool* vc_ptr,int rows,int cols,char players_color[NUM_OF_PLAYERS+1],
                                                                                    int moves[MAX_MOVES],int turn_idx);
//returning which col to delete after an undo move
int col_to_del(int moves[MAX_MOVES], int turn_idx);
//deleting all the documented undos after a legal play so they can't be redo
void del_undos_after_valid_move(int moves[MAX_MOVES],int turn_idx, bool* vc_ptr);
//returning if the opponent has his color on the board
bool opponent_board_is_empty (char board[][BOARD_MAX_SIDE], int rows, int cols, int opponent_color);
void undo_last_move(char board[][BOARD_MAX_SIDE],int col_to_del, int rows, int opponent_color);
//function starts if player have asked to redo his last deleted move
void do_redo_scenario(int moves[MAX_MOVES],int turn_idx,bool* vc_ptr,char board[][BOARD_MAX_SIDE],int rows,char players_color[NUM_OF_PLAYERS+1]);


//-------------------------end game functions:-----------------------------//

//returning if there's a tie
bool check_if_tie(char board[][BOARD_MAX_SIDE],int turn_idx,int rows,int cols);
//returning if there's a victory
bool check_if_won(char board[][BOARD_MAX_SIDE],int turn_idx,char players_color[NUM_OF_PLAYERS+1],int rows,int cols,int tokens);

// searching for different kind of victories and returning if someone won:
bool check_for_row_victory(char board[][BOARD_MAX_SIDE],int rows,int cols,int tokens,char current_color);
bool check_for_col_victory(char board[][BOARD_MAX_SIDE],int rows,int cols,int tokens,char current_color);

bool check_for_right_diagonal_victory(char board[][BOARD_MAX_SIDE],int tokens,char current_color);
bool check_for_right_1st_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color);
bool check_for_right_2nd_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color);

bool check_for_left_diagonal_victory(char board[][BOARD_MAX_SIDE],int tokens,char current_color);
bool check_for_left_1st_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color);
bool check_for_left_2nd_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color);

void win_or_tie_check(bool win_or_tie[WIN_OR_TIE],char board[][BOARD_MAX_SIDE], int turn_idx,
             char players_color[NUM_OF_PLAYERS+1], int rows, int cols, int tokens, int player);
//function that playing the entire game until there's a win or a tie and returning whether or not scanf succeeded
int start_game(char board[][BOARD_MAX_SIDE],int rows,int cols,int moves[MAX_MOVES],char players_color[NUM_OF_PLAYERS+1]
                                                                               ,bool win_or_tie[WIN_OR_TIE],int tokens);

//-------------------------printing functions:-----------------------------//
void print_welcome_message();
void print_read_game_params_message(int param);
void print_chose_color_message();
void print_chose_move_message(int player);
void print_enter_column_message();
void print_full_column_message();
void print_unavailable_undo_redo_message();


/*
 * Outputs game board.
 *
 * board - contains spaces for empty cells, 'Y' or 'R' for
 *         respective player cells.
 * board_side - contains board dimensions {rows, cols}
 */
void print_board(char board[][BOARD_MAX_SIDE], int rows, int cols);

/*
 * Outputs winner or tie message.
 *
 * player_id - the id to be output. Pass <0 for tie.
 */
void print_winner(int player_id);

//--------------------------- Main Program -----------------------------//


// 10 lines function
int main()
{
//setting up possible colors, parameters to chose, board and moves documentation
    char players_color[NUM_OF_PLAYERS+1] = {0}, board[BOARD_MAX_SIDE][BOARD_MAX_SIDE] = {{0}};
    int possible_params[NUM_OF_PARAMS] = {0}, moves[MAX_MOVES] = {0};
//welcome message, user choosing rows, cols and tokens and making sure scanf works
    if (welcome_choose_params(players_color, possible_params))
    {
        return 1;
    }
//taking the parameters out from the array and into variables
    int rows = possible_params[ROWS_IDX], cols = possible_params[COLS_IDX], tokens = possible_params[TOKENS_IDX];

    set_board(board, rows, cols);
     bool win_or_tie[WIN_OR_TIE] = {false, false};
//making sure scanf works and starting the game until we get a winner or a tie
     if(start_game(board, rows, cols, moves, players_color, win_or_tie, tokens))
        {
            return 1;
        }

        return 0;
}
//12 lines in function
int welcome_choose_params(char players_color[NUM_OF_PLAYERS+1], int possible_params[NUM_OF_PARAMS])
{
   print_welcome_message();
//looping until we get a valid color
    while (players_color[P1_IDX] != RED_SLOT_SYMBOL && players_color[P1_IDX] != YELLOW_SLOT_SYMBOL)
    {
        print_chose_color_message();
        if (scanf(" %c", &players_color[P1_IDX]) == 0)
        {
            return 1;
        }
    }
//player 2 get the other color
    players_color[P2_IDX] = apply_p2_color(players_color[P1_IDX]);

//loop for getting all the params: rows, cols and tokens
    for (int param = ROWS_IDX; param < NUM_OF_PARAMS; param++)
    {
        while (Is_Valid(possible_params, param) == false)
        {
            print_read_game_params_message(param);
            if (scanf("%d", &possible_params[param]) == 0)
            {
                return 1;
            }
        }

    }
    return 0;
}
// 12 lines function
bool Is_Valid(int possible_params[NUM_OF_PARAMS], int param)
{
    bool Is_Valid = true;
//checking that every param is in range
    int rows = possible_params[ROWS_IDX];
    if (param == ROWS_IDX)
    {
        if (possible_params[param] < BOARD_MIN_SIDE || possible_params[param] > BOARD_MAX_SIDE )
            {
                Is_Valid = false;
            }
    }

    if (param == COLS_IDX)
    {
        if (possible_params[param] < rows || possible_params[param] > BOARD_MAX_SIDE )
        {
            Is_Valid = false;
        }
    }

    if (param == TOKENS_IDX)
    {
        if(possible_params[param] > rows || possible_params[param] < MIN_TOKENS)
        {
            Is_Valid = false;
        }

    }

        return Is_Valid;
}

// 5 lines function
char apply_p2_color(char player1_color)
{
    char player2_color = 0;
    if (player1_color == RED_SLOT_SYMBOL)
    {
        player2_color = YELLOW_SLOT_SYMBOL;
    }
    else
    {
        player2_color = RED_SLOT_SYMBOL;
    }
    return player2_color;
}

// 5 lines function
int find_player(int turn_idx)
{
    int player = 0;
// if the turn is even, then its player 2 turn
    if (turn_idx % 2 == 0)
    {
        player = 2;
    }
    else
    {
        player  = 1;
    }
    return player;
}

// 3 lines function
void set_board(char board[][BOARD_MAX_SIDE], int rows, int cols)
{
    for (int i = 0; i <= rows; i++)
    {
        for (int j = 0; j <= cols; j++)
        {
            board[i][j] = EMPTY_SLOT_SYMBOL;
        }

    }
}

//6 lines function
int choose_move(int player,int* player_choice,bool is_conventional_choice)
{
//printing full message only for a "new" turn
    if(is_conventional_choice)
    {
    print_chose_move_message(player);
    }
//printing only enter column message for another choice after an illegal play
    print_enter_column_message();
    if (scanf("%d", player_choice) == 0)
    {
        return 1;
    }
    return 0;
}

// 13 lines function
bool apply_players_choice(int moves[MAX_MOVES],int turn_idx, int player_choice, bool* vc_ptr, char board[][BOARD_MAX_SIDE],
                          int rows, int cols, char players_color[NUM_OF_PLAYERS+1])
 {
//finding out current player based on current turn
    int player = find_player(turn_idx);
//documenting the player's choice inside an array
    moves[turn_idx] = player_choice;
//checking if it's a valid choice
    *vc_ptr = player_choice_is_valid(player_choice, cols);
    bool is_conventional_choice = false;
    if (*vc_ptr)
    {
        do_valid_scenerio(board,player_choice,vc_ptr,rows,players_color,player);
//if there was a legal move, making sure not to redo afterwards
        del_undos_after_valid_move(moves, turn_idx, vc_ptr);
//if move was legal and column isn't full then it's a valid, conventional choice
        is_conventional_choice = *vc_ptr;
    }
    else if (player_choice == -1)
    {
        do_undo_scenario(board,vc_ptr,rows,cols,players_color,moves,turn_idx);
    }
    else if (player_choice == -2)
    {
        do_redo_scenario(moves, turn_idx,vc_ptr,board,rows,players_color);
    }
    return is_conventional_choice;
}
// 4 lines function
bool player_choice_is_valid(int player_choice, int cols)
{
    bool player_choice_is_valid = false;
    if (player_choice > 0 && player_choice <= cols)
    {
        player_choice_is_valid = true;
    }

    return player_choice_is_valid;
}
// 4 lines function
void do_valid_scenerio(char board[][BOARD_MAX_SIDE],int player_choice,bool* vc_ptr, int rows,char players_color[NUM_OF_PLAYERS],int player)
{
     if (col_is_full(board, player_choice))
        {
            print_full_column_message();
//changing valid choice value to false because the col is full
            *vc_ptr = false;
        }
    else
    {
//if the col isn't full, then inserting the token there
    insert_token(board, rows, player_choice, players_color[player]);
    }
}

// 4 lines function
void insert_token(char board[][BOARD_MAX_SIDE], int rows, int player_choice, char players_color)
{
    for (int i = rows - 1; i >= 0; i-- )
    {
        if (board[i][player_choice-1] == EMPTY_SLOT_SYMBOL)
        {
            board[i][player_choice-1] = players_color;
            break;
        }
    }
}

// 4 lines function
bool col_is_full(char board[][BOARD_MAX_SIDE], int player_choice)
{
    bool col_is_full = false;
//checking if the chosen col is full
    if (board[0][player_choice-1] != EMPTY_SLOT_SYMBOL)
    {
        col_is_full = true;
    }
    return col_is_full;
}


// 8 lines function
void do_undo_scenario(char board[][BOARD_MAX_SIDE],bool* vc_ptr,int rows,int cols,char players_color[NUM_OF_PLAYERS+1],
                                                                                    int moves[MAX_MOVES],int turn_idx)
 {
// looking for opponent number to find the color
    int opponent = find_player(turn_idx+1);
    int opponent_color = players_color[opponent];
//using opponent color to check if it's on the board
    if(opponent_board_is_empty(board, rows, cols, opponent_color))
    {
//if opponent's color is not on the board then checking valid choice to false so the user could choose again
        print_unavailable_undo_redo_message();
        *vc_ptr = false;
    }
    else
    {
//deleting the last move the opponent have made
    int col_to_undo = col_to_del(moves, turn_idx) - 1;
    undo_last_move(board, col_to_undo, rows, opponent_color);
    *vc_ptr = true;
    }
 }

// 7 lines function
int col_to_del(int moves[MAX_MOVES], int turn_idx)
{
    int col_to_del = 0;

//checking what was the last documented opponent turn that we need to del
    for (int i = turn_idx-1; i > 0; i -= NUM_OF_PLAYERS)
    {
//looking for a valid turn and not an undo/redo one
        if (moves[i] > 0 && moves[i] <= BOARD_MAX_SIDE)
        {
            col_to_del = moves[i];
//pushing the deleted turn to an isolated area so we won't confuse it with a turn that was not erased
            moves[i] = moves[i] - ISOLATED_AREA;
            return col_to_del;
        }
    }
//if the loop is over, there is an error so returning error
   return ERROR_NUM;
}

// 4 lines function
void del_undos_after_valid_move(int moves[MAX_MOVES],int turn_idx, bool* vc_ptr)
{
//only needed if the turn was played, meaning valid choice is true
    if(*vc_ptr)
        {
            for(int i = turn_idx; i >= 0; i--)
            {
//bringing back the undo moves from the isolated area so we cant redo or undo them in future moves
                if(moves[i] <= BOARD_MAX_SIDE - ISOLATED_AREA)
                {
                    moves[i] += ISOLATED_AREA*2;
                }
            }
        }
}


// 7 lines function
bool opponent_board_is_empty (char board[][BOARD_MAX_SIDE], int rows, int cols, int opponent_color)
{
    bool opponent_board_is_empty = true;
// loop that searching the board, if there is one opponent's color there, immediately end the function the returning false
    for (int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            if (board[i][j] == opponent_color)
            {
                opponent_board_is_empty = false;
                return opponent_board_is_empty;
            }
        }
    }
    return opponent_board_is_empty;
}

// 4 lines function
void undo_last_move(char board[][BOARD_MAX_SIDE],int col_to_del, int rows, int opponent_color)
{
    for(int i = 0; i < rows; i++)
    {
//going from top to bottom of the col to del and when finding an oppponent's color, deleting it and ending the function
        if(board[i][col_to_del] == opponent_color)
        {
            board[i][col_to_del] = EMPTY_SLOT_SYMBOL;
            break;
        }
    }
}


// 11 lines function
void do_redo_scenario(int moves[MAX_MOVES],int turn_idx,bool* vc_ptr,char board[][BOARD_MAX_SIDE],int rows,char players_color[NUM_OF_PLAYERS+1])
 {
//checking if the last turn is -1 or -2
    if (moves[turn_idx-1] == UNDO_IDX || moves[turn_idx-1] == REDO_IDX)
    {
        for(int i = turn_idx - NUM_OF_PLAYERS; i >= 0; i -= NUM_OF_PLAYERS)
        {
// checking for a move that moved to the isolated area in an undo move
            if(moves[i] <= BOARD_MAX_SIDE - ISOLATED_AREA)
            {
//bringing the move back from there
                moves[i] += ISOLATED_AREA;
//changing the valid choice to true, inserting the right player's token and exiting the loop
                *vc_ptr = true;
                int player = find_player(turn_idx);
                insert_token(board, rows, moves[i], players_color[player]);
                break;
            }

        }
// if there are not any moves in the isolated place, then there are no moves to redo
        if (!*vc_ptr)
        {
            print_unavailable_undo_redo_message();
        }

    }
    else
    {
        print_unavailable_undo_redo_message();
    }
 }

// 8 lines function
bool check_if_tie(char board[][BOARD_MAX_SIDE],int turn_idx,int rows,int cols)
{
    bool is_tie = false;
//checking only from the 9th turn, can't be a tie before
    if (turn_idx >= BOARD_MAX_SIDE)
    {
//loop that going over the board, if it finds an empty space,ending and returning false
        for (int i = 0; i < rows; i++)
        {
            for(int j = 0; j < cols; j++)
            {
                if(board[i][j] != RED_SLOT_SYMBOL && board[i][j] != YELLOW_SLOT_SYMBOL)
                   {
                       return is_tie;
                   }
            }
        }
//if the loop is over than the board is full
        is_tie = true;
    }
    return is_tie;
}

// 10 lines function
bool check_if_won(char board[][BOARD_MAX_SIDE],int turn_idx,char players_color[NUM_OF_PLAYERS+1],int rows,int cols,int tokens)
{

    bool is_victories[NUM_OF_POSSIBLE_VICTORIES] = {false, false, false, false};
    char current_color = players_color[find_player(turn_idx)];
// can't win before that turn
    if (turn_idx > (tokens-1) * NUM_OF_PLAYERS)
    {
//checking if player have won
        is_victories[ROW_V_IDX] = check_for_row_victory(board, rows, cols, tokens, current_color);
        is_victories[COL_V_IDX] = check_for_col_victory(board, rows, cols, tokens, current_color);
        is_victories[R_DIAGONAL_V_IDX] = check_for_right_diagonal_victory(board,tokens, current_color);
        is_victories[L_DIAGONAL_V_IDX] = check_for_left_diagonal_victory(board, tokens, current_color);

//if on in and way, returning true
        if(is_victories[ROW_V_IDX] || is_victories[COL_V_IDX] || is_victories[R_DIAGONAL_V_IDX] || is_victories[L_DIAGONAL_V_IDX])
        {
            return true;
        }
    }
//if did not win, return false
        return false;
}

// 12 lines function
bool check_for_row_victory(char board[][BOARD_MAX_SIDE],int rows,int cols,int tokens,char current_color)
{
    bool is_victories = false;
    int strike = 0;
    for (int i = 0; i < rows; i++)
    {
//searching for a tokens long row strike
        for (int j = 0; j < cols; j++)
        {
            if (board[i][j] == current_color)
            {
                strike++;
            }
            else
            {
                strike = 0;
            }
            if (strike >= tokens)
            {
               is_victories = true;
               return is_victories;
            }
        }
// zero the strike before a new row
        strike = 0;
    }
    return is_victories;
}

// 12 lines function
bool check_for_col_victory(char board[][BOARD_MAX_SIDE],int rows,int cols,int tokens,char current_color)
{
    bool is_victories = false;
    int strike = 0;
    for (int j = 0 ; j < cols; j++)
    {
//searching for a tokens long col strike
        for (int i = 0; i < rows; i++)
        {
            if (board[i][j] == current_color)
            {
                strike++;
            }
            else
            {
                strike = 0;
            }
            if (strike >= tokens)
            {
               is_victories = true;
               return is_victories;
            }
        }
// zero the strike before a new col
        strike = 0;
    }
    return is_victories;
}


// 5 lines function
bool check_for_right_diagonal_victory(char board[][BOARD_MAX_SIDE],int tokens, char current_color)
{
    bool is_victories = false;
//if one of the possible right diagonal victories are taking place, returning true
   if(check_for_right_1st_kind_of_diagonal_voctory(board, tokens, current_color) ||
      check_for_right_2nd_kind_of_diagonal_voctory(board, tokens, current_color))
    {
        is_victories = true;
    }

    return is_victories;
}

// 11 lines function
bool check_for_right_1st_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color)
{
    bool is_victories = false;
    for (int i = BOARD_MAX_SIDE - MIN_TOKENS; i >= 0 && !is_victories; i--)
    {
        int strike = 0;
        for( int ii = i, jj = BOARD_MAX_SIDE - 1; ii < BOARD_MAX_SIDE; ii++, jj--)
        {
            if (board[ii][jj] == current_color)
            {
                strike++;
            }
            else
            {
                strike = 0;
            }
            if (strike >= tokens)
            {
                is_victories = true;
                break;
            }
        }
    }
    return is_victories;
}

// 11 lines function
bool check_for_right_2nd_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color)
{
    bool is_victories = false;
    for (int j = BOARD_MAX_SIDE - 2; j >= 0 && !is_victories; j--)
    {
        int strike = 0;
        for (int ii = 0, jj = j; jj >= MIN_TOKENS - 1; ii++, jj--)
        {
            if (board[ii][jj] == current_color)
            {
                strike++;
            }
            else
            {
                strike = 0;
            }
            if (strike >= tokens)
            {
                is_victories = true;
                break;
            }
        }
    }
    return is_victories;

}

// 5 lines function
bool check_for_left_diagonal_victory(char board[][BOARD_MAX_SIDE],int tokens,char current_color)
{
    bool is_victories = false;
//if one of the possible right diagonal victories are taking place, returning true
   if(check_for_left_1st_kind_of_diagonal_voctory(board, tokens, current_color) ||
      check_for_left_2nd_kind_of_diagonal_voctory(board, tokens, current_color))
    {
        is_victories = true;
    }

    return is_victories;
}

// 11 lines function
bool check_for_left_1st_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color)
{
    bool is_victories = false;
     for (int i = BOARD_MAX_SIDE - MIN_TOKENS; i >= 0 && !is_victories; i--)
        {
            int strike = 0;
            for( int ii = i, jj = 0; ii < BOARD_MAX_SIDE; ii++, jj++)
            {
                if (board[ii][jj] == current_color)
                {
                    strike++;
                }
                else
                {
                    strike = 0;
                }
                if (strike >= tokens)
                {
                    is_victories = true;
                    break;
                }
            }
        }
        return is_victories;
}

// 11 lines function
bool check_for_left_2nd_kind_of_diagonal_voctory(char board[][BOARD_MAX_SIDE],int tokens, char current_color)
{
    bool is_victories = false;
    for (int j = 1; j  <= BOARD_MAX_SIDE - MIN_TOKENS && !is_victories; j++)
    {
        int strike = 0;
        for (int ii = 0, jj = j; jj < BOARD_MAX_SIDE; ii++, jj++)
        {
            if (board[ii][jj] == current_color)
            {
                strike++;
            }
            else
            {
                strike = 0;
            }
            if (strike >= tokens)
            {
                is_victories = true;
                break;
            }
        }
    }
        return is_victories;
}


// 7 lines function
void win_or_tie_check(bool win_or_tie[WIN_OR_TIE],char board[][BOARD_MAX_SIDE], int turn_idx,
             char players_color[NUM_OF_PLAYERS+1], int rows, int cols, int tokens,int player)
{
//check if there is a win or a tie
    win_or_tie[WIN_IDX] = check_if_won(board, turn_idx, players_color, rows, cols, tokens);
        win_or_tie[TIE_IDX] = check_if_tie(board,turn_idx,rows,cols);
//if there is one of them, printing the appropriate message
        if(win_or_tie[WIN_IDX] || win_or_tie[TIE_IDX])
        {
            if(win_or_tie[TIE_IDX] && !win_or_tie[WIN_IDX])
            {
                player = TIE_PLAYER_NUM;
            }
            print_board(board, rows, cols);
            print_winner(player);
        }
}

// 10 lines function
int start_game(char board[][BOARD_MAX_SIDE],int rows,int cols,int moves[MAX_MOVES], char players_color[NUM_OF_PLAYERS+1]
                                                                                ,bool win_or_tie[WIN_OR_TIE], int tokens)
{
// loop that documenting the turns and ending if there is a draw or a winner
    for (int turn_idx = 1; !win_or_tie[WIN_IDX] && !win_or_tie[TIE_IDX]; turn_idx++)
    {
        bool valid_choice = false, is_conventional_choice = true;
        print_board(board, rows, cols);
//finding which player playing by the current turn
        int player = find_player(turn_idx), player_choice  = 0;
//loop until the move is valid
        while (!valid_choice)
        {
//making sure scanf working
            if (choose_move(player,&player_choice, is_conventional_choice))
            {
                return 1;
            }
//applying player's choice and checking if it's a "new" move or an unconventional illegal move
        is_conventional_choice = apply_players_choice(moves, turn_idx,player_choice, &valid_choice, board, rows, cols, players_color);
        }
//check if the game is over
        win_or_tie_check(win_or_tie,board,turn_idx, players_color, rows, cols, tokens, player);
    }
    return 0;
}

//---------------------- Printing Functions ----------------------------//

// message number 0 = "Welcome to 4-in-a-row game! \n"
void print_welcome_message()
{
    printf("Welcome to 4-in-a-row game! \n");
}


// message number 1 = "Please enter number of rows:"
// message number 2 = "Please enter number of columns:"
// message number 3 = "Please enter number of tokens:"
void print_read_game_params_message(int param)
{
	char const* const possible_params[] = {"rows", "columns", "tokens"};

    printf("Please enter number of %s: ", possible_params[param]);
}

// message number 4 = "Please choose starting color (Y)ellow or (R)ed: "
void print_chose_color_message()
{
    printf("Please choose starting color (Y)ellow or (R)ed: ");
}

// message number 5 = "Your move, player <player>. "
void print_chose_move_message(int player)
{
    printf("Your move, player %d. ", player);
}

// message number 6
void print_enter_column_message()
{
    printf("Please enter column: ");
}

void print_full_column_message()
{
    printf("Column full. ");
}

void print_unavailable_undo_redo_message()
{
    printf("No moves to undo/redo. ");
}

void print_board(char board[][BOARD_MAX_SIDE], int rows, int cols)
{

    for (int row = 0; row < rows; ++row)
    {
        printf("\n%c", BOARD_VERT_SEP);
        for (int col = 0; col < cols; ++col)
            printf("%c%c", board[row][col], BOARD_VERT_SEP);
    }
    printf("\n%c", BOARD_LEFT_ANG);
    for (int ii = 0; ii < cols - 1; ++ii)
        printf("%c%c", BOARD_BOTTOM, BOARD_BOTTOM_SEP);
    printf("%c%c\n", BOARD_BOTTOM, BOARD_RIGHT_ANG);
}

void print_winner(int player_id)
{
    if (player_id >= 0)
        printf("Player %d won! \n", player_id);
    else
        printf("That's a tie. \n");
}

