//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <QtGui/QMouseEvent>
#include <QtCore/QObject>

#include <functional>

namespace mdr {

    namespace details {

        class DragAndDropFilter : public QObject {
            using PressHandler = std::function<void(const QPoint& pos)>;
            PressHandler m_press;
            using MoveHandler = std::function<void(const QPoint& start, const QPoint& pos)>;
            MoveHandler m_move;
            using ReleaseHandler = std::function<void(const QPoint& pos)>;
            ReleaseHandler m_release;
            using WheelHandler = std::function<void()>;
            WheelHandler m_wheel;

            struct {
                QPoint m_start_press_pos;
            } m_pressed_left_button;
        public:
            DragAndDropFilter(QObject* parent,
                              const PressHandler& press,
                              const MoveHandler& move,
                              const ReleaseHandler& release,
                              const WheelHandler& wheel);

            bool eventFilter(QObject *watched, QEvent *e) override;
        };

    } /* namespace details */

} /* namespace mdr */
