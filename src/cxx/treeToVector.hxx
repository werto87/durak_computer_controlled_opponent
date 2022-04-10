#ifndef CD77E88E_7A82_4A96_B983_3A9338969CDC
#define CD77E88E_7A82_4A96_B983_3A9338969CDC

#include <cstddef>
#include <optional>
#include <st_tree.h>
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
          someValue = static_cast<size_t> (index.value ());
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
treeToVector (st_tree::tree<T> const &tree, size_t maxChildren, T const &markerForEmpty, T const &markerForChild)
{
  auto result = std::vector<T>{};
  for (auto &node : tree)
    {
      result.push_back (node.data ());
      auto currentChildren = size_t{};
      while (currentChildren < node.size ())
        {
          result.push_back (markerForChild);
          currentChildren++;
        }
      while (currentChildren < maxChildren)
        {
          result.push_back (markerForEmpty);
          currentChildren++;
        }
    }
  auto nodeCount = size_t{ 1 };
  for (auto &value : result)
    {
      if (value == markerForChild)
        {
          value = static_cast<T> (nodeCount * (maxChildren + 1));
          nodeCount++;
        }
    }
  return result;
}

template <typename T>
size_t
maxChildren (st_tree::tree<T> const &tree)
{
  auto maxChildren = size_t{};
  for (auto const &node : tree)
    {
      if (maxChildren < node.size ())
        {
          maxChildren = node.size ();
        }
    }
  return maxChildren;
}

#endif /* CD77E88E_7A82_4A96_B983_3A9338969CDC */
