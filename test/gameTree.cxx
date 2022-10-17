#include "durak_computer_controlled_opponent/permutation.hxx"
#include "durak_computer_controlled_opponent/solve.hxx"
#include <catch2/catch.hpp>
#include <cstdint>
#include <durak/card.hxx>
#include <functional>
#include <magic_enum.hpp>
#include <st_tree.h>

using namespace durak_computer_controlled_opponent;

    TEST_CASE ("insertDrawCardsAction", "[abc]")
{
  std::vector<durak::Card> attackCards{ { 0, durak::Type::hearts }, { 0, durak::Type::clubs }, { 0, durak::Type::diamonds } };
  std::vector<std::vector<Action> > vectorsOfActions{ { { 0 }, { 1 }, { 2 } } };
  REQUIRE (insertDrawCardsAction (attackCards, vectorsOfActions).at (0).size () == 6);
}

TEST_CASE ("simulate round ", "[abc]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
  // auto start = std::chrono::system_clock::now ();
  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  // gameLookup.insert ({ { 2, 2 }, solveDurak (36, 2, 2, gameLookup) });
  // gameLookup.insert ({ { 3, 1 }, solveDurak (36, 3, 1, gameLookup) });
  // gameLookup.insert ({ { 2, 4 }, solveDurak (36, 2, 4, gameLookup) });
  // gameLookup.insert ({ { 3, 3 }, solveDurak (36, 3, 3, gameLookup) });
  // gameLookup.insert ({ { 4, 2 }, solveDurak (36, 4, 2, gameLookup) });
  // gameLookup.insert ({ { 5, 1 }, solveDurak (36, 5, 1, gameLookup) });
  // gameLookup.insert ({ { 2, 6 }, solveDurak (36, 2, 6, gameLookup) });
  // gameLookup.insert ({ { 3, 5 }, solveDurak (36, 3, 5, gameLookup) });
  // gameLookup.insert ({ { 4, 4 }, solveDurak (36, 4, 4, gameLookup) });
  REQUIRE (gameLookup.at ({ 1, 1 }).at (0).size () == 44);
  REQUIRE (std::get<1> (gameLookup.at ({ 1, 1 }).at (0).at ({ { 0 }, { 4 } }).at (0)) == Result::Draw);
}

#ifdef RUN_BENCHMARK_SOLVE_DURAK
TEST_CASE ("solveDurak benchmark", "[abc]")
{
  BENCHMARK ("solveDurak 2v2")
  {
    auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::map<std::tuple<std::vector<uint8_t>, std::vector<uint8_t> >, std::vector<std::tuple<uint8_t, Result> > >, 4> >{};
    gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
    gameLookup.insert ({ { 2, 2 }, solveDurak (36, 2, 2, gameLookup) });
    gameLookup.insert ({ { 3, 1 }, solveDurak (36, 3, 1, gameLookup) });
    gameLookup.insert ({ { 2, 4 }, solveDurak (36, 2, 4, gameLookup) });
    return gameLookup;
  };
}
#endif
