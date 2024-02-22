#include "main.h"

#include "game.h"
#include "piece.h"
#include "player.h"

wxBEGIN_EVENT_TABLE(GameCanvas, wxGLCanvas)
	EVT_PAINT(GameCanvas::OnPaint)
	EVT_MOTION(GameCanvas::OnMouseMove)
	EVT_LEFT_UP(GameCanvas::OnMouseLeftRelease)
	EVT_LEFT_DOWN(GameCanvas::OnMouseLeftClick)
	EVT_LEFT_DCLICK(GameCanvas::OnMouseLeftDoubleClick)
wxEND_EVENT_TABLE()

const int DEFAULT_BOARD[] = {
	PIECE(ROOK, BLACK), PIECE(KNIGHT, BLACK), PIECE(BISHOP, BLACK), PIECE(QUEEN, BLACK), PIECE(KING, BLACK), PIECE(BISHOP, BLACK), PIECE(KNIGHT, BLACK), PIECE(ROOK, BLACK),
	PIECE(PAWN, BLACK), PIECE(PAWN, BLACK),	  PIECE(PAWN, BLACK),   PIECE(PAWN, BLACK),  PIECE(PAWN, BLACK), PIECE(PAWN, BLACK),   PIECE(PAWN, BLACK),   PIECE(PAWN, BLACK),
	0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,  0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,  0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,  0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,
	0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,  0x0000000000000000, 0x0000000000000000,   0x0000000000000000,   0x0000000000000000,
	PIECE(PAWN, WHITE), PIECE(PAWN, WHITE),   PIECE(PAWN, WHITE),   PIECE(PAWN, WHITE),  PIECE(PAWN, WHITE), PIECE(PAWN, WHITE),   PIECE(PAWN, WHITE),   PIECE(PAWN, WHITE),
	PIECE(ROOK, WHITE), PIECE(KNIGHT, WHITE), PIECE(BISHOP, WHITE), PIECE(QUEEN, WHITE), PIECE(KING, WHITE), PIECE(BISHOP, WHITE), PIECE(KNIGHT, WHITE), PIECE(ROOK, WHITE)
};

GameCanvas::GameCanvas(wxWindow* parent) :
	wxGLCanvas(parent, wxID_ANY, nullptr, wxDefaultPosition, wxSize(640, 640), wxWANTS_CHARS),
	m_board(nullptr)
{
	m_OGLContext = new wxGLContext(this);
	SetCurrent(*m_OGLContext);
}

GameCanvas::~GameCanvas()
{
	for(Piece* piece : m_pieces)
		delete piece;
	delete m_players[0];
	delete m_players[1];
	delete m_OGLContext;
}

void GameCanvas::Load()
{
	struct SPRITE_FILE {
		std::string file;
		Texture* texture;
	};

	m_players[0] = new Player();
	m_players[1] = new Player();

	m_players[0]->SetOpponent(m_players[1]);
	m_players[1]->SetOpponent(m_players[0]);

	m_board = g_sprites.Load("board.png");

	std::map<int, SPRITE_FILE> SPRITES
	{
		{ PIECE(PAWN, BLACK), { "bp.png" } },
		{ PIECE(PAWN, WHITE), { "wp.png" } },
		{ PIECE(ROOK, BLACK), { "br.png" } },
		{ PIECE(ROOK, WHITE), { "wr.png" } },
		{ PIECE(KNIGHT, BLACK), { "bn.png" } },
		{ PIECE(KNIGHT, WHITE), { "wn.png" } },
		{ PIECE(BISHOP, BLACK), { "bb.png" } },
		{ PIECE(BISHOP, WHITE), { "wb.png" } },
		{ PIECE(QUEEN, BLACK), { "bq.png" } },
		{ PIECE(QUEEN, WHITE), { "wq.png" } },
		{ PIECE(KING, BLACK), { "bk.png" } },
		{ PIECE(KING, WHITE), { "wk.png" } },
	};

	for(auto& it : SPRITES)
		it.second.texture = g_sprites.Load(it.second.file);

	wxSize size = GetSize();

	int i = 0;
	for(int y = 0; y < 8; ++y) {
		for(int x = 0; x < 8; ++x) {
			int flag = DEFAULT_BOARD[y * 8 + x];
			if(flag == 0)
				continue;

			int color = 0;
			if(flag > 0)
				color = WHITE;
			else if(flag < 0)
				color = BLACK;

			int type = log2(std::abs(flag));

			Piece* piece = nullptr;
			switch(type) {
			case PAWN: {
				piece = new Pawn(this, SPRITES[flag].texture, color);
				break;
			}
			case ROOK: {
				piece = new Rook(this, SPRITES[flag].texture, color);
				break;
			}
			case KNIGHT: {
				piece = new Knight(this, SPRITES[flag].texture, color);
				break;
			}
			case BISHOP: {
				piece = new Bishop(this, SPRITES[flag].texture, color);
				break;
			}
			case QUEEN: {
				piece = new Queen(this, SPRITES[flag].texture, color);
				break;
			}
			case KING: {
				piece = new King(this, SPRITES[flag].texture, color);
				break;
			}
			default: {
				break;
			}
			}

			if(piece) {
				Tile* tile = GetTile(x, y);
				tile->SetPiece(piece);
			}

			Player* player = nullptr;
			if(color == WHITE)
				player = m_players[0];
			else
				player = m_players[1];
			player->AddPiece(piece);
			piece->SetPlayer(player);

			AddPiece(piece);
		}
	}

	m_turnPlayer = 0;
}

