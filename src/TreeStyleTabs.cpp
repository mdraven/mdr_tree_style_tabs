//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <mdrTreeStyleTabs/TreeStyleTabs.hpp>

#include "details/NodeUtils.hpp"
#include "details/between.hpp"
#include "details/debug.hpp"

#include <QtCore/QCoreApplication>
#include <QtCore/QDataStream>
#include <QtWidgets/QScrollBar>
#include <algorithm>
#include <cassert>
#include <functional>
#include <mdrTreeStyleTabs/ITabWidget.hpp>
#include <mdrTreeStyleTabs/ITabWidgetFactory.hpp>
#include <mdrTreeStyleTabs/Node.hpp>

const quint32 g_magic_num_for_serialization = 0x5e764ea2;

static QEvent::Type g_delete_node_event = QEvent::None;

mdr::TreeStyleTabs::
TreeStyleTabs(QWidget* parent,
              std::shared_ptr<ITabWidgetFactory> tab_widget_factory)
    : QWidget(parent),
      m_tab_widget_factory(tab_widget_factory),
      m_tabs(new Node()),
      m_scroll_area(this),
      m_frame(&m_scroll_area),
      m_filter(this,
               std::bind(&mdr::TreeStyleTabs::clickOnTab, this, std::placeholders::_1),
               std::bind(&mdr::TreeStyleTabs::dragTabs, this, std::placeholders::_1, std::placeholders::_2),
               std::bind(&mdr::TreeStyleTabs::dropTabs, this, std::placeholders::_1),
               std::bind(&mdr::TreeStyleTabs::wheel, this)),
      m_button_group(this) {

    m_scroll_area.setWidget(&m_frame);
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(&m_button_group, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            this, [this](QAbstractButton* btn) {
                auto* widget = static_cast<ITabWidget*>(btn);

                assert(widget != nullptr);
                assert(m_widget_to_node.find(widget) != m_widget_to_node.end());

                Q_EMIT newChecked(m_widget_to_node[widget]);
            });

    // Регистрируем событие для отложенного удаления Node
    if(g_delete_node_event == QEvent::None) {
        int event_id = QEvent::registerEventType();

        if(event_id == -1)
            throw std::runtime_error("TreeStyleTabs: g_delete_node_event == undefined");

        g_delete_node_event = static_cast<QEvent::Type>(event_id);
    }
}

int mdr::TreeStyleTabs::
getTabHeightThreshold() const {
    return m_tab_height / 2;
}

mdr::Node* mdr::TreeStyleTabs::
getRoot() {
    return m_tabs;
}

const mdr::Node* mdr::TreeStyleTabs::
getRoot() const {
    return m_tabs;
}

int mdr::TreeStyleTabs::
getTabHeight() const {
    return m_tab_height;
}

void mdr::TreeStyleTabs::
setTabHeight(int height) {
    assert(height > 0);

    m_tab_height = height;

    fixTabsPosition(m_visible_tabs);
    fixTabsPosition(m_moving_tabs.m_visible_tabs);
}

int mdr::TreeStyleTabs::
getShiftPerDepth() const {
    return m_shift_per_depth;
}

void mdr::TreeStyleTabs::
setShiftPerDepth(int shift) {
    m_shift_per_depth = shift;

    fixTabsPosition(m_visible_tabs);
    fixTabsPosition(m_moving_tabs.m_visible_tabs);
}

Qt::AlignmentFlag mdr::TreeStyleTabs::
getAlignment() const {
    return m_alignment;
}

void mdr::TreeStyleTabs::
setAlignment(Qt::AlignmentFlag alignment) {
    assert(alignment == Qt::AlignLeft || alignment == Qt::AlignRight);

    m_alignment = alignment;

    fixTabsPosition(m_visible_tabs);
    fixTabsPosition(m_moving_tabs.m_visible_tabs);
}

void mdr::TreeStyleTabs::
scrollTabsList(const QPoint& global_mouse_pos) {
    if(m_moving_tabs.m_tabs->getLeaves().empty())
        return;

    // расстояние от края для начала прокручивания
    const int edge_in_px = 10;

    const QPoint local_pos = mapFromGlobal(global_mouse_pos);

    if(local_pos.y() + edge_in_px > height()) {
        QScrollBar* scroll = m_scroll_area.verticalScrollBar();
        int value = scroll->value();
        int max_val = scroll->maximum();
        int step = scroll->singleStep();

        scroll->setValue(std::min(value + step, max_val));
    }
    else if(local_pos.y() - edge_in_px < 0) {
        QScrollBar* scroll = m_scroll_area.verticalScrollBar();
        int value = scroll->value();
        int min_val = scroll->minimum();
        int step = scroll->singleStep();

        scroll->setValue(std::max(value - step, min_val));
    }
}

