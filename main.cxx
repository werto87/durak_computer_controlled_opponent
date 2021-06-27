#include <array>
#include <boost/json/src.hpp> // this file should be included only in one translation unit
#include <boost/math/special_functions/factorials.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <durak/game.hxx>
#include <durak/print.hxx>
#include <iomanip>
#include <iostream>
#include <range/v3/range.hpp>
// Requires: sequence from begin to end is sorted
//           middle is between begin and end
template <typename Bidi, typename Functor>
void
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
  return 15504;
  // return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize / 2)) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

size_t constexpr
combintions2 (size_t setOfNumbersSize, size_t subsetSize)
{
  return 46558512;
  // return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetSize)) * factorial (subsetSize / 2) * factorial (subsetSize / 2))).convert_to<size_t> (); // the size is n! / ((n-(k/2))! * (k/2)!)
}

template <size_t setOfNumbersSize, size_t subsetSize>
std::array<std::array<int, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)>
// constexpr
subset (std::array<int, setOfNumbersSize> setOfNumbers)
{
  if (subsetSize % 2 != 0) return {};
  std::array<std::array<int, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets;
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
std::array<std::array<int, subsetSize>, 46558512>
// constexpr
permutations (std::array<std::array<int, subsetSize / 2>, combintions (setOfNumbersSize, subsetSize)> subsets)
{
  std::array<std::array<int, subsetSize>, 46558512> results;
  for_each_permuted_combination (subsets.begin (), subsets.begin () + 2, subsets.end (), [&results, elementCount = 0U] (auto begin, auto end) mutable {
    std::array<std::array<int, subsetSize / 2>, 2> permutation;
    for (auto i = 0U; begin != end; i++, begin++)
      {
        permutation.at (i) = *begin;
      }
    std::array<int, subsetSize> possibleResult;
    for (size_t i = 0; int number : permutation.front ())
      {
        possibleResult.at (i) = number;
        ++i;
      }
    for (size_t i = subsetSize / 2; int number : permutation.back ())
      {
        possibleResult.at (i) = number;
        ++i;
      }
    std::sort (possibleResult.begin (), possibleResult.end ());
    if (std::adjacent_find (possibleResult.begin (), possibleResult.end ()) == possibleResult.end ())
      {
        results.at (elementCount) = possibleResult;
        elementCount++;
      }
  });
  return results;
}

int
main ()
{
  // std::cout << combintions (20, 10) << std::endl;
  // std::cout << combintions2 (20, 10) << std::endl;
  std::array<std::array<int, 5UL>, 15504UL> subsets;
  auto test = permutations<20, 10> (subsets);
  return 0;
}