#ifndef TILE_H
#define TILE_H

class Piece;
class GameCanvas;

class Tile
{
public:
	Tile() {}

	Piece* GetPiece() const { return m_piece; }
	void SetPiece(Piece* piece);

	wxPoint GetPoint() const { return m_point; }
	void SetPoint(wxPoint point) { m_point = point; }
	std::string GetAlgebraicPosition() const;

	int GetThreats(int color) const;
	void AddThreat(int color);
	void RemoveThreat(int color);

	void Draw(GameCanvas* canvas, const wxRect2DDouble& rect);
	void ToDraw(const wxColor& color);

private:
	Piece* m_piece = nullptr;
	wxColor* m_toDraw = nullptr;

	wxPoint m_point;
	std::string m_algebraicPosition;
	std::multiset<int> m_threats;

	friend class Tiles;
};

class Tiles {
public:
	Tiles();
	~Tiles();

	Tile* GetTile(wxPoint point) const { return GetTile(point.x, point.y); }
	Tile* GetTile(int x, int y) const {
		if(x >= 8 || y >= 8 || x < 0 || y < 0)
			return nullptr;

		int pos = y * 8 + x;
		if(pos < 0 || pos >= m_tiles.size())
			return nullptr;
		return m_tiles[pos];
	}

protected:
	std::vector<Tile*> m_tiles;
};

#endif
