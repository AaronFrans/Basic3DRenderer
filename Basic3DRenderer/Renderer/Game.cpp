#include "pch.h"
#include "Game.h"
#include <algorithm>

Game::Game(const Window& window)
	: m_Window{ window }
	, m_Theta{ 0 }
	, m_Camera{}
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

	std::vector<Triangle3d> toDrawTris;

	for (auto& tri : m_MeshCube.m_Tris)
	{
		Triangle3d triProj, triTrans, triRotZ, triRotZX{};

#pragma region Rotation

		MultiplyMatrixVector(tri.m_Points[0], triRotZ.m_Points[0], m_RotZ);
		MultiplyMatrixVector(tri.m_Points[1], triRotZ.m_Points[1], m_RotZ);
		MultiplyMatrixVector(tri.m_Points[2], triRotZ.m_Points[2], m_RotZ);

		MultiplyMatrixVector(triRotZ.m_Points[0], triRotZX.m_Points[0], m_RotX);
		MultiplyMatrixVector(triRotZ.m_Points[1], triRotZX.m_Points[1], m_RotX);
		MultiplyMatrixVector(triRotZ.m_Points[2], triRotZX.m_Points[2], m_RotX);

#pragma endregion

#pragma region Translation

		triTrans = triRotZX;
		triTrans.m_Points[0].z = triRotZX.m_Points[0].z + 8;
		triTrans.m_Points[1].z = triRotZX.m_Points[1].z + 8;
		triTrans.m_Points[2].z = triRotZX.m_Points[2].z + 8;

#pragma endregion

#pragma region Culling

		Vector3d normal, line1, line2;

		line1.x = triTrans.m_Points[1].x - triTrans.m_Points[0].x;
		line1.y = triTrans.m_Points[1].y - triTrans.m_Points[0].y;
		line1.z = triTrans.m_Points[1].z - triTrans.m_Points[0].z;

		line2.x = triTrans.m_Points[2].x - triTrans.m_Points[0].x;
		line2.y = triTrans.m_Points[2].y - triTrans.m_Points[0].y;
		line2.z = triTrans.m_Points[2].z - triTrans.m_Points[0].z;

		normal.x = line1.y * line2.z - line1.z * line2.y;
		normal.y = line1.z * line2.x - line1.x * line2.z;
		normal.z = line1.x * line2.y - line1.y * line2.x;

		float normalLength = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

		normal.x /= normalLength;
		normal.y /= normalLength;
		normal.z /= normalLength;

#pragma endregion

		//if (normal.z < 0)
		if (normal.x * (triTrans.m_Points[0].x - m_Camera.x) +
			normal.y * (triTrans.m_Points[0].y - m_Camera.y) +
			normal.z * (triTrans.m_Points[0].z - m_Camera.z) < 0)
		{

#pragma region Add Lighting

			Vector3d lightDir{ 0,0, -1.0f };
			float lightNormalLenght =
				sqrtf(lightDir.x * lightDir.x + lightDir.y * lightDir.y + lightDir.z * lightDir.z);

			lightDir.x /= normalLength;
			lightDir.y /= normalLength;
			lightDir.z /= normalLength;

			float dot = normal.x * lightDir.x +
				normal.y * lightDir.y +
				normal.z * lightDir.z;

			triTrans.luminance = dot;

#pragma endregion

#pragma region Projection

			MultiplyMatrixVector(triTrans.m_Points[0], triProj.m_Points[0], m_ProjMatrix);
			MultiplyMatrixVector(triTrans.m_Points[1], triProj.m_Points[1], m_ProjMatrix);
			MultiplyMatrixVector(triTrans.m_Points[2], triProj.m_Points[2], m_ProjMatrix);
			triProj.luminance = triTrans.luminance;

#pragma endregion

#pragma region Scale into view

			triProj.m_Points[0].x += 1.f; triProj.m_Points[0].y += 1.f;
			triProj.m_Points[1].x += 1.f; triProj.m_Points[1].y += 1.f;
			triProj.m_Points[2].x += 1.f; triProj.m_Points[2].y += 1.f;

			triProj.m_Points[0].x *= 0.5f * m_Window.width;
			triProj.m_Points[0].y *= 0.5f * m_Window.height;
			triProj.m_Points[1].x *= 0.5f * m_Window.width;
			triProj.m_Points[1].y *= 0.5f * m_Window.height;
			triProj.m_Points[2].x *= 0.5f * m_Window.width;
			triProj.m_Points[2].y *= 0.5f * m_Window.height;

#pragma endregion


			toDrawTris.push_back(triProj);


		}
	}

	std::sort(toDrawTris.begin(), toDrawTris.end(), [](Triangle3d& t1, Triangle3d& t2)
		{

			float zAvg1 = (t1.m_Points[0].z + t1.m_Points[1].z + t1.m_Points[2].z) / 3.0f;
			float zAvg2 = (t2.m_Points[0].z + t2.m_Points[1].z + t2.m_Points[2].z) / 3.0f;

			return zAvg1 > zAvg2;
		});

	for (auto& triProj : toDrawTris)
	{

#pragma region Draw Object




		FillTriangle(
			triProj.m_Points[0].x, triProj.m_Points[0].y,
			triProj.m_Points[1].x, triProj.m_Points[1].y,
			triProj.m_Points[2].x, triProj.m_Points[2].y
		);

		FillTriangle(
			triProj.m_Points[0].x, triProj.m_Points[0].y,
			triProj.m_Points[1].x, triProj.m_Points[1].y,
			triProj.m_Points[2].x, triProj.m_Points[2].y,
			Color4f{ 0,0,0, 1 - triProj.luminance }
		);

		DrawTriangle(
			triProj.m_Points[0].x, triProj.m_Points[0].y,
			triProj.m_Points[1].x, triProj.m_Points[1].y,
			triProj.m_Points[2].x, triProj.m_Points[2].y,
			Color4f{ 0,0,0, 1 }
		);

#pragma endregion

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
#pragma region Basic Cube

	//m_MeshCube.m_Tris =
	//{
	//	// SOUTH
	//	{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
	//	{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

	//	// EAST                                                      
	//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
	//	{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

	//	// NORTH                                                     
	//	{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
	//	{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

	//	// WEST                                                      
	//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
	//	{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

	//	// TOP                                                       
	//	{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
	//	{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

	//	// BOTTOM                                                    
	//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
	//	{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	//};

#pragma endregion

	m_MeshCube.LoadFromObjFile("Resources/OBJ/Ship.obj");
}

