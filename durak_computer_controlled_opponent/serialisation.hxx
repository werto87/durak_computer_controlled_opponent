
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
#include <boost/numeric/conversion/cast.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/tuple.hpp>
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

template <typename ValueType, typename ChildrenOffsetEnd>
void
save (cereal::BinaryOutputArchiveWithContainingVectorSize &archive, small_memory_tree::Node<ValueType, ChildrenOffsetEnd> const &node)
{
  if (archive.sizeOfVector <= 255)
    {
      archive (node.value, boost::numeric_cast<uint8_t> (node.childrenOffsetEnd));
    }
  else
    {
      archive (node.value, node.childrenOffsetEnd);
    }
}

template <typename ValueType, typename ChildrenOffsetEnd>
void
load (cereal::BinaryInputArchiveWithContainingVectorSize &archive, small_memory_tree::Node<ValueType, ChildrenOffsetEnd> &node)
{
  if (archive.sizeOfVector <= 255)
    {
      auto getUint8_t = uint8_t{};
      archive (node.value, getUint8_t);
      node.childrenOffsetEnd = getUint8_t;
    }
  else
    {
      archive (node.value, node.childrenOffsetEnd);
    }
}
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

template <typename ValueType, typename ChildrenOffsetEnd>
void
save (cereal::BinaryOutputArchiveWithContainingVectorSize &archive, small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEnd> const &vectorOfNodes)
{
  archive (vectorOfNodes.getNodes ());
}

template <typename ValueType, typename ChildrenOffsetEnd>
void
load (cereal::BinaryInputArchiveWithContainingVectorSize &archive, small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEnd> &smallMemoryTree)
{
  auto nodes = std::vector<small_memory_tree::Node<ValueType, ChildrenOffsetEnd> >{};
  archive (nodes);
  smallMemoryTree = small_memory_tree::SmallMemoryTree<ValueType, ChildrenOffsetEnd>{ nodes };
}
}
