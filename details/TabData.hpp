#pragma once

// TODO: delete

class TabWidget;

class TabData final {
    //! TabData хранит указатель на виджет, но не управляет
    //! временем жизни виджета
    TabWidget* m_widget = nullptr;

    //! имеет смысл только если у узла есть дети
    bool m_is_collapsed = false;
public:
    TabData() = default;
    TabData(const TabData&) = delete;
    TabData(TabData&&);

    explicit TabData(TabWidget* widget);

    TabData& operator=(const TabData&) = delete;
    TabData& operator=(TabData&&);

    void setWidget(TabWidget* widget);
    TabWidget* getWidget();

    //! меняет состояние m_widget, если m_widget есть
    void setCollapsed(bool is_collapsed);
    bool isCollapsed();
};
