#include "Board.h"

#include <algorithm>
#include <cassert>
#include <utility>

#define assertm(exp, msg) assert(((void)msg, exp))

using namespace std;
using namespace Match3;

namespace
{
	constexpr int directions[2][2][2] = { { { -1, 0 }, { 1, 0 } }, { { 0, -1 }, { 0, 1 } } };

	inline constexpr int make_index(int x, int y, int width)
	{
		return y * width + x;
	}

	inline constexpr bool is_valid(int index, int capacity)
	{
		return index >= 0 && index < capacity;
	}

	inline constexpr bool is_valid(int x, int y, int width, int height)
	{
		return x >= 0 && y >= 0 && x < width&& y < height;
	}

	constexpr int kMinMatches = 3;
	constexpr int kShuffleAttempts = 100;
}

Board::Board(int width, int height, bool generate)
	: capacity(width* height)
	, width(width)
	, height(height)
{
	assert(width > 2 && height > 2);

	cells.reserve(capacity);

	for (int i = 0; i < capacity; ++i)
	{
		int x = i % width;
		int y = i / width;

		cells.push_back(Cell(i, x, y, EContentsType::Empty));
	}

	if (!generate)
		return;

	Shuffle();
}

Board::Board(int width, int height, const vector<int>& contents)
	: Board(width, height, false)
{
	assert(static_cast<int>(contents.size()) == capacity);

	for (int i = 0; i < capacity; ++i)
		cells[i].contents = static_cast<EContentsType>(contents[i]);
}

void Board::Clear()
{
	for (auto& cell : cells)
		cell.contents = EContentsType::Empty;
}

void Board::Shuffle()
{
	Generate();

	int attempts = kShuffleAttempts;
	while (!HasMoves() && attempts--)
		Generate();
	assertm(attempts, "An infinite loop while the board generation");
}

void Board::Generate()
{
	Clear();

	for (auto& cell : cells)
	{
		cell.contents = GetRandomContents();

		while (HasMatches(cell.index))
			cell.contents = GetRandomContents();
	}
}

bool Board::CanSwapContents(int lhs, int rhs) const
{
	assert(is_valid(lhs, capacity));
	if (!is_valid(lhs, capacity))
		return false;

	assert(is_valid(rhs, capacity));
	if (!is_valid(rhs, capacity))
		return false;

	assert(lhs != rhs);
	if (lhs == rhs)
		return false;

	const Cell& a = cells[lhs];
	const Cell& b = cells[rhs];

	// Check if cells are neighbors
	if ((a.x == b.x && abs(a.y - b.y) == 1) ||
		(a.y == b.y && abs(a.x - b.x) == 1))
	{
		return !(a.contents == b.contents);
	}

	return false;
}

bool Board::SwapContents(int lhs, int rhs)
{
	if (CanSwapContents(lhs, rhs))
	{
		Cell& a = cells[lhs];
		Cell& b = cells[rhs];

		a.contents = exchange(b.contents, a.contents);

		return true;
	}

	return false;
}

bool Board::GetMatches(int index, Indexes* matches) const
{
	return GetMatches(index, -1, matches);
}

bool Board::GetMatches(int lhs, int rhs, Indexes* matches) const
{
	assert(is_valid(lhs, capacity));
	assert(!cells[lhs].IsEmpty());

	const Cell& cell = cells[lhs];
	EContentsType contents = cell.contents;

	if (is_valid(rhs, capacity))
	{
		assert(!cells[rhs].IsEmpty());
		contents = cells[rhs].contents;
	}

	bool result = false;

	Indexes ties;
	for (const auto& direction : directions)
	{
		int tie = 1;
		for (const auto [i, j] : direction)
		{
			int r = 1; // a radius

			int x = cell.x + i;
			int y = cell.y + j;

			if (!is_valid(x, y, width, height))
				continue;

			int idx = make_index(x, y, width);
			while (contents == (idx == rhs ? cell.contents : cells[idx].contents))
			{
				ties.insert(idx);
				tie++; r++;

				x = cell.x + i * r;
				y = cell.y + j * r;

				if (!is_valid(x, y, width, height))
					break;

				idx = make_index(x, y, width);
			}
		}

		if (!matches && tie >= kMinMatches)
			return true;

		if (tie < kMinMatches)
			ties.clear();

		if (matches && ties.size() > 0)
		{
			matches->merge(ties);
			result = true;
		}
	}

	if (matches && result)
	{
		matches->insert(cell.index);
		return true;
	}

	return false;
}

bool Board::HasMatches(int index) const
{
	return GetMatches(index, nullptr);
}

bool Board::HasMatches(int lhs, int rhs) const
{
	return GetMatches(lhs, rhs, nullptr) || GetMatches(rhs, lhs, nullptr);
}

void Board::ClearMatches(const Indexes& matches)
{
	for (auto index : matches)
	{
		if (!is_valid(index, capacity))
			continue;

		cells[index].contents = EContentsType::Empty;
	}
}

void Board::FillBlanks(const Indexes& affected)
{
	for (auto index : affected)
	{
		if (!is_valid(index, capacity))
			continue;

		if (cells[index].IsEmpty())
			cells[index].contents = GetRandomContents();
	}
}

Indexes Board::Scroll(const Indexes& cleared)
{
	Indexes affected;

	for (auto index : cleared)
	{
		if (!is_valid(index, capacity))
			continue;

		int pidx = index; // previous index
		int nidx = index; // next index

		affected.insert(nidx);
		while (cells[nidx].y > 0)
		{
			int x = cells[nidx].x;
			int y = cells[nidx].y - 1;

			if (!is_valid(x, y, width, height))
				break;

			nidx = make_index(x, y, width);
			SwapContents(pidx, nidx);

			affected.insert(nidx);
			pidx = nidx;
		}
	}

	return affected;
}

bool Board::GetHints(Moves* hints) const
{
	// TODO: Explain the brute-force

	for (const auto& cell : cells)
	{
		for (const auto& direction : directions)
		{
			for (const auto [i, j] : direction)
			{
				int x = cell.x + i;
				int y = cell.y + j;

				if (!is_valid(x, y, width, height))
					continue;

				int index = make_index(x, y, width);

				if (cell.contents == cells[index].contents)
					continue;

				if (!CanSwapContents(cell.index, index))
					continue;

				if (HasMatches(cell.index, index))
				{
					if (hints)
					{
						hints->insert({ cell.index, index });
					}
					else
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Board::HasMoves() const
{
	return GetHints(nullptr);
}

const EContentsType Board::GetRandomContents()
{
	return static_cast<EContentsType>(distribution(generator));
}
