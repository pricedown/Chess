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

// Represents a certain posession over pieces
// This is certainly a funky type to choose, but it allowed us to practice manual memory management
// while also reflecting the general intended access method of the pieces.
// If we were given more time, we would have made our own abstract data type for this.
// That way, the Game could be reliant on something else for managing that data, while focuing on
// the definition of Legality in the Game-mode. 
// (That is why this class is named Game and not Chess, because it is not meant to encapsulate all
// of these functions)
typedef std::unordered_map<Square, Piece*> PieceMap;

class Game {
protected:
    // Invariants:
    // pieces do not move between PieceMaps
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


    // Checks a Move's Legality and then performs the move, returns its success.
    bool AttemptMove(const Move& move, const Teams color, const PieceType pieceType);
    // User may attempt making a full move, but it is checked to be precise.
    bool AttemptMove(const CompleteMove& move);
    // Checks an array of CompleteMoves assuming that it is properly interpreted,
    // thus only one of them can be legal. It performs that move if it exists.
    bool AttemptMoves(const std::vector<CompleteMove>& possibleMoves, int color);

    // Infers a LegalMove from a Move using the context of the game.
    // This is the main role of this class.
    CompleteMove LegalMove(const Move& m, const Teams color, const PieceType pieceType = PieceType::NONE, const Move& pretendMove = { }) const; 
    CompleteMove LegalMove(const CompleteMove& m) const;

    // legality queries
    Teams getWinner() const;
    bool hasMoves(const Teams& color) const;
    bool isChecked(const Teams& color) const;
    bool inBounds(const Square& square) const;

    // accessors
    Piece* getPiece(const Square& square) const;
    PieceType getPieceType(const Square& square) const;
    Teams getPieceTeam(const Square& square) const;
    Square getKing(const Teams& color) const;
    PieceMap getTeamPieces(const Teams& team) const { return teams[team]; }
};

// Helper functions
std::vector<CompleteMove> interpretMove(const PieceMap& teamPieces, const AlgebraicMove& algebraicMove, const Teams& color);
std::vector<PieceMap> FEN(std::string in);
