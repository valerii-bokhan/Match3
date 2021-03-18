#pragma once

#include <random>
#include <string>
#include <set>
#include <utility>
#include <vector>

namespace Match3
{
	using Indexes = std::set<int>;
	using Move = std::pair<int, int>;

	enum class EContentsType
	{
		Empty,

		// Gems
		GemYellow,
		GemBlue,
		GemGreen,
		GemPurple,
		GemRed,

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

	class Board final
	{
	public:
		Board(int width, int height, bool generate);
		Board(int width, int height, const std::vector<int>& contents);

	public:
		void Shuffle();

		bool GetMatches(int index, Indexes* matches) const;
		bool HasMatches(int index) const { return GetMatches(index, nullptr); };
		void ClearMatches(const Indexes& matches);

		void FillBlanks(const Indexes& affected);
		Indexes Scroll(const Indexes& cleared);

		bool GetHint(Move* move);
		bool HasMoves() { return GetHint(nullptr); }

		void SwapContents(int lhs, int rhs);

		std::string ToString();

	private:
		void Clear();
		void Generate();

		const EContentsType GetRandomContents();

	private:
		int capacity = 0;
		int width = 0;
		int height = 0;

		std::vector<Cell> cells;

		std::random_device device;
		std::mt19937 generator{ device() };
		std::uniform_int_distribution<int> distribution{
			1, // EContentsType::GemYellow
			static_cast<int>(EContentsType::GemsCount) - 1
		};
	};
}
