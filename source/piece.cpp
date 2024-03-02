#include "main.h"

#include "piece.h"
#include "game.h"
#include "player.h"

Piece::Piece(GameCanvas* canvas, Texture* texture, int color)
{
    static int autoId = 0;
    m_id = autoId++;
    m_canvas = canvas;
    m_texture = texture;
    m_color = color;
}

Piece::~Piece()
{
}

void Piece::Draw(const wxRect2DDouble& rect)
{
    if(!m_texture)
        return;

    if(m_canvas->GetSelectedPiece() == this)
        DrawMovements(rect);

    wxPoint point = GetPoint();
    wxRect2DDouble drawRect((double)point.x * rect.m_width / 8, (double)point.y * rect.m_height / 8, rect.m_width / 8, rect.m_height / 8);
    if(m_canvas->GetSelectedPiece() == this) {
        glEnable(GL_LINE_SMOOTH);
        glColor4ub(uint8_t(209), uint8_t(153), uint8_t(56), uint8_t(127));
        glBegin(GL_QUADS);
        glVertex2f(drawRect.GetLeft(), drawRect.GetTop());
        glVertex2f(drawRect.GetRight(), drawRect.GetTop());
        glVertex2f(drawRect.GetRight(), drawRect.GetBottom());
        glVertex2f(drawRect.GetLeft(), drawRect.GetBottom());
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }

    if(m_drawPoint) {
        wxPoint borderPoint = m_canvas->ToBoardPoint(m_canvas->GetMousePoint() - point);
        wxRect2DDouble borderRect((double)borderPoint.x * rect.m_width / 8, (double)borderPoint.y * rect.m_height / 8, rect.m_width / 8, rect.m_height / 8);
        glLineWidth(2.f);
        glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(0xAA));
        glBegin(GL_LINE_LOOP);
        glVertex2f(borderRect.GetLeft(), borderRect.GetTop());
        glVertex2f(borderRect.GetRight(), borderRect.GetTop());
        glVertex2f(borderRect.GetRight(), borderRect.GetBottom());
        glVertex2f(borderRect.GetLeft(), borderRect.GetBottom());
        glEnd();
        point = *m_drawPoint;
        drawRect = wxRect2DDouble((double)point.x, (double)point.y, rect.m_width / 8, rect.m_height / 8);
    }

    glEnable(GL_TEXTURE_2D);
    m_texture->Bind();
    glColor4ub(uint8_t(255), uint8_t(255), uint8_t(255), uint8_t(255));
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(drawRect.GetLeft(), drawRect.GetTop());
    glTexCoord2f(1.f, 0.f); glVertex2f(drawRect.GetRight(), drawRect.GetTop());
    glTexCoord2f(1.f, 1.f); glVertex2f(drawRect.GetRight(), drawRect.GetBottom());
    glTexCoord2f(0.f, 1.f); glVertex2f(drawRect.GetLeft(), drawRect.GetBottom());
    glEnd();
    glDisable(GL_TEXTURE_2D);

    if(m_hovered || m_drawPoint)
        m_texture->DrawPoints(drawRect);
}

void Piece::DrawMovements(const wxRect2DDouble& rect)
{
    auto DrawCircle = [](const wxPoint2DDouble& center, float radiusX, float radiusY, uint8_t alpha, float width) {
        if(width > 0) {
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(width);
            glColor4ub(0, 0, 0, alpha);
            glBegin(GL_LINE_LOOP);
        } else {
            glColor4ub(0, 0, 0, alpha);
            glBegin(GL_POLYGON);

            glVertex2f(center.m_x, center.m_y);
        }

        for(int i = 0; i <= CIRCLE_SEGMENTS; ++i) {
            float angle = 2.0 * 3.1415926 * float(i) / float(CIRCLE_SEGMENTS);
            float x = radiusX * cosf(angle);
            float y = radiusY * sinf(angle);
            glVertex2f(center.m_x + x, center.m_y + y);
        }

        glEnd();

        if(width > 0)
            glDisable(GL_LINE_SMOOTH);
    };

    for(PieceMovement* movement : m_movements) {
        Tile* tile = movement->GetTile();
        if(!tile || movement->GetType() == PieceMovement::IGNORE_MOVE)
            continue;

        wxPoint point = tile->GetPoint();
        wxRect2DDouble drawRect(point.x * rect.m_width / 8, point.y * rect.m_height / 8, rect.m_width / 8, rect.m_height / 8);

        wxPoint2DDouble center = drawRect.GetCentre();

        Piece* target = movement->GetTarget();

        float radiusX = drawRect.m_width;
        float radiusY = drawRect.m_height;
        if(!target || movement->GetType() == PieceMovement::SMALL_CASTLING_MOVE || movement->GetType() == PieceMovement::BIG_CASTLING_MOVE || movement->GetType() == PieceMovement::ENPASSANT_MOVE)
            DrawCircle(center, radiusX / 6.f, radiusY / 6.f, 0x1A, 0);

        if(target && movement->GetType() != PieceMovement::ENPASSANT_MOVE) {
            wxPoint point = target->GetPoint();
            wxRect2DDouble drawRect(point.x * rect.m_width / 8, point.y * rect.m_height / 8, rect.m_width / 8, rect.m_height / 8);

            wxPoint2DDouble targetCenter = drawRect.GetCentre();

            float width = 7.f;
            DrawCircle(targetCenter, radiusX / 2.f - width / 2.f, radiusY / 2.f - width / 2.f, 0x1A, width);
        }
    }
}

