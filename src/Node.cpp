//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <mdrTreeStyleTabs/Node.hpp>

#include "details/debug.hpp"

#include <cassert>
#include <mdrTreeStyleTabs/ITabWidget.hpp>

mdr::Node::
Node(ITabWidget* widget)
    : m_widget(widget) {}

void mdr::Node::
setParent(Node* parent, int index) {
    // WARN: мы не можем использовать (un)refLeaf так как будет рекурсия

    // Если родитель изменился
    if(m_parent != parent) {
        // Удаляем у старого родителя
        if(m_parent) {
            m_parent->m_leaves.removeOne(this);

            // Если у старого родителя список детей пуст, то удаляем у его виджета
            // кнопку свернуть/развернуть
            if(m_parent->m_widget && m_parent->m_leaves.empty()) {
                ITabWidget* widget = m_parent->m_widget;

                widget->toggleExpandCollapseButton(false);
            }
        }

        // Добавляем к новому родителю
        if(parent) {
            if(index == -1)
                parent->m_leaves.push_back(this);
            else
                parent->m_leaves.insert(index, this);

            // Добавляем виджету нового родителя кнопку свернуть/развернуть
            if(parent->m_widget) {
                ITabWidget* widget = parent->m_widget;

                widget->toggleExpandCollapseButton(true);
            }
        }

        m_parent = parent;
    }
    // Если родителя не было и не будет
    else if(m_parent == nullptr && parent == nullptr) {}
    // Если родитель не изменился, но он есть
    else {
        QList<Node*>& parent_leaves = m_parent->m_leaves;

        int cur_idx = parent_leaves.indexOf(this);

        // Не изменился ни родитель, ни индекс
        if(cur_idx == index)
            return;

        // Если index == -1, то это значит, что перемещаем в конец
        if(index == -1)
            parent_leaves.move(cur_idx, parent_leaves.size() - 1);
        else
            parent_leaves.move(cur_idx, index);
    }
}

mdr::Node* mdr::Node::
getParent() {
    return m_parent;
}

const mdr::Node* mdr::Node::
getParent() const {
    return m_parent;
}

mdr::ITabWidget* mdr::Node::
getWidget() {
    return m_widget;
}

const mdr::ITabWidget* mdr::Node::
getWidget() const {
    return m_widget;
}

void mdr::Node::
regLeaf(Node* leaf, int index) {
    leaf->setParent(this, index);
}

void mdr::Node::
unregLeaf(Node* leaf) {
    assert(leaf->m_parent == this);
    leaf->setParent(nullptr);
}

const QList<mdr::Node*>& mdr::Node::
getLeaves() {
    return m_leaves;
}

const QList<const mdr::Node*>& mdr::Node::
getLeaves() const {
    // WARN: hack
    return reinterpret_cast<const QList<const Node*>&>(m_leaves);
}

void mdr::Node::
free(const TabWidgetDeleter& tab_widget_deleter) {
    auto free_widget = [&tab_widget_deleter](ITabWidget*& widget) {
        if(widget) {
            widget->setParent(nullptr);

            tab_widget_deleter(widget);

            widget = nullptr;
        }
    };

    // если был установлен родитель, то удалить себя
    // из родителя
    setParent(nullptr);

    free_widget(m_widget);

    for(Node* leaf : m_leaves) {
        free_widget(leaf->m_widget);

        // для того чтобы ребёнок не повредил m_leaves по которому
        // мы сейчас проходим
        leaf->m_parent = nullptr;

        leaf->free(tab_widget_deleter);
    }

    // для того чтобы деструктор знал, что free был вызван
    m_leaves.clear();

    delete this;
}

mdr::Node::
~Node() {
    IFDEBUG printf("~Node()\n");

    assert(m_widget == nullptr);
    assert(m_leaves.empty());
    assert(m_parent == nullptr);
}
