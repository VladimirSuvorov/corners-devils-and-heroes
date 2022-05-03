#include "BoardVisualGrid.hpp"

class TiledBoardVisualGrid :
	public BoardVisualGrid
{
public:
	TiledBoardVisualGrid(AABB initial_tile)noexcept :
		initial_tile_(initial_tile)
	{}
	AABB get_tile(board_cell cell)const noexcept final {
		return  {
			initial_tile_.width * cell.x() + initial_tile_.left,
			initial_tile_.top - cell.y() * initial_tile_.height,
			initial_tile_.width,
			initial_tile_.height
		};
	}
private:
	AABB initial_tile_;
};


