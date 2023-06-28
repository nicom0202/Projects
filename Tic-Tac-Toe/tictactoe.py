"""
Tic Tac Toe Player
"""

import math
import copy

X = "X"
O = "O"
EMPTY = None


def initial_state():
    """
    Returns starting state of the board.
    """
    return [[EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY]]


def player(board):
    """
    Returns player who has the next turn on a board.
    """
    # base case
    if board == initial_state():
        return X
    
    # variables
    x_count = sum(row.count(X) for row in board)
    o_count = sum(row.count(O) for row in board)

    # board is full, therefore the game is over 
    if not any(cell == EMPTY for row in board for cell in row):
        return None
    
    # if x_count is greater than o_count THEN it's O's turn
    if(x_count > o_count):
        return O
    # anything else is X's turn
    else:
        return X

def actions(board):
    """
    Returns set of all possible actions (i, j) available on the board.
    """
    # create a set that will hold the tuples of available moves for player
    moves = []

    # iterate over each row
    for i, row in enumerate(board):
        for j, element in enumerate(row):
                # if the cell in the row is empty, THEN it is possible action & add to set
                if board[i][j] == EMPTY:
                    moves.append((i, j))
    return moves


def result(board, action):
    """
    Returns the board that results from making move (i, j) on the board.
    """
    
    # Create a deep copy of the board
    new_board = copy.deepcopy(board)

    # RAISE AN EXCEPTION
    try:
        # check if the action is already occupied 
        if new_board[action[0]][action[1]] != EMPTY:
            raise IndexError
        else:
            # If the spot is not occupied, make the move on the new_board & return new board
            new_board[action[0]][action[1]] = player(new_board)
            return new_board
    except IndexError:
        print("Spot is not Available")


def winner(board):
    """
    Returns the winner of the game, if there is one.
    """

    # need to check all 3 rows 
    for row in board:
        if row[0] == row[1] == row[2] != EMPTY:
            return row[0]

    # need to check all 3 columns
    for col in range(3):
        if board[0][col] == board[1][col] == board[2][col] != EMPTY:
            return board[0][col]
        
    # need to check all 2 diagonals
    if board[0][0] == board[1][1] == board[2][2] != EMPTY:
        return board[0][0]
    if board[0][2] == board[1][1] == board[2][0] != EMPTY:
        return board[0][2]
    
    return None


def terminal(board):
    """
    Returns True if game is over, False otherwise.
    """

    # variable
    result = winner(board)

    # if winner() returns a winner then return True
    if result != None:
        return True
    # result is None there (1) Draw (2) Game is not over
    else:
       # check if game is not over aka there are avaiable actions left
        for row in board:
            if EMPTY in row:
                return False
        # no avaiable actions left therefore game is over
        return True



def utility(board):
    """
    Returns 1 if X has won the game, -1 if O has won, 0 otherwise. Assumes that terminal() is True
    """
    result = winner(board)

    if result == X:
        return 1
    elif result == O:
        return -1
    else:
        return 0



def minimax(board):
    """
    Returns the optimal action for the current player on the board.
    """

    # get current player of the game
    current_player = player(board)

    # if current player is X
    if current_player == X:

        # create a variable that will track the best utility score X can have
        best_score = float('-inf')

        # iterate over all possible actions in actions()
        for action in actions(board):

            # now run min_value() becasue O will play optimally such that X's score is minimized
            optimal_O_move = min_value(result(board, action))

            # update best_score for X and keep track of the action
            if optimal_O_move > best_score:
                best_score = optimal_O_move
                best_move = action
    # if current player is O
    else:
        # create a variable that will track the best utility score O can have
        best_score = float('inf')

        # iterate over all possible actions in actions()
        for action in actions(board):

            # now run max_value() becasue O will play optimally such that O's score is maximized
            optimal_X_move = max_value(result(board, action))

            # update best_score for O and keep track of the action
            if optimal_X_move < best_score:
                best_score = optimal_X_move
                best_move = action
    return best_move



def min_value(board):
    """
    Returns the minimum value of the maximum optimal scores
    """

    # check if the board is a terminal board, if yes then return the utitility of the board()
    if terminal(board):
        return utility(board)
    
    # create a "best score" variable and set to infinity because we want to MINIMIZE
    best_score = float('inf')

    # iterate over all action in actions()
    for action in actions(board):
        # pick the minimum value from best_score or from X's optimal move to MINIMIZE O's utitily value
        best_score = min(best_score, max_value(result(board, action)))
    return best_score



def max_value(board):
    """
    Returns the maximum value of the minimum optimal scores
    """

    # check if the board is a terminal board, if yes then return the utitility of the board()
    if terminal(board):
        return utility(board)
    
    # create a "best score" variable and set to -infinity because we want to MAXIMIZE
    best_score = float('-inf')

    # iterate over all action in actions()
    for action in actions(board):
        # pick the maximum value from best_score or from O's optimal move to MINIMIZE X's utitily value
        best_score = max(best_score, min_value(result(board, action)))
    return best_score
