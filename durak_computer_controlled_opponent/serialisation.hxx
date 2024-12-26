
// DISCLAIMER most of this file is copied from https://github.com/USCiLab/cereal/blob/master/include/cereal/archives/binary.hpp
// I needed to add member variables to BinaryOutputArchive BinaryInputArchive.
/*! \file binary.hpp
    \brief Binary input and output archives */
/*
  Copyright (c) 2014, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <bitset>
#include <boost/numeric/conversion/cast.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/tuple.hpp>
#include <magic_enum/magic_enum.hpp>
#include <small_memory_tree/smallMemoryTree.hxx>
#include <sstream>
namespace cereal
{
// ######################################################################
//! An output archive designed to save data in a compact binary representation
/*! This archive outputs data to a stream in an extremely compact binary
    representation with as little extra metadata as possible.

    This archive does nothing to ensure that the endianness of the saved
    and loaded data is the same.  If you need to have portability over
    architectures with different endianness, use PortableBinaryOutputArchiveWithContainingVectorSize.

    When using a binary archive and a file stream, you must use the
    std::ios::binary format flag to avoid having your data altered
    inadvertently.

    \ingroup Archives */
class BinaryOutputArchiveWithContainingVectorSize : public OutputArchive<BinaryOutputArchiveWithContainingVectorSize, AllowEmptyClassElision>
{
public:
  //! Construct, outputting to the provided stream
  /*! @param stream The stream to output to.  Can be a stringstream, a file
     stream, or even cout! */
  BinaryOutputArchiveWithContainingVectorSize (std::ostream &stream) : OutputArchive<BinaryOutputArchiveWithContainingVectorSize, AllowEmptyClassElision> (this), itsStream (stream) {}

  ~BinaryOutputArchiveWithContainingVectorSize () CEREAL_NOEXCEPT = default;

  //! Writes size bytes of data to the output stream
  void
  saveBinary (const void *data, std::streamsize size)
  {
    auto const writtenSize = itsStream.rdbuf ()->sputn (reinterpret_cast<const char *> (data), size);

    if (writtenSize != size) throw Exception ("Failed to write " + std::to_string (size) + " bytes to output stream! Wrote " + std::to_string (writtenSize));
  }
  size_t sizeOfVector{};

private:
  std::ostream &itsStream;
};

// ######################################################################
//! An input archive designed to load data saved using BinaryOutputArchiveWithContainingVectorSize
/*  This archive does nothing to ensure that the endianness of the saved
    and loaded data is the same.  If you need to have portability over
    architectures with different endianness, use PortableBinaryOutputArchiveWithContainingVectorSize.

    When using a binary archive and a file stream, you must use the
    std::ios::binary format flag to avoid having your data altered
    inadvertently.

    \ingroup Archives */
class BinaryInputArchiveWithContainingVectorSize : public InputArchive<BinaryInputArchiveWithContainingVectorSize, AllowEmptyClassElision>
{
public:
  //! Construct, loading from the provided stream
  BinaryInputArchiveWithContainingVectorSize (std::istream &stream) : InputArchive<BinaryInputArchiveWithContainingVectorSize, AllowEmptyClassElision> (this), itsStream (stream) {}

  ~BinaryInputArchiveWithContainingVectorSize () CEREAL_NOEXCEPT = default;

  //! Reads size bytes of data from the input stream
  void
  loadBinary (void *const data, std::streamsize size)
  {
    auto const readSize = itsStream.rdbuf ()->sgetn (reinterpret_cast<char *> (data), size);

    if (readSize != size) throw Exception ("Failed to read " + std::to_string (size) + " bytes from input stream! Read " + std::to_string (readSize));
  }
  size_t sizeOfVector{};

private:
  std::istream &itsStream;
};

// ######################################################################
// Common BinaryArchive serialization functions

//! Saving for POD types to binary
template <class T>
inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME (BinaryOutputArchiveWithContainingVectorSize &ar, T const &t)
{
  ar.saveBinary (std::addressof (t), sizeof (t));
}

//! Loading for POD types from binary
template <class T>
inline typename std::enable_if<std::is_arithmetic<T>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME (BinaryInputArchiveWithContainingVectorSize &ar, T &t)
{
  ar.loadBinary (std::addressof (t), sizeof (t));
}

