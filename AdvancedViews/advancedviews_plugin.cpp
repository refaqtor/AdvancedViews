/*
    This file is part of AdvancedViews.

    AdvancedViews is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AdvancedViews is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with AdvancedViews.  If not, see <http://www.gnu.org/licenses/>.
*/

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

