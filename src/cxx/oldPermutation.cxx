#include "oldPermutation.hxx"
#include "util.hxx"

std::vector<std::vector<uint8_t> >
oldSubset (int subsetSize, std::vector<uint8_t> setOfNumbers)
{
  if (subsetSize % 2 != 0)
    {
      throw std::logic_error{ "only tested for even subsets" };
      return {};
    }
  auto subsets = std::vector<std::vector<uint8_t> >{};
  for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize / 2, setOfNumbers.end (), [&subsets] (auto begin, auto end) {
    if (std::is_sorted (begin, end))
      {
        subsets.push_back (std::vector<uint8_t> (begin, end));
      }
  });
  auto permutations = std::vector<std::vector<uint8_t> >{};
  for_each_permuted_combination (subsets.begin (), subsets.begin () + 2, subsets.end (), [&permutations] (auto begin, auto end) {
    auto permutation = std::vector<std::vector<uint8_t> > (begin, end);
    auto possibleResult = std::vector<uint8_t>{};
    std::copy (permutation.front ().begin (), permutation.front ().end (), std::back_inserter (possibleResult));
    std::copy (permutation.back ().begin (), permutation.back ().end (), std::back_inserter (possibleResult));
    auto copyOfPossibleResult = possibleResult;
    std::sort (copyOfPossibleResult.begin (), copyOfPossibleResult.end ());
    if (std::adjacent_find (copyOfPossibleResult.begin (), copyOfPossibleResult.end ()) == copyOfPossibleResult.end ())
      {
        permutations.push_back (possibleResult);
      }
  });
  return permutations;
}