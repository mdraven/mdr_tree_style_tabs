//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace mdr {

    namespace details {

        inline
        bool between(int a, int b, int c) {
            return (a <= b) && (b <= c);
        }

    } /* namespace details */

} /* namespace mdr */
