//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <mdrTreeStyleTabs/ITabWidget.hpp>

#include <QtCore/QDataStream>

QDataStream& mdr::
operator<<(QDataStream& ds, const ITabWidget& widget) {
    widget.save(ds);
    return ds;
}

QDataStream& mdr::
operator>>(QDataStream& ds, ITabWidget& widget) {
    widget.load(ds);
    return ds;
}
