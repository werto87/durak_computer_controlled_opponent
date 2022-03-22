#include <catch2/catch.hpp>
#include <durak/card.hxx>

void
solve (std::vector<durak::Card> const &cards)
{
    
}

TEST_CASE ("solve ", "[abc]")
{
  using namespace durak;
  solve ({ { 1, Type::clubs }, { 2, Type::clubs }, { 3, Type::clubs }, { 4, Type::clubs } });
  //   REQUIRE (compressedCombinations.size () == 3678);
}