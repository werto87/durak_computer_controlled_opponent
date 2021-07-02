#include "src/cuda/cuda.cuh"
#include <assert.h>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>
#include <thrust/functional.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/reduce.h>
#include <thrust/scan.h>
#include <thrust/sequence.h>
constexpr boost::multiprecision::uint128_t
factorial (size_t n)
{
  unsigned i = 1;
  boost::multiprecision::uint128_t factorial = 1;
  while (i < n)
    {
      ++i;
      factorial *= i;
    }
  return factorial;
}

size_t constexpr
combintions (size_t setOfNumbersSize, size_t subsetsSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetsSize / 2)) * factorial (subsetsSize / 2))).convert_to<size_t> ();
}

size_t constexpr
combintions2 (size_t setOfNumbersSize, size_t subsetsSize)
{
  return (factorial (setOfNumbersSize) / (factorial (setOfNumbersSize - (subsetsSize)) * factorial (subsetsSize / 2) * factorial (subsetsSize / 2))).convert_to<size_t> ();
}

thrust::host_vector<thrust::host_vector<u_int8_t> >
subset (size_t subsetSize, thrust::host_vector<u_int8_t> setOfNumbers)
{
  if (subsetSize % 2 != 0) return {};
  auto subsets = thrust::host_vector<thrust::host_vector<u_int8_t> > (combintions (setOfNumbers.size (), subsetSize));
  for_each_permuted_combination (setOfNumbers.begin (), setOfNumbers.begin () + subsetSize / 2, setOfNumbers.end (), [&subsets, elementCount = 0U, subsetSize] (auto begin, auto end) mutable {
    if (std::is_sorted (begin, end))
      {
        subsets[(elementCount)] = thrust::host_vector<u_int8_t> (subsetSize / 2);
        for (auto i = 0U; begin != end; i++, begin++)
          {
            subsets[(elementCount)][i] = *begin;
          }
        elementCount++;
      }
  });
  return subsets;
}

bool
validNumbers (thrust::device_vector<bool> const &result, size_t rowSize, size_t row, thrust::device_vector<int> const &numbersToCeck)
{
  for (auto numberToCheck : numbersToCeck)
    {
      if (result[(numberToCheck * rowSize) + row])
        {
          return false;
        }
    }
  return true;
}

struct printf_functor
{
  __host__ __device__ void
  operator() (int x)
  {
    printf ("%d\n", x);
  }
};

thrust::host_vector<thrust::host_vector<u_int8_t> >
someCalculation ()
{
  auto const tmpVector = std::vector<u_int8_t>{ 0, 1, 2, 3, 4, 5, 6, 7 };
  auto const numbersToTake = 4;
  auto const subsetElementCount = numbersToTake / 2;
  auto const setOfNumbers = thrust::host_vector<u_int8_t> (tmpVector.begin (), tmpVector.end ());
  auto const setOfNumbersSize = setOfNumbers.size ();
  auto subsets = subset (numbersToTake, setOfNumbers);
  auto const subsetsSize = subsets.size ();
  thrust::device_vector<int> d_vector (subsetElementCount * subsetsSize); // TODO should be u_int8_t but care with printing it will be wrong with u_int8_t it will print chars and not numbers
  for (auto i = 0UL; i < subsetsSize; i++)
    {
      for (auto j = 0UL; j < subsetElementCount; j++)
        {
          d_vector[(i * subsetElementCount) + j] = subsets[i][j];
        }
    }
  thrust::device_vector<bool> result (setOfNumbersSize * combintions (setOfNumbersSize, numbersToTake));
  for (auto i = 0UL; i < setOfNumbersSize; i++)
    {
      for (auto j = 0UL; j < subsetsSize; j++)
        {
          result[(i * subsetsSize) + j] = thrust::find_if (thrust::device, d_vector.begin () + (j * subsetElementCount), d_vector.begin () + ((j + 1) * subsetElementCount), [i] __device__ (auto value) { return value == i; }) != d_vector.begin () + ((j + 1) * subsetElementCount);
        }
    }
  for (auto i = 0UL; i < subsetsSize; i++)
    {
      auto subset = thrust::device_vector<int> (subsetElementCount);
      for (auto m = 0UL; m < subset.size (); m++)
        {
          subset[m] = d_vector[i * subsetElementCount + m];
        }
      for (auto j = 0UL; j < subsetsSize; j++)
        {
          if (validNumbers (result, subsetsSize, j, subset))
            {
              auto subsetPart2 = thrust::device_vector<int> (subsetElementCount);
              for (auto m = 0UL; m < subsetPart2.size (); m++)
                {
                  subsetPart2[m] = d_vector[j * subsetElementCount + m];
                }
            }
        }
    }
  return {};
}

// We can batch the operation using `thrust::inclusive_scan_by_key`, which
// scans each group of consecutive equal keys. All we need to do is generate
// the right key sequence. We want the keys for elements on the same row to
// be identical.

// So first, we define an unary function object which takes the index of an
// element and returns the row that it belongs to.

struct which_row : thrust::unary_function<int, int>
{
  int row_length;

  __host__ __device__
  which_row (int row_length_)
      : row_length (row_length_)
  {
  }

  __host__ __device__ int
  operator() (int idx) const
  {
    return idx / row_length;
  }
};

__host__ void
scan_matrix_by_rows1 (thrust::device_vector<int> &u, int n, int m)
{
  // This `thrust::counting_iterator` represents the index of the element.
  thrust::counting_iterator<int> c_first (0);

  // We construct a `thrust::transform_iterator` which applies the `which_row`
  // function object to the index of each element.
  thrust::transform_iterator<which_row, thrust::counting_iterator<int> > t_first (c_first, which_row (m));

  // Finally, we use our `thrust::transform_iterator` as the key sequence to
  // `thrust::inclusive_scan_by_key`.

  // for (auto i = 0; i < n * m; i++)
  //   {
  //     std::cout << *t_first << std::endl;
  //     t_first = t_first + 1;
  //   }
  // thrust::inclusive_scan_by_key (t_first, t_first + n * m, u.begin (), u.begin ());
  thrust::for_each (thrust::device, t_first, t_first + n * m, [=] __host__ __device__ (int i) mutable {
    //
    // float *weight_ptr = thrust::raw_pointer_cast (u.data ()) + weights_begin;

    printf ("%d\n", u[5]);
  });
}

void
someOtherCalc ()
{
  int const n = 2;
  int const m = 5;

  thrust::device_vector<int> u1 (n * m);
  thrust::sequence (u1.begin (), u1.end ());
  for (int i = 0; i < n; ++i)
    {
      for (int j = 0; j < m; ++j)
        {
          //   std::cout << u1[j + m * i] << " ";
        }
      //   std::cout << std::endl;
    }

  scan_matrix_by_rows1 (u1, n, m);
  std::cout << "---------------------------------------------------------------" << std::endl;
  for (int i = 0; i < n; ++i)
    {
      for (int j = 0; j < m; ++j)
        {
          //     std::cout << u1[j + m * i] << " ";
        }
      //   std::cout << std::endl;
    }
}