#pragma once
#include "BaseGame.h"
#include <vector>
#include <memory>

class Time;
class Texture;
class ProceduralAnimation; // Project

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void UpdateTextures();
	void UpdatePausedText();

	void Draw( ) const override;
	void DrawUI() const;
	void DrawPausedText() const;

	void Start(); 
	void Pause(); 
	void Restart(); 

	void ZoomIn();
	void ZoomOut();
	void ResetZoom(); 
	float GetScale() const; 

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseWheelEvent(const SDL_MouseWheelEvent& e) override;
	
private:
	// Non-default Game members:
	


	// Default Game members:
	std::unique_ptr<ProceduralAnimation> m_ProAnim; // Project
	const Window m_Window;

	// Mouse
	Point2f m_LastMousePos;
	Point2f m_PreviousMousePos;

	// Frames
	int m_RenderedFrames;
	std::unique_ptr<Texture> m_FPSCounter;
	float m_TargetFPS;

	// For camera movement
	bool m_AltHeld;
	bool m_LeftClickHeld;
	
	// Camera testing: (Make a class for Camera)
	void PushCameraMatrix() const;
	Rectf m_CameraPos;
	
	// Time
	std::unique_ptr<Time> m_Time;
	std::unique_ptr<Time> m_AccumulatedTime;
	std::unique_ptr<Texture> m_TimeState; 
	bool m_TimeStateChanged;
	bool m_Paused; 

	// Zoom 
	Point2f m_ZoomTarget; // Not used yet
	float m_ScaleFactor{1.0f};


	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

};