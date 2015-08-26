#pragma once

#include <QtWidgets/QAbstractButton>

class Node;

class ITabWidget : public QAbstractButton {
protected:
    ITabWidget() {}
public:
    // forbid copying
    ITabWidget(const ITabWidget&) = delete;
    ITabWidget& operator=(const ITabWidget&) = delete;

    virtual bool isExpanded() const = 0;
    virtual bool isCollapsed() const = 0;

    virtual bool hasExpandCollapseButton() const = 0;
    virtual void toggleExpandCollapseButton(bool on) = 0;

    virtual bool hasCloseButton() const = 0;
    virtual void toggleCloseButton(bool on) = 0;

    virtual bool isExpandCollapseButton(const QPoint& pos) const = 0;
    virtual bool isCloseButton(const QPoint& pos) const = 0;

    virtual Node* getNode() = 0;

    virtual ~ITabWidget() {}
};
