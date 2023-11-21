/*
	
	First person shooter in C++ console. Use WASD to move around.
	
	Console Requirements:	Font - Consolus
							Console Window Size - 120 by 40
	
	Special thanks to YouTube: Javidx9 for graphic design tutorial and
	understanding of Windows.h library

*/

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>

#include <stdio.h>
#include <Windows.h>

const int nSCREEN_WIDTH = 120;
const int nSCREEN_HEIGHT = 40;
const int nMAP_WIDTH = 16;
const int nMAP_HEIGHT = 16;

float fplayer_x = 14.7f;
float fplayer_y = 5.09f;
float fplayer = 0.0f;
float fFOV = 3.14159f / 4.0f;
float fdepth = 16.0f;
float fvelocity = 5.0f;

int main()
{
	wchar_t* screen = new wchar_t[nSCREEN_WIDTH * nSCREEN_HEIGHT];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	std::wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#......##......#";
	map += L"#......##......#";
	map += L"#..............#";
	map += L"###............#";
	map += L"##.............#";
	map += L"#......####..###";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"################";

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	do
	{
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fplayer -= (fvelocity * 0.75f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fplayer += (fvelocity * 0.75f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fplayer_x += sinf(fplayer) * fvelocity * fElapsedTime;;
			fplayer_y += cosf(fplayer) * fvelocity * fElapsedTime;;
			if (map.c_str()[(int)fplayer_x * nMAP_WIDTH + (int)fplayer_y] == '#')
			{
				fplayer_x -= sinf(fplayer) * fvelocity * fElapsedTime;;
				fplayer_y -= cosf(fplayer) * fvelocity * fElapsedTime;;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fplayer_x -= sinf(fplayer) * fvelocity * fElapsedTime;;
			fplayer_y -= cosf(fplayer) * fvelocity * fElapsedTime;;
			if (map.c_str()[(int)fplayer_x * nMAP_WIDTH + (int)fplayer_y] == '#')
			{
				fplayer_x += sinf(fplayer) * fvelocity * fElapsedTime;;
				fplayer_y += cosf(fplayer) * fvelocity * fElapsedTime;;
			}
		}

		for (int x = 0; x < nSCREEN_WIDTH; x++)
		{
			
			float fRayAngle = (fplayer - fFOV / 2.0f) + ((float)x / (float)nSCREEN_WIDTH) * fFOV;

			float fStepSize = 0.1f;										
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;
			bool bBoundary = false;	

			float fEyeX = sinf(fRayAngle); 
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fdepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fplayer_x + fEyeX * fDistanceToWall);
				int nTestY = (int)(fplayer_y + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMAP_WIDTH || nTestY < 0 || nTestY >= nMAP_HEIGHT)
				{
					bHitWall = true;
					fDistanceToWall = fdepth;
				}
				else
				{
					if (map.c_str()[nTestX * nMAP_WIDTH + nTestY] == '#')
					{
						bHitWall = true;

						std::vector<std::pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fplayer_y;
								float vx = (float)nTestX + tx - fplayer_x;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));
							}


						sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) {return left.first < right.first; });

						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			int nCeiling = (float)(nSCREEN_HEIGHT / 2.0) - nSCREEN_HEIGHT / ((float)fDistanceToWall);
			int nFloor = nSCREEN_HEIGHT - nCeiling;

			short nShade = ' ';
			if (fDistanceToWall <= fdepth / 4.0f)			nShade = 0x2588;
			else if (fDistanceToWall < fdepth / 3.0f)		nShade = 0x2593;
			else if (fDistanceToWall < fdepth / 2.0f)		nShade = 0x2592;
			else if (fDistanceToWall < fdepth)				nShade = 0x2591;
			else											nShade = ' ';

			if (bBoundary)		nShade = ' ';

			for (int y = 0; y < nSCREEN_HEIGHT; y++)
			{
				if (y <= nCeiling)
					screen[y * nSCREEN_WIDTH + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nSCREEN_WIDTH + x] = nShade;
				else
				{
					float b = 1.0f - (((float)y - nSCREEN_HEIGHT / 2.0f) / ((float)nSCREEN_HEIGHT / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screen[y * nSCREEN_WIDTH + x] = nShade;
				}
			}
		}

		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fplayer_x, fplayer_y, fplayer, 1.0f / fElapsedTime);

		for (int nx = 0; nx < nMAP_WIDTH; nx++)
			for (int ny = 0; ny < nMAP_WIDTH; ny++)
			{
				screen[(ny + 1) * nSCREEN_WIDTH + nx] = map[ny * nMAP_WIDTH + nx];
			}
		screen[((int)fplayer_x + 1) * nSCREEN_WIDTH + (int)fplayer_y] = 'P';

		screen[nSCREEN_WIDTH * nSCREEN_HEIGHT - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nSCREEN_WIDTH * nSCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
	} while (1);

	return 0;
}