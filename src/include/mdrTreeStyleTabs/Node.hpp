//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <QtCore/QList>
#include <functional>

namespace mdr {

    class ITabWidget;

    class Node final {
        Node* m_parent = nullptr;
        QList<Node*> m_leaves;

        ITabWidget* m_widget = nullptr;
    public:
        Node(const Node&) = delete;
        //! Дорого и не часто нужно
        Node(Node&&) = delete;

        explicit Node(ITabWidget* widget = nullptr);

        Node& operator=(const Node&) = delete;
        Node& operator=(Node&&) = delete;

        //! \param index позиция в которую нужно вставить Node;
        //!              -1 -- в конец
        void setParent(Node* parent, int index = -1);

        Node* getParent();
        const Node* getParent() const;

        ITabWidget* getWidget();
        const ITabWidget* getWidget() const;

        //! Возвращает константный список константных указателей.
        //! Те сами Node мы изменять можем. Поэтому метод не const'ый.
        const QList<Node*>& getLeaves();
        const QList<const Node*>& getLeaves() const;

        //! Добавить лист.
        //! Делает себя родителем leaf.
        //! \param index позиция в которую нужно вставить Node;
        //!              -1 -- в конец
        void regLeaf(Node* leaf, int index = -1);
        //! Удаляет leaf из списка детей, но не освобождает
        //! память.
        //! Делает родителем leaf nullptr.
        void unregLeaf(Node* leaf);

        using TabWidgetDeleter = std::function<void(ITabWidget*)>;
        //! Освобождает память от всех своих детей.
        void free(const TabWidgetDeleter& tab_widget_deleter);

        //! До вызова деструктора должен быть вызван free
        ~Node();
    };

} /* namespace mdr */
