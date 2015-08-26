
#include <mdrTreeStyleTabs/ITabWidget.hpp>
#include <mdrTreeStyleTabs/Node.hpp>
#include <mdrTreeStyleTabs/TabWidgetFactory.hpp>
#include <mdrTreeStyleTabs/TreeStyleTabs.hpp>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QtCore/QMimeData>
#include <QtCore/QList>
#include <QtGui/QFrame>
#include <QtCore/QtAlgorithms>
#include <QtGui/QPainter>
#include <QtWidgets/QScrollArea>
#include <QtCore/QObject>

#include <iostream>
#include <stdexcept>

#define SAVE

int main(int argc, char *argv[]) {
    using namespace mdr;

    QApplication app(argc, argv);

    QMainWindow main_window;

    auto widget_factory = std::make_shared<TabWidgetFactory>();

    TreeStyleTabs tree_style_tabs(&main_window, widget_factory);

    QObject::connect(&tree_style_tabs, &TreeStyleTabs::tabCloseRequested,
                     &tree_style_tabs, &TreeStyleTabs::close);

    main_window.setCentralWidget(&tree_style_tabs);
#ifdef SAVE
    Node* root = tree_style_tabs.getRoot();

    Node* child1 = tree_style_tabs.create();
    child1->getWidget()->setText("Hello1");
    child1->getWidget()->setIcon(QIcon(":/icons/close.png"));

    Node* child2 = tree_style_tabs.create();
    child2->getWidget()->setText("Hello2");

    Node* child3 = tree_style_tabs.create();
    child3->getWidget()->setText("Hello3");
    child3->setParent(child2);

    // child1->getWidget()->setEnabled(false);

    // child2->getWidget()->setCollapsed();
    // tree_style_tabs.close(child3);

    for(int i = 0; i < 10; ++i) {
        Node* child = tree_style_tabs.create();
        child->getWidget()->setText("Tab" + QString::number(i));
    }

    tree_style_tabs.updateTabWidgets();

    main_window.show();
    tree_style_tabs.show();

    // tree_style_tabs.setTabHeight(40);
    // tree_style_tabs.setShiftPerDepth(20);
    // tree_style_tabs.setAlignment(Qt::AlignLeft);

    QFile file("file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    out << tree_style_tabs;
#else /* SAVE */
    QFile file("file.dat");printf("xxxx\n");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    in >> tree_style_tabs;

    main_window.show();
    tree_style_tabs.show();
#endif /* SAVE */

    return app.exec();
}
