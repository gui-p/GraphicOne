// GraphicOne.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "olcConsoleGameEngine.h" 
#include <fstream>
#include <strstream>
#include <algorithm>

struct vec3d
{
	float x, y, z;
};

struct tris
{
	vec3d p[3];

	wchar_t sym;
	short col;
};

struct mesh
{
	std::vector<tris> tris;

	bool LoadFromObjFile(std::string sFileName)
	{

		std::ifstream f(sFileName);
		if (!f.is_open())
			return false;

		std::vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
		
			if (line[0] == 'f')
			{
				int f[3];

				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}

		}


		return true;
	}
};

struct mat4x4
{
	float m[4][4] = { 0 };
};

class GraphicOne : public olcConsoleGameEngine
{

private:
	mesh meshCube;
	mat4x4 matProj;

	vec3d vCamera;

	float fTheta;

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
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

	// Taken From Command Line Webcam Video
	CHAR_INFO GetColour(float lum)
	{
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f * lum);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}

		CHAR_INFO c;
		c.Attributes = bg_col | fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}

public:
	GraphicOne()
	{
		m_sAppName = L"3D Demo";
	}

public:
	bool OnUserCreate() override
	{

		//meshCube.tris = {
		//	//SOUTH
		//	{0.0f, 0.0f, 0.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 0.0f},
		//	{0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f},

		//	//EAST
		//	{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f},
		//	{1.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 1.0f},

		//	//NORTH
		//	{1.0f, 0.0f, 1.0f,		1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 1.0f},
		//	{1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f},

		//	//WEST
		//	{0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f},
		//	{0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, 0.0f},

		//	//TOP
		//	{0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 1.0f,		1.0f, 1.0f, 1.0f},
		//	{0.0f, 1.0f, 0.0f,		1.0f, 1.0f, 1.0f,		1.0f, 1.0f, 0.0f},

		//	//BOTTOM
		//	{0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f},
		//	{0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f, 1.0f},

		//};

		meshCube.LoadFromObjFile("VideoShip.obj");

		//Projection Matrix

		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / std::tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		mat4x4 matRotX, matRotZ;
		fTheta += 0.5f * fElapsedTime;


		// Rotation Z

		matRotZ.m[0][0] = std::cosf(fTheta);
		matRotZ.m[0][1] = std::sinf(fTheta);
		matRotZ.m[1][0] = -std::sinf(fTheta);
		matRotZ.m[1][1] = std::cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;
		

		// Rotation X 
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = std::cosf(fTheta * 0.5f);
		matRotX.m[1][2] = std::sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -std::sinf(fTheta * 0.5f);
		matRotX.m[2][2] = std::cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;



		for (auto tri : meshCube.tris)
		{
			tris triProjected, triTranslated, triRotatedZ, triRotatedZX;


			MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

			MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;

			vec3d normal, line1, line2;
			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;
			
			float len = std::sqrtf((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

			normal.x /= len; 
			normal.y /= len; 
			normal.z /= len; 

			std::vector<tris> vecTrisToRaster;

			//if (normal.z < 0)
			if(normal.x * (triTranslated.p[0].x - vCamera.x) + 
			   normal.y * (triTranslated.p[0].y - vCamera.y) +
			   normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
			{

				vec3d lightDir = { 0.0f, 0.0f, -1.0f };

				float len = std::sqrtf((lightDir.x * lightDir.x) + (lightDir.y * lightDir.y) + (lightDir.z * lightDir.z));
				lightDir.x /= len;
				lightDir.y /= len;
				lightDir.z /= len;

				float dotProd = normal.x * lightDir.x + normal.y * lightDir.y + normal.z * lightDir.z;

				CHAR_INFO c = GetColour(dotProd);
				triTranslated.col = c.Attributes;
				triTranslated.sym = c.Char.UnicodeChar;

				MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
				MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
				MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);
				triProjected.col = triTranslated.col;
				triProjected.sym = triTranslated.sym;



				triProjected.p[0].x += 1.0f;
				triProjected.p[0].y += 1.0f;

				triProjected.p[1].x += 1.0f;
				triProjected.p[1].y += 1.0f;

				triProjected.p[2].x += 1.0f;
				triProjected.p[2].y += 1.0f;

				triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
				triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
				triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

				vecTrisToRaster.push_back(triProjected);

			}

			// Sort triangle from back to front;

			std::sort(vecTrisToRaster.begin(), vecTrisToRaster.end(), [](tris& tri1, tris& tri2)
			{
				float midPointZ1 = (tri1.p[0].z + tri1.p[1].z + tri1.p[2].z) / 3.0f;
				float midPointZ2 = (tri2.p[0].z + tri2.p[1].z + tri2.p[2].z) / 3.0f;

				return midPointZ1 > midPointZ2;

			});


			for (auto &triProjected : vecTrisToRaster)
			{
				// Raterization
				FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					triProjected.sym, triProjected.col);

				/*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					PIXEL_SOLID, FG_BLACK);*/
			}

		}

		return true;
	}

};

int main()
{
	GraphicOne demo;

	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();

	return 0;
}


// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
