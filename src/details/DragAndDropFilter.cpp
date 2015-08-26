
#include <mdrTreeStyleTabs/details/DragAndDropFilter.hpp>

mdr::details::DragAndDropFilter::
DragAndDropFilter(QObject* parent,
                  const PressHandler& press,
                  const MoveHandler& move,
                  const ReleaseHandler& release,
                  const WheelHandler& wheel)
    : QObject(parent),
      m_press(press),
      m_move(move),
      m_release(release),
      m_wheel(wheel) {}

bool mdr::details::DragAndDropFilter::
eventFilter(QObject *watched, QEvent *e) {
    (void)watched;

    if(e->type() == QEvent::MouseButtonPress) {
        auto* ev = static_cast<QMouseEvent*>(e);
        if(ev->buttons() == Qt::LeftButton) {
            m_pressed_left_button.m_start_press_pos = ev->globalPos();
            m_press(m_pressed_left_button.m_start_press_pos);
        }
    }
    else if(e->type() == QEvent::MouseMove) {
        auto* ev = static_cast<QMouseEvent*>(e);
        if(ev->buttons() == Qt::LeftButton)
            m_move(m_pressed_left_button.m_start_press_pos, ev->globalPos());
    }
    else if(e->type() == QEvent::MouseButtonRelease) {
        auto* ev = static_cast<QMouseEvent*>(e);
        // Почему-то событие о кнопке не приходит
        // if(ev->buttons() == Qt::LeftButton)
        m_release(ev->globalPos());
    }
    else if(e->type() == QEvent::Wheel) {
        auto* ev = static_cast<QWheelEvent*>(e);
        Q_UNUSED(ev);
        m_wheel();
    }

    return false;
}
