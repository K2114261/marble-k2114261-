#include "stdafx.h"
#include "MyGame.h"

CMarbleGame::CMarbleGame(void) :
	back(CVector(400, 200), "back.bmp", 0),
	pane(CVector(700, 200), "pane.bmp", CColor::LightGray(), 0),
	theGoal(CVector(600, 600), "holegr.gif", 0),
	theMarble(CVector(0, 0), "marble.gif", 0),
	theArrow(CVector(700, 200), "arrow.bmp", CColor::LightGray(), 0)
{
	MousePoint = CVector(700, 200);
	MouseRadius = 80;
	theArrow.SetPivotLocal(0, 13);

	ax = 0;
	ay = 0;
	xstart = 0;
	ystart = 0;
}

CMarbleGame::~CMarbleGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMarbleGame::OnUpdate()
{
	// Some constant values:
	const float r = 12;			// radius of the marble
	const float R = 14;			// radius of the hole
	const float RR = 18;		// radius of the goal hole
	const float gain = 3.0f;	// gains the acceleration of the marble
	const float damp = 0.6f;	// damp factor for bouncing

	// Time variables:
	Uint32 t = GetTime();			// current time (in milliseconds)
	Uint32 dt = GetDeltaTime();	// time since the last frame (in milliseconds)

	// NOTE: Because dt is given in milliseconds, for the actual displacement vector due to velocity
	// use: GetVelocity() * dt / 1000
	

	// 1. TODO: apply acceleration to the marble
	// The acceleration is given as (gain * ax, gain * ay)
	theMarble.Accelerate(gain*ax, gain*ay);

	
	// 2. TODO: Iterate through all the walls (theWalls collection of sprites)
	// For each wall, check the collision of the marble with the upper, lower, right and left surface of the wall
	// In case of a collision, use reflection to change the velocity;
	// then use damp to slow down the speed after the bounce


	for (CSprite* pWall : theWalls)
	{
		const float X = pWall->GetWidth() / 2;
		const float Y = pWall->GetHeight() / 2;
		float alpha = DEG2RAD(pWall->GetRotation());

		CVector v = theMarble.GetVelocity() * t;
		CVector tp = pWall->GetPos() - theMarble.GetPos();
		CVector n = CVector(sin(alpha), cos(alpha));

		if (theMarble.HitTest(&*pWall))
		{

			if (Dot(v, n) < 0)
			{
				// Perpendicular component (oncoming)
				float vy = Dot(v, n);		// velocity component
				CVector d = tp + (Y + r) * -n;	// distance vector between edges
				float dy = Dot(d, n);		// perpendicular space between
				float f1 = dy / vy;

				// Parallel component (breadth control)
				float vx = Cross(v, n);		// velocity component
				float tx = Cross(tp, n);		// distance between centres
				float f2 = (tx - vx * f1) / (X + r);

				if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
					theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
			}

			if (Dot(v, n) > 0)
			{
				// Perpendicular component (oncoming)
				float vy = Dot(v, n);		// velocity component
				CVector d = tp + (Y + r) * n;	// distance vector between edges
				float dy = Dot(d, n);		// perpendicular space between
				float f1 = dy / vy;

				// Parallel component (breadth control)
				float vx = Cross(v, n);		// velocity component
				float tx = Cross(tp, n);		// distance between centres
				float f2 = (tx - vx * f1) / (X + r);

				if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
					theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
			}

			if (Cross(v, n) > 0)
			{
				CVector n = CVector(cos(alpha), -sin(alpha));

				// Perpendicular component (oncoming)
				float vy = Dot(v, n);		// velocity component
				CVector d = tp + (X + r) * n;	// distance vector between edges
				float dy = Dot(d, n);		// perpendicular space between
				float f1 = dy / vy;

				// Parallel component (breadth control)
				float vx = Cross(v, n);		// velocity component
				float tx = Cross(tp, n);		// distance between centres
				float f2 = (tx - vx * f1) / (Y + r);

				if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
					theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
			}

			if (Cross(v, n) < 0)
			{
				CVector n = CVector(-cos(alpha), sin(alpha));

				// Perpendicular component (oncoming)
				float vy = Dot(v, n);		// velocity component
				CVector d = tp + (X + r) * n;	// distance vector between edges
				float dy = Dot(d, n);		// perpendicular space between
				float f1 = dy / vy;

				// Parallel component (breadth control)
				float vx = Cross(v, n);		// velocity component
				float tx = Cross(tp, n);		// distance between centres
				float f2 = (tx - vx * f1) / (Y + r);

				if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
					theMarble.SetVelocity(Reflect(theMarble.GetVelocity(), n));
			}

		}
	}

	// 3. TODO: update the Marble
	theMarble.Update(t);

	for each (CSprite * pWall in theWalls)
		pWall->Update(t);

	// 4. TODO: check collisions with the holes
	// If theMarble is in a hole:
	// - set its position to the initial one (xstart, ystart),
	// - set acceleration to 0
	// - set velocity to 0



	// check collision with the goal hole
	if (theGoal.GetPosition().Distance(theMarble.GetPosition()) < RR)
		// go to the higher level
		NewLevel();
}

