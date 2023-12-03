#pragma once
#include "Move.h"
#include <cmath>

class Piece {
public:
    Piece() = default;

    virtual PieceType Type() const { return PieceType::NONE; }
    virtual bool PossibleMove(const Move&) const { return false; }
};

class Pawn : public Piece {
private:
    bool moved = false;

    int direction;
public:
    Pawn(int direction = 0) : direction(direction) {}

    virtual PieceType Type() const override { return PieceType::PAWN; }

    virtual bool PossibleMove(const Move& m) const override {
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

    bool hasMoved() const { return moved; }
    void setMoved() { this->moved = true; }
};

class Knight : public Piece {
private:
public:
    Knight() = default;
    virtual PieceType Type() const override { return PieceType::KNIGHT; }
    virtual bool PossibleMove(const Move& m) const override {
        if (m.from == m.to)
            return false; 
        
        Square offset = m.to - m.from;
        if (offset.x == 0 || offset.y == 0)
            return false; 
        
        return (abs(offset.x) + abs(offset.y) == 3);
    }
};

class Bishop : public Piece {
private:
public:
    Bishop() = default;
    virtual PieceType Type() const override { return PieceType::BISHOP; }
    virtual bool PossibleMove(const Move& m) const override {
        if (m.from == m.to)
            return false; 
        
        Square offset = m.to - m.from;
        return (abs(offset.x) == abs(offset.y));
    }
};

class Rook : public Piece {
private:
    bool moved;
public:
    Rook(bool moved = false) : moved(moved) {}

    virtual PieceType Type() const override { return PieceType::ROOK; }

    virtual bool PossibleMove(const Move& m) const override {
        // piece cannot move to itself
        if (m.to == m.from)
            return false; 

        return ((m.to.x == m.from.x) || (m.to.y == m.from.y));
    }
    
    bool hasMoved() const { return moved; }
    void setMoved() { this->moved = true; }
};

class Queen : public Piece {
private:
public:
    Queen() = default;

    virtual PieceType Type() const override { return PieceType::QUEEN; }

    virtual bool PossibleMove(const Move& m) const override {
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
};

class King : public Piece {
private:
    bool moved;
public:
    King(bool moved = false) : moved(moved) {}

    virtual PieceType Type() const override { return PieceType::KING; }

    virtual bool PossibleMove(const Move& m) const override {
        // piece cannot move to itself
        if (m.to == m.from)
            return false; 

        return ( (abs(m.to.x - m.from.x) <= 1) && (abs(m.to.y - m.from.y) <= 1) );
    }
    
    bool hasMoved() const { return moved; }
    void setMoved() { this->moved = true; }
};
