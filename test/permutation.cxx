#include "src/cxx/permutation.hxx"
#include "src/cxx/combination.hxx"
#include "src/cxx/oldPermutation.hxx"
// #include "src/cxx/util.hxx"
#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <catch2/catch.hpp>
#include <cstddef>
#include <cstdint>
#include <durak/card.hxx>
#include <durak/print.hxx>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <iostream>
#include <numeric>
#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/algorithm/replace.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <range/v3/range.hpp>
#include <tuple>
#include <vector>
std::string
from_u8string (const std::u8string &s)
{
  return std::string (s.begin (), s.end ());
}

void
printSymbol (uint8_t number)
{
  // clang-format off
  auto myNumber = number % 4;
  auto symbol=std::string{};
  if (myNumber == 0)
    {
      symbol=  from_u8string (std::u8string{ u8"♣" }).c_str () ;
    }
  else if (myNumber == 1)
    {
      symbol= from_u8string (std::u8string{ u8"♠" }).c_str ();
    }
  else if (myNumber == 2)
    {
      symbol= from_u8string (std::u8string{ u8"♥" }).c_str ();
    }
  else 
    {
      symbol= from_u8string (std::u8string{ u8"♦" }).c_str ();
    }
    fmt::print ((number % 4 >= 2) ? fmt::fg (fmt::color::red) : fmt::fg (fmt::color::black), symbol);
  // clang-format on
}

durak::Card
idToCard (uint8_t id)
{
  return durak::Card{ boost::numeric_cast<u_int16_t> (id / 4), static_cast<durak::Type> (id % 4) };
}

uint8_t
cardToId (durak::Card const &card)
{
  return boost::numeric_cast<uint8_t> (card.value * 4 + static_cast<uint8_t> (card.type));
}

std::vector<uint8_t>
cardsToIds (std::vector<durak::Card> const cards)
{
  auto results = std::vector<uint8_t>{};
  ranges::transform (cards, ranges::back_inserter (results), [] (durak::Card const &cards) { return cardToId (cards); });
  return results;
}

std::vector<durak::Card>
idsToCards (std::vector<uint8_t> const ids)
{
  auto results = std::vector<durak::Card>{};
  ranges::transform (ids, ranges::back_inserter (results), [] (uint8_t id) { return idToCard (id); });
  return results;
}

std::vector<durak::Card>
compress (std::vector<durak::Card> cards)
{
  auto idsAndCards = std::vector<std::tuple<size_t, durak::Card> >{};
  ranges::transform (cards, ranges::back_inserter (idsAndCards), [id = size_t{ 0 }] (durak::Card const &card) mutable { return std::tuple<size_t, durak::Card>{ id++, card }; });
  ranges::sort (idsAndCards, {}, [] (std::tuple<size_t, durak::Card> const &idAndCard) { return std::get<1> (idAndCard); });
  auto setToNumber = u_int16_t{ 0 };
  auto numberToChange = std::get<1> (idsAndCards.front ()).value;
  for (auto &[id, card] : idsAndCards)
    {
      if (card.value != numberToChange)
        {
          setToNumber++;
          numberToChange = card.value;
        }
      card.value = setToNumber;
    }
  ranges::sort (idsAndCards);
  ranges::transform (idsAndCards, cards.begin (), [] (std::tuple<size_t, durak::Card> const &idAndCard) { return std::get<1> (idAndCard); });
  return cards;
}

template <typename T, template <typename ELEM, typename ALLOC = std::allocator<ELEM> > class Container>
std::ostream &
operator<< (std::ostream &o, const Container<T> &container)
{
  typename Container<T>::const_iterator beg = container.begin ();
  while (beg != container.end ())
    {
      o << "\n" << *beg++; // 2
    }
  return o;
}

TEST_CASE ("permut validation validation ", "[abc]")
{
  size_t k = 4;
  for (auto n = k + 2; n <= 20; n = n + 2)
    {
      auto combinations = std::vector<std::vector<uint8_t> >{};
      auto compressedCombinations = std::set<std::vector<uint8_t> >{};
      for_each_card_combination (k, n, [&compressedCombinations] (std::vector<uint8_t> combi) {
        compressedCombinations.insert (cardsToIds (compress (idsToCards (combi))));
        return false;
      });
      // std::cout << "n: " << n << " k: " << k << " results: " << compressedCombinations.size () << std::endl;
      // for (auto combi : compressedCombinations)
      //   {
      //     std::cout << idsToCards (combi) << std::endl;
      //   }
      std::cout << "results: " << compressedCombinations.size () << std::endl;
    }
}

// auto jack = u_int16_t{ 11 };
// auto queen = u_int16_t{ 12 };
// auto king = u_int16_t{ 13 };
// auto ace = u_int16_t{ 14 };

