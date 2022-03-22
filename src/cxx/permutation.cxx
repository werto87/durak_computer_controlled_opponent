#include "permutation.hxx"
#include "src/cxx/combination.hxx"
#include <boost/numeric/conversion/cast.hpp>
#include <cstddef>
#include <numeric>
#include <range/v3/algorithm.hpp>
#include <range/v3/iterator/insert_iterators.hpp>

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
  ranges::transform (subResults, ranges::back_inserter (std::get<1> (result)), [&numbers] (auto indexes) {
    ranges::transform (indexes, indexes.begin (), [&numbers] (auto const &index) { return numbers[index]; });
    return indexes;
  });
  std::get<0> (result) = numbersToCheck;
  return result;
}

void
for_each_card_combination (size_t k, size_t n, std::function<bool (std::vector<uint8_t>)> callThis)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  auto results = numbersToCombine (static_cast<long int> (k), n);
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

std::tuple<std::vector<std::vector<uint8_t> >, std::vector<std::vector<uint8_t> > >
numbersToCombine (long int k, size_t n)
{
  auto indexes = std::vector<uint8_t> (n);
  std::iota (indexes.begin (), indexes.end (), 0);
  return { combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, indexes), combinationsNoRepetitionAndOrderDoesNotMatter (k / 2, std::vector<uint8_t> (indexes.begin (), indexes.begin () + static_cast<long int> (n) - (k / 2))) };
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