//! Serializing NVP types to binary
template <class Archive, class T> inline CEREAL_ARCHIVE_RESTRICT (BinaryInputArchiveWithContainingVectorSize, BinaryOutputArchiveWithContainingVectorSize) CEREAL_SERIALIZE_FUNCTION_NAME (Archive &ar, NameValuePair<T> &t) { ar (t.value); }

//! Serializing SizeTags to binary
template <class Archive, class T> inline CEREAL_ARCHIVE_RESTRICT (BinaryInputArchiveWithContainingVectorSize, BinaryOutputArchiveWithContainingVectorSize) CEREAL_SERIALIZE_FUNCTION_NAME (Archive &ar, SizeTag<T> &t) { ar (t.size); }

//! Saving binary data
template <class T>
inline void
CEREAL_SAVE_FUNCTION_NAME (BinaryOutputArchiveWithContainingVectorSize &ar, BinaryData<T> const &bd)
{
  ar.saveBinary (bd.data, static_cast<std::streamsize> (bd.size));
}

//! Loading binary data
template <class T>
inline void
CEREAL_LOAD_FUNCTION_NAME (BinaryInputArchiveWithContainingVectorSize &ar, BinaryData<T> &bd)
{
  ar.loadBinary (bd.data, static_cast<std::streamsize> (bd.size));
}
} // namespace cereal

// register archives for polymorphic support
CEREAL_REGISTER_ARCHIVE (cereal::BinaryOutputArchiveWithContainingVectorSize)
CEREAL_REGISTER_ARCHIVE (cereal::BinaryInputArchiveWithContainingVectorSize)

// tie input and output archives together
CEREAL_SETUP_ARCHIVE_TRAITS (cereal::BinaryInputArchiveWithContainingVectorSize, cereal::BinaryOutputArchiveWithContainingVectorSize)

