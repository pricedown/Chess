#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "Chess.h"
//#include "IO.h"

using namespace std;

/*
 * Program explanation:
 */
// discoverMove turns a user input into an AlgebraicMove
// Chess::interpretMove turns an AlgebraicMove into CompleteMoves.
// Those Moves are validated with LegalMove(CompleteMove.move) == CompleteMove,
// which happens inside AttemptMove.
// If the user inputted correctly, there should be exactly one legal move.
// Then the move is played in AttemptMove.

// TODO: Turns user input into an AlgebraicMove
AlgebraicMove discoverMove(std::string in) {
    AlgebraicMove ret;

    // ...

    return ret;
}

// TODO https://codereview.stackexchange.com/questions/251795/parsing-a-chess-fen
std::vector<PieceMap> FEN(std::string in) {
    std::vector<PieceMap> ret(2);

    // ...

    return ret;
}

void printBoard(const Game& game) {
    // TODO Assuming an 8x8 chessboard (i'll change that later)
    int boardSize = 8;
    for (int y = boardSize; y >= 0; y--) {
        for (int x = 0; x < boardSize; x++) {
            // TODO: cout the chess piece / empty space
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

int main () {

    Game game(FEN("8/2p2p2/8/8/8/8/2P2P2/8"));
    printBoard(game);

    Square lowerLeftPawn = {2, 1};
    Move m = { lowerLeftPawn, lowerLeftPawn + Square{0,1}, };

    game.AttemptMove(m, Teams::WHITE);
    cout << "---------------" << endl;
    printBoard(game);

}
