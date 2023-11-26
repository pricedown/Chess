#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "Chess.h"
#include "IO.h"

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

    int n = in.size();
    char x = -1, y = -1;

    if (n == 0) return ret;

    if (islower(in[0])) {
        ret.pieceType = PieceType::PAWN;
    }
    else {
        switch (in[0]) {
        case 'K': ret.pieceType = PieceType::KING;
            break;
        case 'Q': ret.pieceType = PieceType::QUEEN;
            break;
        case 'R': ret.pieceType = PieceType::ROOK;
            break;
        case 'B': ret.pieceType = PieceType::BISHOP;
            break;
        case 'N': ret.pieceType = PieceType::KNIGHT;
        }
    }
    // assigning piecetype

    if (n > 1 && in[1] == 'x') ret.moveType.captures = true;

    if (in[n - 1] == '+' || in[n - 1] == '#') ret.moveType.checks = true;
    else if (in[n - 1] == 'O') ret.moveType.castles = true;
    else if (isupper(in[n-1])) ret.moveType.promotes = true;
    // checking if move captures, checks, castles or promotes

    if (ret.moveType.captures == true) {
        if (n > 3) {
            x = in[3];
            y = in[4];
        }
    }
    else {
        if (ret.pieceType == PieceType::PAWN) {
            if (n > 1) {
                x = in[0];
                y = in[1];
            }
        }
        else {
            if (n > 2) {
                x = in[1];
                y = in[2];
            }
        }
    }
    // assigning x and y to the correct char in the string

    switch (x) {
    case 'a': ret.to.x = 0;
        break;
    case 'b': ret.to.x = 1;
        break;
    case 'c': ret.to.x = 2;
        break;
    case 'd': ret.to.x = 3;
        break;
    case 'e': ret.to.x = 4;
        break;
    case 'f': ret.to.x = 5;
        break;
    case 'g': ret.to.x = 6;
        break;
    case 'h': ret.to.x = 7;
    }
    if (y > 0 && y < 9) ret.to.y = y - 1;
    // converts chars x, y to 0-indexed board position 

    return ret;
}

// TODO Also optionally read from a file (almost the same thing if you use stringstreams)
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
    
    Game game(IO::FEN("8/1p4p1/2P2P2/8/8/2p2p2/1P4P1/8"));

    Teams tomove = Teams::WHITE;
    while (game.getWinner() == Teams::NONE) {
        if (tomove == Teams::WHITE) cout << "White";
        else cout << "Black";
        
        cout << " to move." << endl;
        cout << "---------------" << endl;
        IO::printBoard(game);
        
        Move m;
        cout << "From: ";
        cin >> m.from.x >> m.from.y;
        
        cout << "to: ";
        cin >> m.to.x >> m.to.y;

        game.AttemptMove(m, tomove);

        if (tomove == Teams::WHITE) tomove = Teams::BLACK;
        else tomove = Teams::WHITE;
    }
}
