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

    CompleteMove LegalMove(const Move& m, const Teams color, const PieceType pieceType, const Move& pretendMove = { }) const {
        // Invariant:
        // there must be only one CompleteMove for every valid move

        CompleteMove legal;
        legal.move = m;
        legal.valid = false;
        legal.color = color;
        legal.pieceType = pieceType;

        Piece* piece = getPiece(m.from);
        Piece* captured;
        Square capturedSquare;
        
        // make sure if you're capturing the pretend move or not
        if ((pretendMove.from != pretendMove.to) && (pretendMove.to == m.to)) {
                captured = getPiece(pretendMove.from);
                capturedSquare = pretendMove.from;
        } else {
            captured = getPiece(m.to);
            capturedSquare = m.to;
        }

        // in the scenario that you're being captured, you cannot move
        if (pretendMove.to == m.from)
            return legal;
        
        // make sure that if you're capturing, the pretendMove doesn't core dump

        // check if it's the right piece type
        if (getPieceType(m.from) != pieceType) {
            return legal;
        }

        // check if it's the right color
        if (getPieceTeam(m.from) != color) {
            std::cerr << getPieceTeam(m.from) << " " << color << std::endl;
            return legal;
        }

        // move is possible via piece definition
        if (!piece->PossibleMove(m))
            return legal;

        legal.moveType.captures = (captured != nullptr);

        // can't capture your own piece
        if (legal.color == getPieceTeam(capturedSquare)) {
            legal.moveType.castles = (legal.pieceType == PieceType::KING)
                && (getPieceType(capturedSquare) == PieceType::ROOK);
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
                if ((p != nullptr) && (p != piece) && (scan != pretendMove.from)) {
                    return legal;
                } else if (scan == pretendMove.to) {
                    return legal;
                }

                scan += step;
            }
        }

        // ... would put its own king in check or not
        // I have to check if any piece moving to the piece
        // that the king is on is a legal move
        
        // checks that it's only the first iteration
        if (pretendMove.from == pretendMove.to) {

            Square king;
            if (legal.pieceType == PieceType::KING) 
                king = m.to;
            else 
                king = getKing(color);

            for (int i = 0; i < teams.size(); i++) {
                if (static_cast<Teams>(i) == color)
                    continue;

                // TODO still no clue why this calls twice, doesn't effect it though
                // std::cout << "Checking team " << i << " for possible checks" << std::endl;

                for (const auto& pair : teams[i]) {
                    Square square = pair.first;
                    if (pair.second == nullptr)
                        continue;

                    if (legal.moveType.castles) {
                        if (LegalMove({square, m.from}, static_cast<Teams>(i), getPieceType(square), { {-1, -1}, {-2, -2} }).valid) {
                            std::cerr << "Cannot castle out of check!" << std::endl;
                            return legal;
                        }

                        if (legal.moveType.castleDir) king.x = 6;
                        else king.x = 2;
                    } 

                    if (LegalMove({square, king}, static_cast<Teams>(i), getPieceType(square), m).valid) {
                        std::cerr << "You would be putting yourself in check!" << std::endl;
                        std::cerr << "Attacking piece location: " << square.x << " " << square.y 
                            << std::endl;
                        return legal;
                    }
                }
            }
        }

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
        Square fromSquare = move.move.from;
        Square toSquare = move.move.to;

        // capture support
        Piece* captured = getPiece(toSquare);
        if (captured != nullptr) {
            delete captured;
            teams[getPieceTeam(toSquare)].erase(toSquare);
            moved.remove(captured);
        }

        // update PieceMap
        teams[move.color][toSquare] = teams[move.color][fromSquare];
        teams[move.color].erase(fromSquare);

        // update the list of moved pieces
        moved.push_back(teams[move.color][toSquare]); 
        
        lastMove = move;
        return true;
    }

    bool AttemptMoves(const std::vector<CompleteMove>& possibleMoves, int color) {

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
                << " legal moves" << std::endl;
            for (auto move : legalMoves) {
                std::cerr << "Move 1: " << static_cast<int>(move.pieceType) << " " << move.move.from.x << " " << move.move.from.y 
                    << ": " << move.move.to.x << " " << move.move.to.y << std::endl;
            }
            return false;
        }

        // TODO: remove touple entirely after further testing?
    }

    PieceMap getTeamPieces(Teams team) const { return teams[team]; }

    PieceType getPieceType(Square square) const {
        for (auto team : teams) {
            if (Piece* p = team[square]) {
                if (p == nullptr) {
                    // This should never be reached
                    std::cerr << "Dead square: " << square.x << " " << square.y 
                        << std::endl;
                    return PieceType::NONE;
                }
                return p->Type();
            }
        }
        return PieceType::NONE;
    }

    Piece* getPiece(Square square) const {
        for (auto team : teams) {

            if (Piece* p = team[square]) {
                if (p == nullptr) {
                    // This should never be reached
                    std::cerr << "Dead square: " << square.x << " " << square.y 
                        << std::endl;
                    return nullptr;
                }
                return p;
            }
        }
        return nullptr;
    }

    Teams getPieceTeam(Square square) const {
        for (int i = 0; i < teams.size(); i++)
            if (teams[i].count(square) > 0)
                return static_cast<Teams>(i);
        return Teams::NONE;
    }

    Square getKing(Teams color) const {
        for (const auto& pair : teams[color]) {
            Square square = pair.first;
            Piece* piece = pair.second;

            if (piece == nullptr) {
            // This should never be reached
                std::cerr << "Dead square: " << square.x << " " << square.y << std::endl;
                continue;
            }

            if (piece->Type() == PieceType::KING)
                return square;
        }
        return Square{};
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
        std::cerr << "There are 0 possible moves" << std::endl;
    return ret;
}

std::vector<PieceMap> FEN(std::string in) {
    std::vector<PieceMap> ret(2);

    int x = 0, y = 7;
    int ptr = 0;

    while (y >= 0 || ptr < in.size()) {
        if (isdigit(in[ptr])) {
            for (int i = 0; i < in[ptr] - '0'; i++){
                if (x < 8) x++;
            }
        }
        else if (in[ptr] != '/') {
            Square square;
            square.x = x;
            square.y = y;
            switch (in[ptr]) {
                case 'p': ret[1][square] = new Pawn(2);
                    break;
                case 'P': ret[0][square] = new Pawn(0);
                    break;
                case 'r': ret[1][square] = new Rook;
                    break;
                case 'R': ret[0][square] = new Rook;
                    break;
                case 'n': ret[1][square] = new Knight;
                    break;
                case 'N': ret[0][square] = new Knight;
                    break;
                case 'b': ret[1][square] = new Bishop;
                    break;
                case 'B': ret[0][square] = new Bishop;
                    break;
                case 'q': ret[1][square] = new Queen;
                    break;
                case 'Q': ret[0][square] = new Queen;
                    break;
                case 'k': ret[1][square] = new King;
                    break;
                case 'K': ret[0][square] = new King;
                    break;
            }
            x++;
        }
        if (x > 7) {
            x = 0;
            y--;
        }
        ptr++;
    }

    return ret;
}

// TODO add modular board/boundaries structure
// TODO encapsulate implementation from declaration (Chess.cpp)
