#pragma once

#include <random>
#include <string>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Match3
{
	enum class EContentsType
	{
		Empty,

		// Gems
		GemRed,
		GemGreen,
		GemYellow,
		GemBlue,
		GemPurple,

		GemsCount
	};

	struct Cell
	{
		EContentsType contents = EContentsType::Empty;

		int index = -1;
		int x = -1;
		int y = -1;

		Cell(int index, int x, int y, EContentsType contents)
			: contents(contents)
			, index(index)
			, x(x)
			, y(y)
		{}

		bool IsEmpty() const
		{
			return contents == EContentsType::Empty;
		}
	};

	class Move
	{
	public:
		Move(int idx1, int idx2)
		{
			idxs[0] = idx1;
			idxs[1] = idx2;

			if (idx1 > idx2)
				std::swap(idxs[0], idxs[1]);
		}

	public:
		constexpr bool operator==(const Move& rhs) const
		{
			return idxs[0] == rhs.idxs[0] && idxs[1] == rhs.idxs[1];
		}

		struct HashFunction
		{
			size_t operator()(const Move& move) const
			{
				size_t xHash = std::hash<int>()(move.GetFirstIndex());
				size_t yHash = std::hash<int>()(move.GetSecondIndex()) << 1;
				return xHash ^ yHash;
			}
		};

		constexpr int GetFirstIndex() const { return idxs[0]; }
		constexpr int GetSecondIndex() const { return idxs[1]; }

	private:
		int idxs[2];
	};

	using Indexes = std::set<int>;
	using Moves = std::unordered_set<Move, Move::HashFunction>;
	using Cells = std::vector<Cell>;

	class Board final
	{
	public:
		Board() = delete;
		Board(int width, int height, bool generate);
		Board(int width, int height, const std::vector<int>& contents);

	public:
		void Shuffle();

		bool GetMatches(int index, Indexes* matches) const;
		bool HasMatches(int index) const { return GetMatches(index, nullptr); };
		void ClearMatches(const Indexes& matches);

		void FillBlanks(const Indexes& affected);
		Indexes Scroll(const Indexes& cleared);

		bool GetHints(Moves* hints);
		bool HasMoves() { return GetHints(nullptr); }

		bool SwapContents(int lhs, int rhs);

		const Cells& GetCells() const { return cells; }

		int GetWidth() const { return width; }
		int GetHeight() const { return height; }

	private:
		void Clear();
		void Generate();

		const EContentsType GetRandomContents();

	private:
		int capacity = 0;
		int width = 0;
		int height = 0;

		Cells cells;

		std::random_device device;
		std::mt19937 generator{ device() };
		std::uniform_int_distribution<int> distribution{
			1, // EContentsType::GemRed
			static_cast<int>(EContentsType::GemsCount) - 1
		};
	};
}
