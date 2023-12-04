#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "ChessCLI.h"
//#include "IO.h"

using namespace std;

int main () {

    //Game game(FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"));
    Game game(FEN("r2qk2r/8/8/8/8/8/8/R2QK2R"));

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
