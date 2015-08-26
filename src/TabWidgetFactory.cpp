//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <mdrTreeStyleTabs/TabWidgetFactory.hpp>

#include "details/TabWidget.hpp"
#include "details/debug.hpp"

mdr::ITabWidget* mdr::TabWidgetFactory::
create() {
    return new details::TabWidget();
}

void mdr::TabWidgetFactory::
free(ITabWidget* widget) {
    IFDEBUG printf("free(ITabWidget* widget)\n");

    widget->deleteLater();
}
