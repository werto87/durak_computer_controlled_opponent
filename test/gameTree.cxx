#include "src/cxx/compressCard.hxx"
#include "src/cxx/permutation.hxx"
#include "src/cxx/solve.hxx"
#include <catch2/catch.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <date/date.h>
#include <durak/card.hxx>
#include <durak/print.hxx>
#include <functional>
#include <magic_enum.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <st_tree.h>
inline const char *
indent_padding (size_t n)
{
  static char const spaces[] = "                                                                   ";
  static const unsigned ns = sizeof (spaces) / sizeof (*spaces);
  if (n >= ns) n = ns - 1;
  return spaces + (ns - 1 - n);
}

template <typename Iterator, typename Stream>
void
serialize_indented (const Iterator &F, const Iterator &L, Stream &s, size_t indent = 2)
{
  for (Iterator j (F); j != L; ++j)
    {
      s << indent_padding (j->ply () * indent);
      using namespace durak;
      s << ((j->ply () != 0) ? std::string{ "Node: " }.c_str () : std::string{ "" }.c_str ()) << ((j->key ().playedCard ()) ? j->key ().playedCard ().value () : Card{ 253, Type::hearts }) << " " << magic_enum::enum_name (std::get<0> (j->data ())) << (std::get<1> (j->data ()) ? std::string{ " Attack" } : std::string{ " Defend" });
      s << "\n";
    }
}

template <typename Tree, typename Stream>
void
serialize_indented (const Tree &t, Stream &s, size_t indent = 2)
{
  serialize_indented (t.df_pre_begin (), t.df_pre_end (), s, indent);
}

TEST_CASE ("gameTree ", "[abc]")
{
  using namespace durak;
  size_t n = 36;
  size_t const attackCardCount = 1;
  size_t const defendCardCount = 1;
  auto combinations = compressed_permutations ({ attackCardCount, defendCardCount }, n);
  auto results = std::vector<std::tuple<std::vector<Card>, std::vector<Card>, Type, st_tree::tree<std::tuple<Result, bool>, st_tree::keyed<Action> > > >{};
  for (auto combi : combinations)
    {
      for (auto trumpType : { Type::hearts, Type::clubs, Type::diamonds, Type::spades })
        {
          auto cards = idsToCards (combi);
          auto attackCards = std::vector<Card> (cards.begin (), cards.begin () + attackCardCount);
          auto defendCards = std::vector<Card> (cards.begin () + attackCardCount, cards.end ());
          auto round = moveTree (attackCards, defendCards, trumpType);
          auto t = createTree (round);
          solveGameTree (t);
          results.push_back ({ attackCards, defendCards, trumpType, t });
        }
    }
  // std::cout << "attack cards:" << std::endl;
  // for (auto card : std::get<0> (results.at (42)))
  //   {
  //     std::cout << card << std::endl;
  //   }
  // std::cout << "defend cards:" << std::endl;
  // for (auto card : std::get<1> (results.at (42)))
  //   {
  //     std::cout << card << std::endl;
  //   }
  // std::cout << "trump: " << magic_enum::enum_name (std::get<2> (results.at (42))) << std::endl;
  // serialize_indented (std::get<3> (results.at (42)), std::cout);
}

const char *
indent (unsigned n)
{
  static char const spaces[] = "                                             ";
  static const unsigned ns = sizeof (spaces) / sizeof (*spaces);
  return spaces + (ns - 1 - n);
}

TEST_CASE ("insertDrawCardsAction", "[abc]")
{

  std::vector<durak::Card> attackCards{ { 0, durak::Type::hearts }, { 0, durak::Type::clubs }, { 0, durak::Type::diamonds } };
  std::vector<std::vector<Action> > vectorsOfActions{ { { 0 }, { 1 }, { 2 } } };
  REQUIRE (insertDrawCardsAction (attackCards, vectorsOfActions).at (0).size () == 6);
}

TEST_CASE ("simulate round ", "[abc]")
{
  auto gameLookup = std::map<std::tuple<uint8_t, uint8_t>, std::array<std::vector<std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, std::vector<std::tuple<uint8_t, Result> > > >, 4> >{};
  auto start = std::chrono::system_clock::now ();
  std::cout << "1v1" << std::endl;
  // solveDurak (36, 1, 1, gameLookup);

  gameLookup.insert ({ { 1, 1 }, solveDurak (36, 1, 1, gameLookup) });
  std::cout << "2v2" << std::endl;
  gameLookup.insert ({ { 2, 2 }, solveDurak (36, 2, 2, gameLookup) });
  std::cout << "3v1" << std::endl;
  gameLookup.insert ({ { 3, 1 }, solveDurak (36, 3, 1, gameLookup) });
  std::cout << "2v4" << std::endl;
  gameLookup.insert ({ { 2, 4 }, solveDurak (36, 2, 4, gameLookup) });
  // std::cout << "3v3" << std::endl;
  // auto threeVersusThree = solveDurak (36, 3, 3, gameLookup);
  using namespace date;
  std::cout << "duration: " << std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now () - start) << std::endl;
}
