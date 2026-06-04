#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <fstream>
#include <random>

const int kParamsAmount = 4;
const int kPlayerBaseX = 50;
const int kPlayerBaseY = 50;
const int kTeamMin = 1;
const int kTeamMax = 3;
const int kCameraDX = 28;
const int kCameraDY = 4;
const int kMovingEntityAmount = 8;

const int kDirX[] = { 0, 0, -1, 1 };
const int kDirY[] = { -1, 1, 0, 0 };
const std::vector<char> kDirChar = { 'w', 's', 'a', 'd' };

enum State { Exploration, Battle };
enum Ability { Ability1, Ability2, Boss_Ability };
enum class Phase { ChooseAction, ChooseTarget, BattleOver };
enum Action { Attack, Heal, Defend };
enum StatIndex { HP, ATK, DEF, SPD, STAT_COUNT };

const std::vector<std::string> kActionEntries_ = {"Attack", "Heal", "Defend"};
const std::string kMapInfo = "assets/map.txt";
const std::string kAnimalInfo = "assets/stats.txt";