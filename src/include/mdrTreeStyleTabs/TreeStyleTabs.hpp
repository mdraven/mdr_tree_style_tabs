//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "details/DragAndDropFilter.hpp"

#include <QtWidgets/QWidget>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QScrollArea>
#include <memory>
#include <functional>

namespace mdr {

    class ITabWidget;
    class ITabWidgetFactory;
    class Node;

    class TreeStyleTabs : public QWidget {
        Q_OBJECT;

        friend QDataStream& operator<<(QDataStream&, const TreeStyleTabs&);
        friend QDataStream& operator>>(QDataStream&, TreeStyleTabs&);

        //! Высота таба
        int m_tab_height = 20;
        //! На сколько пикселов сдвигать вложенный таб
        int m_shift_per_depth = 5;
        //! К какой стороне прижимаются табы
        Qt::AlignmentFlag m_alignment = Qt::AlignmentFlag::AlignRight;

        //! Фабрика по созданию/удалению виджетов используемых
        //! для вкладок
        std::shared_ptr<ITabWidgetFactory> m_tab_widget_factory;

        //! Корневой узел. Не имеет виджета и нужен для
        //! того чтобы единообразно управлять всеми табами.
        //! FIXME: нужно сделать exception-safety
        Node* m_tabs;

        struct WidgetDepth final {
            ITabWidget* m_widget;
            int m_depth;
        };

        //! Отображаемые табы
        //! - Табы расположены в порядке сверху вниз.
        //! - Табы которые не отображаются удаляются из
        //!   этого списка.
        //! Этот список заполняет метод updateTabWidgets и нет смысла
        //! редактировать его откуда-нибудь ещё.
        QList<WidgetDepth> m_visible_tabs;

        //! Node содержит в себе указатель на виджет, но виджет не
        //! содержит указатель на Node для того чтобы не перегружать
        //! разработчика виджета деталями реализации. Поэтому это
        //! отображение хранится тут.
        std::map<ITabWidget*, Node*> m_widget_to_node;

        //! Сюда помещаются узля для отложенного удаления
        QList<Node*> m_nodes_for_delete;

        struct MovingTabs {
            //! Перемещаемый мышкой табы.
            //! Если nullptr, то ничего не двигаем.
            //! FIXME: нужно сделать exception-safety
            Node* m_tabs;
            //! Тоже что и m_visible_tabs для обычных табов
            QList<WidgetDepth> m_visible_tabs;
            //! Точка в которой располагался верхний tab
            QPoint m_tab_start_position;
            //! Значение смещения табов под действием
            //! вертикальной полосы прокрутки в начале
            //! перетаскивания табов
            int m_vert_scroll_bar_value;
            //! Последнее из полученных значений разницы
            //! координат мыши при перемещении табов
            QPoint m_mouse_pos_diff;

            MovingTabs();
        } m_moving_tabs;

        QScrollArea m_scroll_area;
        QWidget m_frame;
        details::DragAndDropFilter m_filter;
        QButtonGroup m_button_group;

        void unregAndDeleteWidget(ITabWidget* widget);

        //! Обработчик события "щелчок по табу"
        void clickOnTab(const QPoint& p);

        //! Обработчик события "перетаскивание таба"
        void dragTabs(const QPoint& start, const QPoint& pos);

        //! Обработчик события "установить таб на выбранное место"
        void dropTabs(const QPoint& pos);

        //! Обработчик вращения колеса мышки
        void wheel();

        //! Изменяет размеры таба и сдвигает его вправо
        //! в зависимости от значения вложенности.
        //! \param offset_y добавочное смещение для координаты Y
        void fixTabPosition(const WidgetDepth& visible_tab, int offset_y = 0);

        //! Изменяет размеры табов и сдвигает их вправо
        //! в зависимости от значения вложенности.
        //! \param offset_y добавочное смещение для координаты Y
        void fixTabsPosition(const QList<WidgetDepth>& visible_tabs, int offset_y = 0);

        //! Обработать вращение полосы прокрутки при
        //! поднесении перемещаемых табов к краям окна
        void scrollTabsList(const QPoint& global_mouse_pos);

        //! Установить перемещаемые табы в позицию pos
        void moveMovingTabs(QPoint pos);

        //! Ищет самый ближайший таб снизу. Если не находит, то возвращает
        //! m_tabs.end().
        //! \warning При вызове этого метода должны быть перемещаемые табы.
        QList<WidgetDepth>::Iterator
        searchTabWidgetUnderMovingRootTab();

        //! Сдвигает табы под перемещаемым табом
        void moveTabsUnderMovingTabs();

        //! Возвращает порог по которому определяется прикрепляется таб
        //! или сдвигает нижележащий
        int getTabHeightThreshold() const;
    protected:
        void resizeEvent(QResizeEvent* event) override;
        bool event(QEvent* e) override;
    Q_SIGNALS:
        void tabCloseRequested(Node* tab);
        void newChecked(Node* tab);
        void newTab(Node* tab);
        void wasMoved(Node* tab);
        void wasExpandedCollapsed(Node* tab);
    public:
        explicit TreeStyleTabs(QWidget* parent/* = nullptr*/,
                               std::shared_ptr<ITabWidgetFactory> tab_widget_factory);

        Node* getRoot();
        const Node* getRoot() const;

        int getTabHeight() const;
        void setTabHeight(int height);

        int getShiftPerDepth() const;
        void setShiftPerDepth(int shift);

        Qt::AlignmentFlag getAlignment() const;
        //! \param alignment или Qt::AlignLeft или Qt::AlignRight
        void setAlignment(Qt::AlignmentFlag alignment);

        //! Возвращает указатель на активный таб или nullptr
        Node* getChecked();
        const Node* getChecked() const;

        //! Эту функцию нужно вызывать после изменений в дереве
        //! табов
        void updateTabWidgets();

        //! Создаёт Node с виджетом. Созданый Node имеет родителем
        //! корневой Node
        Node* create();

        ~TreeStyleTabs() override;
    public Q_SLOTS:
        void close(Node* tab);

        void show();
        void hide();

        void clear();
    };

    QDataStream&
    operator<<(QDataStream& ds, const TreeStyleTabs& tst);

    QDataStream&
    operator>>(QDataStream& ds, TreeStyleTabs& tst);

} /* namespace mdr */
