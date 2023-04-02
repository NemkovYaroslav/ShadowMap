#include <iostream>
#include "KatamariDamacyGame.h"
#include "Transform.h"
#include "SimpleMath.h"

int main()
{
	KatamariDamacyGame* game = new KatamariDamacyGame(L" Katamari Damacy ", 1200, 1200);
	game->Run();
	std::cout << "GitLFS" << std::endl;
	return EXIT_SUCCESS;
}