void GameCanvas::AddPiece(Piece* piece)
{
	m_pieces.push_back(piece);
}

void GameCanvas::RemovePiece(Piece* piece)
{
	auto it = std::find(m_pieces.begin(), m_pieces.end(), piece);
	if(it == m_pieces.end())
		return;
	m_pieces.erase(it);
}

wxPoint GameCanvas::ToBoardPoint(wxPoint point)
{
	wxSize size = GetSize();
	wxPoint translatedPoint(point.x / (size.GetWidth() / 8), point.y / (size.GetHeight() / 8));
	return translatedPoint;
}

wxPoint GameCanvas::ToNormalPoint(wxPoint point)
{
	wxSize size = GetSize();
	wxPoint translatedPoint(point.x * (size.GetWidth() / 8), point.y * (size.GetHeight() / 8));
	return translatedPoint;
}

void GameCanvas::ClearMovements()
{
	m_players[0]->ResetAttacker();
	m_players[0]->ClearMovementCount();
	m_players[0]->UnCheck();

	m_players[1]->ResetAttacker();
	m_players[1]->ClearMovementCount();
	m_players[1]->UnCheck();

	for(Piece* piece : m_pieces)
		piece->ClearMovements();
}

void GameCanvas::CalculateMovements()
{
	Player* turnPlayer = m_players[m_turnPlayer];
	Player* opponentPlayer = turnPlayer->GetOpponent();
	
	for(Piece* piece : opponentPlayer->GetPieces()) {
		if(piece->CalculateMovements())
			turnPlayer->Check();
	}

	if(m_selectedPiece)
		m_selectedPiece->CalculateMovements();

	for(Piece* piece : turnPlayer->GetPieces()) {
		if(piece != m_selectedPiece)
			piece->CalculateMovements();
	}

	if(turnPlayer->GetMovementCount() == 0) {
		if(turnPlayer->IsCheck()) {
			//TODO Mate
		}
		else {
			//TODO Drowning
		}
	}
	Refresh();
}

void GameCanvas::Promote(Pawn* pawn, PieceMovement* pieceMovement, wxPoint promotePoint)
{
	const wxSize& size = GetSize();

	if(pawn->GetColor() == BLACK)
		promotePoint.y += 1;

	m_promoteWindow = new PromoteWindow;
	m_promoteWindow->pawn = pawn;
	m_promoteWindow->rect = wxRect2DDouble((double)promotePoint.x * (double)size.GetWidth() / 8., (double)promotePoint.y * (double)size.GetHeight() / 8., (double)size.GetWidth() / 8., (double)size.GetHeight() / 8);
	m_promoteWindow->function = [pieceMovement, this](Piece* newPiece) {
		newPiece->Move(pieceMovement);
		m_turnPlayer = (m_turnPlayer + 1) % 2;
		ClearMovements();
		CalculateMovements();
	};

	std::vector<std::pair<Texture*, int>> textures;
	if(pawn->GetColor() == BLACK) {
		textures.push_back({ g_sprites.GetTexture("images/bb.png"), BISHOP });
		textures.push_back({ g_sprites.GetTexture("images/br.png"), ROOK });
		textures.push_back({ g_sprites.GetTexture("images/bn.png"), KNIGHT });
		textures.push_back({ g_sprites.GetTexture("images/bq.png"), QUEEN });
	}
	else {
		textures.push_back({ g_sprites.GetTexture("images/wq.png"), QUEEN });
		textures.push_back({ g_sprites.GetTexture("images/wn.png"), KNIGHT });
		textures.push_back({ g_sprites.GetTexture("images/wr.png"), ROOK });
		textures.push_back({ g_sprites.GetTexture("images/wb.png"), BISHOP });
	}

	m_promoteWindow->rect.m_height *= textures.size();
	if(pawn->GetColor() == BLACK)
		m_promoteWindow->rect.m_y -= m_promoteWindow->rect.m_height;
	m_promoteWindow->textures = std::move(textures);
}

