
#include "TabWidget.hpp"

#include <QtWidgets/QStyleOptionButton>
#include <QtGui/QPainter>

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

TabWidget::
TabWidget(QWidget* parent)
    : QAbstractButton(parent) {}

bool TabWidget::
isExpanded() const {
    return m_has_expand_collapse_button && !m_is_collapsed;
}

bool TabWidget::
isCollapsed() const {
    return m_has_expand_collapse_button && m_is_collapsed;
}

bool TabWidget::
hasExpandCollapseButton() const {
    return m_has_expand_collapse_button;
}

void TabWidget::
toggleExpandCollapseButton(bool on) {
    m_has_expand_collapse_button = on;
}

bool TabWidget::
hasCloseButton() const {
    return m_has_close_button;
}

void TabWidget::
toggleCloseButton(bool on) {
    m_has_close_button = on;
}

bool TabWidget::
isExpandCollapseButton(const QPoint& pos) const {
    if(!hasExpandCollapseButton())
        return false;

    const int eps = 4;

    return QRect(g_expand_collapse_left - eps,
                 0,
                 g_expand_collapse_width + eps * 2,
                 height()).contains(pos);
}

bool TabWidget::
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

void TabWidget::
setIcon(const QIcon& icon) {
    m_icon = icon;
}

const QIcon& TabWidget::
getIcon() const {
    return m_icon;
}

void TabWidget::
setText(const QString& text) {
    m_text = text;
}

const QString& TabWidget::
getText() const {
    return m_text;
}

void TabWidget::
paintEvent(QPaintEvent* event) {
    auto draw_expand_collapse_mark = [this](QPainter& painter) {
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
    };

    auto draw_icon = [this, &painter](QPainter& painter) {
        m_icon.paint(&painter, QRect(g_icon_left,
                                     (height() - g_icon_height) / 2,
                                     g_icon_width,
                                     g_icon_height));
    };

    auto draw_text = [this, &painter](QPainter& painter) {
        const int between_text_and_close_button = 3;
        const int wid = (width() - g_text_left -
                         (g_close_button_width +
                          g_close_button_right +
                          between_text_and_close_button));

        QFont font = painter.font();
        font.setPixelSize(g_text_height);
        painter.setFont(font);

        painter.drawText(QRect(g_text_left,
                               (height() - g_text_height) / 2,
                               wid,
                               g_text_height),
                         Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                         m_text);
    };

    auto draw_close_button = [this, &painter](QPainter& painter) {
        int x = width() - (g_close_button_right +
                           g_close_button_width);
        m_close_button.paint(&painter, QRect(x,
                                             (height() - g_close_button_height) / 2,
                                             g_close_button_width,
                                             g_close_button_height));
    };

    QAbstractButton::paintEvent(event);
    QPainter painter(this);

    if(hasExpandCollapseButton())
        draw_expand_collapse_button(painter);

    draw_icon(painter);
    draw_text(painter);

    if(hasCloseButton())
        draw_close_button(painter);
}
