#include "advancedviews_plugin.h"
#include "tableviewprivate.h"

#include <qqml.h>
#include <QFile>

void AdvancedViewsPlugin::registerTypes(const char *uri)
{
    qmlRegisterType(QUrl("qrc:///AdvancedViews/TableView.qml"), uri, 1, 0, "TableView");
    // @uri AdvancedViews
    qmlRegisterType<TableViewPrivate>(uri, 1, 0, "TableViewPrivate");
}