void Piece::SetDrawPoint(wxPoint drawPoint)
{   
    if(!m_drawPoint)
        m_drawPoint = new wxPoint(drawPoint);
    else
        *m_drawPoint = drawPoint;
}

void Piece::ClearDrawPoint()
{
    if(m_drawPoint) {
        delete m_drawPoint;
        m_drawPoint = nullptr;
    }
}

wxPoint Piece::GetPoint() const
{
    return m_tile->GetPoint();
}

bool Piece::CalculateMovements()
{
    if(m_calculed)
        return false;

    Piece* attacker = m_player->GetOpponent()->GetAttacker();
    const auto& targetTiles = m_player->GetOpponent()->GetTargetTiles();

    static const int DIRECTIONS[][2] = {
        { 1, 1 },
        { 1, -1 },
        { -1, -1 },
        { -1, 1 },

        { 1, 0 },
        { 0, 1 },
        { -1, 0 },
        { 0, -1 }
    };

    int tileCount = 8, startDir = 0, endDir = 8;
    if(GetType() == KING)
        tileCount = 1;
    else if(GetType() == ROOK)
        startDir = 4;
    else if(GetType() == BISHOP)
        endDir = 4;

    bool globalCheck = false;
    for(int i = startDir; i < endDir; ++i) {
        std::unordered_set<Tile*> tiles;
        Piece* target = nullptr;
        uint8_t movementType = PieceMovement::DEFAULT_MOVE;
        int kingDefender = 0;

        const int* direction = DIRECTIONS[i];
        wxPoint point = GetPoint();
        bool check = false;
        for(int i = 1; i <= tileCount; ++i) {
            wxPoint tilePoint(point.x + i * direction[0], point.y + i * direction[1]);

            Tile* tile = m_canvas->GetTile(tilePoint);
            PieceMovement* movement = PieceMovement::CreateGeneric(this, tile, movementType);
            if(!movement)
                break;

            if(!m_kingDefender.empty()) { // Only allow moves that block attacks on the king
                auto it = m_kingDefender.find(tile);
                if(it == m_kingDefender.end()) {
                    delete movement;
                    continue;
                }
            }

            if(!CheckMovement(movement, attacker, targetTiles)) {
                delete movement;
                continue;
            }

            if(target && !check) {
                tiles.insert(tile);
                Piece* targetPiece = tile->GetPiece();
                if(kingDefender == 0 && targetPiece) {
                    if(targetPiece != movement->GetTarget())
                        kingDefender = -1;
                    else if(targetPiece->GetType() != KING)
                        kingDefender = -1;
                    else
                        kingDefender = 1;
                }
                delete movement;
            }
            else {
                m_movements.push_back(movement);
                if(!target) {
                    Piece* targetPiece = movement->GetTarget();
                    if(targetPiece) {
                        target = targetPiece;
                        if(!check)
                            check = target->GetType() == KING;
                        if(!check)
                            tiles.insert(tile);
                    }
                    else if(!check)
                        tiles.insert(tile);
                }
            }
        }

        if(kingDefender == 1)
            target->m_kingDefender = tiles;
        if(check) {
            m_player->SetAttacker(this, tiles);
            check = false;
            globalCheck = true;
        }
    }

    m_calculed = true;
    return globalCheck;
}

