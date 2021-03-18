﻿#include "Game.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace Match3;

namespace
{
	inline void print_matches(const char* title, const Indexes& matches)
	{
		cout << title << ": ";

		for (auto& index : matches)
			cout << index << " ";

		cout << "\n\n";
	}
}

Game::Game(const Config& config)
	: board(Board(config.width, config.height, true))
{}

bool Game::MakeMove(int idx1, int idx2)
{
	matches.clear();

	board.SwapContents(idx1, idx2);

	board.GetMatches(idx1, &matches);
	board.GetMatches(idx2, &matches);

	if (!matches.size())
	{
		cout << "Wrong move: " << idx1 << " " << idx2 << "\n\n";

		board.SwapContents(idx1, idx2);

		return false;
	}

	print_matches("Matches", matches);

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
		print_matches("EXTRA Matches", matches);

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

	print_matches("Affected", affected);

	PrintBoard("Board (Scroll)");

	board.FillBlanks(affected);

	PrintBoard("Board (Fill Blanks)");
}

void Game::PrintHint()
{
	board.GetHint(&hint);

	cout << "Hint: " << hint.first << " " << hint.second << "\n\n";
}

void Game::PrintBoard(const char* title)
{
	cout << title << ":\n\n" << board.ToString() << "\n\n";
}