void mdr::TreeStyleTabs::
moveMovingTabs(QPoint pos) {
    if(!m_moving_tabs.m_tabs->getLeaves().empty()) {
        ITabWidget* root_widget = m_moving_tabs.m_tabs->getLeaves()[0]->getWidget();
        assert(root_widget != nullptr);

        QPoint diff = pos - root_widget->pos();

        details::walk_tree(m_moving_tabs.m_tabs, 0,
                           [&diff](Node* tab, int) {
                               ITabWidget* widget = tab->getWidget();
                               if(widget == nullptr)
                                   return true;

                               widget->move(widget->pos() + diff);
                               return widget->isExpanded();
                           });
    }
}

decltype(mdr::TreeStyleTabs::m_visible_tabs)::Iterator mdr::TreeStyleTabs::
searchTabWidgetUnderMovingRootTab() {
    assert(!m_moving_tabs.m_visible_tabs.empty());

    const WidgetDepth& root_wd = m_moving_tabs.m_visible_tabs[0];

    auto it = std::lower_bound(m_visible_tabs.begin(), m_visible_tabs.end(),
                               root_wd, [this](const WidgetDepth& a,
                                               const WidgetDepth& b) {
                                   // добавляем m_tab_height для того чтобы
                                   // можно было перетаскиваемый мышкой таб занести
                                   // сверху
                                   ITabWidget* a_widget = a.m_widget;
                                   ITabWidget* b_widget = b.m_widget;
                                   return a_widget->y() + m_tab_height < b_widget->y();
                               });
    return it;
}

void mdr::TreeStyleTabs::
moveTabsUnderMovingTabs() {
    if(m_moving_tabs.m_tabs->getLeaves().empty())
        return;

    const QList<WidgetDepth>::Iterator tab_under_it = searchTabWidgetUnderMovingRootTab();
    ITabWidget* moving_tabs_root_widget = m_moving_tabs.m_visible_tabs[0].m_widget;

    // Верхняя граница перемещаемых табов
    const int moving_tabs_upper = moving_tabs_root_widget->y();

    // Передвинуть элемент под перемещаемыми табами вниз
    const bool move_down = [&] {
        if(tab_under_it == m_visible_tabs.end())
            return false;

        int tab_upper = tab_under_it->m_widget->y();
        int tab_lower = tab_under_it->m_widget->y() + getTabHeightThreshold();

        ITabWidget* moving_tabs_last_widget = m_moving_tabs.m_visible_tabs.back().m_widget;
        int moving_tabs_lower = moving_tabs_last_widget->y() + getTabHeightThreshold();

        bool move_down = (moving_tabs_upper < tab_lower) && !details::between(tab_upper, moving_tabs_lower, tab_lower);
        return move_down;
    }();

    //! Выровнить перемещаемые табы по глубине depth
    auto set_moving_visible_tabs_depth = [this](int depth) {
        QList<WidgetDepth>& visible_tabs = m_moving_tabs.m_visible_tabs;

        if(visible_tabs.isEmpty())
            return;

        int d = depth - visible_tabs.front().m_depth;

        for(WidgetDepth& wd : visible_tabs)
            wd.m_depth += d;
    };

    if(tab_under_it == m_visible_tabs.end()) {
        // Перемещаемые табы находится в конце списка табов,
        // поэтому делаем им вложенность == 0.
        set_moving_visible_tabs_depth(0);
        fixTabsPosition(m_moving_tabs.m_visible_tabs, moving_tabs_upper);
    }
    else {
        int depth = tab_under_it->m_depth;

        if(move_down) {
            // Перемещаемые табы будет на том же уровне, что и лежащий
            // под ним.
            set_moving_visible_tabs_depth(depth);
            fixTabsPosition(m_moving_tabs.m_visible_tabs, moving_tabs_upper);
        }
        else {
            // Перемещаемые таб будут вложены в таб лежащий под ним.
            set_moving_visible_tabs_depth(depth + 1);
            fixTabsPosition(m_moving_tabs.m_visible_tabs, moving_tabs_upper);
        }
    }

    // Двигаем табы под перемещаемыми табами
    int idx = 0;
    for(auto it = m_visible_tabs.begin(), endit = m_visible_tabs.end();
        it != endit; ++it) {
        WidgetDepth& wd = *it;

        // Этот таб не находится под перемещаемыми.
        if(it != tab_under_it) {
            fixTabPosition(wd, idx * m_tab_height);

            ++idx;
        }
        // Сдвигаем таб который находится под перемещаемыми
        // табами вниз
        else if(it == tab_under_it && move_down) {
            idx += m_moving_tabs.m_visible_tabs.size();

            fixTabPosition(wd, idx * m_tab_height);

            ++idx;
        }
        else if(it == tab_under_it && !move_down) {
            fixTabPosition(wd, idx * m_tab_height);

            idx += m_moving_tabs.m_visible_tabs.size();

            ++idx;
        }
    }
}