bool Piece::CheckMovement(PieceMovement* movement, Piece* attacker, const std::unordered_set<Tile*>& targetTiles)
{
    if(attacker) {
        Piece* targetPiece = movement->GetTarget();
        if(GetType() == KING) {
            if(targetPiece && attacker != targetPiece)
                return false;
        }
        else if(!targetPiece) {
            auto it = targetTiles.find(movement->GetTile());
            if(it == targetTiles.end())
                return false;;
        }
        else if(targetPiece != attacker)
            return false;
    }
    else if(m_player->IsCheck() && GetType() != KING)
        return false;

    if(movement->GetType() != PieceMovement::IGNORE_MOVE)
        m_player->AddMovement();
    return true;
}

void Piece::ClearMovements()
{
    for(auto& movements : m_movements)
        delete movements;
    m_movements.clear();
    m_kingDefender.clear();
    m_calculed = false;
}

bool Piece::Move(PieceMovement* pieceMovement)
{
    wxPoint oldPoint = GetPoint();

    Tile* tile = pieceMovement->GetTile();
    wxPoint newPoint = tile->GetPoint();

    wxPoint deltaPoint = oldPoint - newPoint;
    int distance = std::sqrt(std::pow(deltaPoint.x, 2) + std::pow(deltaPoint.y, 2));

    Pawn* pawn = dynamic_cast<Pawn*>(this);
    if(pawn && ((GetColor() == WHITE && newPoint.y == 0) || (GetColor() == BLACK && newPoint.y == 7))) {
        m_canvas->Promote(pawn, pieceMovement, newPoint);
        return false;
    }

    m_lastMovementCount = distance;

    Piece* target = pieceMovement->GetTarget();
    if(target) {
        uint8_t castling = pieceMovement->GetType();
        if(castling == PieceMovement::SMALL_CASTLING_MOVE || castling == PieceMovement::BIG_CASTLING_MOVE) {
            if(castling == PieceMovement::SMALL_CASTLING_MOVE)
                newPoint.x -= 1;
            else if(castling == PieceMovement::BIG_CASTLING_MOVE)
                newPoint.x += 1;
            Tile* rookTile = m_canvas->GetTile(newPoint);
            PieceMovement* rookMovement = PieceMovement::CreateCastlingMovement(target, rookTile, nullptr, castling);
            assert(rookMovement);
            target->Move(rookMovement);
            delete rookMovement;
        }
        else {
            Player* otherPlayer = target->GetPlayer();
            otherPlayer->RemovePiece(target);

            m_player->CapturePiece(target);
            target->OnRemove();
        }
    }

    bool cleanEnpassant = true;
    if(pawn) {
        if(m_lastMovementCount == 2) {
            for(int x : { -1, 1 }) {
                wxPoint attackPoint(newPoint.x + x, newPoint.y);

                Tile* attackTile = m_canvas->GetTile(attackPoint);
                if(attackTile) {
                    Pawn* adjascentPawn = dynamic_cast<Pawn*>(attackTile->GetPiece());
                    if(adjascentPawn) {
                        adjascentPawn->SetEnpassant(pawn, newPoint.x);
                        cleanEnpassant = false;
                    }
                }
            }
        }
    }

    if(cleanEnpassant) {
        for(Piece* piece : m_player->GetPieces()) {
            if(Pawn* pawn = dynamic_cast<Pawn*>(piece))
                pawn->SetEnpassant(nullptr, 0);
        }
    }

    tile->SetPiece(this);

    m_movementCount++;
    return true;
}

void Piece::OnRemove()
{
    if(m_tile) {
        m_tile->SetPiece(nullptr);
        m_tile = nullptr;
    }

    ClearMovements();
    m_canvas->RemovePiece(this);
}

void Piece::OnHoverChange(bool hover)
{
    m_hovered = hover;
}

