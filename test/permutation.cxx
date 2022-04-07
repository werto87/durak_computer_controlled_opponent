#include "src/cxx/permutation.hxx"
#include "src/cxx/combination.hxx"
// #include "src/cxx/util.hxx"
#include "src/cxx/compressCard.hxx"
#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <cstdint>
#include <durak/card.hxx>
#include <durak/print.hxx>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <iostream>
#include <numeric>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/replace.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range.hpp>
#include <tuple>
#include <vector>

TEST_CASE ("permut validation validation ", "[abc]")
{
  size_t const attackCardCount = 2;
  size_t const defendCardCount = 2;
  size_t n = 20;
  auto combinations = std::vector<std::vector<uint8_t> >{};
  for_each_card_combination ({ attackCardCount, defendCardCount }, n, [&combinations] (std::vector<uint8_t> combi) {
    combinations.push_back (std::move (combi));
    return false;
  });
  REQUIRE (combinations.size () == 29070);
}

TEST_CASE ("compressed_permutations 2v2 ", "[abc]")
{
  size_t const attackCardCount = 2;
  size_t const defendCardCount = 2;
  REQUIRE (compressed_permutations ({ attackCardCount, defendCardCount }, 20).size () == 3678);
}
TEST_CASE ("compressed_permutations 1v3 ", "[abc]")
{
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 3;
  REQUIRE (compressed_permutations ({ attackCardCount, defendCardCount }, 20).size () == 2452);
}

TEST_CASE ("compressed_permutations 3v1 ", "[abc]")
{
  size_t const attackCardCount = 3;
  size_t const defendCardCount = 1;
  REQUIRE (compressed_permutations ({ attackCardCount, defendCardCount }, 20).size () == 2452);
}

#ifdef RUN_BENCHMARK
TEST_CASE ("subset benchmark ", "[abc]")
{
  BENCHMARK ("subset (4, 6)") { return subset (4, 6); };
  BENCHMARK ("subset (4, 8)") { return subset (4, 8); };
  BENCHMARK ("subset (6, 8)") { return subset (6, 8); };
  BENCHMARK ("subset (4, 10)") { return subset (4, 10); };
  BENCHMARK ("subset (6, 10)") { return subset (6, 10); };
  BENCHMARK ("subset (8, 10)") { return subset (8, 10); };
  BENCHMARK ("subset (4, 12)") { return subset (4, 12); };
  BENCHMARK ("subset (6, 12)") { return subset (6, 12); };
  BENCHMARK ("subset (8, 12)") { return subset (8, 12); };
  BENCHMARK ("subset (10, 12)") { return subset (10, 12); };
  BENCHMARK ("subset (10, 14)") { return subset (10, 14); };
  BENCHMARK ("subset (12, 14)") { return subset (12, 14); };
  BENCHMARK ("subset (12, 36)") { return subset (12, 36); };
  auto n = 36;
  auto r = 12;
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto numbersToCheck = combinationsNoRepetitionAndOrderDoesNotMatter (r / 2, indexes);
  auto subResults = combinationsNoRepetitionAndOrderDoesNotMatter (r / 2, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (r / 2)));
  BENCHMARK ("subset2 (12, 36)") { return combinationsFor (numbersToCheck.at (0), subResults, indexes); };
}

TEST_CASE ("combinationsFor benchmark ", "[abc]")
{
  auto k = 12;
  auto n = 14;
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto results = subsetPermutation (k, indexes); // THIS TAKES 90% of the time
  const int r = 3;
  const int n = 5;
  std::vector<int> v (n);
  std::iota (v.begin (), v.end (), 0);
  std::uint64_t count = for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ()));
  std::cout << count << std::endl;
  CHECK (for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_reversible_circular_permutation (r, n));
  CHECK (for_each_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_permutation (r, n));

  BENCHMARK ("subsetPermutation (k, indexes)") { return subsetPermutation (k, indexes); };
  BENCHMARK ("subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)))") { return subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2))); };
  auto const &subResult = subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)));
  auto combineResult = std::vector<subsetAndCombinations>{};
  BENCHMARK ("combinationsFor (12, 14)")
  {
    for (auto &&result : results)
      {
        combineResult.emplace_back (combinationsFor (result, subResult, indexes));
      }
  };

  BENCHMARK ("for_each_reversible_circular_permutation")
  {
    size_t n = 36;
    size_t r = 6;
    std::vector<uint8_t> v (n);
    std::iota (v.begin (), v.end (), 0);
    auto results = std::vector<std::vector<uint8_t> >{};
    results.reserve (boost::numeric_cast<size_t> (combinationsNoRepetitionAndOrderDoesNotMatter (n, r)));
    for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), [&results, n, r] (auto first, auto last) {
      if (std::is_sorted (first, last))
        {
          results.push_back (std::vector<uint8_t>{ first, last });
          return results.size () == combinationsNoRepetitionAndOrderDoesNotMatter (n, r);
        }
      else
        {
          return false;
        }
    });
    std::cout << combinationsNoRepetitionOrderDoesNotMatter (n, r) << std::endl;
    std::cout << results.size () << std::endl;
  };
}
#endif