void GameCanvas::OnDragEnter(wxMouseEvent& event)
{
	m_draggStartPoint = new wxPoint(event.GetPosition());
	if(m_selectedPiece)
		m_pieceStartPoint = m_selectedPiece->GetDrawPoint();
}

void GameCanvas::OnDragMove(wxMouseEvent& event, const wxPoint& delta)
{
	if(m_selectedPiece) {
		m_selectedPiece->SetDrawPoint(m_pieceStartPoint + delta);
		Refresh();
	}
}

void GameCanvas::OnDragRelease(wxMouseEvent& event)
{
	delete m_draggStartPoint;
	m_draggStartPoint = nullptr;

	if(m_selectedPiece) {
		wxPoint point = event.GetPosition();
		m_selectedPiece->ClearDrawPoint();
		MoveSelectedPiece(point);
		Refresh();
	}
}

void GameCanvas::OnMouseMove(wxMouseEvent& event)
{
	m_mousePosition = event.GetPosition();

	if(m_mouseDownPoint) {
		wxPoint mousePoint = event.GetPosition();
		wxPoint deltaPoint = mousePoint - *m_mouseDownPoint;
		if(m_draggStartPoint)
			OnDragMove(event, deltaPoint);
		else {
			int distance = std::sqrt(std::pow(deltaPoint.x, 2) + std::pow(deltaPoint.y, 2));
			if(distance >= MIN_DRAGG_PIXELS)
				OnDragEnter(event);
		}
	}

	wxPoint point = event.GetPosition();
	wxPoint boardCoord = ToBoardPoint(point);

	Tile* tile = GetTile(boardCoord);
	if(!tile)
		return;

	Piece* piece = tile->GetPiece();
	
	const wxSize& size = GetSize();
	wxRect2DDouble rect(0, 0, size.GetWidth() / 8, size.GetHeight() / 8);
	bool inPieceArea = false;
	if(piece)
		inPieceArea = piece->GetTexture()->IsInside(point - ToNormalPoint(boardCoord), rect);

	if(inPieceArea) {
		if(m_hoveredPiece == piece)
			return;
		else if(m_hoveredPiece)
			m_hoveredPiece->OnHoverChange(false);
		Refresh();
		piece->OnHoverChange(true);
		m_hoveredPiece = piece;
	} else if(piece && piece == m_hoveredPiece) {
		Refresh();
		m_hoveredPiece->OnHoverChange(false);
		m_hoveredPiece = nullptr;
	}
}

void GameCanvas::OnMouseLeftRelease(wxMouseEvent& event)
{
	if(m_mouseDownPoint) {
		delete m_mouseDownPoint;
		m_mouseDownPoint = nullptr;
	}

	if(m_draggStartPoint)
		OnDragRelease(event);
	else if(m_selectedPiece) {
		m_selectedPiece->ClearDrawPoint();
		if(m_selectedPiece->Release())
			ReleaseSelectedPiece();
		Refresh();
	}
}