bool Pawn::CalculateMovements()
{
    if(m_calculed)
        return false;

    int tiles = 1;
    if(m_movementCount == 0)
        tiles++;

    Piece* attacker = m_player->GetOpponent()->GetAttacker();
    const auto& targetTiles = m_player->GetOpponent()->GetTargetTiles();

    int direction = -m_color;

    wxPoint point = GetPoint();
    bool check = false;
    for(int y = 1; y <= tiles; ++y) {
        wxPoint tilePoint(point.x, point.y + (y * direction));

        if(y == 1) {
            for(int x : { -1, 1 }) {
                wxPoint attackPoint(tilePoint.x + x, tilePoint.y);

                Tile* tile = m_canvas->GetTile(attackPoint);
                PieceMovement* movement = PieceMovement::CreatePawnAttack(this, tile);
                if(!movement)
                    continue;

                if(!m_kingDefender.empty()) {
                    auto it = m_kingDefender.find(tile);
                    if(it == m_kingDefender.end()) {
                        delete movement;
                        continue;
                    }
                }

                if(!CheckMovement(movement, attacker, targetTiles)) {
                    delete movement;
                    continue;
                }

                m_movements.push_back(movement);
                if(Piece* target = movement->GetTarget()) {
                    if(!check)
                        check = target->GetType() == KING;
                }
            }
        }

        Tile* tile = m_canvas->GetTile(tilePoint);
        PieceMovement* movement = PieceMovement::CreatePawnMovement(this, tile);
        if(!movement)
            break;

        if(!m_kingDefender.empty()) {
            auto it = m_kingDefender.find(tile);
            if(it == m_kingDefender.end()) {
                delete movement;
                continue;
            }
        }

        if(!CheckMovement(movement, attacker, targetTiles)) {
            delete movement;
            continue;
        }

        m_movements.push_back(movement);
    }

    m_calculed = true;
    return check;
}

std::string Rook::GetAlgebraicName() const
{
    return "R";
}

std::string Knight::GetAlgebraicName() const
{
    return "N";
}

bool Knight::CalculateMovements()
{
    if(m_calculed)
        return false;

    static const int knightMoves[][2] {
        { -1, -2 },
        { 1, -2 },
        { -2, -1 },
        { 2, -1 },
        { -1, 2 },
        { 1, 2 },
        { -2, 1 },
        { 2, 1 },
    };

    Piece* attacker = m_player->GetOpponent()->GetAttacker();
    const auto& targetTiles = m_player->GetOpponent()->GetTargetTiles();

    bool check = false;
    for(const int* direction : knightMoves) {
        wxPoint point = GetPoint();
        wxPoint tilePoint(point.x + direction[0], point.y + direction[1]);

        Tile* tile = m_canvas->GetTile(tilePoint);
        PieceMovement* movement = PieceMovement::CreateGeneric(this, tile);
        if(!movement)
            continue;

        if(!m_kingDefender.empty()) {
            auto it = m_kingDefender.find(tile);
            if(it == m_kingDefender.end()) {
                delete movement;
                continue;
            }
        }

        if(!CheckMovement(movement, attacker, targetTiles)) {
            delete movement;
            continue;
        }

        if(Piece* target = movement->GetTarget()) {
            if(!check)
                check = target->GetType() == KING;
        }

        m_movements.push_back(movement);
    }
    m_calculed = true;
    return check;
}

std::string Bishop::GetAlgebraicName() const
{
    return "B";
}

std::string Queen::GetAlgebraicName() const
{
    return "Q";
}

std::string King::GetAlgebraicName() const
{
    return "K";
}

bool King::CalculateMovements()
{
    if(m_calculed)
        return false;

    Piece::CalculateMovements();

    if(m_player->IsCheck() || m_movementCount != 0)
        return false;

    wxPoint point = GetPoint();
    int size = m_movements.size();
    for(int i = 0; i < size; ++i) {
        PieceMovement* movement = m_movements[i];
        Tile* tile = movement->GetTile();
        if(!tile)
            continue;

        wxPoint tilePoint = tile->GetPoint();
        if(tilePoint.y != point.y)
            continue;

        Piece* rook = nullptr;
        uint8_t castling = 0;
        if(tilePoint.x > point.x) {
            tilePoint.x += 1;
            Tile* rookTile = m_canvas->GetTile(wxPoint(tilePoint.x + 1, tilePoint.y));
            if(!rookTile)
                continue;
            rook = rookTile->GetPiece();
            castling = PieceMovement::SMALL_CASTLING_MOVE;
        }
        else if(tilePoint.x < point.x) {
            tilePoint.x -= 1;
            Tile* rookTile = m_canvas->GetTile(wxPoint(tilePoint.x - 2, tilePoint.y));
            if(!rookTile)
                continue;
            rook = rookTile->GetPiece();
            castling = PieceMovement::BIG_CASTLING_MOVE;
        }

        if(!rook || rook->GetType() != ROOK || rook->GetMovementCount() != 0 || rook->GetColor() != GetColor())
            continue;

        tile = m_canvas->GetTile(tilePoint);
        if(!tile)
            continue;

        PieceMovement* pieceMovement = PieceMovement::CreateCastlingMovement(this, tile, rook, castling);
        if(!pieceMovement)
            continue;
        m_movements.push_back(pieceMovement);
    }

    return false;
}
