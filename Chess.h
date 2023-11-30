#pragma once
#include "Piece.h"
#include <unordered_map>
#include <vector>
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
        std::list<Piece*> unmoved;

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
                        if (square.y == 2 || square.y == 7)
                            unmoved.push_back(piece);
                        break;
                }
            }
        }
        ~Game() = default;

        Teams getWinner() {
            return Teams::NONE; // TODO determine if anyone has won the game
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

        //virtual MoveType EvaluateMoveType(Move); // Considered, potentially viable strategy

        CompleteMove LegalMove(const Move& m, const Teams color) const {
            // Invariant:
            // there must be only one CompleteMove for every valid move

            CompleteMove legal;
            legal.move = m;
            legal.valid = false;
            legal.color = color;

            Piece* piece = getPiece(m.from);
            // move is possible via piece definition
            if (!piece->PossibleMove(m))
                return legal;

            // TODO
            switch (getPieceType(m.from)) {
                case PieceType::PAWN:
                    // a pawn captures if and only if it moves sideways
                    legal.moveType.captures = (m.to.x - m.from.x != 0);
                    if (legal.moveType.captures) {
                        // nothing at that square
                        if (!getPiece(m.to)) {
                            // if (getPiece(m.to-{})) // TODO en passant
                            // TODO promotions
                            return legal;
                        }
                    }

                    break;
            }

            // ... FIXME
            // ... check if it collides with something on the way
            // ... king is checked or not
            // ... would put its own king in check or not
            // ... can castle or not

            legal.valid = true;
            return legal;
        }

        CompleteMove LegalMove(const CompleteMove& m) {
            return LegalMove(m.move, m.color);
        }

        // TODO make the relationship between CompleteMove methods and
        // normal Move methods concrete & consistent.
        // try to make it so that duplication of operations doesn't happen
        // all that often

        // Checks if a move is possible & legal, then performs the move
        bool AttemptMove(const Move& move, const Teams color) { return AttemptMove(LegalMove(move, color)); }

        // You can attempt at making a full move, but you must be precise
        bool AttemptMove(CompleteMove move) {
            if (!move.valid)
                return false;

            // the move must be precise
            if (move != LegalMove(move.move, move.color))
                return false;

            // perform actual move
            
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
                    << " conflicting legal moves possible." << std::endl;
                return false;
            }

            // TODO: remove touple entirely after further testing
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
        if (pieceMap.second->PossibleMove(move)) {
            // add a possible (not necessarily legal) move to vector
            CompleteMove complete = partial;
            complete.move = move;
            ret.push_back(complete);
        }
    }

    return ret;
}

// TODO add modular board/boundaries structure
// TODO encapsulate implementation from declaration (Chess.cpp)
