#include "main.h"

#include "piecemovement.h"
#include "tile.h"
#include "piece.h"
#include "game.h"

PieceMovement* PieceMovement::CreateGeneric(Piece* piece, Tile* tile, uint8_t type)
{
    if(!tile)
        return nullptr;

    if(piece->GetType() == KING && tile->GetThreats(piece->GetColor()) > 0)
        return nullptr;

    PieceMovement* pieceMovement = new PieceMovement;
    pieceMovement->SetColor(-piece->GetColor());
    pieceMovement->SetType(type);
    pieceMovement->SetTile(tile);

    Piece* tilePiece = tile->GetPiece();
    if(tilePiece) {
        if(tilePiece->GetColor() != piece->GetColor())
            pieceMovement->SetTarget(tilePiece);
        else {
            delete pieceMovement;
            return nullptr;
        }
    }
    return pieceMovement;
}

PieceMovement* PieceMovement::CreatePawnAttack(Piece* piece, Tile* tile)
{
    if(!tile)
        return nullptr;

    PieceMovement* pieceMovement = new PieceMovement;
    pieceMovement->SetColor(-piece->GetColor());
    pieceMovement->SetTile(tile);

    Piece* tilePiece = tile->GetPiece();
    if(!tilePiece) {
        Pawn* pawn = dynamic_cast<Pawn*>(piece);
        if(tile->GetPoint().x == pawn->GetEnpassantX()) {
            tilePiece = pawn->GetEnpassant();
            pieceMovement->SetType(ENPASSANT_MOVE);
        }
    }
    else if(tilePiece->GetColor() == piece->GetColor())
        tilePiece = nullptr;

    if(tilePiece)
        pieceMovement->SetTarget(tilePiece);
    else
        pieceMovement->SetType(IGNORE_MOVE);
    return pieceMovement;
}

PieceMovement* PieceMovement::CreatePawnMovement(Piece* piece, Tile* tile)
{
    if(!tile)
        return nullptr;

    Piece* tilePiece = tile->GetPiece();
    if(tilePiece)
        return nullptr;
    
    PieceMovement* pieceMovement = new PieceMovement;
    pieceMovement->SetColor(-piece->GetColor());
    pieceMovement->m_ignoreThreat = true;
    pieceMovement->SetTile(tile);
    return pieceMovement;
}

PieceMovement* PieceMovement::CreateCastlingMovement(Piece* piece, Tile* tile, Piece* rook, uint8_t type)
{
    if(!tile)
        return nullptr;

    Piece* tilePiece = tile->GetPiece();
    if(tilePiece)
        return nullptr;

    PieceMovement* pieceMovement = new PieceMovement;
    pieceMovement->SetType(type);
    pieceMovement->SetTarget(rook);
    pieceMovement->m_ignoreThreat = true;
    pieceMovement->SetTile(tile);
    return pieceMovement;
}

PieceMovement::~PieceMovement()
{
    SetTile(nullptr);
}

void PieceMovement::SetTile(Tile* tile)
{
    if(m_tile == tile)
        return;

    Tile* oldTile = m_tile;
    m_tile = tile;

    if(m_ignoreThreat)
        return;

    if(oldTile)
        oldTile->RemoveThreat(m_color);
    if(tile)
        tile->AddThreat(m_color);
}
