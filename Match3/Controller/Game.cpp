#include "Game.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace Match3;

namespace
{
	inline constexpr int make_index(int x, int y, int width)
	{
		return y * width + x;
	}
}

Game::Game(const Config& config)
	: config(config)
	, board(Board(config.width, config.height, true))
	, renderer(Renderer(board))
{}

void Game::Run()
{
	renderer.RenderBoard("Board");
	renderer.RenderHints();

	while (true)
	{
		int x1, x2, y1, y2;
		cin >> x1 >> y1 >> x2 >> y2;
		cout << "\n";

		const int idx1 = make_index(x1, y1, config.width);
		const int idx2 = make_index(x2, y2, config.width);

		if (!MakeMove(idx1, idx2))
			continue;

		ProcessMove();

		renderer.RenderHints();
	}
}

bool Game::MakeMove(int idx1, int idx2)
{
	matches.clear();

	if (!board.SwapContents(idx1, idx2))
	{
		renderer.RenderWrongMove(idx1, idx2);
		return false;
	}

	board.GetMatches(idx1, &matches);
	board.GetMatches(idx2, &matches);

	if (!matches.size())
	{
		board.SwapContents(idx1, idx2);

		renderer.RenderWrongMove(idx1, idx2);
		return false;
	}

	renderer.RenderMatches("Matches", matches);

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
		renderer.RenderMatches("EXTRA Matches", matches);

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

	renderer.RenderBoard("Board (Clear Matches)");

	affected = board.Scroll(matches);

	renderer.RenderMatches("Affected", affected);

	renderer.RenderBoard("Board (Scroll)");

	board.FillBlanks(affected);

	renderer.RenderBoard("Board (Fill Blanks)");
}
