#include "Renderer.h"

#include <iostream>

using namespace std;
using namespace Match3;

Renderer::Renderer(const Board& board)
	: board(board)
	, width(board.GetWidth())
	, height(board.GetHeight())
{}

void Renderer::RenderBoard(const char* title) const
{
	string result;
	int carriagew = 0;
	int carriageh = 0;

	result += "   ";

	for (int i = 0; i < width; ++i)
		result += " " + to_string(i) + " ";

	result += "\n 0 ";

	const auto cells = board.GetCells();

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

	cout << title << ":\n\n" << result << "\n\n";
}

void Renderer::RenderHints()
{
	hints.clear();

	board.GetHints(&hints);

	RenderMoves("Hints", hints);
}

void Renderer::RenderMatches(const char* title, const Indexes& matches)
{
	cout << title << ":\n\t";

	int carriage = 0;

	for (auto& index : matches)
	{
		cout << "[" << index % width << ", " << index / width << "] ";

		if (++carriage == 6)
		{
			carriage = 0;
			cout << "\n\t";
		}
	}

	cout << "\n\n";
}

void Renderer::RenderMoves(const char* title, const Moves& moves)
{
	cout << title << ":\n";

	int carriage = 0;

	for (const auto& move : moves)
	{
		const int idx1 = move.GetFirstIndex();
		const int idx2 = move.GetSecondIndex();

		cout << "\t[" << idx1 % width << ", " << idx1 / width << "], "
			<< "[" << idx2 % width << ", " << idx2 / width << "], ";

		if (++carriage == 3)
		{
			carriage = 0;
			cout << "\n";
		}
	}

	cout << "\n\n";
}

void Renderer::RenderWrongMove(int idx1, int idx2)
{
	cout << "Wrong move: " << idx1 << " " << idx2 << "\n\n";
}
