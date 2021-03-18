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

	int attempts = 100;
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

void Board::SwapContents(int lhs, int rhs)
{
	assert(is_valid(lhs, capacity));
	if (!is_valid(lhs, capacity))
		return;

	assert(is_valid(rhs, capacity));
	if (!is_valid(rhs, capacity))
		return;

	assert(lhs != rhs);
	if (lhs == rhs)
		return;

	Cell& a = cells[lhs];
	Cell& b = cells[rhs];

	if (a.contents == b.contents)
		return;

	a.contents = exchange(b.contents, a.contents);
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

bool Board::GetHint(Move* move)
{
	// TODO: нужен комментарий почему брутфорс

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

				SwapContents(index, cell.index);

				if (HasMatches(cell.index) || HasMatches(index))
				{
					if (move)
					{
						move->first = cell.index;
						move->second = index;
					}

					SwapContents(index, cell.index);
					return true;
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
	int carriage = 0;

	for (const auto& cell : cells)
	{
		result += to_string(static_cast<int>(cell.contents)) + ", ";

		if (++carriage == width)
		{
			carriage = 0;
			result += "\n";
		}
	}

	return result;
}
