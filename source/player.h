#ifndef PLAYER_H
#define PLAYER_H

class Piece;
class Tile;

class Player {
public:
	Player() : m_opponent(nullptr) {}

	Player* GetOpponent() const { return m_opponent; }
	void SetOpponent(Player* opponent) { m_opponent = opponent; }

	const std::vector<Piece*>& GetPieces() const { return m_pieces; }
	void AddPiece(Piece* piece);
	void RemovePiece(Piece* piece);

	const std::vector<Piece*>& GetCapturedPieces() const { return m_capturedPieces; }
	void CapturePiece(Piece* piece);
	void RemoveCapturedPiece(Piece* piece);

	/// <summary>
	/// Piece that is threatening the king with check.
	/// </summary>
	/// <returns>
	/// Return null if double check
	/// </returns>
	Piece* GetAttacker() const { return m_attacker; }
	/// <summary>
	/// Tile before the one with the threatened king
	/// </summary>
	/// <returns>
	/// Return null if double check
	/// </returns>
	const std::unordered_set<Tile*>& GetTargetTiles() const { return m_targetTiles; }

	void SetAttacker(Piece* piece, const std::unordered_set<Tile*>& targetTiles);
	void ResetAttacker();

	bool IsCheck() const { return m_check; }
	void Check() { m_check = true; }
	void UnCheck() { m_check = false; }

	int GetMovementCount() const { return m_movementCount; }
	void AddMovement() { m_movementCount++; }
	void ClearMovementCount() { m_movementCount = 0; }

private:
	std::vector<Piece*> m_pieces;
	std::vector<Piece*> m_capturedPieces;
	std::unordered_set<Tile*> m_targetTiles;

	Player* m_opponent = nullptr;
	Piece* m_attacker = nullptr;

	int m_attackerCount = 0;
	int m_movementCount = 0;

	bool m_check = false;
};

#endif
