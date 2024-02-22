#include "main.h"

#include "player.h"

void Player::AddPiece(Piece* piece)
{
	m_pieces.push_back(piece);
}

void Player::RemovePiece(Piece* piece)
{
	auto it = std::find(m_pieces.begin(), m_pieces.end(), piece);
	if(it == m_pieces.end())
		return;
	m_pieces.erase(it);
}

void Player::CapturePiece(Piece* piece)
{
	m_capturedPieces.push_back(piece);
}

void Player::RemoveCapturedPiece(Piece* piece)
{
	auto it = std::find(m_capturedPieces.begin(), m_capturedPieces.end(), piece);
	if(it == m_capturedPieces.end())
		return;
	m_capturedPieces.erase(it);
}

void Player::SetAttacker(Piece* piece, const std::unordered_set<Tile*>& targetTiles)
{
	if(m_attacker) {
		assert(m_attackerCount != 0);
		m_attacker = nullptr;
		m_targetTiles.clear();
	}
	else {
		m_attacker = piece;
		m_targetTiles = targetTiles;
	}
	++m_attackerCount;
}

void Player::ResetAttacker()
{
	m_attackerCount = 0;
	m_attacker = nullptr;
	m_targetTiles.clear();
}
