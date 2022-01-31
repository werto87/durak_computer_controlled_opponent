#ifndef AD23FB0A_B3C3_406D_A401_3231159AE486
#define AD23FB0A_B3C3_406D_A401_3231159AE486

#include "util.hxx"
#include <vector>

typedef std::tuple<std::vector<u_int8_t>, std::vector<std::vector<u_int8_t> > > subsetAndCombinations;

std::vector<subsetAndCombinations> subset (long int k, std::vector<uint8_t> const &numbersToChoseFrom);

#endif /* AD23FB0A_B3C3_406D_A401_3231159AE486 */
