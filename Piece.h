#pragma once
#include "Move.h"

class Piece {
public:
    Piece() = default;

    virtual constexpr PieceType Type() const { return PieceType::NONE; }
    virtual bool PossibleMove(const Move&) const { return false; }
};

class Pawn : public Piece {
private:
    // TODO:
    // expand tracking of moved pawns into a method inside of the Game
    // it deserves responsibility and when importing a board this will
    // be implicitly defined by the board
    bool moved = false;

    int direction;
public:
    Pawn(int direction = 0) : direction(direction) {}

    virtual constexpr PieceType Type() const override {
        return PieceType::PAWN;
    }

    virtual bool PossibleMove(const Move& m) const override {
        // piece cannot move to itself
        if (m.to == m.from)
            return false; 

        // take the offset
        Square forwardStep = (m.to-m.from);

        // counter-rotate the offset
        for (int i = 0; i < direction % 4; i++) {
            forwardStep.x = forwardStep.y;
            forwardStep.y = -forwardStep.x;
        }

        switch (forwardStep.y) {
            case 1:
                // cannot move sideways more than one square
                return (forwardStep.x <= 1 && forwardStep.x >= -1);
            case 2:
                // first move 2-square jump
                return !moved;
        }

        return false;
    }

    // TODO (above)
    bool hasMoved() const { return moved; }
    void setMoved() { this->moved = true; }
};
