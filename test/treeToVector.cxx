#include "src/cxx/treeToVector.hxx"
#include <catch2/catch.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <range/v3/algorithm.hpp>
#include <range/v3/range_fwd.hpp>
#include <st_tree.h>
#include <sys/types.h>
#include <vector>

TEST_CASE ("2 children", "[abc]")
{
  auto tree = st_tree::tree<int>{};
  tree.insert (1);
  tree.root ().insert (2);
  tree.root ().insert (3);
  tree.root ()[0].insert (4);
  tree.root ()[0][0].insert (42);
  auto myVec = treeToVector (tree, maxChildren (tree), 255, 254);
  for (auto &value : childrenByPath (myVec, { 2, 4 }, maxChildren (tree), 255))
    {
      REQUIRE (myVec[static_cast<size_t> (value)] == 42);
    }
}

TEST_CASE ("3 children and tuple", "[abc]")
{
  auto tree = st_tree::tree<std::tuple<uint8_t, int8_t> >{};
  tree.insert ({ 1, 1 });
  tree.root ().insert ({ 2, 2 });
  tree.root ().insert ({ 3, 3 });
  tree.root ().insert ({ 69, 69 });
  tree.root ()[0].insert ({ 4, 4 });
  tree.root ()[0][0].insert ({ 42, 42 });
  auto myVec = treeToVector (tree, maxChildren (tree), { 255, -1 }, { 254, -1 });
  for (auto &value : childrenByPath (myVec, { { 2, 2 }, { 4, 4 } }, maxChildren (tree), { 255, -1 }))
    {
      REQUIRE (myVec[static_cast<size_t> (std::get<0> (value))] == std::tuple<uint8_t, int8_t>{ 42, 42 });
    }
}
