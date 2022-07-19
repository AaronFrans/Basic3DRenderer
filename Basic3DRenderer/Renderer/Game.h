#pragma once
#pragma once
#include "Mesh3d.h"
#include "Matrix4x43d.h"

class Game final
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	const Window m_Window;

	float m_Theta;

	Mesh3d m_MeshCube;
	Matrix4x43d m_ProjMatrix;
	Matrix4x43d m_RotX;
	Matrix4x43d m_RotZ;

	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;

	void InitMesh();
	void InitProjectionMatrix();
	void InitRotMatrixes();


	void UpdateRotMatrixes(float elapsedSec);

	void MultiplyMatrixVector(const Vector3d& i, Vector3d& o, const Matrix4x43d& m) const;
	
	
	//Drawing
	void DrawLine(float x1, float y1, float x2, float y2) const;
	void DrawTriangle(float x1, float y1, float x2,
		float y2, float x3, float y3) const;

};