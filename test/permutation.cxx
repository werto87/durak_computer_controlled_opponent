#include "src/cxx/permutation.hxx"
#include "src/cxx/combination.hxx"
#include "src/cxx/oldPermutation.hxx"
// #include "src/cxx/util.hxx"
#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

auto const sixNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6 };
auto const eightNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8 };
auto const tenNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
auto const twelveNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

TEST_CASE ("permut validation validation ", "[abc]")
{
  auto results = subset (4, 6);
  REQUIRE (results.size () * get<1> (results.at (0)).size () == 90);
  // print results
  for (auto const &[subset, combis] : results)
    {
      for (auto const &combi : combis)
        {
          std::copy (subset.begin (), subset.end (), std::ostream_iterator<int> (std::cout, " "));
          std::copy (combi.begin (), combi.end (), std::ostream_iterator<int> (std::cout, " "));
          std::cout << std::endl;
        }
      std::cout << std::endl;
    }
}

#ifdef RUN_BENCHMARK
TEST_CASE ("subset benchmark ", "[abc]")
{
  // BENCHMARK ("subset (4, 6)") { return subset (4, 6); };
  // BENCHMARK ("subset (4, 8)") { return subset (4, 8); };
  // BENCHMARK ("subset (6, 8)") { return subset (6, 8); };
  // BENCHMARK ("subset (4, 10)") { return subset (4, 10); };
  // BENCHMARK ("subset (6, 10)") { return subset (6, 10); };
  // BENCHMARK ("subset (8, 10)") { return subset (8, 10); };
  // BENCHMARK ("subset (4, 12)") { return subset (4, 12); };
  // BENCHMARK ("subset (6, 12)") { return subset (6, 12); };
  // BENCHMARK ("subset (8, 12)") { return subset (8, 12); };
  // BENCHMARK ("subset (10, 12)") { return subset (10, 12); };
  // BENCHMARK ("subset (10, 14)") { return subset (10, 14); };
  // BENCHMARK ("subset (12, 14)") { return subset (12, 14); };
  // BENCHMARK ("subset (12, 36)") { return subset (12, 36); };
  // BENCHMARK ("subset2 (10, 12)")
  // {
  //
  auto n = 36;
  auto r = 12;

  // for (auto &&result : results)
  //   {
  //     combineResult.emplace_back (combinationsFor (result, subResult, indexes));
  //   }

  // BENCHMARK ("subset2 (12, 36)") { return subset2 (r, n); };
  auto subset2Result = subset2 (r, n);
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  BENCHMARK ("subset2 (12, 36)") { return combinationsFor (get<0> (subset2Result).at (0), get<1> (subset2Result), indexes); };
  // // TODO it looks like it could be possible to calculate all the subsets in (1947792 * 0.023)/3600) 12.44 hours
  // //  TODO memory 7 terabyte :(
  // combineResult.push_back (combinationsFor (std::move (get<0> (subset2Result).at (0)), get<1> (subset2Result), indexes));
  // std::cout << get<0> (subset2Result).size () << std::endl;
  // combineResult.emplace_back (combinationsFor (results.at (0), subResult, indexes));
  // combineResult.emplace_back (combinationsFor (results.at (1), subResult, indexes));

  // };
}

TEST_CASE ("combinationsFor benchmark ", "[abc]")
{
  // auto k = 12;
  // auto n = 14;
  // auto indexes = std::vector<uint8_t> (n);
  // std::iota (indexes.begin (), indexes.end (), 0);
  // auto results = subsetPermutation (k, indexes); // THIS TAKES 90% of the time
  // const int r = 3;
  // const int n = 5;
  // std::vector<int> v (n);
  // std::iota (v.begin (), v.end (), 0);
  // std::uint64_t count = for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ()));
  // std::cout << count << std::endl;
  // CHECK (for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_reversible_circular_permutation (r, n));
  // CHECK (for_each_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_permutation (r, n));

  // BENCHMARK ("subsetPermutation (k, indexes)") { return subsetPermutation (k, indexes); };
  // BENCHMARK ("subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)))") { return subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2))); };
  // auto const &subResult = subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)));
  // auto combineResult = std::vector<subsetAndCombinations>{};
  // BENCHMARK ("combinationsFor (12, 14)")
  // {
  //   for (auto &&result : results)
  //     {
  //       combineResult.emplace_back (combinationsFor (std::move (result), subResult, indexes));
  //     }
  // };

  // BENCHMARK ("for_each_reversible_circular_permutation")
  // {
  //   size_t n = 36;
  //   size_t r = 6;
  //   std::vector<uint8_t> v (n);
  //   std::iota (v.begin (), v.end (), 0);
  //   auto results = std::vector<std::vector<uint8_t> >{};
  //   results.reserve (boost::numeric_cast<size_t> (combinationsNoRepetitionAndOrderDoesNotMatter (n, r)));
  //   for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), [&results, n, r] (auto first, auto last) {
  //     if (std::is_sorted (first, last))
  //       {
  //         results.push_back (std::vector<uint8_t>{ first, last });
  //         return results.size () == combinationsNoRepetitionAndOrderDoesNotMatter (n, r);
  //       }
  //     else
  //       {
  //         return false;
  //       }
  //   });
  // std::cout << combinationsNoRepetitionOrderDoesNotMatter (n, r) << std::endl;
  // std::cout << results.size () << std::endl;
  // };
}
#endif