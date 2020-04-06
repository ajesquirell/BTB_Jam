//Define Statements for using PGE + Extensions with an OOP structure
// and main()

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"

#define OLC_PGE_GRAPHICS2D
#include "olcPGEX_Graphics2D.h"


#include "Jerry_Engine.h"
int main()
{
	Platformer game;
	if (game.Construct(330, 308, 3, 3))
		game.Start();

	return 0;
}