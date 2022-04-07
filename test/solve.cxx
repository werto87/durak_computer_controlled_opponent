#include "src/cxx/solve.hxx"
#include "src/cxx/compressCard.hxx"
#include "src/cxx/permutation.hxx"
#include <boost/fusion/adapted/struct/detail/adapt_auto.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <catch2/catch.hpp>
#include <chrono>
#include <confu_json/to_json.hxx>
#include <cstddef>
#include <cstdint>
#include <date/date.h>
#include <durak/card.hxx>
#include <durak/game.hxx>
#include <durak/gameData.hxx>
#include <durak/gameOption.hxx>
#include <iterator>
#include <magic_enum.hpp>
#include <numeric>
#include <ostream>
#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/iterator/insert_iterators.hpp>
#include <ratio>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <variant>

TEST_CASE ("solve ", "[abc]")
{
  using namespace durak;
  auto gameToAnalyse = Game{ { "a", "b" }, GameOption{ .numberOfCardsPlayerShouldHave = 2, .customCardDeck = std::vector<Card>{ { 1, Type::hearts }, { 1, Type::clubs }, { 1, Type::diamonds }, { 1, Type::spades } } } };
  auto results = solve (gameToAnalyse);
  REQUIRE (results.size () == 6);
}

using Histories = std::vector<durak::HistoryEvent>;
using Ids = std::vector<uint8_t>;
using ResultAndHistory = std::tuple<boost::optional<durak::Player>, Histories>;

TEST_CASE ("solve multiple games", "[abc]")
{
  using namespace durak;
  auto results = std::array<std::vector<Round>, 4>{};
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          results.at (static_cast<size_t> (trumpType)).push_back (moveTree (std::vector<Card> (cards.begin (), cards.begin () + attackCardCount), std::vector<Card> (cards.begin () + attackCardCount, cards.end ()), trumpType));
        }
    }
  REQUIRE (results.at (0).at (3).attackIsWinning.size () == 1);
  REQUIRE (results.at (0).at (3).defendIsWinning.size () == 0);
  REQUIRE (results.at (0).at (3).draw.size () == 1);
}
