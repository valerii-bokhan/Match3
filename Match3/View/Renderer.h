#pragma once

#include <Model/Board.h>

namespace Match3
{
	class Renderer
	{
	public:
		Renderer() = delete;
		// TODO: make const reference!!!
		explicit Renderer(Board& board);

		void RenderHints();
		void RenderBoard(const char* title) const;

		void RenderMatches(const char* title, const Indexes& matches);
		void RenderMoves(const char* title, const Moves& moves);

		static void RenderWrongMove(int idx1, int idx2);

	private:
		Board& board;

		Moves hints;

		int width;
		int height;
	};
}
