#pragma once
#include "Move.h"

class Piece {
    public:
        Piece() = default;

        virtual constexpr PieceType Type() const {
            return PieceType::NONE;
        }
        virtual bool PossibleMove(const Move&) const {
            return false;
        }
};

class Pawn : public Piece {
    private:
        bool moved = false;
        int direction;
    public:
        Pawn(int direction = 0) : direction(direction) {}

        virtual constexpr PieceType Type() const override {
            return PieceType::PAWN;
        }

        virtual bool PossibleMove(const Move& m) const override {
            if (m.to == m.from)
                return false;

            // TODO: this is a stub
            Square offset = m.to - m.from;
            if (offset <= 1)
                return true;

            return true;
        }

        bool hasMoved() const { return moved; }
};