mdr::Node* mdr::TreeStyleTabs::
create() {
    ITabWidget* widget = m_tab_widget_factory->create();
    widget->setParent(&m_frame);
    widget->installEventFilter(&m_filter);
    widget->setCheckable(true);

    m_button_group.addButton(widget);

    Node* node = new Node(widget);

    m_widget_to_node[widget] = node;

    node->setParent(m_tabs);

    updateTabWidgets();

    Q_EMIT newTab(node);

    return node;
}

void mdr::TreeStyleTabs::
unregAndDeleteWidget(ITabWidget* widget) {
    widget->setParent(nullptr);
    widget->removeEventFilter(&m_filter);
    m_button_group.removeButton(widget);

    m_widget_to_node.erase(widget);

    m_tab_widget_factory->free(widget);
}

void mdr::TreeStyleTabs::
close(Node* tab) {
    // Игнорируем запросы удаления корня
    if(tab == m_tabs || tab == m_moving_tabs.m_tabs)
        return;

    Node* parent = tab->getParent();

    // Узнаём индекс удаляемого таба для того чтобы
    // вставить в это место ребёнка удаляемого таба
    const int tab_index = [parent, tab] {
        if(parent)
            return parent->getLeaves().indexOf(tab);
        // Если нет родителя, то, вероятно, это откреплённая от дерева
        // вкладка. Или для этой вкладки два раза вызвали close.
        else
            return 0;
    }();

    tab->setParent(nullptr);

    // Делаем копию списка листьев так как после использования
    // setParent итераторы на внутренний список в tab будут некорректны
    QList<Node*> leaves = tab->getLeaves();
    // Разрегистрируем Node'ы для того чтобы free не удалил их
    details::unreg_all_leaves(tab);

    // Откладываем удаление для того чтобы те кто обрабатывает сигнал
    // tabCloseRequested не получили указатель в никуда
    if(!m_nodes_for_delete.contains(tab))
        m_nodes_for_delete.push_back(tab);
    QCoreApplication::postEvent(this, new QEvent(g_delete_node_event));
    struct tab;

    if(!leaves.isEmpty()) {
        Node* first_child = leaves.front();
        first_child->setParent(parent, tab_index);
        leaves.pop_front();

        for(Node* tab : leaves)
            tab->setParent(first_child);
    }

    updateTabWidgets();
}

void mdr::TreeStyleTabs::
clickOnTab(const QPoint& p) {
    const QPoint local_pos = mapFromGlobal(p);

    auto* widget = static_cast<ITabWidget*>(childAt(local_pos));
    if(widget == nullptr)
        return;

    const QPoint rel_tab_pos = local_pos - widget->pos();

    Node* tab = m_widget_to_node[widget];

    if(widget->isExpandCollapseButton(rel_tab_pos)) {
        if(widget->isExpanded())
            widget->setCollapsed();
        else if(widget->isCollapsed())
            widget->setExpanded();

        updateTabWidgets();

        Q_EMIT wasExpandedCollapsed(tab);
    }
    else if(widget->isCloseButton(rel_tab_pos))
        Q_EMIT tabCloseRequested(tab);
}

