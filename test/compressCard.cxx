#include "durak_computer_controlled_opponent/compressCard.hxx"
#include "durak_computer_controlled_opponent/permutation.hxx"
#include <catch2/catch.hpp>
#include <cstddef>

using namespace durak_computer_controlled_opponent;

TEST_CASE ("compress", "[abc]")
{
  SECTION ("4 different values", "[matchmaking]")
  {
    auto const result = compress ({ { 6, durak::Type::clubs }, { 11, durak::Type::clubs }, { 14, durak::Type::clubs }, { 8, durak::Type::hearts } });
    auto const desire = std::vector<durak::Card>{ { 0, durak::Type::clubs }, { 2, durak::Type::clubs }, { 3, durak::Type::clubs }, { 1, durak::Type::hearts } };
    REQUIRE (result == desire);
  }
  SECTION ("4 times the same value", "[matchmaking]")
  {
    auto const result = compress ({ { 6, durak::Type::clubs }, { 6, durak::Type::hearts }, { 6, durak::Type::diamonds }, { 6, durak::Type::spades } });
    auto const desire = std::vector<durak::Card>{ { 0, durak::Type::clubs }, { 0, durak::Type::hearts }, { 0, durak::Type::diamonds }, { 0, durak::Type::spades } };
    REQUIRE (result == desire);
  }
}

TEST_CASE ("compress card permutations", "[abc]")
{
  size_t const attackCardCount = 2;
  size_t const defendCardCount = 2;
  size_t n = 20;
  auto compressedCombinations = std::set<std::vector<uint8_t> >{};
  for_each_card_combination ({ attackCardCount, defendCardCount }, n, [&compressedCombinations] (std::vector<uint8_t> combi) {
    compressedCombinations.insert (cardsToIds (compress (idsToCards (std::move (combi)))));
    return false;
  });
  REQUIRE (compressedCombinations.size () == 3678);
}
