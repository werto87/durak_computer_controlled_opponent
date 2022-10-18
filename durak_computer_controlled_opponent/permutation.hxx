#ifndef AD23FB0A_B3C3_406D_A401_3231159AE486
#define AD23FB0A_B3C3_406D_A401_3231159AE486

#include "util.hxx"
#include <boost/multiprecision/cpp_int.hpp>
#include <durak/card.hxx>
#include <set>
#include <vector>

namespace durak_computer_controlled_opponent
{

typedef std::tuple<std::vector<uint8_t>, std::vector<std::vector<uint8_t> > > subsetAndCombinations;

std::vector<std::vector<uint8_t> > combinationsNoRepetitionAndOrderDoesNotMatter (long int subsetSize, std::vector<uint8_t> setOfNumbers);

subsetAndCombinations combinationsFor (std::vector<uint8_t> const &numbersToCheck, std::vector<std::vector<uint8_t> > const &subResults, std::vector<uint8_t> const &indexes);

// calls "callThis" for every card_combination. If callThis returns true stops.
void for_each_card_combination (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n, const std::function<bool (std::vector<uint8_t>)>& callThis);

std::set<std::vector<uint8_t> > compressed_permutations (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n);

std::tuple<std::vector<std::vector<uint8_t> >, std::vector<std::vector<uint8_t> > > numbersToCombine (std::tuple<size_t, size_t> const &attackAndDefendCardCount, size_t n);

constexpr boost::multiprecision::uint256_t
factorial (size_t n)
{
  unsigned i = 1;
  boost::multiprecision::uint256_t factorial = 1;
  while (i < n)
    {
      ++i;
      factorial *= i;
    }
  return factorial;
}

constexpr boost::multiprecision::uint256_t
countCombinationsNoRepetitionAndOrderDoesNotMatter (size_t setOfNumbersSize, size_t subsetSize)
{
  return factorial (setOfNumbersSize) / (factorial (subsetSize) * factorial (setOfNumbersSize - subsetSize));
}

size_t constexpr
combinations (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize / 2)) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

size_t constexpr
combinations2 (size_t setOfNumbersSize, size_t subsetSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize)) * factorial (subsetSize / 2) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

}
#endif /* AD23FB0A_B3C3_406D_A401_3231159AE486 */
