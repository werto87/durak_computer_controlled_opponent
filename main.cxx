#include <array>
#include <boost/json/src.hpp> // this file should be included only in one translation unit
#include <boost/math/special_functions/factorials.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <durak/game.hxx>
#include <durak/print.hxx>
#include <iomanip>
#include <iostream>
#include <range/v3/range.hpp>

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

template <size_t setOfNumbersSize, size_t subsetSize>
std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> constexpr
subset (std::array<uint8_t, setOfNumbersSize> setOfNumbers)
{
  if (subsetSize % 2 != 0) return {};
  std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets;
  for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize / 2, setOfNumbers.end (), [&subsets, elementCount = 0U] (auto begin, auto end) mutable {
    if (std::is_sorted (begin, end))
      {
        for (auto i = 0U; begin != end; i++, begin++)
          {
            subsets.at (elementCount).at (i) = *begin;
          }
        elementCount++;
      }
  });
  return subsets;
}

template <size_t setOfNumbersSize, size_t subsetSize>
constexpr std::array<std::array<uint8_t, subsetSize>, combintions2 (setOfNumbersSize, subsetSize)>
permutations (std::array<std::array<uint8_t, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets)
{
  auto resultsToMirror = std::array<std::array<uint8_t, subsetSize>, combintions2 (setOfNumbersSize, subsetSize) / 2>{};
  auto resultCount = 0UL;
  auto itr = 0UL;
  for (auto subset : subsets)
    {
      auto const numbersNotInArray = [&subset] (auto const &array) {
        for (auto number : subset)
          {
            if (std::binary_search (array.begin (), array.end (), number)) return false;
          }
        return true;
      };
      auto resultItr = std::find_if (subsets.begin (), subsets.end (), numbersNotInArray);
      auto i = 0UL;
      while (resultItr != subsets.end ())
        {
          ranges::copy (subset, resultsToMirror.at (resultCount).begin ());
          ranges::copy (*resultItr, resultsToMirror.at (resultCount).begin () + subsetSize / 2);
          resultCount++;
          resultItr = std::find_if (resultItr + 1, subsets.end (), numbersNotInArray);
          i++;
          if (i >= (combintions2 (setOfNumbersSize, subsetSize) / subsets.size ()))
            {
              break;
            }
        }
      itr++;
      if (itr >= (subsets.size () / 2))
        {
          break;
        }
    }
  auto results = std::array<std::array<uint8_t, subsetSize>, combintions2 (setOfNumbersSize, subsetSize)>{};
  // ranges::copy (resultsToMirror, results.begin ());
  // std::transform (resultsToMirror.begin (), resultsToMirror.end (), results.begin () + resultsToMirror.size (), [] (auto element) {
  //   std::rotate (element.begin (), element.begin () + element.size () / 2, element.end ());
  //   return element;
  // });
  return results;
}

int
main ()
{
  // auto constexpr setOfNumbersSize = 24;
  // auto constexpr subsetSize = 12;
  // auto result = subset<setOfNumbersSize, subsetSize> ({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 });
  // permutations<setOfNumbersSize, subsetSize> (result);
  // return 0;
}