void GameCanvas::OnMouseLeftClick(wxMouseEvent& event)
{
	wxPoint point = event.GetPosition();
	if(m_promoteWindow) {
		m_promoteWindow->Promote(this, point);
		m_selectedPiece = nullptr;
		delete m_promoteWindow;
		m_promoteWindow = nullptr;
		Refresh();
		return;
	}

	if(m_draggStartPoint)
		OnDragRelease(event);

	wxPoint boardCoord = ToBoardPoint(point);

	Tile* tile = GetTile(boardCoord);
	if(!tile)
		return;

	if(m_mouseDownPoint)
		delete m_mouseDownPoint;
	m_mouseDownPoint = new wxPoint(point);

	Piece* tilePiece = tile->GetPiece();
	SelectPiece(point, tilePiece);

	Refresh();
}

void GameCanvas::OnMouseLeftDoubleClick(wxMouseEvent& event)
{
	wxPoint point = event.GetPosition();
	if(m_promoteWindow) {
		m_promoteWindow->Promote(this, point);
		m_selectedPiece = nullptr;
		delete m_promoteWindow;
		m_promoteWindow = nullptr;
		Refresh();
		return;
	}

	wxPoint boardCoord = ToBoardPoint(point);

	Tile* tile = GetTile(boardCoord);
	if(!tile)
		return;

	if(m_mouseDownPoint)
		delete m_mouseDownPoint;
	m_mouseDownPoint = new wxPoint(point);

	Piece* tilePiece = tile->GetPiece();

	SelectPiece(point, tilePiece);

	Refresh();
}

void GameCanvas::OnPaint(wxPaintEvent& event)
{
	Setup();
	Draw();
	Release();
	SwapBuffers();
}

void GameCanvas::Setup()
{
	const wxSize& size = GetSize();

	glViewport(0, 0, size.GetWidth(), size.GetHeight());

	// Enable 2D mode
	int vPort[4];

	glGetIntegerv(GL_VIEWPORT, vPort);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, vPort[2], vPort[3], 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void GameCanvas::Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	const wxSize& size = GetSize();
	wxRect2DDouble rect(0, 0, size.GetWidth(), size.GetHeight());

	glEnable(GL_TEXTURE_2D);
	m_board->Bind();
	glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(255));
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex2f(rect.GetLeft(), rect.GetTop());
	glTexCoord2f(1.f, 0.f); glVertex2f(rect.GetRight(), rect.GetTop());
	glTexCoord2f(1.f, 1.f); glVertex2f(rect.GetRight(), rect.GetBottom());
	glTexCoord2f(0.f, 1.f); glVertex2f(rect.GetLeft(), rect.GetBottom());
	glEnd();
	glDisable(GL_TEXTURE_2D);

	for(Tile* tile : m_tiles)
		tile->Draw(this, rect);

	for(Piece* piece : m_pieces) {
		if(piece != m_selectedPiece)
			piece->Draw(rect);
	}

	if(m_selectedPiece)
		m_selectedPiece->Draw(rect);

	if(m_promoteWindow) {
		wxRect2DDouble drawRect = m_promoteWindow->rect;

		glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(255));
		glBegin(GL_QUADS);
		glVertex2f(drawRect.GetLeft(), drawRect.GetTop());
		glVertex2f(drawRect.GetRight(), drawRect.GetTop());
		glVertex2f(drawRect.GetRight(), drawRect.GetBottom());
		glVertex2f(drawRect.GetLeft(), drawRect.GetBottom());
		glEnd();

		int i = 0;
		for(const auto& text : m_promoteWindow->textures) {
			wxRect2DDouble pieceRect = drawRect;
			pieceRect.m_height /= (double)m_promoteWindow->textures.size();
			pieceRect.m_y = drawRect.m_y + pieceRect.m_height * i++;

			glEnable(GL_TEXTURE_2D);
			text.first->Bind();
			glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(255));
			glBegin(GL_QUADS);
			glTexCoord2f(0.f, 0.f); glVertex2f(pieceRect.GetLeft(), pieceRect.GetTop());
			glTexCoord2f(1.f, 0.f); glVertex2f(pieceRect.GetRight(), pieceRect.GetTop());
			glTexCoord2f(1.f, 1.f); glVertex2f(pieceRect.GetRight(), pieceRect.GetBottom());
			glTexCoord2f(0.f, 1.f); glVertex2f(pieceRect.GetLeft(), pieceRect.GetBottom());
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
	}
}

