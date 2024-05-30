//
// Created by walde on 10/18/22.
//
#include "durak_computer_controlled_opponent/database.hxx"
#include "catch2/catch.hpp"

using namespace durak_computer_controlled_opponent::database;
using namespace durak_computer_controlled_opponent;

std::filesystem::path const databasePath = std::string{ CURRENT_BINARY_DIR } + "/test_database/combination.db";

TEST_CASE ("database", "[database]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  SECTION ("createDatabaseIfNotExist", "[database]")
  {
    REQUIRE_FALSE (std::filesystem::exists (databasePath));
    createDatabaseIfNotExist (databasePath);
    REQUIRE (std::filesystem::exists (databasePath));
  }
  SECTION ("deleteDatabaseAndCreateNewDatabase", "[database]")
  {
    createDatabaseIfNotExist (databasePath);
    REQUIRE (std::filesystem::exists (databasePath));
    deleteDatabaseAndCreateNewDatabase (databasePath);
    REQUIRE (std::filesystem::exists (databasePath));
  }
  SECTION ("createTables", "[database]")
  {
    createDatabaseIfNotExist (databasePath);
    auto oldSize = std::filesystem::file_size (databasePath);
    createTables (databasePath);
    REQUIRE (oldSize < std::filesystem::file_size (databasePath));
  }
  SECTION ("insertGameLookUp", "[database]")
  {
    createDatabaseIfNotExist (databasePath);
    createTables (databasePath);
    auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> >{};
    gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
    insertGameLookUp (databasePath, gameLookup);
    soci::session sql (soci::sqlite3, databasePath);
    REQUIRE (confu_soci::findStruct<durak_computer_controlled_opponent::database::Round> (sql, "gameState", "0;1;1").has_value ());
  }
  std::filesystem::remove_all (databasePath.parent_path ());
}

TEST_CASE ("gameStateAsString", "[database]")
{
  auto cards = std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >{ { 42 }, { 43 } };
  auto trump = durak::Type{};
  REQUIRE (gameStateAsString (cards, trump) == "42;43;0");
}

TEST_CASE ("binaryToMoveResult", "[database]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4>{ gameLookup.at ({ 1, 1 }) };
  auto moveResultBinary = moveResultToBinary (oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }).data);
  auto test = binaryToMoveResult (moveResultBinary);
  REQUIRE (test.data.size () == 5);
}