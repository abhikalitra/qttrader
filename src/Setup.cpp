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

#include "Setup.h"
#include "Global.h"
#include "qttrader_defines.h"
#include "PluginFactory.h"

#include <QtDebug>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QPluginLoader>

Setup::Setup ()
{
}

void Setup::setup (QString session)
{
  // set the global variables
  g_session = session;
  if (session.isEmpty())
    g_session = "session0";
  
  // setup the disk environment and init databases
  // order is critical here
  setupDirectories(); // initialize directory structure

  // setup the system scripts
  setupDefaults();
}

void Setup::setupDirectories ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath();

  // create the home directory where all local data is stored
  home.append("/QtTrader");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "Unable to create" << home <<  "directory.";
  }
  
  QString s = home + "/db";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s))
      qDebug() << "Unable to create" << s <<  "directory.";
  }

  // setup the config defaults
  g_settings = "QtTrader/QtTraderrc";
  
  QStringList tl;
  tl << "QtTrader" << "(" + g_session + ")" << "-";
  g_title = tl.join(" ");
}

void Setup::setupDefaults ()
{
  QSettings settings(g_settings);

  // plugin directory
#ifdef DEBUG
    QString s = QDir::currentPath();
#else
    QString s = INSTALL_PLUGIN_DIR;
#endif
  settings.setValue("plugin_directory", s);

  settings.sync();
}

void Setup::scanPlugins ()
{
  QSettings settings(g_settings);
  
  QHash<QString, QStringList> h;

  QDir dir = QDir(settings.value("plugin_directory").toString());
     
  foreach (QString fileName, dir.entryList(QDir::Files))
  {
    QString name = fileName.right(fileName.length() - 3);
    name.truncate(name.lastIndexOf(".", -1));
    
    QPluginLoader loader(dir.absoluteFilePath(fileName));
    QObject *plugin = loader.instance();
    if (! plugin)
    {
      qDebug() << "Setup::scanPlugins:" << loader.errorString();
      continue;
    }
    
    Plugin *plug = qobject_cast<Plugin *>(plugin);
    if (! plug)
    {
      qDebug() << "Setup::scanPlugins:load: error casting Plugin";
      continue;
    }
    
    PluginData pd;
    pd.command = QString("type");
    
    if (! plug->command(&pd))
      continue;
    
    QStringList l = h.value(pd.type);
    l << name;
    h.insert(pd.type, l);
  }     

  settings.beginGroup("plugins");
  
  QHashIterator<QString, QStringList> it(h);
  while (it.hasNext())
  {
    it.next();
    QStringList l = it.value();
    settings.setValue(it.key(), l);
  }
}
