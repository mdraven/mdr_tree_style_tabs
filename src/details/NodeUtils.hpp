//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <mdrTreeStyleTabs/Node.hpp>

namespace mdr {

    namespace details {

        //! Обходит дерево в глубину. Для каждого узла начиная с root вызывает
        //! функцию func. Если func возвращает true, то walk_tree продолжает
        //! обход в глубину. Если false, то обходит другие узлы.
        template <class StopFunc>
        void walk_tree(Node* root, const int root_depth, StopFunc func) {
            if(!func(root, root_depth))
                return;

            const int leaf_depth = root_depth + 1;

            for(Node* leaf : root->getLeaves())
                walk_tree(leaf, leaf_depth, func);
        }

        //! Разрегистрирует всех детей у root
        void unreg_all_leaves(Node* root);

        //! Устанавливает родителя для add таким образом, чтобы
        //! он(add) был предыдущим ребёнком относительно sibling
        void add_prev_sibling(Node* sibling, Node* add);

    } /* namespace details */

} /* namespace mdr */
