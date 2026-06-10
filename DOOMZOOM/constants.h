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
const int kHealingEffect = 10;
const int kDefendingEffect = 5;
const int kFlowersPerBattle = 3;
const int kEntityMoveChance = 50;

const int kDirX[] = { 0, 0, -1, 1 };
const int kDirY[] = { -1, 1, 0, 0 };
const std::vector<char> kDirChar = { 'w', 's', 'a', 'd' };

enum State { Exploration, Battle, GameOver };
enum Phase { ChooseAction, ChooseTarget, BattleOver };
enum AbilityTarget {Enemy, Self, Comrade, AllEnemies};
enum Action { Attack, Heal, Defend, Ability1, Ability2 };
enum StatIndex { HP, ATK, DEF, SPD, STAT_COUNT };

const std::vector<std::string> kActionEntries_ = {"Attack", "Heal", "Defend",
                                                  "Ability 1", "Ability 2"};
const std::string kMapInfo = "assets/map.txt";
const std::string kAnimalStats = "assets/stats.txt";
const std::string kAnimalAbilities = "assets/abilities.txt";
const std::string kDescriptions = "assets/descriptions.txt";