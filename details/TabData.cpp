
#include "TabData.hpp"

TabData::
TabData(TabData&& td)
    : m_widget(td.m_widget),
      m_is_collapsed(td.m_is_collapsed) {
    td.m_widget = nullptr;
}

TabData::
TabData(TabWidget* widget)
    : m_widget(widget) {}

TabData& TabData::
operator=(TabData&& td) {
    if(this == &td)
        return *this;

    m_widget = td.m_widget;
    m_is_collapsed = td.m_is_collapsed;

    td.m_widget = nullptr;

    return *this;
}

void TabData::
setWidget(TabWidget* widget) {
    m_widget = widget;
}

TabWidget* TabData::
getWidget() {
    return m_widget;
}

void TabData::
setCollapsed(bool is_collapsed) {
    m_is_collapsed = is_collapsed;
}

bool TabData::
isCollapsed() {
    return m_is_collapsed;
}
