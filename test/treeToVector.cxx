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

template <typename T>
std::vector<T>
children (std::vector<T> const &vec, size_t index, size_t childrenCount)
{
  auto result = std::vector<T>{};
  for (auto i = size_t{ 1 }; i <= childrenCount; i++)
    {
      if (vec[index + i] != 255)
        {
          result.push_back (vec[index + i]);
        }
    }
  return result;
}

template <typename T>
std::optional<T>
childWithValue (std::vector<T> const &vec, size_t index, size_t childrenCount, T value)
{
  for (auto i = size_t{ 1 }; i <= childrenCount; i++)
    {
      if (vec[static_cast<size_t> (vec[index + i])] == value)
        {
          return vec[index + i];
        }
    }
  return {};
}

template <typename T>
std::vector<T>
childrenByPath (std::vector<T> const &vec, std::vector<T> const &path, size_t childrenCount)
{
  auto someValue = size_t{ 0 };
  for (auto value : path)
    {
      if (auto index = childWithValue (vec, someValue, childrenCount, value))
        {
          someValue = index.value ();
        }
      else
        {
          break;
        }
    }
  return children (vec, someValue, childrenCount);
}

template <typename T>
std::vector<T>
treeToVector (st_tree::tree<T> const &tree, size_t maxChildren)
{
  for (auto const &node : tree)
    {
      if (maxChildren < node.size ())
        {
          maxChildren = node.size ();
        }
    }
  auto result = std::vector<T>{};
  for (auto &node : tree)
    {
      result.push_back (node.data ());
      auto currentChildren = size_t{};
      while (currentChildren < node.size ())
        {
          result.push_back (254);
          currentChildren++;
        }
      while (currentChildren < maxChildren)
        {
          result.push_back (255);
          currentChildren++;
        }
    }

  auto nodeCount = size_t{ 1 };
  for (auto &value : result)
    {
      if (value == 254)
        {
          value = static_cast<T> (nodeCount * (maxChildren + 1));
          nodeCount++;
        }
    }
  return result;
}

TEST_CASE ("2 children", "[abc]")
{
  auto tree = st_tree::tree<int>{};
  tree.insert (1);
  tree.root ().insert (2);
  tree.root ().insert (3);
  tree.root ()[0].insert (4);
  tree.root ()[0][0].insert (42);
  auto myVec = treeToVector (tree, 2);
  for (auto &value : childrenByPath (myVec, { 2, 4 }, 2))
    {
      REQUIRE (myVec[value] == 42);
    }
}

TEST_CASE ("3 children", "[abc]")
{

  auto tree = st_tree::tree<int>{};
  tree.insert (1);
  tree.root ().insert (2);
  tree.root ().insert (3);
  tree.root ().insert (69);
  tree.root ()[0].insert (4);
  tree.root ()[0][0].insert (42);
  auto myVec = treeToVector (tree, 3);
  for (auto &value : childrenByPath (myVec, { 2, 4 }, 3))
    {
      REQUIRE (myVec[value] == 42);
    }
}