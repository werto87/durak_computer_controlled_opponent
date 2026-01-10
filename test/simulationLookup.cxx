#include "durak_computer_controlled_opponent/simulationLookup.hxx"
#include "durak_computer_controlled_opponent/simulation/action.hxx"
#include "durak_computer_controlled_opponent/simulation/database.hxx"
#include <catch2/catch.hpp>
#include <durak/game.hxx>

std::filesystem::path const databasePath = std::string{ CURRENT_BINARY_DIR } + "/test_database/combination.db";

using namespace durak_computer_controlled_opponent::simulation_lookup;
using namespace durak_computer_controlled_opponent::database;
using namespace durak_computer_controlled_opponent;

TEST_CASE ("nextMoveToPlayForRole database does not exist", "[abc]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  auto game = durak::Game{};
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).error () == NextMoveToPlayForRoleError::databaseDoesNotExist);
}

TEST_CASE ("nextMoveToPlayForRole database exists but has no table", "[abc]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  createDatabaseIfNotExist (databasePath);
  auto game = durak::Game{};
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).error () == NextMoveToPlayForRoleError::databaseMissingTable);
}

TEST_CASE ("nextMoveToPlayForRole database exists has table but no data", "[abc]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  createDatabaseIfNotExist (databasePath);
  createTables (databasePath);
  auto game = durak::Game{};
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).error () == NextMoveToPlayForRoleError::gameNotInLookupTable);
}

TEST_CASE ("nextMoveToPlayForRole database exists has table game in database", "[abc]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  createDatabaseIfNotExist (databasePath);
  createTables (databasePath);
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { uint8_t{ 1 }, uint8_t{ 1 } }, durak_computer_controlled_opponent::solveDurak (36, 1, 1, gameLookup) });
  insertGameLookUp (databasePath, gameLookup);
  auto gameOption = durak::GameOption{};
  gameOption.customCardDeck = std::vector<durak::Card>{};
  gameOption.cardsInHands = std::vector<std::vector<durak::Card> >{ { { 1, durak::Type::clubs } }, { { 1, durak::Type::hearts } } };
  auto game = durak::Game{ { "a", "b" }, gameOption };
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).has_value ());
}

TEST_CASE ("nextMoveToPlayForRole attack defend next move pass", "[.slow]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  createDatabaseIfNotExist (databasePath);
  createTables (databasePath);
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { uint8_t{ 1 }, uint8_t{ 1 } }, durak_computer_controlled_opponent::solveDurak (36, 1, 1, gameLookup, { durak::Type::diamonds }) });
  gameLookup.insert ({ { uint8_t{ 2 }, uint8_t{ 2 } }, solveDurak (36, 2, 2, gameLookup, { durak::Type::diamonds }) });
  insertGameLookUp (databasePath, gameLookup);
  auto game = durak::Game{ { "a", "b" }, durak::GameOption{ .numberOfCardsPlayerShouldHave = 2, .trump = durak::Type::diamonds, .customCardDeck = std::vector<durak::Card>{ { 7, durak::Type::clubs }, { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } };
  game.playerStartsAttack ({ { 3, durak::Type::clubs } });
  game.playerDefends ({ 3, durak::Type::clubs }, { 7, durak::Type::clubs });
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).has_value ());
  REQUIRE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::defend).has_value ());
}

TEST_CASE ("nextMoveToPlayForRole attack 1vs3 cards nextMoveToPlayForRole for attack", "[.slow]")
{
  std::filesystem::remove_all (databasePath.parent_path ());
  createDatabaseIfNotExist (databasePath);
  createTables (databasePath);
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, small_memory_tree::SmallMemoryTree<std::tuple<Action, Result> > >, 4> >{};
  gameLookup.insert ({ { uint8_t{ 1 }, uint8_t{ 1 } }, durak_computer_controlled_opponent::solveDurak (36, 1, 1, gameLookup, { durak::Type::diamonds }) });
  gameLookup.insert ({ { uint8_t{ 1 }, uint8_t{ 3 } }, solveDurak (36, 1, 3, gameLookup, { durak::Type::diamonds }) });
  insertGameLookUp (databasePath, gameLookup);
  auto game = durak::Game{ { "a", "b" }, durak::GameOption{ .trump = durak::Type::diamonds, .cardsInHands = std::vector<std::vector<durak::Card> >{ { { 7, durak::Type::clubs } }, { { 8, durak::Type::clubs }, { 3, durak::Type::hearts }, { 3, durak::Type::clubs } } } } };
  game.playerStartsAttack ({ { 7, durak::Type::clubs } });
  REQUIRE_FALSE (nextMoveToPlayForRole (databasePath, game, durak::PlayerRole::attack).has_value ());
}