TEST_CASE ("compress", "[abc]")
{
  SECTION ("4 different values", "[matchmaking]")
  {
    auto const result = compress ({ { 6, durak::Type::clubs }, { 11, durak::Type::clubs }, { 14, durak::Type::clubs }, { 8, durak::Type::hearts } });
    auto const desire = std::vector<durak::Card>{ { 0, durak::Type::clubs }, { 2, durak::Type::clubs }, { 3, durak::Type::clubs }, { 1, durak::Type::hearts } };
    REQUIRE (result == desire);
  }
  SECTION ("4 times the same value", "[matchmaking]")
  {
    auto const result = compress ({ { 6, durak::Type::clubs }, { 6, durak::Type::hearts }, { 6, durak::Type::diamonds }, { 6, durak::Type::spades } });
    auto const desire = std::vector<durak::Card>{ { 0, durak::Type::clubs }, { 0, durak::Type::hearts }, { 0, durak::Type::diamonds }, { 0, durak::Type::spades } };
    REQUIRE (result == desire);
  }
}

#ifdef RUN_BENCHMARK
TEST_CASE ("subset benchmark ", "[abc]")
{
  BENCHMARK ("subset (4, 6)") { return subset (4, 6); };
  BENCHMARK ("subset (4, 8)") { return subset (4, 8); };
  BENCHMARK ("subset (6, 8)") { return subset (6, 8); };
  BENCHMARK ("subset (4, 10)") { return subset (4, 10); };
  BENCHMARK ("subset (6, 10)") { return subset (6, 10); };
  BENCHMARK ("subset (8, 10)") { return subset (8, 10); };
  BENCHMARK ("subset (4, 12)") { return subset (4, 12); };
  BENCHMARK ("subset (6, 12)") { return subset (6, 12); };
  BENCHMARK ("subset (8, 12)") { return subset (8, 12); };
  BENCHMARK ("subset (10, 12)") { return subset (10, 12); };
  BENCHMARK ("subset (10, 14)") { return subset (10, 14); };
  BENCHMARK ("subset (12, 14)") { return subset (12, 14); };
  BENCHMARK ("subset (12, 36)") { return subset (12, 36); };
  auto n = 36;
  auto r = 12;
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto numbersToCheck = combinationsNoRepetitionAndOrderDoesNotMatter (r / 2, indexes);
  auto subResults = combinationsNoRepetitionAndOrderDoesNotMatter (r / 2, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (r / 2)));
  BENCHMARK ("subset2 (12, 36)") { return combinationsFor (numbersToCheck.at (0), subResults, indexes); };
}

TEST_CASE ("combinationsFor benchmark ", "[abc]")
{
  auto k = 12;
  auto n = 14;
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto results = subsetPermutation (k, indexes); // THIS TAKES 90% of the time
  const int r = 3;
  const int n = 5;
  std::vector<int> v (n);
  std::iota (v.begin (), v.end (), 0);
  std::uint64_t count = for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ()));
  std::cout << count << std::endl;
  CHECK (for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_reversible_circular_permutation (r, n));
  CHECK (for_each_permutation (v.begin (), v.begin () + r, v.end (), f (v.size ())) == count_each_permutation (r, n));

  BENCHMARK ("subsetPermutation (k, indexes)") { return subsetPermutation (k, indexes); };
  BENCHMARK ("subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)))") { return subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2))); };
  auto const &subResult = subsetPermutation (k, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2)));
  auto combineResult = std::vector<subsetAndCombinations>{};
  BENCHMARK ("combinationsFor (12, 14)")
  {
    for (auto &&result : results)
      {
        combineResult.emplace_back (combinationsFor (result, subResult, indexes));
      }
  };

  BENCHMARK ("for_each_reversible_circular_permutation")
  {
    size_t n = 36;
    size_t r = 6;
    std::vector<uint8_t> v (n);
    std::iota (v.begin (), v.end (), 0);
    auto results = std::vector<std::vector<uint8_t> >{};
    results.reserve (boost::numeric_cast<size_t> (combinationsNoRepetitionAndOrderDoesNotMatter (n, r)));
    for_each_reversible_circular_permutation (v.begin (), v.begin () + r, v.end (), [&results, n, r] (auto first, auto last) {
      if (std::is_sorted (first, last))
        {
          results.push_back (std::vector<uint8_t>{ first, last });
          return results.size () == combinationsNoRepetitionAndOrderDoesNotMatter (n, r);
        }
      else
        {
          return false;
        }
    });
    std::cout << combinationsNoRepetitionOrderDoesNotMatter (n, r) << std::endl;
    std::cout << results.size () << std::endl;
  };
}
#endif