void CMarbleGame::OnDraw(CGraphics* g)
{
	back.Draw(g);
	pane.Draw(g);
	for each (CSprite *pWall in theWalls)
		pWall->Draw(g);
	for each (CSprite *pHole in theHoles)
		pHole->Draw(g);
	theGoal.Draw(g);
	theMarble.Draw(g);
	theArrow.Draw(g);

	if (IsGameOver())
		*g << font(24) << endl << "  CONGRATULATIONS!" << endl << "  YOU WON!!!";
	else
		*g << font(11) << bottom << right << strInfo2 << endl << strInfo1;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMarbleGame::OnInitialize()
{
	Gmusic.Play("music.wav", -1);

}

// called at the start of a new game - display menu here
void CMarbleGame::OnDisplayMenu()
{
	StartGame();
}

// called when Game Mode entered
void CMarbleGame::OnStartGame()
{
}

void CMarbleGame::OnStartLevel(Sint16 nLevel)
{
	// destroy the old playfield
	for each (CSprite *pWall in theWalls)
		delete pWall;
	theWalls.clear();
	for each (CSprite *pHole in theHoles)
		delete pHole;
	theHoles.clear();

	// build the walls around
	theWalls.push_back(new CSprite(CRectangle(0, 0, 600, 10), "wallhorz.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(0, 391, 600, 10), "wallhorz.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(0, 0, 10, 400), "wallvert.bmp", GetTime()));
	theWalls.push_back(new CSprite(CRectangle(591, 0, 10, 400), "wallvert.bmp", GetTime()));

	switch (nLevel)
	{
	case 0:
		// Menu Mode only
		break;
	case 1:
		strInfo1 = "LEVEL 1"; strInfo2 = "by Kennedy";

		// build the walls inside
		theWalls.push_back(new CSprite(CRectangle(  0,  95, 300, 10), "wallhorz.bmp", GetTime()));


		// dig the holes

		theHoles.push_back(new CSprite(CVector(150, 230), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(300, 250), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(450, 270), "hole.gif", GetTime()));

		theHoles.push_back(new CSprite(CVector(150, 130), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(300, 140), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(450, 170), "hole.gif", GetTime()));



		// set up the goal
		theGoal.SetPosition(30, 55);

		// set up the marble
		xstart = 55; ystart = 345;
		theMarble.SetPosition(xstart, ystart);
		theMarble.SetVelocity(0, 0);
		ax = ay = 0;

		break;

	case 2:
		strInfo1 = "LEVEL 2"; strInfo2 = "by Kennedy";

		// build the walls inside
		theWalls.push_back(new CSprite(CRectangle(295, 80, 10, 240), "wallvert.bmp", GetTime()));
		theWalls.push_back(new CSprite(CRectangle(495, 150, 10, 100), "wallvert.bmp", GetTime()));

		// dig the holes
		theHoles.push_back(new CSprite(CVector(150,  30), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(150,  115), "hole.gif", GetTime()));


		theWalls.push_back(new CSprite(CRectangle(0, 95, 300, 10), "wallhorz.bmp", GetTime()));
		theWalls.push_back(new CSprite(CRectangle(100, 195, 300, 20), "wallhorz.bmp", GetTime()));
		
		// set up the goal
		theGoal.SetPosition(370, 300);

		// set up the marble
		xstart = 55; ystart = 345;
		theMarble.SetPosition(xstart, ystart);
		theMarble.SetVelocity(0, 0);
		ax = ay = 0;

		break;

	case 3:
		strInfo1 = "LEVEL 3"; strInfo2 = "by Kennedy (again)";

		theHoles.push_back(new CSprite(CVector(470, 150), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(470, 250), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(420, 175), "hole.gif", GetTime()));
		theHoles.push_back(new CSprite(CVector(420, 225), "hole.gif", GetTime()));

		theWalls.push_back(new CSprite(CRectangle(0, 95, 300, 10), "wallhorz.bmp", GetTime()));
		theWalls.push_back(new CSprite(CRectangle(100, 195, 300, 20), "wallhorz.bmp", GetTime()));
		theWalls.push_back(new CSprite(CRectangle(200, 295, 300, 30), "wallhorz.bmp", GetTime()));

		// set up the goal
		theGoal.SetPosition(770, 200);

		// set up the marble
		xstart = 55; ystart = 345;
		theMarble.SetPosition(xstart, ystart);
		theMarble.SetVelocity(0, 0);
		ax = ay = 0;

		break;

	default:
		theGoal.SetPosition(30, 1345);
		GameOver();
		break;		// you won!
	}
}

// called when Game is Over
void CMarbleGame::OnGameOver()
{
}

// one time termination code
void CMarbleGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMarbleGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMarbleGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMarbleGame::ControlTilt(Uint16 x,Uint16 y)
{
	float dist = CVector(x, y).Distance(MousePoint);
	if (dist > MouseRadius)
		return;

	theArrow.SetPivotFromCenter(0, -13);
	theArrow.SetPosition(x, y);
	theArrow.SetRotation(x - MousePoint.GetX(), y - MousePoint.GetY());
	ax = (float)(x - MousePoint.GetX()) / MouseRadius;
	ay = (float)(y - MousePoint.GetY()) / MouseRadius;
}

void CMarbleGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	ControlTilt(x, y);
}

void CMarbleGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	ControlTilt(x, y);
}

void CMarbleGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMarbleGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMarbleGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMarbleGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMarbleGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
