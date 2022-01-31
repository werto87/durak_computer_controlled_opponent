#include <boost/multiprecision/cpp_int.hpp>

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

constexpr boost::multiprecision::uint128_t factorial (size_t n);

size_t constexpr combintions (size_t setOfNumbersSize, size_t subsetSize);

size_t constexpr combintions2 (size_t setOfNumbersSize, size_t subsetSize);