void GameCanvas::Release()
{
	// Disable 2D mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void GameCanvas::SelectPiece(const wxPoint& point, Piece* piece)
{
	if(!m_selectedPiece)
		m_selectedPiece = piece;
	else if(m_selectedPiece) {
		if(m_selectedPiece == piece)
			m_selectedPiece->SetRelease(true);
		else if(!MoveSelectedPiece(point) || (piece && !piece->GetTile()))
			m_selectedPiece = nullptr;
		else
			m_selectedPiece = piece;
	}

	wxPoint selectPoint(point.x - (GetSize().GetWidth() / 8) / 2, point.y - (GetSize().GetHeight() / 8) / 2);
	if(m_selectedPiece) {
		m_selectedPiece->SetDrawPoint(selectPoint);
		CalculateMovements();
	}
}

bool GameCanvas::MoveSelectedPiece(const wxPoint& point)
{
	if(m_selectedPiece && m_selectedPiece->GetPlayer() == m_players[m_turnPlayer]) {
		wxPoint boardCoord = ToBoardPoint(point);

		Tile* tile = GetTile(boardCoord);
		if(!tile)
			return true;

		auto& movements = m_selectedPiece->GetMovements();
		bool move = false;
		bool promote = false;
		for(PieceMovement* it : movements) {
			if(it->GetTile() == tile && it->GetType() != PieceMovement::IGNORE_MOVE) {
				promote = !m_selectedPiece->Move(it);
				move = true;
				break;
			}
		}

		if(promote)
			return false;

		if(move) {
			m_turnPlayer = (m_turnPlayer + 1) % 2;
			ClearMovements();
			CalculateMovements();
		}
	}
	return true;
}

void GameCanvas::ReleaseSelectedPiece()
{
	if(m_selectedPiece) {
		m_selectedPiece->SetRelease(false);
		m_selectedPiece = nullptr;
	}
}

void GameCanvas::PromoteWindow::Promote(GameCanvas* canvas, const wxPoint& mousePoint)
{
	wxPoint point(mousePoint.x - rect.m_x, mousePoint.y - rect.m_y);
	point.y /= rect.m_height / 4;

	int index = point.y;
	int color = pawn->GetColor();
	if(point.x >= 0 && point.x <= rect.m_width && index >= 0 && index < textures.size()) {
		auto it = textures[index];
		Piece* piece = nullptr;
		switch(it.second) {
		case ROOK: {
			piece = new Rook(canvas, it.first, color);
			break;
		}
		case KNIGHT: {
			piece = new Knight(canvas, it.first, color);
			break;
		}
		case BISHOP: {
			piece = new Bishop(canvas, it.first, color);
			break;
		}
		case QUEEN: {
			piece = new Queen(canvas, it.first, color);
			break;
		}
		default: {
			break;
		}
		}

		if(!piece) {
			// TODO 'x' Button
			return;
		}

		Tile* tile = pawn->GetTile();
		tile->SetPiece(piece);

		Player* player = pawn->GetPlayer();
		player->RemovePiece(pawn);
		canvas->RemovePiece(pawn);

		delete pawn;

		piece->SetPlayer(player);

		player->AddPiece(piece);
		canvas->AddPiece(piece);

		function(piece);
	}
}

GameBoard::GameBoard(wxWindow* parent) :
	wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(720, 480))
{
	m_sizer = new wxBoxSizer(wxVERTICAL);

	SetBackgroundColour(wxColour(48, 46, 43));

	m_topPanel = new wxPanel(this);

	wxBoxSizer* centerSizer = new wxBoxSizer(wxHORIZONTAL);
	m_centerPanel = new wxPanel(this);
	m_centerPanel->SetSizer(centerSizer);

	m_leftPanel = new wxPanel(m_centerPanel);

	m_canvas = new GameCanvas(m_centerPanel);
	m_canvas->Load();

	m_rightPanel = new wxPanel(m_centerPanel);

	m_bottomPanel = new wxPanel(this);

	m_sizer->Add(m_topPanel, 1, wxALIGN_TOP);
	m_sizer->Add(m_centerPanel, 2, wxALIGN_CENTER);
	m_sizer->Add(m_bottomPanel, 1, wxEXPAND);

	centerSizer->Add(m_leftPanel, 1, wxALIGN_LEFT);
	centerSizer->Add(m_canvas, 2, wxALIGN_CENTER);
	centerSizer->Add(m_rightPanel, 1, wxEXPAND);

	SetSizer(m_sizer);
}
