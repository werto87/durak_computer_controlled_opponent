//
// Created by walde on 10/18/22.
//
#include "catch2/catch.hpp"
#include "durak_computer_controlled_opponent/database.hxx"

using namespace durak_computer_controlled_opponent;
TEST_CASE ("moveResultToBinary", "[util]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard= std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> {gameLookup.at ({1,1})};
  auto moveResultBinary=moveResultToBinary( oneCardVsOneCard.at(0).at ({{0},{1}}));
  auto test=binaryToMoveResult (moveResultBinary);
  REQUIRE(test.size()==6);
}