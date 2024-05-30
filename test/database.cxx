//
// Created by walde on 10/18/22.
//
#include "durak_computer_controlled_opponent/database.hxx"
#include "catch2/catch.hpp"
#include "durak_computer_controlled_opponent/solve.hxx"
#include <cstdint>
#include <stlplus/persistence/persistent_contexts.hpp>
#include <stlplus/persistence/persistent_pair.hpp>
#include <stlplus/persistence/persistent_vector.hpp>
#include <tuple>

using namespace durak_computer_controlled_opponent::database;
using namespace durak_computer_controlled_opponent;

std::filesystem::path const databasePath = std::string{ CURRENT_BINARY_DIR } + "/test_database/combination.db";

namespace durak_computer_controlled_opponent::database
{
std::string smallMemoryTreeDataHierarchyToBinary (std::vector<bool> const &hierarchy);

std::vector<bool> binaryToSmallMemoryTreeDataHierarchy (std::string hierarchyBinary);

std::vector<std::tuple<Action, Result> > binaryToSmallMemoryTreeDataData (std::string movesAndResultAsBinary);
std::string smallMemoryTreeDataDataToBinary (std::vector<std::tuple<Action, Result> > const &moveResults);

}

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
    auto result = confu_soci::findStruct<durak_computer_controlled_opponent::database::Round> (sql, "gameState", "0;1;1");
    REQUIRE (result.has_value ());
    auto game = gameLookup.find ({ 1, 1 })->second.at (1).find ({ { 0 }, { 1 } })->second;
    REQUIRE (result.value ().data == smallMemoryTreeDataDataToBinary (game.data));
    REQUIRE (result.value ().hierarchy == smallMemoryTreeDataHierarchyToBinary (game.hierarchy));
    REQUIRE (result.value ().maxChildren == game.maxChildren);
  }
  std::filesystem::remove_all (databasePath.parent_path ());
}

TEST_CASE ("gameStateAsString", "[database]")
{
  auto cards = std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >{ { 42 }, { 43 } };
  auto trump = durak::Type{};
  REQUIRE (gameStateAsString (cards, trump) == "42;43;0");
}

TEST_CASE ("binaryToSmallMemoryTreeDataData", "[database]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4>{ gameLookup.at ({ 1, 1 }) };
  auto moveResultBinary = smallMemoryTreeDataDataToBinary (oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }).data);
  auto test = binaryToSmallMemoryTreeDataData (moveResultBinary);
  REQUIRE (test.size () == 3);
}

TEST_CASE ("binaryToSmallMemoryTreeDataHierarchy", "[database]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  auto oneCardVsOneCard = std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTreeData<std::tuple<Action, Result> > >, 4>{ gameLookup.at ({ 1, 1 }) };
  auto moveResultBinary = smallMemoryTreeDataHierarchyToBinary (oneCardVsOneCard.at (0).at ({ { 0 }, { 1 } }).hierarchy);
  auto test = binaryToSmallMemoryTreeDataHierarchy (moveResultBinary);
  REQUIRE (test.size () == moveResultBinary.size ());
}
