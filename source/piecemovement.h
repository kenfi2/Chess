#ifndef PIECEMOVEMENT_H
#define PIECEMOVEMENT_H

class Tile;
class Piece;

class PieceMovement {
public:
	enum {
		DEFAULT_MOVE,
		IGNORE_MOVE,
		ENPASSANT_MOVE,
		SMALL_CASTLING_MOVE,
		BIG_CASTLING_MOVE
	};

	PieceMovement() = default;
	~PieceMovement();

	PieceMovement(const PieceMovement&) = delete;
	PieceMovement& operator=(const PieceMovement&) = delete;

	static PieceMovement* CreateGeneric(Piece* piece, Tile* tile, uint8_t type = DEFAULT_MOVE);
	static PieceMovement* CreatePawnAttack(Piece* piece, Tile* tile);
	static PieceMovement* CreatePawnMovement(Piece* piece, Tile* tile);
	static PieceMovement* CreateCastlingMovement(Piece* piece, Tile* tile, Piece* rook, uint8_t type);

	int GetColor() const { return m_color; }
	void SetColor(int color) { m_color = color; }

	Tile* GetTile() const { return m_tile; }

	Piece* GetTarget() const { return m_target; }
	void SetTarget(Piece* target) { m_target = target; }

	uint8_t GetType() const { return m_type; }
	void SetType(uint8_t type) { m_type = type; }

private:
	Tile* m_tile = nullptr;
	Piece* m_target = nullptr;

	int m_color = 0;

	uint8_t m_type = DEFAULT_MOVE;
	bool m_ignoreThreat = false;

	void SetTile(Tile* tile);
};

#endif