namespace cereal
{

//! Serialization for non-arithmetic vector types
template <class Archive, class T, class A>
inline typename std::enable_if<(!traits::is_output_serializable<BinaryData<T>, Archive>::value || !std::is_arithmetic<T>::value) && !std::is_same<T, bool>::value, void>::type
CEREAL_SAVE_FUNCTION_NAME (Archive &ar, std::vector<T, A> const &vector)
{
  ar.sizeOfVector = vector.size ();
  ar (make_size_tag (static_cast<size_type> (vector.size ()))); // number of elements
  for (auto &&v : vector)
    ar (v);
}

//! Serialization for non-arithmetic vector types
template <class Archive, class T, class A>
inline typename std::enable_if<(!traits::is_input_serializable<BinaryData<T>, Archive>::value || !std::is_arithmetic<T>::value) && !std::is_same<T, bool>::value, void>::type
CEREAL_LOAD_FUNCTION_NAME (Archive &ar, std::vector<T, A> &vector)
{
  size_type size;
  ar (make_size_tag (size));
  ar.sizeOfVector = static_cast<std::size_t> (size);
  vector.resize (static_cast<std::size_t> (size));
  for (auto &&v : vector)
    ar (v);
}

// copied from https://stackoverflow.com/questions/3061721/concatenate-boostdynamic-bitset-or-stdbitset
template <size_t N1, size_t N2>
std::bitset<N1 + N2>
concat (const std::bitset<N1> &b1, const std::bitset<N2> &b2)
{
  std::string s1 = b1.to_string ();
  std::string s2 = b2.to_string ();
  return std::bitset<N1 + N2> (s1 + s2);
}

// struct MyStruct
// {
//   uint8_t byteOne{};
//   uint8_t byteTwo{};
// };

// template <typename ValueType, typename ChildrenOffsetEndType>
// void
// save (cereal::BinaryOutputArchiveWithContainingVectorSize &archive, MyStruct const &myStruct)
// {
//   auto const &[action, result] = std::get<0> (node);
//   auto firstFourBits = std::bitset<4>{ magic_enum::enum_integer (result) };
//   auto lastFourBits = std::bitset<4>{ std::get<1> (node) };
//   archive (action, boost::numeric_cast<uint8_t> (concat (firstFourBits, lastFourBits).to_ulong ()));
// }

// template <typename ValueType, typename ChildrenOffsetEndType>
// void
// load (cereal::BinaryInputArchiveWithContainingVectorSize &archive, MyStruct &myStruct)
// {
//   auto action = durak_computer_controlled_opponent::Action{};
//   auto resultWithChildrenCount = uint8_t{};
//   archive (action, resultWithChildrenCount);
//   auto const &tmpBitset = std::bitset<8> (resultWithChildrenCount);
//   auto const &bitsAsString = tmpBitset.to_string ();
//   auto const &result = boost::numeric_cast<uint8_t> (std::bitset<4>{ std::string (bitsAsString.begin (), bitsAsString.begin () + 4) }.to_ulong ());
//   auto const &childrenCount = boost::numeric_cast<uint8_t> (std::bitset<4>{ std::string (bitsAsString.begin () + 4, bitsAsString.end ()) }.to_ulong ());
//   std::get<0> (node) = std::make_tuple (action, magic_enum::enum_cast<durak_computer_controlled_opponent::Result> (result).value ());
//   std::get<1> (node) = childrenCount;
// }
inline void
save (cereal::BinaryOutputArchiveWithContainingVectorSize &archive, durak_computer_controlled_opponent::Action const &action)
{
  archive (action.value ());
}
inline void
load (cereal::BinaryInputArchiveWithContainingVectorSize &archive, durak_computer_controlled_opponent::Action &action)
{
  auto getValue = uint8_t{};
  archive (getValue);
  action = durak_computer_controlled_opponent::Action{ getValue };
}

template <typename ValueType, typename ChildrenOffsetEndType>
void
save (cereal::BinaryOutputArchiveWithContainingVectorSize &archive, small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEndType> const &smallMemoryTree)
{
  auto result = std::vector<std::tuple<durak_computer_controlled_opponent::Action, uint8_t> >{};
  auto const &values = smallMemoryTree.getValues ();
  for (uint64_t i{}; i < values.size (); ++i)
    {
      auto const &[action, actionResult] = values.at (i);
      auto const &firstFourBits = std::bitset<4>{ magic_enum::enum_integer (actionResult) };
      auto const &childrenCount = small_memory_tree::internals::getChildrenCount (smallMemoryTree, i);
      auto const &lastFourBits = std::bitset<4>{ childrenCount.value () };
      result.emplace_back (action, boost::numeric_cast<uint8_t> (concat (firstFourBits, lastFourBits).to_ulong ()));
    }
  archive (result);
}
template <typename ValueType, typename ChildrenOffsetEndType>
void
load (cereal::BinaryInputArchiveWithContainingVectorSize &archive, small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEndType> &smallMemoryTree)
{
  auto actionActionResultWithChildrenCount = std::vector<std::tuple<durak_computer_controlled_opponent::Action, uint8_t> >{};
  archive (actionActionResultWithChildrenCount);
  auto values = std::vector<std::tuple<durak_computer_controlled_opponent::Action, durak_computer_controlled_opponent::Result> >{};
  auto childrenOffsetEnds = std::vector<ChildrenOffsetEndType>{};
  auto childrenSum = 0;
  for (uint64_t i{}; i < actionActionResultWithChildrenCount.size (); ++i)
    {
      auto const &[action, actionResultWithChildrenCount] = actionActionResultWithChildrenCount.at (i);
      auto const &tmpBitset = std::bitset<8> (actionResultWithChildrenCount);
      auto const &bitsAsString = tmpBitset.to_string ();
      auto const &actionResult = boost::numeric_cast<uint8_t> (std::bitset<4>{ std::string (bitsAsString.begin (), bitsAsString.begin () + 4) }.to_ulong ());
      values.push_back (std::make_tuple (action, magic_enum::enum_cast<durak_computer_controlled_opponent::Result> (actionResult).value ()));
      childrenSum += boost::numeric_cast<uint8_t> (std::bitset<4>{ std::string (bitsAsString.begin () + 4, bitsAsString.end ()) }.to_ulong ());
      childrenOffsetEnds.push_back (childrenSum);
    }
  smallMemoryTree = small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEndType>{ std::move (values), std::move (childrenOffsetEnds) };
}
}
