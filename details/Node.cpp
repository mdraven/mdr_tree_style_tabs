
#include "Node.hpp"

Node::
Node(Node&& n)
    : m_parent(n.m_parent),
      m_leaves(std::move(n.m_leaves)) {
    n.setParent(nullptr);
    n.m_leaves.clear();
}

void Node::
setParent(Node* parent) {
    // WARN: мы не можем использовать (un)refLeaf так как будет рекурсия
    if(parent)
        parent->m_leaves.push_back(this);
    else
        parent->m_leaves.removeOne(this);

    m_parent = parent;
}

Node* Node::
getParent() {
    return m_parent;
}

// void Node::
// setWidget(ITabWidget* widget) {
//     m_widget = widget;
// }

// ITabWidget* getWidget() {
//     return m_widget;
// }

void Node::
regLeaf(Node* leaf) {
    // WARN: мы не можем использовать setParent так как будет рекурсия
    leaf->m_parent = this;
    m_leaves.push_back(leaf);
}

void Node::
unregLeaf(Node* leaf) {
    // WARN: мы не можем использовать setParent так как будет рекурсия
    leaf->m_parent = nullptr;
    m_leaves.removeOne(leaf);
}

const QList<Node*>& Node::
getLeaves() {
    return m_leaves;
}

Node::
~Node() {
    for(Node* leaf : m_leaves) {
        delete leaf;
    }
}
