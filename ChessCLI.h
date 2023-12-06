#include "Chess.h"
#include <iostream>

// **THIS IS NOT WRITTEN BY ME, CREDIT GOES TO @EddieSlobodow :)**

using namespace std;

AlgebraicMove discoverMove(std::string in) {
    AlgebraicMove ret;

    int n = in.size();
    char x, y;

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
    if (y - '0' > 0 && y - '0' < 9) ret.to.y = (y - '0') - 1;
    // converts chars x, y to 0-indexed board position

    return ret;
}

void printBoard(const Game& game) {
    // TODO Assuming an 8x8 chessboard (i'll change that later)
    int boardSize = 8;
    for (int y = boardSize-1; y >= 0; y--) {
        for (int x = 0; x < boardSize; x++) {
            char squareOut;
            Square square;
            square.x = x;
            square.y = y;
            switch (game.getPieceType(square)) {
                case PieceType::PAWN: squareOut = 'p';
                    break;
                case PieceType::ROOK: squareOut = 'r';
                    break;
                case PieceType::KNIGHT: squareOut = 'n';
                    break;
                case PieceType::BISHOP: squareOut = 'b';
                    break;
                case PieceType::KING: squareOut = 'k';
                    break;
                case PieceType::QUEEN: squareOut = 'q';
                    break;
                default: squareOut = '.';
            }

            if (game.getPieceTeam(square) == 0) squareOut = toupper(squareOut);
            cout << squareOut << " ";
            if (x == boardSize-1){
                cout << y+1;
            }
        }
        std::cout << std::endl;
    }
    char yIndex = 'a';
    for (int i = 0; i < boardSize; i++){
        cout << yIndex << " ";
        yIndex++;
    }
    std::cout << std::endl;
}

