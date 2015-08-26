//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "NodeUtils.hpp"

#include <cassert>

void mdr::details::unreg_all_leaves(Node* root) {
    // Используется такой способ удаления, так как вызов
    // unregLeaf приведёт к инвалидации итераторов на m_leaves

    const QList<Node*>& leaves = root->getLeaves();
    while(!leaves.empty())
        root->unregLeaf(leaves.front());
}

void mdr::details::add_prev_sibling(Node* sibling, Node* add) {
    Node* parent = sibling->getParent();
    assert(parent != nullptr);

    int idx = parent->getLeaves().indexOf(sibling);
    assert(idx != -1);

    add->setParent(parent, idx);
}
