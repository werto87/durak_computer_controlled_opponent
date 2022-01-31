#include "permutation.hxx"



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
combinationsFor (std::vector<u_int8_t> const &numbersToCheck, std::vector<std::vector<u_int8_t> > const &subResults, std::vector<u_int8_t> const &numbersToChoseFrom)
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
subset (long int k, std::vector<uint8_t> const &numbersToChoseFrom)
{
  auto results = subsetPermutation (k, numbersToChoseFrom);
  auto subResult = subsetPermutation (k, std::vector<uint8_t> (numbersToChoseFrom.begin (), numbersToChoseFrom.begin () + static_cast<long int> (numbersToChoseFrom.size ()) - (k / 2)));
  auto combineResult = std::vector<subsetAndCombinations>{};
  for (auto result : results)
    {
      combineResult.push_back (combinationsFor (result, subResult, numbersToChoseFrom));
    }
  return combineResult;
}