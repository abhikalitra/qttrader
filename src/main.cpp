/*
 *  Qtstalker stock charter
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

#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QLocale>
#include <QtDebug>

#include "qttrader.h"
#include "qtstalker_defines.h"

int main(int argc, char *argv[])
{
  QString session;
  QString plugin;
  int loop;
  for (loop = 1; loop <= argc; loop++)
  {
    QString s = argv[loop];
    if (s == "-session")
    {
      loop++;
      if (loop > argc)
        break;
      
      session = argv[loop];
      continue;
    }
    
    if (s == "-plugin")
    {
      loop++;
      if (loop > argc)
        break;
      
      plugin = argv[loop];
      continue;
    }
  }

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("QtTrader");
  QCoreApplication::setApplicationName("QtTrader");
  QTranslator tor(0);

  QString i18nDir = QString("%1/QtTrader/i18n").arg(INSTALL_DATA_DIR);
  QString i18nFilename = QString("qttrader_%1").arg(QLocale::system().name());
  tor.load(i18nFilename, i18nDir);
  a.installTranslator( &tor );

  qDebug() << "QtTrader::main: using args" << session << plugin;

  QtTrader app(session, plugin);
  app.show();
  return a.exec();
}
