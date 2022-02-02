#include "permutation.hxx"
#include "src/cxx/combination.hxx"
#include <boost/numeric/conversion/cast.hpp>
#include <cstddef>
#include <numeric>
#include <range/v3/algorithm.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

std::vector<std::vector<u_int8_t> >
combinationsNoRepetitionAndOrderDoesNotMatter (long int subsetSize, std::vector<uint8_t> setOfNumbers)
{
  std::vector<std::vector<u_int8_t> > subsets{};
  subsets.reserve (count_each_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize, setOfNumbers.end ()));
  for_each_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize, setOfNumbers.end (), [&subsets] (auto first, auto last) {
    subsets.push_back (std::vector<uint8_t>{ first, last });
    return false;
  });
  return subsets;
}

subsetAndCombinations
combinationsFor (std::vector<u_int8_t> const &numbersToCheck, std::vector<std::vector<u_int8_t> > const &subResults, std::vector<u_int8_t> const &indexes)
{
  auto result = subsetAndCombinations{};
  std::get<1> (result).reserve (subResults.size ());
  std::vector<u_int8_t> numbers (indexes.size () - numbersToCheck.size ());
  ranges::set_difference (indexes, numbersToCheck, numbers.begin ());
  ranges::transform (subResults, ranges::back_inserter (std::get<1> (result)), [&numbers] (auto indexes) {
    ranges::transform (indexes, indexes.begin (), [&numbers] (auto const &index) { return numbers[index]; });
    return indexes;
  });
  std::get<0> (result) = numbersToCheck;
  return result;
}

std::vector<subsetAndCombinations>
subset (long int k, size_t n)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto const &subResult = combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)));
  auto combineResult = std::vector<subsetAndCombinations>{};
  for (auto &&result : combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, indexes))
    {
      combineResult.emplace_back (combinationsFor (std::move (result), subResult, indexes));
    }
  return combineResult;
}

std::tuple<std::vector<std::vector<u_int8_t> >, std::vector<std::vector<u_int8_t> > >
subset2 (long int k, size_t n)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  return { combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, indexes), combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2))) };
}