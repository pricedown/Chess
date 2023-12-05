#pragma once
#include "Piece.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <list>
#include <sstream>

// NOTE draw detection might be costly...

typedef std::unordered_map<Square, Piece*> PieceMap;

class Game {
protected:
    // vector of teams
    // each team has its own pieces
    // pieces are mapped from the squares they occupy
    std::vector<PieceMap> teams;

    CompleteMove lastMove;
    int lastReversableMove = 0;
    std::vector<std::string> positionHistory;

    std::list<Piece*> moved;

    // Invariants:
    // pieces do not move between the maps
    // pieces cannot occupy the same square
    
    void UpdateHistory() {
        std::ostringstream key;

        // Board position
        for (int y = 7; y >= 0; --y) {
            for (int x = 0; x < 8; ++x) {
                Square square{x, y};
                Piece* piece = getPiece(square);

                if (piece != nullptr) {
                    key << static_cast<int>(piece->Type()) << (getPieceTeam(square) == Teams::WHITE ? 'w' : 'b');
                } else {
                    key << '-';
                }
            }
            key << "/";
        }

        positionHistory.push_back(key.str());
    }

    void DeletePiece(Square square) {
        Piece* piece = getPiece(square);
        Teams color = getPieceTeam(square);
        delete piece;
        teams[color].erase(square);
        moved.remove(piece);
    }

    void MovePiece(Move move, Teams color) {
        // capture support
        Piece* captured = getPiece(move.to);
        if (captured != nullptr) {
            delete captured;
            teams[getPieceTeam(move.to)].erase(move.to);
            moved.remove(captured);
        }

        // update PieceMap
        teams[color][move.to] = teams[color][move.from];
        teams[color].erase(move.from);

        // update the list of moved pieces
        moved.push_back(teams[color][move.to]); 
    }

    void PromotePiece(Move move, Teams color, PieceType type) {

        Piece* piece = getPiece(move.from);
        delete piece;
        teams[color].erase(move.from);
        moved.remove(piece);

        switch (type) {
            case PieceType::KNIGHT:
                teams[color][move.to] = new Knight();
                break;
            case PieceType::BISHOP:
                teams[color][move.to] = new Bishop();
            break;
            case PieceType::ROOK:
                teams[color][move.to] = new Rook();
            break;
            case PieceType::QUEEN:
                teams[color][move.to] = new Queen();
            break;
        }
        moved.push_back(teams[color][move.to]);
    }

public:
    Game(int teamcount = 2) { teams.reserve(teamcount); positionHistory.reserve(100); }
    Game(std::vector<PieceMap> teams) : teams(teams) {
        positionHistory.reserve(100);

        // initialization logic
        for (const auto& team : teams) for (const auto& pair : team) {
            Square square = pair.first;
            Piece* piece = pair.second;

            if (!inBounds(pair.first))
                continue;

            switch (piece->Type()) {
                case PieceType::PAWN:
                    if (square.y != 1 && square.y != 6)
                        moved.push_back(piece);
                    break;
            }
        }

        UpdateHistory();
    }
    ~Game() = default;

    bool inBounds(const Square& square) const {
        return square.x <= 7 && square.x >= 0
                && square.y <= 7 && square.y >= 0;
    }

    bool hasMoves(Teams color) {
        for (auto& pair : teams[color]) {
            Move move;
            
            move.from = pair.first;
            Piece* piece = pair.second;
            PieceType pieceType = getPieceType(move.from);

            if (piece == nullptr) {
                std::cerr << "Dead square: " << move.from.x << move.from.y << std::endl;
                continue;
            }

            // iterate through all the squares it can go to
            // (maybe make a virtual iterator for this in the future)
            for (move.to.y = 0; move.to.y < 8; move.to.y++) for (move.to.x = 0; move.to.x < 8; move.to.x++) {
                //if (inBounds(move.to)) continue;
                if (LegalMove(move, color, pieceType).valid) {
                    std::cout << "Possible move: " << move.from.x << move.from.y << move.to.x << move.to.y << std::endl;
                    return true;
                }
            }
        }

        return false;
    }

