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
	, height(height)
	, width(width)
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

bool Board::SwapContents(int lhs, int rhs)
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

	Cell& a = cells[lhs];
	Cell& b = cells[rhs];

	// Check if cells are neighbors
	if ((a.x == b.x && abs(a.y - b.y) == 1) ||
		(a.y == b.y && abs(a.x - b.x) == 1))
	{
		if (a.contents == b.contents)
			return false;

		a.contents = exchange(b.contents, a.contents);

		return true;
	}

	return false;
}

bool Board::GetMatches(int index, Indexes* matches) const
{
	assert(is_valid(index, capacity));
	assert(!cells[index].IsEmpty());

	const Cell& cell = cells[index];
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
			while (cell.contents == cells[idx].contents)
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

bool Board::GetHints(Moves* hints)
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

				if (!SwapContents(index, cell.index))
					continue;

				if (HasMatches(cell.index) || HasMatches(index))
				{
					if (hints)
					{
						hints->insert({ cell.index, index });
					}
					else
					{
						SwapContents(index, cell.index);
						return true;
					}
				}

				SwapContents(index, cell.index);
			}
		}
	}

	return false;
}

const EContentsType Board::GetRandomContents()
{
	return static_cast<EContentsType>(distribution(generator));
}

string Board::ToString()
{
	string result;
	int carriagew = 0;
	int carriageh = 0;

	result += "   ";

	for (int i = 0; i < width; ++i)
		result += " " + to_string(i) + " ";

	result += "\n 0 ";

	for (const auto& cell : cells)
	{
		int content = static_cast<int>(cell.contents);
		result += u8"\033[" + to_string(content + 40) + "m " + to_string(content) + " \033[49m";

		if (++carriagew == width)
		{
			carriagew = 0;
			if (++carriageh < height)
				result += "\n " + to_string(carriageh) + " ";
		}
	}

	return result;
}
