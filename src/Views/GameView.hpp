#pragma once
#include <vector>
#include "../Graphics/GraphicTypes.hpp"
class GameView {
public:
	virtual const std::vector<quad>* GetQuadsToDisplay()const = 0;
};