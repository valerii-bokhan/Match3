#include "Game.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace Match3;

namespace
{
	inline void print_matches(const char* title, const Indexes& matches, int width)
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

	inline void print_moves(const char* title, const Moves& moves, int width)
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

	inline void print_wrong_move(int idx1, int idx2)
	{
		cout << "Wrong move: " << idx1 << " " << idx2 << "\n\n";
	}
}

Game::Game(const Config& config)
	: board(Board(config.width, config.height, true))
	, config(config)
{}

bool Game::MakeMove(int idx1, int idx2)
{
	matches.clear();

	if (!board.SwapContents(idx1, idx2))
	{
		print_wrong_move(idx1, idx2);
		return false;
	}

	board.GetMatches(idx1, &matches);
	board.GetMatches(idx2, &matches);

	if (!matches.size())
	{
		board.SwapContents(idx1, idx2);

		print_wrong_move(idx1, idx2);
		return false;
	}

	print_matches("Matches", matches, config.width);

	return true;
}

void Game::ProcessMove()
{
	assert(matches.size());
	if (!matches.size())
		return;

	UpdateBoard();

	matches.clear();
	for (auto index : affected)
		board.GetMatches(index, &matches);

	while (matches.size())
	{
		print_matches("EXTRA Matches", matches, config.width);

		UpdateBoard();

		matches.clear();
		for (auto index : affected)
			board.GetMatches(index, &matches);
	}

	if (!board.HasMoves())
	{
		cout << "NO MORE MOVES: Shuffling\n\n";
		board.Shuffle();
	}
}

void Game::UpdateBoard()
{
	assert(matches.size());
	if (!matches.size())
		return;

	board.ClearMatches(matches);

	PrintBoard("Board (Clear Matches)");

	affected = board.Scroll(matches);

	print_matches("Affected", affected, config.width);

	PrintBoard("Board (Scroll)");

	board.FillBlanks(affected);

	PrintBoard("Board (Fill Blanks)");
}

void Game::PrintHints()
{
	hints.clear();

	board.GetHints(&hints);

	print_moves("Hints", hints, config.width);
}

void Game::PrintBoard(const char* title)
{
	cout << title << ":\n\n" << board.ToString() << "\n\n";
}
