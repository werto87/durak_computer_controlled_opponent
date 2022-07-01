#include "permutation.hxx"
#include "durak_computer_controlled_opponent/combination.hxx"
#include "durak_computer_controlled_opponent/compressCard.hxx"
#include <boost/numeric/conversion/cast.hpp>
#include <cstddef>
#include <numeric>
#include <range/v3/algorithm.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

namespace durak_computer_controlled_opponent
{

std::vector<std::vector<uint8_t> >
combinationsNoRepetitionAndOrderDoesNotMatter (long int subsetSize, std::vector<uint8_t> setOfNumbers)
{
  std::vector<std::vector<uint8_t> > subsets{};
  subsets.reserve (count_each_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize, setOfNumbers.end ()));
  for_each_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize, setOfNumbers.end (), [&subsets] (auto first, auto last) {
    subsets.push_back (std::vector<uint8_t>{ first, last });
    return false;
  });
  return subsets;
}

subsetAndCombinations
combinationsFor (std::vector<uint8_t> const &numbersToCheck, std::vector<std::vector<uint8_t> > const &subResults, std::vector<uint8_t> const &indexes)
{
  auto result = subsetAndCombinations{};
  std::get<1> (result).reserve (subResults.size ());
  std::vector<uint8_t> numbers (indexes.size () - numbersToCheck.size ());
  ranges::set_difference (indexes, numbersToCheck, numbers.begin ());
  ranges::transform (subResults, ranges::back_inserter (std::get<1> (result)), [&numbers] (auto indexes_) {
    ranges::transform (indexes_, indexes_.begin (), [&numbers] (auto const &index) { return numbers[index]; });
    return indexes_;
  });
  std::get<0> (result) = numbersToCheck;
  return result;
}

void
for_each_card_combination (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n, std::function<bool (std::vector<uint8_t>)> callThis)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto results = numbersToCombine (attackAndDefendCardCount, n);
  for (auto &&result : get<0> (results))
    {
      auto const [subset, combis] = combinationsFor (result, std::get<1> (results), indexes);
      for (auto const &combi : combis)
        {
          auto temp = std::vector<uint8_t>{};
          temp.insert (temp.end (), subset.begin (), subset.end ());
          temp.insert (temp.end (), combi.begin (), combi.end ());
          if (callThis (std::move (temp)))
            {
              break;
            }
        }
    }
}

std::set<std::vector<uint8_t> >
compressed_permutations (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n)
{
  auto compressedCombinations = std::set<std::vector<uint8_t> >{};
  for_each_card_combination (attackAndDefendCardCount, n, [&compressedCombinations] (std::vector<uint8_t> combi) {
    compressedCombinations.insert (cardsToIds (compress (idsToCards (std::move (combi)))));
    return false;
  });
  return compressedCombinations;
}

std::tuple<std::vector<std::vector<uint8_t> >, std::vector<std::vector<uint8_t> > >
numbersToCombine (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  return { combinationsNoRepetitionAndOrderDoesNotMatter (static_cast<long> (std::get<0> (attackAndDefendCardCount)), indexes), combinationsNoRepetitionAndOrderDoesNotMatter (static_cast<long> (std::get<1> (attackAndDefendCardCount)), std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (static_cast<long> (std::get<0> (attackAndDefendCardCount))))) };
}
}