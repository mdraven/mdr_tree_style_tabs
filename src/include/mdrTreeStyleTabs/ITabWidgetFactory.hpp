//  Copyright Ramil Iljasov 2015. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace mdr {

    class ITabWidget;

    class ITabWidgetFactory {
    protected:
        ITabWidgetFactory() {}
    public:
        // forbid copying
        ITabWidgetFactory(const ITabWidgetFactory&) = delete;
        ITabWidgetFactory& operator=(const ITabWidgetFactory&) = delete;

        virtual ITabWidget* create() = 0;
        virtual void free(ITabWidget*) = 0;

        virtual ~ITabWidgetFactory() {}
    };

} /* namespace mdr */
