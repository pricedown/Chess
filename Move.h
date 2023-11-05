enum class PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
};

enum Teams {
    NONE = -1,
    WHITE = 0,
    BLACK = 1,
};

struct MoveType {
    bool captures, checks, promotes, castles;

    bool operator==(const MoveType& other) const {
        return
            (captures == other.captures) &&
            (checks == other.checks) &&
            (promotes == other.promotes) &&
            (castles == other.castles);
    }

    bool operator!=(const MoveType& other) const { return !(*this == other); }
};

struct Square {
    int x, y;

    bool operator==(const Square& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Square& other) const {
        return !(*this == other);
    }

    bool operator>(const int& n) const { return (x > n && y > n); }
    bool operator<(const int& n) const { return (x < n && y < n); }
    bool operator>=(const int& n) const { return (x >= n && y >= n); }
    bool operator<=(const int& n) const { return (x <= n && y <= n); }

    Square operator+(const Square& other) const {
        return {x + other.x, y + other.y};
    }

    Square operator-(const Square& other) const {
        return {x - other.x, y - other.y};
    }
};

// Define a hash function for Square
// no clue what this does but the internet hath graced me with it
namespace std {
    template <>
    struct hash<Square> {
        size_t operator()(const Square& square) const {
            return hash<int>()(square.x) ^ hash<int>()(square.y);
        }
    };
}

struct Move {
    Square from, to; // this is all you need for a completely unique move

    bool operator==(const Move& other) const {
        return (from == other.from) && (to == other.to);
    }
    bool operator!=(const Move& other) const { return !(*this == other); }
};


struct AlgebraicMove {
    Square to;
    MoveType moveType;
    PieceType pieceType;
};

struct CompleteMove {
    bool valid = false;
    Teams color = Teams::NONE;

    Move move;
    MoveType moveType;
    PieceType pieceType = PieceType::NONE;

    bool operator==(const CompleteMove& other) const {
        return true;
        return (valid == other.valid) &&
            (color == other.color) &&
            (move == other.move) &&
            (moveType == other.moveType) &&
            (pieceType == other.pieceType);
        // TODO clean up all of these horrible overloads
    }

    bool operator!=(const CompleteMove& other) const { return !(*this == other); }

    CompleteMove operator+(const Teams&& color) {
        CompleteMove ret = (*this);
        ret.color = color;
        return ret;
    }
};
