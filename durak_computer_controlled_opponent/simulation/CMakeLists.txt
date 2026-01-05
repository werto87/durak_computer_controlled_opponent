##
## Copyright (c) 2024 Waldemar Schneider (w-schneider1987 at web dot de)
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

target_sources(durak_computer_controlled_opponent PUBLIC
        compressCard.cxx
        database.cxx
        permutation.cxx
        solve.cxx
        util.cxx
        action.cxx
)


install(FILES
        combination.hxx
        compressCard.hxx
        database.hxx
        permutation.hxx
        solve.hxx
        util.hxx
        action.hxx
        DESTINATION include/simulation/durak_computer_controlled_opponent
        )
