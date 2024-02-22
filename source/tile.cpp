#include "main.h"

#include "game.h"
#include "piece.h"

#include <sstream>

Tiles::Tiles()
{
	for(int y = 0; y < 8; ++y) {
		for(int x = 0; x < 8; ++x) {
			Tile* tile = new Tile;
			tile->m_point = wxPoint(x, y);

			m_tiles.push_back(tile);
		}
	}
}

Tiles::~Tiles()
{
	for(Tile* tile : m_tiles)
		delete tile;
}

void Tile::SetPiece(Piece* piece)
{
	if(piece) {
		Tile* oldTile = piece->GetTile();
		if(oldTile)
			oldTile->SetPiece(nullptr);

		piece->SetTile(this);
	}
	m_piece = piece;
}

std::string Tile::GetAlgebraicPosition() const
{
	std::stringstream ss;
	ss << ALGEBRAIC_POSITION_ROW[GetPoint().x];
	ss << ALGEBRAIC_POSITION_COLUMN[GetPoint().y];

	return ss.str();
}

int Tile::GetThreats(int color) const
{
	return m_threats.count(color);
}

void Tile::AddThreat(int color)
{
	m_threats.insert(color);
}

void Tile::RemoveThreat(int color)
{
	m_threats.erase(color);
}

void Tile::Draw(GameCanvas* canvas, const wxRect2DDouble& rect)
{
	wxPoint point = GetPoint();
	wxRect2DDouble drawRect((double)point.x * rect.m_width / 8, (double)point.y * rect.m_height / 8, rect.m_width / 8, rect.m_height / 8);

	if(m_toDraw) {
		glColor4ub(m_toDraw->Red(), m_toDraw->Green(), m_toDraw->Blue(), m_toDraw->Alpha());
		glBegin(GL_QUADS);
		glVertex2f(drawRect.GetLeft(), drawRect.GetTop());
		glVertex2f(drawRect.GetRight(), drawRect.GetTop());
		glVertex2f(drawRect.GetRight(), drawRect.GetBottom());
		glVertex2f(drawRect.GetLeft(), drawRect.GetBottom());
		glEnd();

		delete m_toDraw;
		m_toDraw = nullptr;
	}
	else {/*
		for(int i : { 1, -1 }) {
			if(m_threats.count(i) == 0)
				continue;

			if(i == 1)
				glColor4ub(uint8_t(0), uint8_t(255), uint8_t(0), uint8_t(m_threats.count(i) * 0x1F));
			else	
				glColor4ub(uint8_t(0), uint8_t(0), uint8_t(255), uint8_t(m_threats.count(i) * 0x1F));
			glBegin(GL_QUADS);
			glVertex2f(drawRect.GetLeft(), drawRect.GetTop());
			glVertex2f(drawRect.GetRight(), drawRect.GetTop());
			glVertex2f(drawRect.GetRight(), drawRect.GetBottom());
			glVertex2f(drawRect.GetLeft(), drawRect.GetBottom());
			glEnd();
		}*/
	}
}

void Tile::ToDraw(const wxColor& color)
{
	m_toDraw = new wxColor(color);
}