void mdr::TreeStyleTabs::
dragTabs(const QPoint& start, const QPoint& pos) {
    scrollTabsList(pos);

    if(m_moving_tabs.m_tabs->getLeaves().empty()) {
        // цепляем табы под курсором только если
        // курсор прошёл определённое расстояние

        const int drag_sensitivity = 3;
        if((start - pos).manhattanLength() < drag_sensitivity)
            return;

        const QPoint local_pos = mapFromGlobal(pos);

        auto* widget = static_cast<ITabWidget*>(childAt(local_pos));
        if(widget == nullptr)
            return;

        m_moving_tabs.m_tab_start_position = widget->pos();
        m_moving_tabs.m_vert_scroll_bar_value = m_frame.y();

        Node* tab = m_widget_to_node[widget];
        // Сделать вкладку перемещаемой
        tab->setParent(m_moving_tabs.m_tabs);

        // Обновляем списки видимых табов после перестроения дерева
        updateTabWidgets();

        // поднять вкладки над остальными
        details::walk_tree(tab, 0, [](Node* tab, int) {
                ITabWidget* widget = tab->getWidget();
                if(widget == nullptr)
                    return true;

                widget->raise();
                return widget->isExpanded();
            });
    }

    // После первого if может быть уже не пустым
    if(!m_moving_tabs.m_tabs->getLeaves().empty()) {
        m_moving_tabs.m_mouse_pos_diff = pos - start;

        m_moving_tabs.m_mouse_pos_diff.setX(0); // двигаем только по вертикали

        // поправка на прокручивание окна scrollbar'ом
        QPoint scroll_fix(0, m_moving_tabs.m_vert_scroll_bar_value - m_frame.y());

        moveMovingTabs(m_moving_tabs.m_tab_start_position +
                       m_moving_tabs.m_mouse_pos_diff +
                       scroll_fix);

        moveTabsUnderMovingTabs();
    }
}

void mdr::TreeStyleTabs::
dropTabs(const QPoint& pos) {
    Q_UNUSED(pos);

    if(m_moving_tabs.m_visible_tabs.empty())
        return;

    assert(m_moving_tabs.m_tabs->getLeaves().size() == 1);

    Node* moving_tabs_node = m_moving_tabs.m_tabs->getLeaves()[0];

    if(m_visible_tabs.empty())
        moving_tabs_node->setParent(m_tabs);
    else {
        QList<WidgetDepth>::Iterator tab_under_it = searchTabWidgetUnderMovingRootTab();

        // Если ниже всех табов
        if(tab_under_it == m_visible_tabs.end())
            moving_tabs_node->setParent(m_tabs);
        else {
            int tab_upper = tab_under_it->m_widget->y();

            WidgetDepth& moving_tabs_root = m_moving_tabs.m_visible_tabs[0];
            int moving_tab_upper = moving_tabs_root.m_widget->y();

            Node* tab_under_node = m_widget_to_node[tab_under_it->m_widget];

            // Устанавливаем перемещаемые табы на таб лежащий под ними
            if(tab_upper < moving_tab_upper) {
                // Если таб свёрнут, то нужно добавлять в конец
                if(tab_under_it->m_widget->isCollapsed())
                    moving_tabs_node->setParent(tab_under_node);
                // Если развёрнут или не имеет детей, то в начало
                else
                    moving_tabs_node->setParent(tab_under_node, 0);
            }
            // Устанавливаем перемещаемые табы в случае если они выше таба
            // лежащего под ними
            else
                details::add_prev_sibling(tab_under_node, moving_tabs_node);
        }
    }

    assert(m_moving_tabs.m_tabs->getLeaves().empty());

    updateTabWidgets();

    Q_EMIT wasMoved(moving_tabs_node);
}

void mdr::TreeStyleTabs::
wheel() {
    // Если мы перетаскиваем вкладки, то будем учитывать
    // прокрутку списка с вкладками для пересчёта смещения
    // перетаскиваемых вкладок
    if(!m_moving_tabs.m_visible_tabs.empty()) {
        QPoint scroll_fix(0, m_moving_tabs.m_vert_scroll_bar_value - m_frame.y());

        moveMovingTabs(m_moving_tabs.m_tab_start_position +
                       m_moving_tabs.m_mouse_pos_diff +
                       scroll_fix);

        moveTabsUnderMovingTabs();
    }
}

void mdr::TreeStyleTabs::
show() {
    QWidget::show();

    m_scroll_area.show();
    m_frame.show();

    for(WidgetDepth& wd : m_visible_tabs)
        wd.m_widget->show();
}

void mdr::TreeStyleTabs::
hide() {
    for(WidgetDepth& wd : m_visible_tabs)
        wd.m_widget->hide();

    m_frame.hide();
    m_scroll_area.hide();

    QWidget::hide();
}

