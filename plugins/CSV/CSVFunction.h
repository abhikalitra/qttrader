/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef CSV_FUNCTION_HPP
#define CSV_FUNCTION_HPP

#include <QHash>
#include <QStringList>
#include <QObject>

#include "Bars.h"

class CSVFunction : public QObject
{
  Q_OBJECT
  
  signals:
    void signalMessage (QString);
  
  public:
    CSVFunction (QObject *);
    ~CSVFunction ();
    int import (QStringList files, QString format, QString dateFormat, QString delimiter,
                QString type, QString exchange, bool useFilename);
    int verifyDate (CBar *bar, QString &data, QString &format);
    int verifyValue (CBar *bar, QString &data, int field);
    int verifySymbol (Bars &symbol, CBar *bar);
    
  private:
    QHash<QString, Bars *> _symbols;
};

#endif
