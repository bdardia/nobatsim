#pragma once

void readData(HDC hdc);
void updateStats(HDC hdc);

const int PITCH_TYPE = 0;
const int BALL_COUNT = 1;
const int STRIKE_COUNT = 2;
const int PLATE_X = 3;
const int PLATE_Y = 4;
const int ZONE_TOP = 5;
const int ZONE_BOTTOM = 6;
const std::string currentField[7] = { "Pitch type", "Balls in count", "Strikes in count", "Plate x-coordinate", "Plate y-coordinate", "Top of zone y-coordinate", "Bottom of zone y-coordinate" };

static int totalStrikeouts = 0;
static int totalWalks = 0;
static int totalPlateAppearences = 0;
static int totalHitByPitches = 0;
static double onBasePercentage = 0.0;