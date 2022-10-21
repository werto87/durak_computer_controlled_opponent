//
// Created by walde on 10/18/22.
//
#include "catch2/catch.hpp"
#include "durak_computer_controlled_opponent/database.hxx"

using namespace durak_computer_controlled_opponent::database;
using namespace durak_computer_controlled_opponent;

std::filesystem::path const databasePath = std::string{ CURRENT_BINARY_DIR } + "/test_database/combination.db";

TEST_CASE ("database", "[database]")
{
  std::filesystem::remove_all(databasePath.parent_path());
  SECTION ("createEmptyDatabase", "[database]")
  {
    deleteDatabaseAndCreateNewDatabase (databasePath);
  }
  SECTION ("createDatabaseIfNotExist", "[database]")
  {
    createDatabaseIfNotExist (databasePath);
  }
  SECTION ("createTables", "[database]")
  {
    createDatabaseIfNotExist (databasePath);
    createTables (databasePath);
  }
  SECTION ("gameStateAsString", "[database]")
  {
    auto cards=std::tuple<std::vector<uint8_t>, std::vector<uint8_t> > {};
    auto trump=durak::Type{};
    auto test= gameStateAsString (cards,trump );
  }

  SECTION ("binaryToMoveResult", "[database]")
  {
    auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
    gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
    auto oneCardVsOneCard= std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> {gameLookup.at ({1,1})};
    auto moveResultBinary=moveResultToBinary( oneCardVsOneCard.at(0).at ({{0},{1}}));
    auto test=binaryToMoveResult (moveResultBinary);
    REQUIRE(test.size()==6);
  }

  SECTION ("insertGameLookUp", "[database]")
  {
    //  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
    //  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
    //  insertGameLookUp ("",gameLookup);
  }
  std::filesystem::remove_all(databasePath.parent_path());
}