    bool isChecked(Teams color) {
        Teams otherTeam;
        Square kingSquare = getKing(color);

        for (int i = 0; i < teams.size(); i++) {
            if (i != color)
                otherTeam = static_cast<Teams>(i);

            for (auto& pair : teams[otherTeam]) {
                Move move = { pair.first, kingSquare };

                if (pair.second == nullptr) {
                    std::cerr << "Dead square: " << move.from.x << move.from.y << std::endl;
                    continue;
                }

                if (LegalMove(move, otherTeam, getPieceType(move.from)).valid) {
                    std::cout << "Checking move: " << move.from.x << move.from.y << move.to.x << move.to.y << std::endl;
                    return true;
                }
            }
        }
        return false;
    }

    Teams getWinner() {

        Teams tomove = lastMove.color;
        Teams color;
        for (int i = 0; i < teams.size(); i++)
            if (i != tomove)
                color = static_cast<Teams>(i);

        // check checkmate
        if (!hasMoves(color)) {
            if (!isChecked(color))
                return Teams::ALL;
            return tomove;
        }
        
        // check fifty move rule
        if (lastReversableMove == 50)
            return Teams::ALL;

        // check threefold repitition
        if (std::count(positionHistory.begin(), positionHistory.end(), positionHistory.back()) >= 3)
            return Teams::ALL;

        return Teams::NONE;
    }

    //virtual MoveType EvaluateMoveType(Move); // Considered, potentially viable strategy

