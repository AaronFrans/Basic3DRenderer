#include "pch.h"
#include "Game.h"

Game::Game(const Window& window)
	:m_Window{ window }
	, m_Theta{ 0 }
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	InitMesh();
	InitProjectionMatrix();
	InitRotMatrixes();
}

void Game::Cleanup()
{
}

void Game::Update(float elapsedSec)
{
	UpdateRotMatrixes(elapsedSec);
}

void Game::Draw() const
{
	ClearBackground();

	for (auto& tri : m_MeshCube.tris)
	{
		Triangle3d triProj, triTrans, triRotZ, triRotZX{};

		MultiplyMatrixVector(tri.p[0], triRotZ.p[0], m_RotZ);
		MultiplyMatrixVector(tri.p[1], triRotZ.p[1], m_RotZ);
		MultiplyMatrixVector(tri.p[2], triRotZ.p[2], m_RotZ);

		MultiplyMatrixVector(triRotZ.p[0], triRotZX.p[0], m_RotX);
		MultiplyMatrixVector(triRotZ.p[1], triRotZX.p[1], m_RotX);
		MultiplyMatrixVector(triRotZ.p[2], triRotZX.p[2], m_RotX);


		triTrans = triRotZX;
		triTrans.p[0].z = triRotZX.p[0].z + 3;
		triTrans.p[1].z = triRotZX.p[1].z + 3;
		triTrans.p[2].z = triRotZX.p[2].z + 3;

		MultiplyMatrixVector(triTrans.p[0], triProj.p[0], m_ProjMatrix);
		MultiplyMatrixVector(triTrans.p[1], triProj.p[1], m_ProjMatrix);
		MultiplyMatrixVector(triTrans.p[2], triProj.p[2], m_ProjMatrix);

#pragma region Scale into view
		triProj.p[0].x += 1.f; triProj.p[0].y += 1.f;
		triProj.p[1].x += 1.f; triProj.p[1].y += 1.f;
		triProj.p[2].x += 1.f; triProj.p[2].y += 1.f;

		triProj.p[0].x *= 0.5f * m_Window.width;
		triProj.p[0].y *= 0.5f * m_Window.height;
		triProj.p[1].x *= 0.5f * m_Window.width;
		triProj.p[1].y *= 0.5f * m_Window.height;
		triProj.p[2].x *= 0.5f * m_Window.width;
		triProj.p[2].y *= 0.5f * m_Window.height;
#pragma endregion

		DrawTriangle(
			triProj.p[0].x, triProj.p[0].y,
			triProj.p[1].x, triProj.p[1].y,
			triProj.p[2].x, triProj.p[2].y
		);
	}
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::InitMesh()
{
	m_MeshCube.tris =
	{
		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },


	};
}

void Game::InitProjectionMatrix()
{
	float zNear = 0.1f;
	float zFar = 1000.0f;
	float fov = 90.0f;
	float aspect = m_Window.height / m_Window.width;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);


	m_ProjMatrix.m[0][0] = aspect * fovRad;
	m_ProjMatrix.m[1][1] = fovRad;
	m_ProjMatrix.m[2][2] = zFar / (zFar - zNear);
	m_ProjMatrix.m[3][2] = (-zFar * zNear) / (zFar - zNear);
	m_ProjMatrix.m[2][3] = 1.0f;
	m_ProjMatrix.m[3][3] = 0.0f;
}

void Game::InitRotMatrixes()
{
	m_RotZ.m[0][0] = cosf(m_Theta);
	m_RotZ.m[0][1] = sinf(m_Theta);
	m_RotZ.m[1][0] = -sinf(m_Theta);
	m_RotZ.m[1][1] = cosf(m_Theta);
	m_RotZ.m[2][2] = 1;
	m_RotZ.m[3][3] = 1;

	m_RotX.m[0][0] = 1;
	m_RotX.m[1][1] = cosf(m_Theta);
	m_RotX.m[1][2] = sinf(m_Theta);
	m_RotX.m[2][1] = -sinf(m_Theta);
	m_RotX.m[2][2] = cosf(m_Theta);
	m_RotX.m[3][3] = 1;
}

void Game::UpdateRotMatrixes(float elapsedSec)
{

	m_Theta += 0.5f * elapsedSec;

	m_RotZ.m[0][0] = cosf(m_Theta);
	m_RotZ.m[0][1] = sinf(m_Theta);
	m_RotZ.m[1][0] = -sinf(m_Theta);
	m_RotZ.m[1][1] = cosf(m_Theta);
	m_RotZ.m[2][2] = 1;
	m_RotZ.m[3][3] = 1;

	m_RotX.m[0][0] = 1;
	m_RotX.m[1][1] = cosf(m_Theta * 0.5f);
	m_RotX.m[1][2] = sinf(m_Theta * 0.5f);
	m_RotX.m[2][1] = -sinf(m_Theta * 0.5f);
	m_RotX.m[2][2] = cosf(m_Theta * 0.5f);
	m_RotX.m[3][3] = 1;
}

void Game::MultiplyMatrixVector(const Vector3d& i, Vector3d& o, const Matrix4x43d& m) const
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}
}

void Game::DrawLine(float x1, float y1, float x2, float y2) const
{
	glLineWidth(3);
	glBegin(GL_LINES);
	{
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	}
	glEnd();
}

void Game::DrawTriangle(float x1, float y1, float x2,
	float y2, float x3, float y3) const
{
	glColor4f(1, 1, 1, 1);
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x3, y3, x1, y1);
}
