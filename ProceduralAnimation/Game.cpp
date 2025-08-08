#include "Game.h"

#include "utils.h"
#include "Time.h"
#include "Texture.h"
#include "OrientationManager.h"
#include "ProceduralAnimation.h" // Project

#include <iostream>
#include <cmath>
#include <print>

Game::Game(const Window& window)
	:BaseGame(window)
	,m_Window{ window }
	,m_RenderedFrames{0}
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{


	// Default
	m_ProAnim = std::make_unique<ProceduralAnimation>(); // Project
	m_FPSCounter = std::make_unique<Texture>("placeHolder","../Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1});
	m_TargetFPS = 60.0f;
	m_Time = std::make_unique<Time>(0.0f);
	m_AccumulatedTime = std::make_unique<Time>(0.0f);
	m_TimeState = std::make_unique<Texture>("Paused","../Resources/Fonts/consola.ttf",24,Color4f{1,1,1,1});
	OrientationManager::UpdateCamera(GetViewPort());
	// Default
}

void Game::Cleanup()
{
	
}

void Game::Update(float elapsedSec)
{
	// Check keyboard state
	using namespace std;

	// Camera code
	float maxCameraSpeedDivider{1.5f};
	float minCameraSpeedDivider{0.5f};
	float cameraSpeedDivider{1.0f};

	Point2f mousePosDiff{m_LastMousePos-m_PreviousMousePos};
	m_PreviousMousePos = m_LastMousePos;

	if (m_AltHeld && m_LeftClickHeld)
	{
		m_CameraPos.left -= (mousePosDiff.x / cameraSpeedDivider);
		m_CameraPos.bottom -= (mousePosDiff.y/ cameraSpeedDivider);
		
	}

	OrientationManager::UpdateCamera(m_CameraPos);
	//

	m_Time->Update(elapsedSec);
	m_AccumulatedTime->Update(elapsedSec); // void UpdateTimers(float elapsedSec)????
	
	const float frameDuration{1.0f / m_TargetFPS};
	// Limits the frequency of code inside   // Put this into a separate function?
	if (m_AccumulatedTime->GetTime() >= frameDuration)
	{
		m_ProAnim->Update(elapsedSec);
		m_AccumulatedTime->Restart();
		UpdateTextures();
	}

	m_RenderedFrames += 1;
}
void Game::UpdateTextures()
{
	int m_FPS = int((float)m_RenderedFrames / m_Time->GetTime());
	m_FPSCounter = std::make_unique<Texture>("FPS: " + std::to_string(m_FPS),"../Resources/Fonts/consola.ttf",16,Color4f{1,1,1,1}); // Has to be better

	UpdatePausedText();
	m_ProAnim->UpdateTextures();
}
void Game::UpdatePausedText()
{
	using namespace PrettyColors;
	int ptSize{24};
	Color4f color{GetColor(white)};
	const char* path{"../Resources/Fonts/consola.ttf"};
	const char* text{"No state"};
	if (m_Paused == true)
	{
		text = "Paused";
	}
	else
	{
		text = "Going";
	}
	m_TimeState = std::make_unique<Texture>(text,path,ptSize,color);
}

void Game::PushCameraMatrix() const
{

	glTranslatef(-m_CameraPos.left,-m_CameraPos.bottom,0);
	
}

void Game::Draw() const
{
	ClearBackground();
	glPushMatrix();
	{
		PushCameraMatrix();
		glScalef(m_ScaleFactor,m_ScaleFactor,0);
		m_ProAnim->Draw();

		/*utils::SetColor(PrettyColors::GetColor(PrettyColors::rose));
		utils::FillRect(100.0f,100.0f,10.0f,10.0f); DEBUG*/
	}
	glPopMatrix();

	DrawUI(); // FIX VVV
	m_ProAnim->DrawUI(GetViewPort());
	m_FPSCounter->Draw(Point2f{10.0f, GetViewPort().height - m_FPSCounter->GetHeight() - 4.0f},Rectf{});

}

void Game::DrawUI() const
{
	DrawPausedText();
}
void Game::DrawPausedText() const
{
	Rectf viewport = GetViewPort();
	float edgeOffset{20.0f};
	Rectf drawDst{viewport.width - m_TimeState->GetWidth() - edgeOffset
	,viewport.bottom + edgeOffset
	,m_TimeState->GetWidth()
	,m_TimeState->GetHeight()};
	m_TimeState->Draw(drawDst);
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	// It's kind of an overkill to check all letters everytime when I am using like 20% of them
	// Also makes it so the switch is annoying to search through // FIX ? (in keyUp too)
	switch (e.keysym.sym)
	{
	case SDLK_q:

		break;
	case SDLK_w:
		m_ProAnim->ConstrainSelectedLinks();
		break;
	case SDLK_e:

		break;
	case SDLK_r:
		m_ProAnim->UnConstrainAll();
		break;
	case SDLK_t:

		break;
	case SDLK_y:
		m_ProAnim->ConstrainToMouse();
		break;
	case SDLK_u:
		ResetZoom();
		break;
	case SDLK_i:

		break;
	case SDLK_o:
		Start();
		break;
	case SDLK_p:
		Pause();
		break;
	case SDLK_a:

		break;
	case SDLK_s:
		m_ProAnim->SwitchContraintDrawing();
		break;
	case SDLK_d:

		break;
	case SDLK_f:

		break;
	case SDLK_g:

		break;
	case SDLK_h:

		break;
	case SDLK_j:

		break;
	case SDLK_k:

		break;
	case SDLK_l:

		break;
	case SDLK_z:

		break;
	case SDLK_x:

		break;
	case SDLK_c:

		break;
	case SDLK_v:

		break;
	case SDLK_b:

		break;
	case SDLK_n:

		break;
	case SDLK_m:

		break;
	case SDLK_COMMA:
		m_TargetFPS -= 5.0f;
		if (m_TargetFPS <= 0.0f)
		{
			m_TargetFPS = 5.0f;
		}
		break;
	case SDLK_PERIOD:
		m_TargetFPS += 5.0f;
		if (m_TargetFPS >= 120.0f)
		{
			m_TargetFPS = 120.0f;
		}
	case SDLK_LALT:
		m_AltHeld = true;
		break;
	case SDLK_ESCAPE:
		// Not sure about this, but works ¯\_(^^)_/¯
		SDL_Event e{SDL_QUIT};
		SDL_PushEvent(&e);
		break;
	}
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_q:

		break;
	case SDLK_w:

		break;
	case SDLK_e:

		break;
	case SDLK_r:

		break;
	case SDLK_t:

		break;
	case SDLK_y:

		break;
	case SDLK_u:

		break;
	case SDLK_i:

		break;
	case SDLK_o:

		break;
	case SDLK_p:
	
		break;
	case SDLK_a:

		break;
	case SDLK_s:

		break;
	case SDLK_d:

		break;
	case SDLK_f:

		break;
	case SDLK_g:

		break;
	case SDLK_h:

		break;
	case SDLK_j:

		break;
	case SDLK_k:

		break;
	case SDLK_l:

		break;
	case SDLK_z:

		break;
	case SDLK_x:

		break;
	case SDLK_c:

		break;
	case SDLK_v:

		break;
	case SDLK_b:

		break;
	case SDLK_n:

		break;
	case SDLK_m:

		break;
	case SDLK_LALT:
		m_AltHeld = false;
		break;
	}
	
}
void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	m_LastMousePos = Point2f{ float(e.x),float(e.y) };

	m_ProAnim->OnMouseMotion(m_LastMousePos);
}
void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	switch (e.button)
	{
	case SDL_BUTTON_LEFT:
		m_LeftClickHeld = true;
		break;
	case SDL_BUTTON_RIGHT:
		break;
	case SDL_BUTTON_MIDDLE:
		break;
	}

	m_ProAnim->OnMouseDown();
}
void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{

	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		m_LeftClickHeld = false;
		break;
	case SDL_BUTTON_RIGHT:
		
		break;
	case SDL_BUTTON_MIDDLE:
		
		break;
	}
	m_ProAnim->OnMouseUp();
}
void Game::ProcessMouseWheelEvent(const SDL_MouseWheelEvent& e)
{
	// Wheel up
	if (e.y > 0)
	{
		ZoomIn();
	}
	// Wheel down
	else if (e.y < 0)
	{
		ZoomOut();
	}	
}

void Game::Start()
{
	m_TimeStateChanged = true;
	m_Paused = false;
}
void Game::Pause()
{
	m_TimeStateChanged = true;
	m_Paused = true;
}
void Game::Restart()
{

}

void Game::ZoomIn()
{
	m_ScaleFactor += 0.1f;
	m_ZoomTarget = m_LastMousePos;
}
void Game::ZoomOut()
{
	m_ScaleFactor -= 0.1f;
	if (m_ScaleFactor < 0.1f)
	{
		m_ScaleFactor = 0.1f;
	}
	m_ZoomTarget = m_LastMousePos;

}
void Game::ResetZoom()
{
	m_ScaleFactor = 1.0f;
}
float Game::GetScale() const
{
	return m_ScaleFactor;
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
