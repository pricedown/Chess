#pragma once
#include "Piece.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <list>
#include <sstream>
#include <iostream>

// NOTE draw detection might be costly...

// represents a certain posession over pieces
typedef std::unordered_map<Square, Piece*> PieceMap;
// if we had more time, I would have made my own data type for this

class Game {
protected:
    // Invariants:
    // pieces do not move between the maps
    // pieces cannot occupy the same square

    // vector of teams
    // each team has its own pieces
    // squares are mapped to the pieces that occupy them
    std::vector<PieceMap> teams;

    // necessary persistant game data
    std::list<Piece*> moved;
    CompleteMove lastMove;
    int lastReversableMove = 0;
    std::vector<std::string> positionHistory;
    
    void UpdateHistory();
    void DeletePiece(const Square& square);
    void MovePiece(const Move& move, const Teams& color);
    void PromotePiece(const Move& move, const Teams& color, const PieceType& type);

public:
    Game(int teamcount = 2);
    Game(std::vector<PieceMap> teams);
    ~Game();

    Teams getWinner() const;
    bool hasMoves(const Teams& color) const;
    bool isChecked(const Teams& color) const;
    bool inBounds(const Square& square) const;

    // Builds / infers a LegalMove using a Move and the context of the game.
    CompleteMove LegalMove(const Move& m, const Teams color, const PieceType pieceType = PieceType::NONE, const Move& pretendMove = { }) const; 
    CompleteMove LegalMove(const CompleteMove& m) const;

    // Checks a Move's legality and then performs the move, returns its success.
    bool AttemptMove(const Move& move, const Teams color, const PieceType pieceType);
    // User may attempt making a full move, but it is checked to be precise.
    bool AttemptMove(const CompleteMove& move);

    // Checks an array of CompleteMoves assuming that it is properly interpreted, and thus only one can be legal.
    bool AttemptMoves(const std::vector<CompleteMove>& possibleMoves, int color);

    Piece* getPiece(const Square& square) const;
    PieceType getPieceType(const Square& square) const;
    Teams getPieceTeam(const Square& square) const;
    Square getKing(const Teams& color) const;
    PieceMap getTeamPieces(const Teams& team) const { return teams[team]; }
};

// Helper functions
std::vector<CompleteMove> interpretMove(const PieceMap& teamPieces, const AlgebraicMove& algebraicMove, const Teams& color);
std::vector<PieceMap> FEN(std::string in);
