#pragma once

#include "details/Node.hpp"
#include "details/TabData.hpp"

#include <memory>

class TreeStyleTabs : public QWidget {
    Q_OBJECT;

    //! Корневой узел. Не имеет виджета и нужен для
    //! того чтобы единообразно управлять всеми табами.
    std::unique_ptr<Node<TabData>> m_tabs;

    struct MovingTabs {
        /**
           Перемещаемый мышкой табы.

           Если nullptr, то ничего не двигаем.
        */
        std::unique_ptr<Node<TabData>> m_tabs;
        /**
           Точка в которой располагался верхний tab
        */
        QPoint m_tab_start_position;
        /**
           Значение смещения табов под действием
           вертикальной полосы прокрутки в начале
           перетаскивания табов
        */
        int m_vert_scroll_bar_value;
        /**
           Последнее из полученных значений разницы
           координат мыши при перемещении табов
        */
        QPoint m_mouse_pos_diff;
    } m_moving_tabs;
Q_SIGNALS:
    void tabCloseRequested(Node<TabData>* tab);
    void newActive(Node<TabData>* tab);
    void newTab(Node<TabData>* tab);
    void wasMoved(Node<TabData>* tab);
public:
    explicit TreeStyleTabs(QWidget* parent = nullptr);

    Node<TabData>* getRoot();

    //! Эту функцию нужно вызывать после изменений в дереве
    //! табов
    void updateTabWidgets();

    //! Установить обработку событий для виджета. Установить
    //! для виджета родителя
    void regWidget(ITabWidget* widget);
    //! Снять обработку с виджета, сделать родителем виджета
    //! nullptr
    void unregWidget(ITabWidget* widget);

    //! Создаёт Node, но без виджета
    static Node<TabData>* create();
    //! Удаляет Node, но не трогает виджет
    static free(Node<TabData>* tab);

    //! Удаляет Node, но не участвует в удалении виджетов
    ~TreeStyleTabs() override;
public Q_SLOTS:
    void close(Node<TabData>* tab);

    void show();
    void hide();
};
