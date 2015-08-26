
#include "TreeStyleTabs.hpp"

TreeStyleTabs::
TreeStyleTabs(QWidget* parent)
    : QWidget(parent),
      m_scroll_area(this),
      m_frame(&m_scroll_area),
      m_filter(this,
               std::bind(&TreeStyleTabs::clickOnTab, this, std::placeholders::_1),
               std::bind(&TreeStyleTabs::dragTabs, this, std::placeholders::_1, std::placeholders::_2),
               std::bind(&TreeStyleTabs::dropTabs, this, std::placeholders::_1),
               std::bind(&TreeStyleTabs::wheel, this)),
      m_button_group(this) {
    m_scroll_area.setWidget(&m_frame);
    m_scroll_area.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(&m_button_group, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
            this, [this](QAbstractButton* btn) {
                auto* tab = static_cast<ITabWidget*>(btn);
                assert(tab != nullptr);
                Q_EMIT newActive(TreeStyleTabProxy(tab));
            });
}

    Node<TabData>* getRoot();

    //! Создаёт Node, но без виджета
    static Node<TabData>* create();
    //! Удаляет Node, но не трогает виджет
    static free(Node<TabData>* tab);

    //! Эту функцию нужно вызывать после изменений в дереве
    //! табов
    void updateTabWidgets();

    //! Удаляет Node, но не участвует в удалении виджетов
    ~TreeStyleTabs() override;
public Q_SLOTS:
    void close(Node<TabData>* tab);

    void show();
    void hide();
