//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <mdrTreeStyleTabs/ITabWidget.hpp>

namespace mdr {

    namespace details {

        class TabWidget
            : public ITabWidget {

            QIcon m_close_button;

            bool m_is_collapsed = false;
            bool m_has_expand_collapse_button = false;
            bool m_has_close_button = true;

            void updateExpandCollapseMark();
        public:
            explicit TabWidget(QWidget* parent = nullptr);

            bool isExpanded() const override;
            bool isCollapsed() const override;

            void setExpanded() override;
            void setCollapsed() override;

            bool hasExpandCollapseButton() const override;
            void toggleExpandCollapseButton(bool enable) override;

            bool hasCloseButton() const override;
            void toggleCloseButton(bool enable);

            bool isExpandCollapseButton(const QPoint& pos) const override;
            bool isCloseButton(const QPoint& pos) const override;

            void save(QDataStream& ds) const override;
            void load(QDataStream& ds) override;

            ~TabWidget() override;
        protected:
            void paintEvent(QPaintEvent* event) override;
        };

    } /* namespace details */

} /* namespace mdr */