void mdr::TreeStyleTabs::
resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    m_scroll_area.resize(event->size());

    // Свободное место после табов. Без него табы будут обрезаться
    // краем widget'а.
    const int padding = 2;
    m_frame.resize(event->size().width(),
                   (m_visible_tabs.size() + padding) * m_tab_height);

    fixTabsPosition(m_visible_tabs);
}

void mdr::TreeStyleTabs::
fixTabPosition(const WidgetDepth& visible_tab,
               int offset_y) {
    // Учтём ширину scrollbar'а,
    // иначе scrollbar закрывает часть tab'а
    int scroll_bar_width = 0;

    // Раньше проверял на scroll->isVisible(),
    // но событие resizeEvent приходило в QScrollArea
    // поздно и QScrollArea неуспевало обновить scrollbar.
    if(m_frame.height() > m_scroll_area.height()) {
        QScrollBar* scroll = m_scroll_area.verticalScrollBar();
        scroll_bar_width = scroll->sizeHint().width();
    }

    ITabWidget* widget = visible_tab.m_widget;
    int depth = visible_tab.m_depth;

    // сдвиг таба связанный с глубиной вложенности
    int shift = m_shift_per_depth * depth;

    if(m_alignment == Qt::AlignLeft)
        widget->move(0, offset_y);
    else if(m_alignment == Qt::AlignRight)
        widget->move(shift, offset_y);

    widget->resize(m_frame.width() - shift - scroll_bar_width - 2/*FIXME:кажется это толщина бордюров, но я не знаю как её получить*/,
                   m_tab_height);
}

void mdr::TreeStyleTabs::
fixTabsPosition(const QList<WidgetDepth>& visible_tabs,
                int offset_y) {

    for(int i = 0; i < visible_tabs.size(); ++i)
        fixTabPosition(visible_tabs[i], m_tab_height * i + offset_y);
}

void mdr::TreeStyleTabs::
updateTabWidgets() {
    IFDEBUG printf("updateTabWidgets() -- is slow\n");

    //! Обновляет список видимых табов visible_tabs из узлов начиная с корня root.
    auto updateTabWidgets = [this](Node* root, QList<WidgetDepth>& visible_tabs) {
        visible_tabs.clear();

        auto only_expanded = [&visible_tabs](Node* tab, int tab_depth) {
            ITabWidget* widget = tab->getWidget();
            // Наверное это m_tabs или m_moving_tabs.m_tabs.
            // Включать их в m_visible_tabs не нужно.
            if(widget == nullptr)
                return true;

            visible_tabs.push_back(WidgetDepth{widget, tab_depth - 1});

            if(widget->isExpanded())
                return true;
            // если свёрнутый или не имеет детей
            else
                return false;
        };
        details::walk_tree(root, 0, only_expanded);

        fixTabsPosition(visible_tabs);
    };

    // Табы которые были видимы до перестройки TreeStyleTabs
    QList<ITabWidget*> visible_before;
    visible_before.reserve(m_visible_tabs.size() + m_moving_tabs.m_visible_tabs.size());

    for(WidgetDepth& wd : m_visible_tabs)
        visible_before.push_back(wd.m_widget);

    for(WidgetDepth& wd : m_moving_tabs.m_visible_tabs)
        visible_before.push_back(wd.m_widget);

    updateTabWidgets(m_tabs, m_visible_tabs);
    updateTabWidgets(m_moving_tabs.m_tabs, m_moving_tabs.m_visible_tabs);

    auto delete_visible_and_show_new
        = [&visible_before](QList<WidgetDepth>& visible_tabs) {
        for(WidgetDepth& wd : visible_tabs)
            // Удаляем виджеты которые были видимыми и до изменения
            // и после изменения. Если появился новый виджет, то делаем
            // его видимым.
            if(!visible_before.removeOne(wd.m_widget))
                wd.m_widget->show();
    };

    // Удаляем из visible_before те что всё ещё видимы и делаем
    // видимыми новые табы
    delete_visible_and_show_new(m_visible_tabs);
    delete_visible_and_show_new(m_moving_tabs.m_visible_tabs);

    // В visible_before остались только те, которые теперь невидимы
    for(ITabWidget* widget : visible_before)
        widget->hide();
}

bool mdr::TreeStyleTabs::
event(QEvent* e) {
    // Отложенное удаленние узлов
    if(e->type() == g_delete_node_event) {
        auto func = std::bind(&mdr::TreeStyleTabs::unregAndDeleteWidget, this,
                              std::placeholders::_1);

        for(Node* tab : m_nodes_for_delete)
            tab->free(func);
        m_nodes_for_delete.clear();

        return true;
    }
    else if(QWidget::event(e))
        return true;

    return false;
}

