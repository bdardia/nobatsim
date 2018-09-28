#include <list>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include "NoBatSim.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

VOID OnPaint(HDC hdc)
{
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, OPAQUE);

	for (int x = 0; x <= 300; x++)
	{
		for (int y = 0; y <= 300; y++)
		{
			SetPixel(hdc, x, y, RGB(0, 0, 0));
		}
	}
	std::string ab = "PA: ";
	TextOut(hdc, 0, 0, ab.c_str(), strlen(ab.c_str()));
	std::string bb = "BB: ";
	TextOut(hdc, 0, 30, bb.c_str(), strlen(bb.c_str()));
	std::string hbp = "HBP: ";
	TextOut(hdc, 0, 60, hbp.c_str(), strlen(hbp.c_str()));
	std::string k = "Strikeouts: ";
	TextOut(hdc, 0, 90, k.c_str(), strlen(k.c_str()));
	std::string obp = "OBP: ";
	TextOut(hdc, 0, 120, obp.c_str(), strlen(obp.c_str()));
	std::string log = "Last plate appearence: ";
	TextOut(hdc, 0, 150, log.c_str(), strlen(log.c_str()));
	readData(hdc);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("No-Bat Simulator");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("No-Bat Simulator"),   // window class name
		TEXT("No-Bat Simulator"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	SetWindowPos(hWnd, NULL, 0, 0, 300, 300, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
		EndPaint(hWnd, &ps);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc

void readData(HDC hdc)
{
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, OPAQUE);
	using namespace std;
	srand(time(0));
	string line;
	string subline;
	string breakChar = string(",");
	ifstream myfile("savant_data.csv");
	string reviewableOutcomes[10] = { "swinging_strike,", "swinging_strike_blocked,", "foul,", "foul_tip,", "foul_bunt,", "missed_bunt,", "hit_into_play,", "hit_into_play_no_out,", "hit_into_play_score,", "hit_by_pitch," };
	string called = string("called_strike,");
	vector<string> currentPitch;
	bool inPA = false;
	bool newPitch = false;
	if (myfile.is_open())
	{
		int y = 0;
		int i = 0;
		int x;
		int startValue = 0;
		int outerBallCount = 0, outerStrikeCount = 0;
		while (getline(myfile, subline))
		{
			updateStats(hdc);
			x = 180;
			newPitch = false;
			for (int tx = 0; tx < 80; tx++)
			{
				for (int ty = 170; ty < 210; ty++)
				{
					SetPixel(hdc, tx, ty, RGB(0, 0, 0));
				}
			}
			for (int j = 0; j < subline.length(); j++)
			{
				if (subline.substr(j, 1).compare(breakChar) == 0 || j + 1 == subline.length()) // loop through string and store each cell
				{
					if ((j - startValue + 1) < subline.length())
					{
						line = subline.substr(startValue, (j - startValue) + 1);
						//TextOut(hdc, 0, x += 20, currentField[i].c_str(), strlen(currentField[i].c_str()));
						//TextOut(hdc, 0, x += 20, line.c_str(), strlen(line.c_str()));
						//TextOut(hdc, 700, 700, " ", 1);
						currentPitch.push_back(line);
						startValue = j + 1;
						//std::this_thread::sleep_for(std::chrono::milliseconds(10));
					}
					i++;
				}
			}
			// check if our count matches the data. If it doesn't, either skip pitches or generate new ones to realign.
			int ballCount = stoi(currentPitch.at(BALL_COUNT));
			int strikeCount = stoi(currentPitch.at(STRIKE_COUNT));
			if ((ballCount > outerBallCount || strikeCount > outerStrikeCount) && (outerBallCount == 0 && outerStrikeCount == 0))
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			else if ((ballCount < outerBallCount || strikeCount < outerStrikeCount) && (ballCount == 0 && strikeCount == 0))
			{
				while (outerBallCount < 4 && outerStrikeCount < 3)
				{
					int randomPitch = std::rand() % 1000 + 1;
					if (randomPitch <= 424)
						outerStrikeCount++;
					else
						outerBallCount++;
				}
				if (outerBallCount > outerStrikeCount)
				{
					TextOut(hdc, 0, 175, "walk", strlen("walk"));
					totalPlateAppearences++;
					totalWalks++;
				}
				else
				{
					TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
					totalPlateAppearences++;
					totalStrikeouts++;
				}
				TextOut(hdc, 700, 700, " ", 1);
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			// check if this pitch is reviewable
			for (int pti = 0; pti < 10; pti++)
			{
				if (currentPitch.at(PITCH_TYPE).compare(reviewableOutcomes[pti]) == 0)
				{
					if (pti == 9)
					{
						TextOut(hdc, 0, 175, "hit by pitch", strlen("hit by pitch"));
						TextOut(hdc, 700, 700, " ", 1);
						outerBallCount = 0, outerStrikeCount = 0;
						inPA = false;
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						newPitch = true;
						totalHitByPitches++;
						totalPlateAppearences++;
					}
					else if (pti > 5)
					{
						double ball_x = stod(currentPitch.at(PLATE_X));
						double ball_y = stod(currentPitch.at(PLATE_Y));
						double top_zone = stod(currentPitch.at(ZONE_TOP));
						double bot_zone = stod(currentPitch.at(ZONE_BOTTOM));
						if (ball_x > -0.839 && ball_x < 0.839)
							if (ball_y < top_zone && ball_y > bot_zone)
								outerStrikeCount++;
							else
								outerBallCount++;
						else
							outerBallCount++;

						if (outerBallCount == 4)
						{
							TextOut(hdc, 0, 175, "walk", strlen("walk"));
							inPA = false;
							TextOut(hdc, 700, 700, " ", 1);
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							totalPlateAppearences++;
							totalWalks++;
						}
						else if (outerStrikeCount == 3)
						{
							TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
							inPA = false;
							TextOut(hdc, 700, 700, " ", 1);
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							totalPlateAppearences++;
							totalStrikeouts++;
						}
						TextOut(hdc, 700, 700, " ", 1);
						newPitch = true;

						if (outerBallCount < 4 && outerStrikeCount < 3)
						{
							while (outerBallCount < 4 && outerStrikeCount < 3)
							{
								int randomPitch = std::rand() % 1000 + 1;
								if (randomPitch <= 424)
									outerStrikeCount++;
								else
									outerBallCount++;
							}
							if (outerBallCount > outerStrikeCount)
							{
								TextOut(hdc, 0, 175, "walk", strlen("walk"));
								totalPlateAppearences++;
								totalWalks++;
							}
							else
							{
								TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
								totalPlateAppearences++;
								totalStrikeouts++;
							}
							TextOut(hdc, 700, 700, " ", 1);
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}
						outerBallCount = 0, outerStrikeCount = 0;
					}
					else
					{
						double ball_x = stod(currentPitch.at(PLATE_X));
						double ball_y = stod(currentPitch.at(PLATE_Y));
						double top_zone = stod(currentPitch.at(ZONE_TOP));
						double bot_zone = stod(currentPitch.at(ZONE_BOTTOM));
						if (ball_x > -0.839 && ball_x < 0.839)
							if (ball_y < top_zone && ball_y > bot_zone)
								outerStrikeCount++;
							else
								outerBallCount++;
						else
							outerBallCount++;
						if (outerBallCount == 4)
						{
							TextOut(hdc, 0, 175, "walk", strlen("walk"));
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
							TextOut(hdc, 700, 700, " ", 1);
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							totalPlateAppearences++;
							totalWalks++;
						}
						else if (outerStrikeCount == 3)
						{
							TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
							outerBallCount = 0, outerStrikeCount = 0;
							inPA = false;
							TextOut(hdc, 700, 700, " ", 1);
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
							totalPlateAppearences++;
							totalStrikeouts++;
						}
						else
						{
							inPA = true;
						}
						newPitch = true;
					}
				}
			}
			// continue if we altered the pitch
			if (newPitch)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			// if we're still in a count but ran out of real pitches, generate new ones
			if (ballCount == 0 && strikeCount == 0)
			{
				if (inPA)
				{
					while (outerBallCount < 4 && outerStrikeCount < 3)
					{
						int randomPitch = std::rand() % 1000 + 1;
						if (randomPitch <= 424)
							outerStrikeCount++;
						else
							outerBallCount++;
					}
					if (outerBallCount > outerStrikeCount)
					{
						TextOut(hdc, 0, 175, "walk", strlen("walk"));
						totalPlateAppearences++;
						totalWalks++;
					}
					else
					{
						TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
						totalPlateAppearences++;
						totalStrikeouts++;
					}
					TextOut(hdc, 700, 700, " ", 1);
					outerBallCount = 0, outerStrikeCount = 0;
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
			}
			else if (!inPA)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			if (newPitch)
			{
				currentPitch.clear();
				i = 0;
				startValue = 0;
				continue;
			}
			inPA = true;
			if (currentPitch.at(PITCH_TYPE).compare(called) == 0)
				outerStrikeCount++;
			else
				outerBallCount++;
			if (outerBallCount == 4)
			{
				TextOut(hdc, 0, 175, "walk", strlen("walk"));
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
				TextOut(hdc, 700, 700, " ", 1);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				totalPlateAppearences++;
				totalWalks++;
			}
			else if (outerStrikeCount == 3)
			{
				TextOut(hdc, 0, 175, "strikeout", strlen("strikeout"));
				outerBallCount = 0, outerStrikeCount = 0;
				inPA = false;
				TextOut(hdc, 700, 700, " ", 1);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				totalPlateAppearences++;
				totalStrikeouts++;
			}
			TextOut(hdc, 700, 700, " ", 1);
			currentPitch.clear();
			i = 0;
			startValue = 0;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		myfile.close();
	}

	else cout << "Unable to open file";
}

void updateStats(HDC hdc)
{
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, OPAQUE);
	using namespace std;
	string ab = "PA: ";
	ab += to_string(totalPlateAppearences);
	TextOut(hdc, 0, 0, ab.c_str(), strlen(ab.c_str()));
	string bb = "BB: ";
	bb += to_string(totalWalks);
	TextOut(hdc, 0, 30, bb.c_str(), strlen(bb.c_str()));
	string hbp = "HBP: ";
	hbp += to_string(totalHitByPitches);
	TextOut(hdc, 0, 60, hbp.c_str(), strlen(hbp.c_str()));
	string k = "Strikeouts: ";
	k += to_string(totalStrikeouts);
	TextOut(hdc, 0, 90, k.c_str(), strlen(k.c_str()));
	string obp = "OBP: ";
	double calcOBP = ((double)(totalWalks + totalHitByPitches)/((double)totalPlateAppearences));
	obp += to_string(calcOBP);
	TextOut(hdc, 0, 120, obp.c_str(), strlen(obp.c_str()));
	string log = "Last plate appearence: ";
	TextOut(hdc, 0, 150, log.c_str(), strlen(log.c_str()));
}