void Game::InitProjectionMatrix()
{
	float zNear = 0.1f;
	float zFar = 1000.0f;
	float fov = 90.0f;
	float aspect = m_Window.height / m_Window.width;
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);


	m_ProjMatrix.m_Matrix[0][0] = aspect * fovRad;
	m_ProjMatrix.m_Matrix[1][1] = fovRad;
	m_ProjMatrix.m_Matrix[2][2] = zFar / (zFar - zNear);
	m_ProjMatrix.m_Matrix[3][2] = (-zFar * zNear) / (zFar - zNear);
	m_ProjMatrix.m_Matrix[2][3] = 1.0f;
	m_ProjMatrix.m_Matrix[3][3] = 0.0f;
}

void Game::InitRotMatrixes()
{
	m_RotZ.m_Matrix[0][0] = cosf(m_Theta);
	m_RotZ.m_Matrix[0][1] = sinf(m_Theta);
	m_RotZ.m_Matrix[1][0] = -sinf(m_Theta);
	m_RotZ.m_Matrix[1][1] = cosf(m_Theta);
	m_RotZ.m_Matrix[2][2] = 1;
	m_RotZ.m_Matrix[3][3] = 1;

	m_RotX.m_Matrix[0][0] = 1;
	m_RotX.m_Matrix[1][1] = cosf(m_Theta);
	m_RotX.m_Matrix[1][2] = sinf(m_Theta);
	m_RotX.m_Matrix[2][1] = -sinf(m_Theta);
	m_RotX.m_Matrix[2][2] = cosf(m_Theta);
	m_RotX.m_Matrix[3][3] = 1;
}

void Game::UpdateRotMatrixes(float elapsedSec)
{

	m_Theta += 0.5f * elapsedSec;

	m_RotZ.m_Matrix[0][0] = cosf(m_Theta);
	m_RotZ.m_Matrix[0][1] = sinf(m_Theta);
	m_RotZ.m_Matrix[1][0] = -sinf(m_Theta);
	m_RotZ.m_Matrix[1][1] = cosf(m_Theta);
	m_RotZ.m_Matrix[2][2] = 1;
	m_RotZ.m_Matrix[3][3] = 1;

	m_RotX.m_Matrix[0][0] = 1;
	m_RotX.m_Matrix[1][1] = cosf(m_Theta * 0.5f);
	m_RotX.m_Matrix[1][2] = sinf(m_Theta * 0.5f);
	m_RotX.m_Matrix[2][1] = -sinf(m_Theta * 0.5f);
	m_RotX.m_Matrix[2][2] = cosf(m_Theta * 0.5f);
	m_RotX.m_Matrix[3][3] = 1;
}

void Game::MultiplyMatrixVector(const Vector3d& i, Vector3d& o, const Matrix4x43d& m) const
{
	o.x = i.x * m.m_Matrix[0][0] + i.y * m.m_Matrix[1][0] + i.z * m.m_Matrix[2][0] + m.m_Matrix[3][0];
	o.y = i.x * m.m_Matrix[0][1] + i.y * m.m_Matrix[1][1] + i.z * m.m_Matrix[2][1] + m.m_Matrix[3][1];
	o.z = i.x * m.m_Matrix[0][2] + i.y * m.m_Matrix[1][2] + i.z * m.m_Matrix[2][2] + m.m_Matrix[3][2];
	float w = i.x * m.m_Matrix[0][3] + i.y * m.m_Matrix[1][3] + i.z * m.m_Matrix[2][3] + m.m_Matrix[3][3];

	if (w != 0.0f)
	{
		o.x /= w;
		o.y /= w;
		o.z /= w;
	}
}

void Game::DrawLine(float x1, float y1, float x2, float y2, Color4f color) const
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINES);
	{
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	}
	glEnd();
}

void Game::DrawTriangle(float x1, float y1, float x2,
	float y2, float x3, float y3, Color4f color) const
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glVertex2f(x3, y3);
	}
	glEnd();
}

void Game::FillTriangle(float x1, float y1, float x2,
	float y2, float x3, float y3, Color4f color) const
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLES);
	{
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glVertex2f(x3, y3);
	}
	glEnd();
}
