//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "TabWidget.hpp"

#include "debug.hpp"

#include <QtGui/QPainter>
#include <QtGui/QStylePainter>
#include <QtWidgets/QStyleOptionButton>

static const int g_expand_collapse_width = 6;
static const int g_expand_collapse_height = 6;
static const int g_expand_collapse_left = 5;

static const int g_icon_width = 16;
static const int g_icon_height = 16;
static const int g_icon_left = (g_expand_collapse_left +
                                g_expand_collapse_width + 3);

static const int g_text_height = 12;
static const int g_text_left = (g_icon_left +
                                g_icon_width + 3);

static const int g_close_button_width = 10;
static const int g_close_button_height = 10;
static const int g_close_button_right = 5;

mdr::details::TabWidget::
TabWidget(QWidget* parent)
    : ITabWidget(parent),
      m_close_button(":/icons/close.png") {}

bool mdr::details::TabWidget::
isExpanded() const {
    return m_has_expand_collapse_button && !m_is_collapsed;
}

bool mdr::details::TabWidget::
isCollapsed() const {
    return m_has_expand_collapse_button && m_is_collapsed;
}

void mdr::details::TabWidget::
setExpanded() {
    m_is_collapsed = false;
    updateExpandCollapseMark();
}

void mdr::details::TabWidget::
setCollapsed() {
    m_is_collapsed = true;
    updateExpandCollapseMark();
}

bool mdr::details::TabWidget::
hasExpandCollapseButton() const {
    return m_has_expand_collapse_button;
}

void mdr::details::TabWidget::
toggleExpandCollapseButton(bool enable) {
    m_has_expand_collapse_button = enable;
    updateExpandCollapseMark();
}

bool mdr::details::TabWidget::
hasCloseButton() const {
    return m_has_close_button;
}

void mdr::details::TabWidget::
toggleCloseButton(bool enable) {
    m_has_close_button = enable;
}

bool mdr::details::TabWidget::
isExpandCollapseButton(const QPoint& pos) const {
    if(!hasExpandCollapseButton())
        return false;

    const int eps = 4;

    return QRect(g_expand_collapse_left - eps,
                 0,
                 g_expand_collapse_width + eps * 2,
                 height()).contains(pos);
}

bool mdr::details::TabWidget::
isCloseButton(const QPoint& pos) const {
    if(!hasCloseButton())
        return false;

    const int eps = 4;

    int x = width() - (g_close_button_right +
                       g_close_button_width);

    return QRect(x - eps,
                 0,
                 g_close_button_width + eps * 2,
                 height()).contains(pos);
}

void mdr::details::TabWidget::
updateExpandCollapseMark() {
    update(QRect(g_expand_collapse_left,
                 (height() - g_expand_collapse_height) / 2,
                 g_expand_collapse_width,
                 g_expand_collapse_height));
}

void mdr::details::TabWidget::
paintEvent(QPaintEvent*) {

    auto draw_expand_collapse_button = [this](QPainter& painter) {
        painter.save();

        painter.setPen(Qt::black);
        painter.setBrush(Qt::SolidPattern);
        painter.setRenderHint(QPainter::Antialiasing);

        QPolygon polygon;
        if(isCollapsed())
            polygon << QPoint(0, -3)
                    << QPoint(3, 0)
                    << QPoint(0, 3);
        else if(isExpanded())
            polygon << QPoint(0, -1)
                    << QPoint(3, 2)
                    << QPoint(6, -1);

        polygon.translate(g_expand_collapse_left, height() / 2);

        painter.drawPolygon(polygon);

        painter.restore(); // TODO: raii?
    };

    auto draw_icon = [this](QPainter& painter) {
        icon().paint(&painter, QRect(g_icon_left,
                                     (height() - g_icon_height) / 2,
                                     g_icon_width,
                                     g_icon_height),
                     Qt::AlignCenter,
                     isEnabled() ? QIcon::Normal : QIcon::Disabled);
    };

    auto draw_text = [this](QPainter& painter) {
        const int between_text_and_close_button = 3;
        const int wid = (width() - g_text_left -
                         (g_close_button_width +
                          g_close_button_right +
                          between_text_and_close_button));

        painter.save();

        QFont font = painter.font();
        font.setPixelSize(g_text_height);
        painter.setFont(font);

        painter.drawText(QRect(g_text_left,
                               (height() - g_text_height) / 2,
                               wid,
                               g_text_height),
                         Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                         text());

        painter.restore(); // TODO: raii?
    };

    auto draw_close_button = [this](QPainter& painter) {
        int x = width() - (g_close_button_right +
                           g_close_button_width);
        m_close_button.paint(&painter, QRect(x,
                                             (height() - g_close_button_height) / 2,
                                             g_close_button_width,
                                             g_close_button_height));
    };

    auto draw_button_back = [this] {
        QStylePainter p(this);
        QStyleOptionButton option;
        option.initFrom(this);

        option.features = QStyleOptionButton::None;

        if(isChecked())
            option.state = QStyle::State_On;

        p.drawControl(QStyle::CE_PushButton, option);
    };

    draw_button_back();

    QPainter painter(this);

    if(hasExpandCollapseButton())
        draw_expand_collapse_button(painter);

    draw_icon(painter);
    draw_text(painter);

    if(hasCloseButton())
        draw_close_button(painter);
}

void mdr::details::TabWidget::
save(QDataStream& ds) const {
    ds << text();
    ds << icon();
    ds << m_is_collapsed;
    ds << m_has_expand_collapse_button;
    ds << m_has_close_button;
}

void mdr::details::TabWidget::
load(QDataStream& ds) {
    QString t;
    ds >> t;
    setText(t);

    QIcon i;
    ds >> i;
    setIcon(i);

    ds >> m_is_collapsed;
    ds >> m_has_expand_collapse_button;
    ds >> m_has_close_button;
}

mdr::details::TabWidget::
~TabWidget() {
    IFDEBUG printf("~TabWidget()\n");
}
