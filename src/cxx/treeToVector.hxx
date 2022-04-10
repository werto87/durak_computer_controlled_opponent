#ifndef CD77E88E_7A82_4A96_B983_3A9338969CDC
#define CD77E88E_7A82_4A96_B983_3A9338969CDC

#include <cstddef>
#include <optional>
#include <st_tree.h>
#include <vector>

template <typename T> concept TupleLike = requires (T a)
{
  std::tuple_size<T>::value;
  std::get<0> (a);
};

template <typename T>
std::vector<T>
children (std::vector<T> const &vec, size_t index, size_t childrenCount, T emptyMarker)
{
  auto result = std::vector<T>{};
  for (auto i = size_t{ 1 }; i <= childrenCount; i++)
    {
      if (vec[index + i] != emptyMarker)
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
      if constexpr (TupleLike<T>)
        {
          if (vec[static_cast<size_t> (std::get<0> (vec[index + i]))] == value)
            {
              return vec[index + i];
            }
        }
      else
        {
          if (vec[static_cast<size_t> (vec[index + i])] == value)
            {
              return vec[index + i];
            }
        }
    }
  return {};
}

template <typename T>
std::vector<T>
childrenByPath (std::vector<T> const &vec, std::vector<T> const &path, size_t childrenCount, T const &markerForEmpty)
{
  auto someValue = size_t{ 0 };
  for (auto value : path)
    {
      if (auto index = childWithValue (vec, someValue, childrenCount, value))
        {
          if constexpr (TupleLike<T>)
            {
              someValue = static_cast<size_t> (std::get<0> (index.value ()));
            }
          else
            {
              someValue = static_cast<size_t> (index.value ());
            }
        }
      else
        {
          break;
        }
    }
  return children (vec, someValue, childrenCount, markerForEmpty);
}

template <typename T>
void
fillChilds (std::vector<T> &vec, size_t maxChildren, T const &markerForChild)
{
  auto nodeCount = size_t{ 1 };
  for (auto &value : vec)
    {
      if (value == markerForChild)
        {
          if constexpr (TupleLike<T>)
            {
              std::get<0> (value) = static_cast<typename std::decay<decltype (std::get<0> (value))>::type> (nodeCount * (maxChildren + 1));
              nodeCount++;
            }
          else
            {
              value = static_cast<T> (nodeCount * (maxChildren + 1));
              nodeCount++;
            }
        }
    }
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
  fillChilds (result, maxChildren, markerForChild);
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
