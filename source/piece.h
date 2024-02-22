#ifndef PIECE_H
#define PIECE_H

class Tile;
class Player;
class Piece;
class Texture;

#include "piecemovement.h"

class GameCanvas;

class Piece
{
public:
	Piece(GameCanvas* canvas, Texture* texture, int color);
	virtual ~Piece();

	Piece(const Piece& piece) = delete;
	Piece& operator()(const Piece& piece) = delete;

	void Draw(const wxRect2DDouble& rect);
	virtual void DrawMovements(const wxRect2DDouble& rect);

	int GetId() const { return m_id; }

	GameCanvas* GetCanvas() const { return m_canvas; }

	wxPoint GetDrawPoint() const { return *m_drawPoint; }
	void SetDrawPoint(wxPoint drawPoint);
	void ClearDrawPoint();

	Texture* GetTexture() const { return m_texture; }
	int GetColor() const { return m_color; }

	wxPoint GetPoint() const;
	Tile* GetTile() const { return m_tile; }
	void SetTile(Tile* tile) { m_tile = tile; }

	std::vector<PieceMovement*>& GetMovements() { return m_movements; }
	virtual bool CalculateMovements();
	bool CheckMovement(PieceMovement* movement, Piece* attacker, const std::unordered_set<Tile*>& targetTiles);
	void ClearMovements();

	int GetMovementCount() const { return m_movementCount; }
	int GetLastMovementCount() const { return m_lastMovementCount; }

	Player* GetPlayer() const { return m_player; }
	void SetPlayer(Player* player) { m_player = player; }

	bool Move(PieceMovement* pieceMovement);
	void OnRemove();

	bool Release() const { return m_release; }
	void SetRelease(bool release) { m_release = release; }

	void OnHoverChange(bool hover);

	virtual int GetType() const = 0;
	virtual std::string GetAlgebraicName() const = 0;
	virtual int GetWeight() const = 0;

protected:
	int m_id;
	GameCanvas* m_canvas;
	wxPoint* m_drawPoint;
	Texture* m_texture;
	int m_color;

	Tile* m_tile = nullptr;

	std::vector<PieceMovement*> m_movements;

	int m_movementCount = 0;
	int m_lastMovementCount = 0;

	Player* m_player = nullptr;

	bool m_calculed = false;

	std::unordered_set<Tile*> m_kingDefender;
	
	bool m_release = false;
	bool m_hovered = false;
};

class Pawn : public Piece
{
public:
	explicit Pawn(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return PAWN; }
	std::string GetAlgebraicName() const override { return ""; }
	int GetWeight() const override { return 1; }
	bool CalculateMovements() override;

	Pawn* GetEnpassant() const { return m_enpassant; }
	int GetEnpassantX() const { return m_enpassantX; }

	void SetEnpassant(Pawn* enpassant, int enpassantX) { m_enpassant = enpassant; m_enpassantX = enpassantX; }

private:
	Pawn* m_enpassant = nullptr;
	int m_enpassantX = 0;
};

class Rook : public Piece
{
public:
	explicit Rook(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return ROOK; }
	std::string GetAlgebraicName() const override;
	int GetWeight() const override { return 5; }
};

class Knight : public Piece
{
public:
	explicit Knight(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return KNIGHT; }
	std::string GetAlgebraicName() const override;
	int GetWeight() const override { return 3; }
	bool CalculateMovements() override;
};

class Bishop : public Piece
{
public:
	explicit Bishop(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return BISHOP; }
	int GetWeight() const override { return 3; }
	std::string GetAlgebraicName() const override;
};

class Queen : public Piece
{
public:
	explicit Queen(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return QUEEN; }
	int GetWeight() const override { return 9; }
	std::string GetAlgebraicName() const override;
};

class King : public Piece
{
public:
	explicit King(GameCanvas* canvas, Texture* texture, int color) : Piece(canvas, texture, color) {}

	int GetType() const override { return KING; }
	int GetWeight() const override { return 0xFFFFFFFF; }
	std::string GetAlgebraicName() const override;
	bool CalculateMovements() override;
};

#endif
