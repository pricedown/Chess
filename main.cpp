#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "ChessCLI.h"
//#include "IO.h"

using namespace std;

int main () {

    const string startingPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    const string castleTest = "r3k2r/8/8/8/8/8/8/R3K2R";
    const string pawnTest = "8/1pP1pP2/8/2P2P2/2p1p3/8/1Pp1Pp2/8";
    Game game(FEN(pawnTest));

    Teams tomove = Teams::WHITE;
    while (game.getWinner() == Teams::NONE) {
        if (tomove == Teams::WHITE) cout << "White";
        else cout << "Black";

        cout << " to move." << endl;
        cout << "---------------" << endl;
        printBoard(game);

        string notation;
        cout << "Enter a move: ";
        cin >> notation;

        //cout << discoverMove(notation).to.x << " " << discoverMove(notation).to.y << endl;

        if (game.AttemptMoves(interpretMove(game.getTeamPieces(tomove), discoverMove(notation), tomove), tomove))
        {
            if (tomove == Teams::WHITE) tomove = Teams::BLACK;
            else tomove = Teams::WHITE;
        }

    }
}
