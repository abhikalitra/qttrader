/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "PluginFactory.h"

#include <QDir>
#include <QtDebug>
#include <QPluginLoader>
#include "qttrader_defines.h"

PluginFactory::PluginFactory ()
{
}

Plugin *
PluginFactory::load (QString d)
{

#ifdef DEBUG
    QString s = QDir::currentPath();
    s.append("/lib").append(d);
#else
    QString s = INSTALL_PLUGIN_DIR;
    s.append("/lib").append(d);
#endif

#if defined(Q_OS_MAC)  
  s.append(".dylib");
#endif
#if defined(Q_OS_UNIX)  
  s.append(".so");
#endif
  
  QPluginLoader pluginLoader(s);
  QObject *tp = pluginLoader.instance();
  if (! tp)
  {
    qDebug() << "PluginFactory::load:" << pluginLoader.errorString();
    return 0;
  }

  Plugin *plugin = qobject_cast<Plugin *>(tp);
  if (! plugin)
    qDebug() << "PluginFactory::load: error casting Plugin";

  return plugin;
}
