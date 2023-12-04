#pragma once
#include "Piece.h"
#include <unordered_map>
#include <vector>
#include <cmath>
#include <math.h>
#include <list>

// TODO maybe rewrite everything with std::find_if

// NOTE draw detection might be costly...

typedef std::unordered_map<Square, Piece*> PieceMap;

class Game {
protected:
    // vector of teams
    // each team has its own pieces
    // pieces are mapped from the squares they occupy
    std::vector<PieceMap> teams;

    CompleteMove lastMove;
    std::list<Piece*> moved;

    // Invariants:
    // pieces do not move between the maps
    // pieces cannot occupy the same square

public:
    Game(int teamcount = 2) { teams.reserve(teamcount); }
    Game(std::vector<PieceMap> teams) : teams(teams) {
        // initialization logic
        for (const auto& team : teams) for (const auto& pair : team) {
            Square square = pair.first;
            Piece* piece = pair.second;

            switch (piece->Type()) {
                case PieceType::PAWN:
                    if (square.y != 1 && square.y != 6)
                        moved.push_back(piece);
                    break;
            }
        }
    }
    ~Game() = default;

    Teams getWinner() {
        return Teams::NONE; // TODO determine if anyone has won the game
    }

    //virtual MoveType EvaluateMoveType(Move); // Considered, potentially viable strategy

    CompleteMove LegalMove(const Move& m, const Teams color, const PieceType pieceType) const {
        // Invariant:
        // there must be only one CompleteMove for every valid move

        CompleteMove legal;
        legal.move = m;
        legal.valid = false;
        legal.color = color;
        legal.pieceType = getPieceType(m.from);

        Piece* piece = getPiece(m.from);
        Piece* captured = getPiece(m.to);

        // move is possible via piece definition
        if (!piece->PossibleMove(m))
            return legal;

        // piece is the right type
        if (getPieceType(m.from) != pieceType)
            return legal;

        legal.moveType.captures = (captured != nullptr);

        // can't capture your own piece
        if (legal.color == getPieceTeam(m.to)) {
            std::cerr << "you're trying to capture your own piece" << std::endl;
            std::cerr << "team: " << legal.color << std::endl;
            legal.moveType.castles = (legal.pieceType == PieceType::KING) && (getPieceType(m.to) == PieceType::ROOK);
            if (legal.moveType.castles) {
                for (auto movedPiece : moved) {
                    if (movedPiece == piece)
                        return legal;
                    if (movedPiece == captured)
                        return legal;
                }
                // TODO more castling logic
            } else {
                return legal;
            }
        }

        // TODO
        Square offset = (m.to - m.from);

        switch (legal.pieceType) {
            case PieceType::PAWN:
                // a pawn captures if and only if it moves sideways
                
                if (legal.moveType.captures != (offset.x != 0))
                    return legal;

                if (abs(offset.y) >= 2) {
                    for (auto movedPiece : moved) {
                        if (movedPiece == piece)
                            return legal;
                    }
                }
                break;
        }


        // ... FIXME
        // ... check if it collides with something on the way
        if (legal.pieceType != PieceType::KNIGHT) {
            // it didn't / cannot jump
            
            Square step = offset.normalized();
            Square scan = m.from;

            while (scan != m.to) {
                Piece* p = getPiece(scan);
                if ((p != nullptr) && (p != piece)) {
                    return legal;
                }

                scan += step;
            }
        }

        // ... king is checked or not
        // ... would put its own king in check or not
        // ... can castle or not

        legal.valid = true;
        return legal;
    }

    CompleteMove LegalMove(const CompleteMove& m) {
        return LegalMove(m.move, m.color, m.pieceType);
    }

    // TODO make the relationship between CompleteMove methods and
    // normal Move methods concrete & consistent.
    // try to make it so that duplication of operations doesn't happen
    // all that often

    // Checks if a move is possible & legal, then performs the move
    // Returns success
    bool AttemptMove(const Move& move, const Teams color, const PieceType pieceType) 
    { return AttemptMove(LegalMove(move, color, pieceType)); }

    // You can attempt at making a full move, but you must be precise
    bool AttemptMove(CompleteMove move) {
        if (!move.valid)
            return false;

        // the move must be precise
        if (move != LegalMove(move.move, move.color, move.pieceType))
            return false;

        // perform actual move
        
        moved.push_back(teams[move.color][move.move.from]);
        // FIXME: add capturing support
        teams[move.color][move.move.to] = teams[move.color][move.move.from];
        teams[move.color][move.move.from] = nullptr;

        lastMove = move;
        return true;
    }

    bool AttemptMove(const std::vector<CompleteMove>& possibleMoves, int color) {

        // discover which of the moves are legal
        std::vector<CompleteMove> legalMoves;
        for (auto pm : possibleMoves) {
            CompleteMove legal = LegalMove(pm);
            if (legal.valid && legal == pm) {
                legalMoves.push_back(legal);
            }
        }

        // If the player has entered it properly, only one legal move
        // should be possible.
        if (legalMoves.size() == 1) {
            AttemptMove(legalMoves[0]);
            return true;
        } else {
            std::cerr << "There are " << legalMoves.size()
                << " legal moves possible!" << std::endl;
            for (auto move : legalMoves) {
                std::cerr << "Move 1: " << static_cast<int>(move.pieceType) << " " << move.move.from.x << " " << move.move.from.y 
                    << ": " << move.move.to.x << " " << move.move.to.y << std::endl;
            }
            return false;
        }

        // TODO: remove touple entirely after further testing
    }

    PieceMap getTeamPieces(Teams team) const { return teams[team]; }

    PieceType getPieceType(Square square) const {
        for (auto team : teams) {
            if (Piece* p = team[square])
                return p->Type();
        }
        return PieceType::NONE;
    }

    Piece* getPiece(Square square) const {
        for (auto team : teams) {
            if (Piece* p = team[square])
                return p;
        }
        return nullptr;
    }

    Teams getPieceTeam(Square square) const {
        for (int i = 0; i < teams.size(); i++)
            if (teams[i].count(square) > 0)
                return static_cast<Teams>(i);
        return Teams::NONE;
    }

};

using namespace std;
// Transforms an AlgebraicMove into possible interpreted Moves
vector<CompleteMove> interpretMove(PieceMap teamPieces, AlgebraicMove algebraicMove, Teams color) {
    vector<CompleteMove> ret;

    CompleteMove partial;

    // TODO overload = operator on algebraicMove, this is messy

    // partially create move with parameter knowns
    partial.valid = true; // interpreter assumes all of these moves are legal
    partial.color = color;
    partial.pieceType = algebraicMove.pieceType;
    partial.moveType = algebraicMove.moveType;

    // complete the partially created move using Piece definitions
    for (auto pieceMap : teamPieces) {
        Move move = { pieceMap.first, algebraicMove.to };
        if (pieceMap.second && pieceMap.second->PossibleMove(move)) {
            // add a complete, possible (not necessarily legal) move to vector
            CompleteMove complete = partial;
            complete.move = move;
            ret.push_back(complete);
        }
    }

    if (ret.size() == 0)
        std::cerr << "no possible moves" << std::endl;
    return ret;
}

// TODO add modular board/boundaries structure
// TODO encapsulate implementation from declaration (Chess.cpp)
