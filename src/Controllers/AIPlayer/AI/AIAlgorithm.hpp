#pragma once






//TODO: ALL Select*Pawns -> class PawnSelection with same methods as that free functions, to perform subselections



/*
// Core idea is:
//	1) Generate alternatives (for all possible moves).
//	2) Sort the alternatives by ability to approach to (at least) or reach (at best) one of the goals. For that purpose use A* pathfinding algorithm. 
//	3) "Feed" minimax with that alternatives
//
// Note: actually that is impossible to compare alternatives, so we compare possible actions AI can make (according to goals e.t.c.), 
//		sorting them respectively and then generating alternatives by them.
//
*/




/*
//first of all:
//1)select all my moveable pawns
//2)select all my pawns can reach any goal


//3)select all my pawns

//then
//if there is pawns can reach a goal - push them as alternatives to minimax;
//else push to minimax any moveable pawns (sorted by max available approach to the goal)
//		

//Pawn.possible_actions()->vector
//obstacles except tested pawn
//(Pawn + board_cell)->Action


	//first - satisfy pawns with less reachable goals
	//then - satisfy pawns with more reachable goals

	//sort from distant to close to closest goal


	//TODO: select neighbours that are at goal, whereas current pawn is not at goal
	//???TODO: ALL Select*Pawns -> class PawnSelection with same methods as that free functions, to perform subselections
*/