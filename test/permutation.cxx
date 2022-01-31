#include "src/cxx/permutation.hxx"
#include "src/cxx/oldPermutation.hxx"
#include <catch2/catch.hpp>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

// TEST_CASE ("permut validation validation ", "[abc]")
// {
//   auto results = subset (6, 4);
//   for (auto const &[subset, combis] : results)
//     {
//       for (auto const &combi : combis)
//         {
//           std::copy (subset.begin (), subset.end (), std::ostream_iterator<int> (std::cout, " "));
//           std::copy (combi.begin (), combi.end (), std::ostream_iterator<int> (std::cout, " "));
//           std::cout << std::endl;
//         }
//       std::cout << std::endl;
//     }
// }


TEST_CASE ("subset benchmark ", "[abc]")
{
  auto const sixNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6 };
  auto const eightNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8 };
  auto const tenNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  auto const twelveNumbers = std::vector<uint8_t>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };

  BENCHMARK ("subset (4, sixNumbers)") { return subset (4, sixNumbers); };
  BENCHMARK ("oldSubset (4, sixNumbers)") { return oldSubset (4, sixNumbers); };
  BENCHMARK ("subset (4, eightNumbers)") { return subset (4, eightNumbers); };
  BENCHMARK ("oldSubset (4, eightNumbers)") { return oldSubset (4, eightNumbers); };
  BENCHMARK ("subset (6, eightNumbers)") { return subset (6, eightNumbers); };
  BENCHMARK ("oldSubset (6, eightNumbers)") { return oldSubset (6, eightNumbers); };
  BENCHMARK ("subset (4, tenNumbers)") { return subset (4, tenNumbers); };
  BENCHMARK ("oldSubset (4, tenNumbers)") { return oldSubset (4, tenNumbers); };
  BENCHMARK ("subset (6, tenNumbers)") { return subset (6, tenNumbers); };
  BENCHMARK ("oldSubset (6, tenNumbers)") { return oldSubset (6, tenNumbers); };
  BENCHMARK ("subset (8, tenNumbers)") { return subset (8, tenNumbers); };
  BENCHMARK ("oldSubset (8, tenNumbers)") { return oldSubset (8, tenNumbers); };
  BENCHMARK ("subset (4, twelveNumbers)") { return subset (4, twelveNumbers); };
  BENCHMARK ("oldSubset (4, twelveNumbers)") { return oldSubset (4, twelveNumbers); };
  BENCHMARK ("subset (6, twelveNumbers)") { return subset (6, twelveNumbers); };
  BENCHMARK ("oldSubset (6, twelveNumbers)") { return oldSubset (6, twelveNumbers); };
  BENCHMARK ("subset (8, twelveNumbers)") { return subset (8, twelveNumbers); };
  BENCHMARK ("oldSubset (8, twelveNumbers)") { return oldSubset (8, twelveNumbers); };
  BENCHMARK ("subset (10, twelveNumbers)") { return subset (10, twelveNumbers); };
  BENCHMARK ("oldSubset (10, twelveNumbers)") { return oldSubset (10, twelveNumbers); };
}
