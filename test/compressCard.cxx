#include "src/cxx/compressCard.hxx"
#include "src/cxx/permutation.hxx"
#include <catch2/catch.hpp>
#include <cstddef>

TEST_CASE ("compress ", "[abc]")
{
  size_t k = 4;
  size_t n = 20;
  auto compressedCombinations = std::set<std::vector<uint8_t> >{};
  for_each_card_combination (k, n, [&compressedCombinations] (std::vector<uint8_t> combi) {
    compressedCombinations.insert (cardsToIds (compress (idsToCards (combi))));
    return false;
  });
  REQUIRE (compressedCombinations.size () == 3678);
}