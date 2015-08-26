#pragma once

#include <QtCore/QList>

class Node final {
    Node* m_parent = nullptr;
    QList<Node*> m_leaves;

    // ITabWidget* m_widget;
public:
    Node() = default;
    Node(const Node&) = delete;
    Node(Node&&);

    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&);

    void setParent(Node* parent);
    Node* getParent();

    // void setWidget(ITabWidget* widget);
    // ITabWidget* getWidget();

    //! Возвращает константный список константных указателей.
    //! Те сами Node мы изменять можем. Поэтому метод не const'ый.
    const QList<Node*>& getLeaves();

    //! Добавить лист.
    //! Делает себя родителем leaf.
    void regLeaf(Node* leaf);
    //! Удаляет leaf из списка детей, но не освобождает
    //! память.
    //! Делает родителем leaf nullptr.
    void unregLeaf(Node* leaf);

    //! Освобождает память от всех своих детей
    ~Node();
};
