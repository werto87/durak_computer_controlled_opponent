#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "test/constant.hxx"
#include <array>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <catch2/catch.hpp>
#include <durak/game.hxx>
#include <durak/print.hxx>
#include <execution>
#include <iomanip>
#include <iostream>
#include <range/v3/range.hpp>
#include <tuple>
#include <vector>
template <typename Bidi, typename Functor>
constexpr void
for_each_permuted_combination (Bidi begin, Bidi middle, Bidi end, Functor func)
{
  do
    {
      func (begin, middle);
      std::reverse (middle, end);
    }
  while (std::next_permutation (begin, end));
}

constexpr boost::multiprecision::uint128_t
factorial (size_t n)
{
  unsigned i = 1;
  boost::multiprecision::uint128_t factorial = 1;
  while (i < n)
    {
      ++i;
      factorial *= i;
    }
  return factorial;
}

size_t constexpr
combintions (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize / 2)) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

size_t constexpr
combintions2 (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize)) * factorial (subsetSize / 2) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

std::vector<std::vector<u_int8_t> >
subset (size_t subsetSize, std::vector<uint8_t> setOfNumbers)
{
  if (subsetSize % 2 != 0) return {};
  std::vector<std::vector<u_int8_t> > subsets{};
  for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize / 2, setOfNumbers.end (), [&subsets] (auto begin, auto end) mutable {
    if (std::is_sorted (begin, end))
      {
        subsets.push_back ({ begin, end });
      }
  });
  return subsets;
}

typedef std::tuple<std::vector<u_int8_t>, std::vector<std::vector<u_int8_t> > > subsetAndCombinations;

subsetAndCombinations
combinationsFor (size_t subsetSize, std::vector<u_int8_t> const &numbersToCheck, std::vector<std::vector<u_int8_t> > const &subResults, std::vector<u_int8_t> const &numbersToChoseFrom)
{
  auto result = std::tuple<std::vector<u_int8_t>, std::vector<std::vector<u_int8_t> > >{};
  std::get<0> (result) = numbersToCheck;
  std::get<1> (result).reserve (subResults.size ());
  std::vector<u_int8_t> numbers (numbersToChoseFrom.size () - numbersToCheck.size ());
  std::set_difference (numbersToChoseFrom.begin (), numbersToChoseFrom.end (), numbersToCheck.begin (), numbersToCheck.end (), numbers.begin ());
  std::transform (subResults.begin (), subResults.end (), std::back_inserter (std::get<1> (result)), [&numbers] (auto indexes) {
    std::transform (indexes.begin (), indexes.end (), indexes.begin (), [&numbers] (auto index) { return numbers[index]; });
    return indexes;
  });
  return result;
}

std::vector<subsetAndCombinations>
combine (size_t n, size_t k)
{
  auto numbersToChoseFrom = std::vector<uint8_t> (n);
  std::iota (numbersToChoseFrom.begin (), numbersToChoseFrom.end (), 0);
  auto results = subset (k, numbersToChoseFrom);
  auto subResult = subset (k, std::vector<uint8_t> (numbersToChoseFrom.begin (), numbersToChoseFrom.begin () + (n - (k / 2))));
  auto combineResult = std::vector<subsetAndCombinations>{};
  for (auto result : results)
    {
      combineResult.push_back (combinationsFor (n, result, subResult, numbersToChoseFrom));
    }
  return combineResult;
}

TEST_CASE ("permut validation validation ", "[abc]")
{
  auto results = combine (6, 4);
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