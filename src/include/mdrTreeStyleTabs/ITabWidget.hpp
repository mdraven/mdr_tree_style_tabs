//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <QtWidgets/QAbstractButton>

class QDataStream;

namespace mdr {

    class Node;

    class ITabWidget : public QAbstractButton {
    protected:
        ITabWidget() {}
        ITabWidget(QWidget* parent)
            : QAbstractButton(parent) {}
    public:
        // forbid copying
        ITabWidget(const ITabWidget&) = delete;
        ITabWidget& operator=(const ITabWidget&) = delete;

        virtual bool isExpanded() const = 0;
        virtual bool isCollapsed() const = 0;

        virtual void setExpanded() = 0;
        virtual void setCollapsed() = 0;

        virtual bool hasExpandCollapseButton() const = 0;
        virtual void toggleExpandCollapseButton(bool enable) = 0;

        virtual bool hasCloseButton() const = 0;

        virtual bool isExpandCollapseButton(const QPoint& pos) const = 0;
        virtual bool isCloseButton(const QPoint& pos) const = 0;

        virtual void save(QDataStream& ds) const = 0;
        virtual void load(QDataStream& ds) = 0;

        virtual ~ITabWidget() {}
    };

    QDataStream& operator<<(QDataStream& ds, const ITabWidget& widget);
    QDataStream& operator>>(QDataStream& ds, ITabWidget& widget);

} /* namespace mdr */