void mdr::TreeStyleTabs::
clear() {
    auto delete_tabs = [this](Node* root) {
        for(Node* tab : root->getLeaves()) {
            tab->setParent(nullptr);
            m_nodes_for_delete.push_back(tab);
            QCoreApplication::postEvent(this, new QEvent(g_delete_node_event));
        }
    };

    delete_tabs(m_tabs);
    delete_tabs(m_moving_tabs.m_tabs);

    m_visible_tabs.clear();
    m_moving_tabs.m_visible_tabs.clear();
}

mdr::Node* mdr::TreeStyleTabs::
getChecked() {
    return const_cast<Node*>(getChecked());
}

const mdr::Node* mdr::TreeStyleTabs::
getChecked() const {
    auto* tab = static_cast<ITabWidget*>(m_button_group.checkedButton());
    if(tab == nullptr)
        return nullptr;

    return m_widget_to_node.at(tab);
}

mdr::TreeStyleTabs::
~TreeStyleTabs() {
    IFDEBUG printf("~TreeStyleTabs()\n");

    auto func = std::bind(&mdr::TreeStyleTabs::unregAndDeleteWidget, this,
                          std::placeholders::_1);
    m_tabs->free(func);
    m_moving_tabs.m_tabs->free(func);
}

mdr::TreeStyleTabs::MovingTabs::
MovingTabs()
    : m_tabs(new Node()) {}

QDataStream& mdr::
operator<<(QDataStream& ds, const TreeStyleTabs& tst) {
    ds << g_magic_num_for_serialization;

    const quint8 version = 1;

    ds << version;

    auto save_tree = [&ds](Node* tab) {
        details::walk_tree(tab, 0, [&ds](Node* tab, int) {
                const QList<Node*>& leaves = tab->getLeaves();
                ds << leaves.size();

                // Обрабатываем случай, когда нет виджета
                if(tab->getWidget()) {
                    ds << true;
                    ds << *tab->getWidget();
                }
                else
                    ds << false;

                return true;
            });
    };

    save_tree(tst.m_tabs);

    // Сохраняем перемещаемые табы в конец списка.
    // Пусть хоть так, чем пропадут вообще.
    save_tree(tst.m_moving_tabs.m_tabs);

    ds << tst.m_tab_height;
    ds << tst.m_shift_per_depth;

    // Индекс активного таба
    auto find_widget_index = [&tst](const ITabWidget* widget) -> int {
        auto it = std::find_if(tst.m_visible_tabs.begin(), tst.m_visible_tabs.end(),
                               [widget](const mdr::TreeStyleTabs::WidgetDepth& wd) {
                                   return wd.m_widget == widget;
                               });
        if(it == tst.m_visible_tabs.end())
            return -1;
        else
            return std::distance(tst.m_visible_tabs.begin(), it);
    };
    auto* widget = static_cast<ITabWidget*>(tst.m_button_group.checkedButton());
    int checked_widget = find_widget_index(widget);
    ds << checked_widget;

    return ds;
}

QDataStream& mdr::
operator>>(QDataStream& ds, TreeStyleTabs& tst) {
    quint32 magic_num_for_serialization;
    ds >> magic_num_for_serialization;

    if(magic_num_for_serialization != g_magic_num_for_serialization)
        throw std::runtime_error("magic_num_for_serialization != g_magic_num_for_serialization");

    quint8 version;
    ds >> version;

    if(version != 1)
        throw std::runtime_error("version != 1");

    tst.clear();

    std::function<void(Node* tab)> load_tree;
    load_tree = [&ds, &tst, &load_tree](Node* tab) {
        int size;
        ds >> size;

        bool has_widget;
        ds >> has_widget;

        if(has_widget)
            ds >> *tab->getWidget();

        for(int i = 0; i < size; ++i) {
            Node* new_tab = tst.create();
            new_tab->setParent(tab);
            load_tree(new_tab);
        }
    };

    // Загружаем обычные табы
    load_tree(tst.m_tabs);
    // Загружаем перемещаемыет табы
    load_tree(tst.m_tabs);

    ds >> tst.m_tab_height;
    ds >> tst.m_shift_per_depth;

    tst.updateTabWidgets();

    int checked_widget;
    ds >> checked_widget;

    if(checked_widget != -1)
        tst.m_visible_tabs[checked_widget].m_widget->click();

    return ds;
}
