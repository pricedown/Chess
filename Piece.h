#pragma once
#include "Move.h"
#include <cmath>

class Piece {
public:
    inline virtual PieceType Type() const { return PieceType::NONE; }
    inline virtual bool PossibleMove(const Move&) const { return false; }
};

class Pawn : public Piece {
private:
    int direction;
public:
    Pawn(int direction = 0) : direction(direction) {}
    inline virtual PieceType Type() const override { return PieceType::PAWN; }
    inline virtual bool PossibleMove(const Move& m) const override;
};

class Knight : public Piece {
public:
    inline virtual PieceType Type() const override { return PieceType::KNIGHT; }
    inline virtual bool PossibleMove(const Move& m) const override;
};

class Bishop : public Piece {
public:
    inline virtual PieceType Type() const override { return PieceType::BISHOP; }
    inline virtual bool PossibleMove(const Move& m) const override; 
};

class Rook : public Piece {
public:
    inline virtual PieceType Type() const override { return PieceType::ROOK; }
    inline virtual bool PossibleMove(const Move& m) const override; 
};

class Queen : public Piece {
private:
public:
    inline virtual PieceType Type() const override { return PieceType::QUEEN; }
    inline virtual bool PossibleMove(const Move& m) const override;
};

class King : public Piece {
public:
    inline virtual PieceType Type() const override { return PieceType::KING; }
    inline virtual bool PossibleMove(const Move& m) const override;
};

bool Pawn::PossibleMove(const Move& m) const {
        // piece cannot move to itself
        if (m.to == m.from)
            return false; 

        Square forwardStep;
        // counter-rotate the offset
        forwardStep = (m.to - m.from);
        for (int i = 0; i < direction % 4; i++) {
            int x = forwardStep.x;
            forwardStep.x = -forwardStep.y;
            forwardStep.y = x;
        }

        if (forwardStep.y == 1)
            return (abs(forwardStep.x) <= 1);
        if (forwardStep.y == 2)
            return (forwardStep.x == 0);

        return false;
}

bool Knight::PossibleMove(const Move& m) const {
    // piece cannot move to itself
    if (m.from == m.to)
        return false; 

    Square offset = m.to - m.from;
    if (offset.x == 0 || offset.y == 0)
        return false; 

    return (abs(offset.x) + abs(offset.y) == 3);
}


bool Bishop::PossibleMove(const Move& m) const {
    // piece cannot move to itself
    if (m.from == m.to)
        return false; 

    Square offset = m.to - m.from;
    return (abs(offset.x) == abs(offset.y));
}


bool Rook::PossibleMove(const Move& m) const {
    // piece cannot move to itself
    if (m.to == m.from)
        return false; 

    return ((m.to.x == m.from.x) || (m.to.y == m.from.y));
}

bool Queen::PossibleMove(const Move& m) const {
    // piece cannot move to itself
    if (m.to == m.from)
        return false; 

    // combine logic from both rook & bishop
    Square offset = m.to - m.from;
    if (abs(offset.x) == abs(offset.y)) 
        return true;
    if ((m.to.x == m.from.x) || (m.to.y == m.from.y))
        return true;

    return false;
}


bool King::PossibleMove(const Move& m) const {
    // piece cannot move to itself
    if (m.to == m.from)
        return false; 

    if (abs(m.to.x - m.from.x) <= 1 && abs(m.to.y - m.from.y) <= 1)
        return true;
    if (abs(m.to.x - m.from.x) <= 1 || abs(m.to.y - m.from.y) <= 1)
        return true;

    return false;
}
