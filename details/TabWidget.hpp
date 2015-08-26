#pragma once

#include "ITabWidget.hpp"

class TabWidget
    : public ITabWidget {

    QIcon m_icon;
    QString m_text;

    static QIcon m_close_button;

    bool m_is_collapsed = false;
    bool m_has_expand_collapse_button = false;
    bool m_has_close_button = true;
public:
    explicit TabWidget(QWidget* parent = nullptr);

    bool isExpanded() const;
    bool isCollapsed() const;

    bool hasExpandCollapseButton() const;
    void toggleExpandCollapseButton(bool on);

    bool hasCloseButton() const;
    void toggleCloseButton(bool on);

    bool isExpandCollapseButton(const QPoint& pos) const;
    bool isCloseButton(const QPoint& pos) const;

    void setIcon(const QIcon& icon);
    const QIcon& getIcon() const;

    void setText(const QString& text);
    const QString& getText() const;
protected:
    void paintEvent(QPaintEvent* event) override;
};