    CompleteMove LegalMove(const Move& m, const Teams color, const PieceType pieceType = PieceType::NONE, const Move& pretendMove = { }) const {
        // Invariant:
        // there must be only one CompleteMove for every valid move

        CompleteMove legal;
        legal.move = m;
        legal.valid = false;
        legal.pieceType = pieceType;
        legal.move.promotion = m.promotion;

        if (!inBounds(legal.move.to))
            return legal;

        // check if it's the right color
        if (getPieceTeam(legal.move.from) != color) {
            return legal;
        } else 
            legal.color = color;

        Piece* captured;
        Square capturedSquare;

        // detect if capturing the pretend move
        if ((pretendMove.from != pretendMove.to) && (pretendMove.to == legal.move.to))
            capturedSquare = pretendMove.from;
        else
            capturedSquare = legal.move.to;
        captured = getPiece(capturedSquare);
        legal.moveType.captures = (captured != nullptr);

        Piece* piece;

        // There are two types of castle moves:
        // - one is a move where the king tries to capture its own rook
        // - TODO the other is just O-O or O-O-O: it has no Move, just flagged as a certain castle direction
        // that way we can make it not part of the piece definition, and just bypass these features... 
        
        if (legal.moveType.castles == true) {
            legal.move.from = getKing(color);
            legal.pieceType = PieceType::KING;
            legal.move.to = legal.move.from;
            if (legal.moveType.castleDir)
                legal.move.to.x = 0;
            else
                legal.move.to.x = 7;
        }
        
        // now we can define the piece ptr, since legal.move.from has been built
        piece = getPiece(legal.move.from);

        // determine if user is trying to castle & complete the move
        if (legal.color == getPieceTeam(capturedSquare)) { 
            if (legal.pieceType == PieceType::KING && getPieceType(capturedSquare) == PieceType::ROOK) {
                legal.moveType.castles = true;
                legal.moveType.castleDir = legal.move.to.x == 7;
            }
                
            // you can't normally capture your own piece...
            if (!legal.moveType.castles)
                return legal;

            // qualify that a castle requires the king and rook to not have moved
            for (auto movedPiece : moved) {
                if (movedPiece == piece) {
                    // std::cerr << "king has already moved, cannot castle" << std::endl;
                    return legal;
                }
                if (movedPiece == captured) {
                    // std::cerr << "that rook has already moved, cannot castle" << std::endl;
                    return legal;
                }
            }
        }

        // ensure it's the right piece type
        if (pieceType == PieceType::NONE) {
            legal.pieceType = getPieceType(legal.move.from);
        } else if (legal.moveType.castles) {
            legal.pieceType = PieceType::KING;  // Set to KING for castling
        } else {
            if (getPieceType(legal.move.from) != legal.pieceType)
                return legal;
        }

        if (legal.move.promotion != PieceType::NONE && legal.pieceType != PieceType::PAWN)
            return legal;

        // check if move is possible via piece definition
        if (!legal.moveType.castles && !piece->PossibleMove(m)) {
            return legal;
        }
        
        // in the scenario that you're being captured, you cannot move
        if (pretendMove.from != pretendMove.to && pretendMove.to == legal.move.from)
            return legal;

        Square offset = (legal.move.to - legal.move.from);
        switch (legal.pieceType) {
            case PieceType::PAWN:

                // detect en passant
                if (lastMove.pieceType == PieceType::PAWN && abs(lastMove.move.from.y - lastMove.move.to.y) > 1) {
                    if (legal.move.to.x == lastMove.move.from.x && legal.move.to.y + offset.y == lastMove.move.from.y) {
                        legal.moveType.enPassant = true;
                        legal.moveType.captures = true;
                    }
                }

                // a pawn captures if and only if it moves sideways
                if (legal.moveType.captures != (offset.x != 0))
                    return legal;


                if (abs(offset.y) >= 2) {
                    for (auto movedPiece : moved) {
                        if (movedPiece == piece)
                            return legal;
                    }
                }

                if (legal.move.to.y == 7 || legal.move.to.y == 0) {
                    // default to queen for promoting
                    if (legal.move.promotion == PieceType::NONE)
                        legal.move.promotion = PieceType::QUEEN;
                }               

                break;
            case PieceType::KING:
                if (abs(offset.x) > 1 || abs(offset.y) > 1) {
                    if (!legal.moveType.castles)
                        return legal;
                }
                
                break;
        }

        // ... FIXME
        // ... check if it collides with something on the way
        if (legal.pieceType != PieceType::KNIGHT) {
            // it didn't / cannot jump
            
            Square step = offset.normalized();
            Square scan = legal.move.from;
            while (true) {
                scan += step;
                if (scan == legal.move.to)
                    break;

                if (scan == pretendMove.to)
                    return legal;

                if (scan == pretendMove.from)
                    continue;

                if (getPiece(scan) == nullptr)
                    continue;

                return legal;
            }
        }

        // ... check that it's not moving into check
        if (pretendMove.from == pretendMove.to) {
            // it is the first iteration / check

            Square king;
            if (legal.pieceType == PieceType::KING) {
                king = legal.move.to;
                if (legal.moveType.castles) {
                    if (legal.moveType.castleDir)
                        king.x = 6;
                    else
                        king.x = 2;
                } 
            } else 
            king = getKing(color);

            for (int i = 0; i < teams.size(); i++) {
                if (static_cast<Teams>(i) == color)
                    continue;

                // TODO still no clue why this calls twice, doesn't effect it though
                // std::cout << "Checking team " << i << " for possible checks" << std::endl;

                for (const auto& pair : teams[i]) {
                    Square square = pair.first;
                    if (pair.second == nullptr) {
                        std::cerr << "dead square: " << pair.first.x << pair.first.y << std::endl;
                        continue;
                    }

                    if (legal.moveType.castles) {
                        if (LegalMove({square, legal.move.from}, static_cast<Teams>(i), getPieceType(square), { {-1, -1}, {-2, -2} }).valid) {
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
        if (move.moveType.castles) {
            // castles are technically two 'moves'
            if (move.moveType.castleDir) {
                MovePiece({ move.move.from, { 6, move.move.from.y }}, move.color); // move king
                MovePiece({ { 7, move.move.from.y }, { 5, move.move.from.y }}, move.color); // move rook
            } else {
                MovePiece({ move.move.from, { 2, move.move.from.y }}, move.color); // move king
                MovePiece({ { 0, move.move.from.y }, { 3, move.move.from.y }}, move.color); // move rook
            }
        } else {
            MovePiece(move.move, move.color);
            
            // en passant deletes the lastMove hack
            if (move.moveType.enPassant) {
                DeletePiece(lastMove.move.to);   
            }
        }

        // promotions
        if (move.move.promotion != PieceType::NONE) {
            PromotePiece(move.move, move.color, move.move.promotion);
        }

        // update fifty move rule
        if (move.moveType.captures || move.pieceType == PieceType::PAWN)
            lastReversableMove = 0;
        else
            lastReversableMove++;

        UpdateHistory();
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
        if (!pieceMap.second) {
            std::cerr << "Dead Square: " << pieceMap.first.x << " " << pieceMap.first.y << std::endl;
            continue;
        }

        if (pieceMap.second->PossibleMove(move)) {
            // add a complete, possible (not necessarily legal) move to vector
            CompleteMove complete = partial;
            complete.move = move;
            ret.push_back(complete);
        } 
        // else if (partial.pieceType == PieceType::KING) {
        //     partial.moveType.castles = true;   
        //     CompleteMove complete = partial;
        //     complete.move = move;
        //     ret.push_back(complete);
        // }

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
