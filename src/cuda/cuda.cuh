#ifndef BA817EBF_1CC3_404D_9AB7_E3524F836DC0
#define BA817EBF_1CC3_404D_9AB7_E3524F836DC0

#include <algorithm>
#include <array>
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdlib>
#include <thrust/copy.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>
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

size_t constexpr combintions (size_t setOfNumbersSize, size_t subsetSize);

size_t constexpr combintions2 (size_t setOfNumbersSize, size_t subsetSize);

thrust::host_vector<thrust::host_vector<u_int8_t> > subset (size_t subsetSize, thrust::host_vector<u_int8_t> setOfNumbers);

template <size_t setOfNumbersSize, size_t subsetSize, typename T>
void
calculatePermutation (T &result, thrust::host_vector<u_int8_t> const &subset, thrust::host_vector<thrust::host_vector<u_int8_t> > const &subsets)
{
  auto const numbersNotInArray = [&subset] (auto const &array) {
    for (auto number : subset)
      if (std::binary_search (array.begin (), array.end (), number)) return false;
    return true;
  };
  auto resultItr = std::find_if (subsets.begin (), subsets.end (), numbersNotInArray);
  auto i = 0UL;
  while (resultItr != subsets.end ())
    {
      auto tmpResult = thrust::host_vector<u_int8_t>{};
      thrust::copy (subset.begin (), subset.end (), std::back_inserter (tmpResult));
      thrust::copy ((*resultItr).begin (), (*resultItr).end (), std::back_inserter (tmpResult));
      *result = tmpResult;
      std::advance (result, 1);
      resultItr = std::find_if (resultItr + 1, subsets.end (), numbersNotInArray);
      i++;
      if (i >= (combintions2 (setOfNumbersSize, subsetSize) / subsets.size ())) break;
    }
}

void someOtherCalc ();

template <size_t setOfNumbersSize, size_t subsetSize>
thrust::host_vector<thrust::host_vector<u_int8_t> >
permutations (thrust::host_vector<thrust::host_vector<u_int8_t> > const &subsets)
{
  auto results = thrust::host_vector<thrust::host_vector<u_int8_t> > (combintions2 (setOfNumbersSize, subsetSize));
  auto result = results.begin ();
  for (auto i = 0UL; i < subsets.size (); i++)
    {
      calculatePermutation<setOfNumbersSize, subsetSize> (result, subsets[i], subsets);
    }
  return results;
}

thrust::host_vector<thrust::host_vector<u_int8_t> > someCalculation ();

constexpr boost::multiprecision::uint128_t factorial (size_t n);

#endif /* BA817EBF_1CC3_404D_9AB7_E3524F836DC0 */
