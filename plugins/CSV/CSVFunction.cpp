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

#include "CSVFunction.h"
#include "Bar.h"
#include "CSVFormat.h"
#include "PluginFactory.h"
#include "BarType.h"
#include "Delimiter.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

CSVFunction::CSVFunction (QObject *p) : QObject (p)
{
}

CSVFunction::~CSVFunction ()
{
  qDeleteAll(_symbols);
}

int
CSVFunction::import (QStringList files, QString tformat, QString dateFormat, QString tdelimiter,
                     QString type, QString exchange, bool useFilename)
{
  qDeleteAll(_symbols);
  _symbols.clear();
  
  QStringList format = tformat.split(",", QString::SkipEmptyParts);
  
  Delimiter delimit;
  QString delimiter = delimit.stringToActual(tdelimiter);

  // away we go
  CSVFormat typeFormat;
  int loop = 0;
  for (; loop < files.size(); loop++)
  {
    QFile f(files.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QStringList mess;
      mess << tr("File error") << files.at(loop) << tr("skipped");
      emit signalMessage(mess.join(" "));
      continue;
    }

    QString fileNameSymbol;
    if (useFilename)
    {
      QFileInfo fi(files.at(loop));
      fileNameSymbol = fi.baseName();
    }

    while (! f.atEnd())
    {
      QString s = f.readLine();
      s = s.trimmed();
      if (s.isEmpty())
        continue;

      QStringList data = s.split(delimiter, QString::SkipEmptyParts);

      if (format.size() != data.size())
      {
        QStringList mess;
        mess << tr("Error, invalid # of fields") << format << data;
        emit signalMessage(mess.join(" "));
        continue;
      }

      Bars tsymbol;
      if (useFilename)
      {
        tsymbol.setTicker(fileNameSymbol);
        tsymbol.setName(fileNameSymbol);
      }

      if (! exchange.isEmpty())
        tsymbol.setExchange(exchange);

      CBar *tbar = new CBar;
      int loop2 = 0;
      int flag = 0;
      for (; loop2 < format.count(); loop2++)
      {
        QString ts = data.at(loop2).trimmed();
        
        switch ((CSVFormat::Key) typeFormat.stringToIndex(format.at(loop2)))
        {
          case CSVFormat::_TYPE:
            tsymbol.setSymbolType(ts);
            break;
          case CSVFormat::_EXCHANGE:
            tsymbol.setExchange(ts);
            break;
          case CSVFormat::_TICKER:
            tsymbol.setTicker(ts);
            break;
          case CSVFormat::_NAME:
            tsymbol.setName(ts);
            break;
          case CSVFormat::_DATE:
            if (! verifyDate(tbar, ts, dateFormat))
              flag++;
            break;
          case CSVFormat::_OPEN:
            if (! verifyValue(tbar, ts, BarType::_OPEN))
              flag++;
            break;
          case CSVFormat::_HIGH:
            if (! verifyValue(tbar, ts, BarType::_HIGH))
              flag++;
            break;
          case CSVFormat::_LOW:
            if (! verifyValue(tbar, ts, BarType::_LOW))
              flag++;
            break;
          case CSVFormat::_CLOSE:
            if (! verifyValue(tbar, ts, BarType::_CLOSE))
              flag++;
            break;
          case CSVFormat::_VOLUME:
            if (! verifyValue(tbar, ts, BarType::_VOLUME))
              flag++;
            break;
          case CSVFormat::_OI:
            if (! verifyValue(tbar, ts, BarType::_OI))
              flag++;
            break;
          default:
          {
            QStringList mess;
            mess << tr("Error, invalid format") << s;
            emit signalMessage(mess.join(" "));
            flag++;
            break;
          }
        }

        if (flag)
          break;
      }

      if (! verifySymbol(tsymbol, tbar))
        flag++;
      
      if (flag)
	delete tbar;
    }

    f.close();
  }

  PluginFactory fac;
  Plugin *plug = fac.load(QString("DBSymbol"));
  if (! plug)
  {
    QStringList mess;
    mess << tr("Error, DBSymbol plugin missing");
    emit signalMessage(mess.join(" "));
    return 0;
  }
  
  PluginData pd;
  pd.command = QString("init");
  if (! plug->command(&pd))
    return 0;
  
  QHashIterator<QString, Bars *> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    Bars *bd = it.value();

    pd.command = QString("transaction");
    if (! plug->command(&pd))
      continue;
    
    pd.command = QString("setBars");
    pd.bars = bd;
    if (! plug->command(&pd))
      continue;
    
    pd.command = QString("commit");
    if (! plug->command(&pd))
      continue;
  }
  
  return 1;
}

int
CSVFunction::verifyDate (CBar *bar, QString &data, QString &format)
{
  QDateTime dt = QDateTime::fromString(data.trimmed(), format);
  if (! dt.isValid())
  {
    QStringList mess;
    mess << tr("Error, invalid date") << data;
    emit signalMessage(mess.join(" "));
    return 0;
  }
  
  bar->setDate(dt);
  
  return 1;
}

int
CSVFunction::verifyValue (CBar *bar, QString &data, int field)
{
  BarType bt;
  bool ok;
  QString s = data.trimmed();
  double t = s.toDouble(&ok);
  if (! ok)
  {
    QStringList mess;
    mess << tr("Error, invalid") << bt.indexToString(field) << data;
    emit signalMessage(mess.join(" "));
    return 0;
  }

  bar->set(bt.indexToString(field), t);
  
  return 1;
}

int
CSVFunction::verifySymbol (Bars &symbol, CBar *bar)
{
  if (symbol.exchange().isEmpty())
  {
    emit signalMessage(tr("Error, invalid exchange"));
    return 0;
  }

  if (symbol.ticker().isEmpty())
  {
    emit signalMessage(tr("Error, invalid symbol"));
    return 0;
  }

  QString key = symbol.symbol();
      
  Bars *sym = _symbols.value(key);
  if (! sym)
  {
    sym = new Bars;
    sym->setSymbol(key);
    sym->setName(symbol.name());
    sym->setSymbolType(symbol.symbolType());
    _symbols.insert(key, sym);
  }

  sym->setBar(sym->bars(), bar);
  
  return 1;
}
