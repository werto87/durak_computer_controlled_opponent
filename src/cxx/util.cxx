#include "util.hxx"
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