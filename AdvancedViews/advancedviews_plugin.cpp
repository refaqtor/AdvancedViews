#include "advancedviews_plugin.h"
#include "tableview.h"

#include <qqml.h>

void AdvancedViewsPlugin::registerTypes(const char *uri)
{
    // @uri AdvancedViews
    qmlRegisterType<TableViewPrivate>(uri, 1, 0, "TableViewPrivate");
}

