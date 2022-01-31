#include "permutation.hxx"
#include <cstddef>
#include <numeric>

std::vector<std::vector<u_int8_t> >
subsetPermutation (long int subsetSize, std::vector<uint8_t> setOfNumbers)
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

subsetAndCombinations
combinationsFor (std::vector<u_int8_t> const &numbersToCheck, std::vector<std::vector<u_int8_t> > const &subResults, std::vector<u_int8_t> const &indexes)
{
  auto result = std::tuple<std::vector<u_int8_t>, std::vector<std::vector<u_int8_t> > >{};
  std::get<0> (result) = numbersToCheck;
  std::get<1> (result).reserve (subResults.size ());
  std::vector<u_int8_t> numbers (indexes.size () - numbersToCheck.size ());
  std::set_difference (indexes.begin (), indexes.end (), numbersToCheck.begin (), numbersToCheck.end (), numbers.begin ());
  std::transform (subResults.begin (), subResults.end (), std::back_inserter (std::get<1> (result)), [&numbers] (auto indexes) {
    std::transform (indexes.begin (), indexes.end (), indexes.begin (), [&numbers] (auto index) { return numbers[index]; });
    return indexes;
  });
  return result;
}

std::vector<subsetAndCombinations>
subset (long int k, size_t n)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto results = subsetPermutation (k, indexes);
  auto subResult = subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)));
  auto combineResult = std::vector<subsetAndCombinations>{};
  for (auto result : results)
    {
      combineResult.push_back (combinationsFor (result, subResult, indexes));
    }
  return combineResult;
}