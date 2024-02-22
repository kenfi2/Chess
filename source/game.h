#ifndef GAME_H
#define GAME_H

#include "tile.h"
#include "sprites.h"

class Piece;
class Pawn;
class PieceMovement;
class Player;

class GameCanvas : public wxGLCanvas, public Tiles
{
	struct PromoteWindow {
		wxRect2DDouble rect;
		Pawn* pawn;
		std::function<void(Piece*)> function;
		std::vector<std::pair<Texture*, int>> textures;
		void Promote(GameCanvas* canvas, const wxPoint& mousePoint);
	};

public:
	GameCanvas(wxWindow* parent);
	~GameCanvas();

	void Load();

	void AddPiece(Piece* piece);
	void RemovePiece(Piece* piece);

	wxPoint ToBoardPoint(wxPoint point);
	wxPoint ToNormalPoint(wxPoint point);

	Piece* GetSelectedPiece() const { return m_selectedPiece; }
	wxPoint GetMousePoint() const { return m_mousePosition; }

	void ClearMovements();
	void CalculateMovements();

	void Promote(Pawn* pawn, PieceMovement* pieceMovement, wxPoint promotePoint);

private:
	void OnDragEnter(wxMouseEvent& event);
	void OnDragMove(wxMouseEvent& event, const wxPoint& delta);
	void OnDragRelease(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseLeftRelease(wxMouseEvent& event);
	void OnMouseLeftClick(wxMouseEvent& event);
	void OnMouseLeftDoubleClick(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& event);
	void Setup();
	void Draw();
	void Release();

	void SelectPiece(const wxPoint& point, Piece* piece);
	bool MoveSelectedPiece(const wxPoint& point);
	void ReleaseSelectedPiece();

	Piece* m_selectedPiece = nullptr;
	Piece* m_hoveredPiece = nullptr;
	wxPoint* m_mouseDownPoint = nullptr;
	wxPoint* m_draggStartPoint = nullptr;
	wxPoint m_pieceStartPoint;
	wxPoint m_mousePosition;

	PromoteWindow* m_promoteWindow = nullptr;

private:
	wxDECLARE_EVENT_TABLE();

	wxGLContext* m_OGLContext;
	Texture* m_board;

	Player* m_players[2];
	int m_turnPlayer = 0;
	std::vector<Piece*> m_pieces;
};

class GameBoard : public wxPanel {
public:
	GameBoard(wxWindow* parent);

private:
	GameCanvas* m_canvas;
	wxSizer* m_sizer;

	wxPanel* m_centerPanel;
	wxPanel* m_topPanel;
	wxPanel* m_leftPanel;
	wxPanel* m_rightPanel;
	wxPanel* m_bottomPanel;
};

#endif
