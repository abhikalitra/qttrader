/*
 *  Qtstalker stock charter
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

#include "CDL.h"
#include "CurveOHLCType.h"
#include "Global.h"
#include "TypeCandle.h"
#include "ta_libc.h"
#include "BarType.h"
#include "CDLDialog.h"

#include <QtGui>


int
CDL::command (PluginData *pd)
{
  int rc = 0;

  QStringList cl;
  cl << "type" << "dialog" << "runIndicator" << "settings";
  
  switch (cl.indexOf(pd->command))
  {
    case 0: // type
      pd->type = QString("indicator");
      rc = 1;
      break;
    case 1: // dialog
      rc = dialog(pd);
      break;
    case 2: // runIndicator
      rc = run(pd);
      break;
    case 3: // settings
      rc = settings(pd);
      break;
    default:
      break;
  }
  
  return rc;
}

int
CDL::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "CDL::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "CDL::dialog: invalid settings";
    return 0;
  }
  
  CDLDialog *dialog = new CDLDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
CDL::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *var = pd->settings->get(QString("color"));
  if (! var)
    return 0;
  QColor color(var->toString());
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;
  
  QVariant *pen = pd->settings->get(QString("pen"));
  if (! pen)
    return 0;

  QVariant *m1 = pd->settings->get(QString("method1"));
  if (! m1)
    return 0;

  var = pd->settings->get(QString("bullColor1"));
  if (! var)
    return 0;
  QColor m1ColorUp(var->toString());

  var = pd->settings->get(QString("bearColor1"));
  if (! var)
    return 0;
  QColor m1ColorDown(var->toString());

  QVariant *m1Show = pd->settings->get(QString("show1"));
  if (! m1Show)
    return 0;

  QVariant *m2 = pd->settings->get(QString("method2"));
  if (! m2)
    return 0;

  var = pd->settings->get(QString("bullColor2"));
  if (! var)
    return 0;
  QColor m2ColorUp(var->toString());

  var = pd->settings->get(QString("bearColor2"));
  if (! var)
    return 0;
  QColor m2ColorDown(var->toString());

  QVariant *m2Show = pd->settings->get(QString("show2"));
  if (! m2Show)
    return 0;

  // create candles
  Curve *ohlc = new Curve(QString("CurveOHLC"));
  ohlc->setLabel(label->toString());
  ohlc->setStyle(CurveOHLCType::_CANDLE);
  
  BarType bt;
  ohlc->fill(bt.indexToString(BarType::_OPEN),
             bt.indexToString(BarType::_HIGH),
             bt.indexToString(BarType::_LOW),
             bt.indexToString(BarType::_CLOSE),
             color);
  pd->curves << ohlc;

  // create method1
  TypeCandle ct;
  QString key("CDL::run:m1");
  if (m1Show->toBool())
    getCDL(ct.stringToIndex(m1->toString()), pen->toDouble(), key);
  
  // create method2
  QString key2("CDL::run:m2");
  if (m2Show->toBool())
    getCDL(ct.stringToIndex(m2->toString()), pen->toDouble(), key2);

  QList<int> keys = g_symbol->keys();
  
  // color the candles
  for (int pos = 0; pos < keys.size(); pos++)
  {
    CBar *bar = g_symbol->bar(keys.at(pos));

    Bar *ohlcbar = ohlc->bar(keys.at(pos));
    if (! ohlcbar)
      continue;
    
    if (m1Show->toBool())
    {
      double v;
      if (bar->get(key, v))
      {
        if (v < 0)
          ohlcbar->setColor(m1ColorDown);
        else
        {
          if (v > 0)
            ohlcbar->setColor(m1ColorUp);
        }
      }
    }
    
    if (m2Show->toBool())
    {
      double v;
      if (bar->get(key2, v))
      {
        if (v < 0)
          ohlcbar->setColor(m2ColorDown);
        else
        {
          if (v > 0)
            ohlcbar->setColor(m2ColorUp);
        }
      }
    }
  }
  
  return 1;
}

int
CDL::getCDL (int method, double pen, QString okey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "CDL::getCDL: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer iout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  BarType bt;
  QString openKey = bt.indexToString(BarType::_OPEN);
  QString highKey = bt.indexToString(BarType::_HIGH);
  QString lowKey = bt.indexToString(BarType::_LOW);
  QString closeKey = bt.indexToString(BarType::_CLOSE);
  
  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double o;
    if (! bar->get(openKey, o))
      continue;
    
    double h;
    if (! bar->get(highKey, h))
      continue;
    
    double l;
    if (! bar->get(lowKey, l))
      continue;
    
    double c;
    if (! bar->get(closeKey, c))
      continue;
    
    open[dpos] = (TA_Real) o;
    high[dpos] = (TA_Real) h;
    low[dpos] = (TA_Real) l;
    close[dpos] = (TA_Real) c;
    dpos++;
  }

  switch ((TypeCandle::Key) method)
  {
    case TypeCandle::_2CROWS:
      rc = TA_CDL2CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3BLACKCROWS:
      rc = TA_CDL3BLACKCROWS(0,  dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3INSIDE:
      rc = TA_CDL3INSIDE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3LINESTRIKE:
      rc = TA_CDL3LINESTRIKE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3OUTSIDE:
      rc = TA_CDL3OUTSIDE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3STARSINSOUTH:
      rc = TA_CDL3STARSINSOUTH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_3WHITESOLDIERS:
      rc = TA_CDL3WHITESOLDIERS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_ABANDONEDBABY:
      rc = TA_CDLABANDONEDBABY(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_ADVANCEBLOCK:
      rc = TA_CDLADVANCEBLOCK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_BELTHOLD:
      rc = TA_CDLBELTHOLD(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_BREAKAWAY:
      rc = TA_CDLBREAKAWAY(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_CLOSINGMARUBOZU:
      rc = TA_CDLCLOSINGMARUBOZU(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_CONCEALBABYSWALL:
      rc = TA_CDLCONCEALBABYSWALL(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_COUNTERATTACK:
      rc = TA_CDLCOUNTERATTACK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_DARKCLOUDCOVER:
      rc = TA_CDLDARKCLOUDCOVER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_DOJI:
      rc = TA_CDLDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_DOJISTAR:
      rc = TA_CDLDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_DRAGONFLYDOJI:
      rc = TA_CDLDRAGONFLYDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_ENGULFING:
      rc = TA_CDLENGULFING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_EVENINGDOJISTAR:
      rc = TA_CDLEVENINGDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_EVENINGSTAR:
      rc = TA_CDLEVENINGSTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_GAPSIDESIDEWHITE:
      rc = TA_CDLGAPSIDESIDEWHITE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_GRAVESTONEDOJI:
      rc = TA_CDLGRAVESTONEDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HAMMER:
      rc = TA_CDLHAMMER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HANGINGMAN:
      rc = TA_CDLHANGINGMAN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HARAMI:
      rc = TA_CDLHARAMI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HARAMICROSS:
      rc = TA_CDLHARAMICROSS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HIGHWAVE:
      rc = TA_CDLHIGHWAVE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HIKKAKE:
      rc = TA_CDLHIKKAKE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HIKKAKEMOD:
      rc = TA_CDLHIKKAKEMOD(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_HOMINGPIGEON:
      rc = TA_CDLHOMINGPIGEON(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_IDENTICAL3CROWS:
      rc = TA_CDLIDENTICAL3CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_INNECK:
      rc = TA_CDLINNECK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_INVERTEDHAMMER:
      rc = TA_CDLINVERTEDHAMMER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_KICKING:
      rc = TA_CDLKICKING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_KICKINGBYLENGTH:
      rc = TA_CDLKICKINGBYLENGTH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_LADDERBOTTOM:
      rc = TA_CDLLADDERBOTTOM(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_LONGLEGGEDDOJI:
      rc = TA_CDLLONGLEGGEDDOJI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_LONGLINE:
      rc = TA_CDLLONGLINE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_MARUBOZU:
      rc = TA_CDLMARUBOZU(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_MATCHINGLOW:
      rc = TA_CDLMATCHINGLOW(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_MORNINGDOJISTAR:
      rc = TA_CDLMORNINGDOJISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], pen, &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_ONNECK:
      rc = TA_CDLONNECK(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_PIERCING:
      rc = TA_CDLPIERCING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_RICKSHAWMAN:
      rc = TA_CDLRICKSHAWMAN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_RISEFALL3METHODS:
      rc = TA_CDLRISEFALL3METHODS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_SEPARATINGLINES:
      rc = TA_CDLSEPARATINGLINES(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_SHOOTINGSTAR:
      rc = TA_CDLSHOOTINGSTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_SHORTLINE:
      rc = TA_CDLSHORTLINE(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_SPINNINGTOP:
      rc = TA_CDLSPINNINGTOP(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_STALLEDPATTERN:
      rc = TA_CDLSTALLEDPATTERN(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_STICKSANDWICH:
      rc = TA_CDLSTICKSANDWICH(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_TAKURI:
      rc = TA_CDLTAKURI(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_TASUKIGAP:
      rc = TA_CDLTASUKIGAP(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_THRUSTING:
      rc = TA_CDLTHRUSTING(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_TRISTAR:
      rc = TA_CDLTRISTAR(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_UNIQUE3RIVER:
      rc = TA_CDLUNIQUE3RIVER(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_UPSIDEGAP2CROWS:
      rc = TA_CDLUPSIDEGAP2CROWS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    case TypeCandle::_XSIDEGAP3METHODS:
      rc = TA_CDLXSIDEGAP3METHODS(0, dpos - 1, &open[0], &high[0], &low[0], &close[0], &outBeg, &outNb, &iout[0]);
      break;
    default:
      break;
  }
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "CDL::getCDL: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(okey, iout[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

int
CDL::settings (PluginData *pd)
{
  Entity *command = new Entity;
  TypeCandle tc;
  
  command->set(QString("plugin"), new QVariant(QString("CDL")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("color"), new QVariant(QString("blue")));
  command->set(QString("label"), new QVariant(QString("CDL")));
  command->set(QString("pen"), new QVariant(0.75));

  command->set(QString("method1"), new QVariant(tc.indexToString(TypeCandle::_HARAMI)));
  command->set(QString("bullColor1"), new QVariant(QString("green")));
  command->set(QString("bearColor1"), new QVariant(QString("red")));
  command->set(QString("show1"), new QVariant(TRUE));

  command->set(QString("method2"), new QVariant(tc.indexToString(TypeCandle::_HARAMI)));
  command->set(QString("bullColor2"), new QVariant(QString("green")));
  command->set(QString("bearColor2"), new QVariant(QString("red")));
  command->set(QString("show2"), new QVariant(TRUE));

  pd->settings = command;
  
  return 1;
}

int
CDL::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(cdl, CDL);
