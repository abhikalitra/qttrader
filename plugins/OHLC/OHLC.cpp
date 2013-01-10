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

#include <QtGui>

#include "OHLC.h"
#include "ta_libc.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "CurveOHLCType.h"
#include "Global.h"
#include "OHLCDialog.h"



int
OHLC::command (PluginData *pd)
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
      rc = runIndicator(pd);
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
OHLC::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "OHLC::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "OHLC::dialog: invalid settings";
    return 0;
  }
  
  OHLCDialog *dialog = new OHLCDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
OHLC::runIndicator (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  if (! pd->settings)
    return 0;
  
  QVariant *uc = pd->settings->get(QString("upColor"));
  if (! uc)
    return 0;
  
  QVariant *dc = pd->settings->get(QString("downColor"));
  if (! dc)
    return 0;
  
  QVariant *nc = pd->settings->get(QString("neutralColor"));
  if (! nc)
    return 0;
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;
  
  QVariant *style = pd->settings->get(QString("style"));
  if (! style)
    return 0;
  
  Curve *ohlc = getOHLC(style->toString(), label->toString(), uc->toString(), dc->toString(), nc->toString());
  if (! ohlc)
    return 0;
  pd->curves << ohlc;
  
  for (int pos = 1; pos < 4; pos++)
  {
    Curve *ma = getMA(pd->settings, pos);
    if (! ma)
      continue;
    
    pd->curves << ma;
  }
  
  return 1;
}

Curve *
OHLC::getOHLC (QString tstyle, QString key, QString tuc, QString tdc, QString tnc)
{
  if (! g_symbol)
    return 0;
  
  Curve *ohlc = new Curve(QString("CurveOHLC"));
  ohlc->setLabel(key);
  
  CurveOHLCType ohlcType;
  ohlc->setStyle(ohlcType.stringToIndex(tstyle));
  
  QColor uc(tuc);
  QColor dc(tdc);
  QColor nc(tnc);
  
  BarType bt;
  ohlc->fill(bt.indexToString(BarType::_OPEN),
             bt.indexToString(BarType::_HIGH),
             bt.indexToString(BarType::_LOW),
             bt.indexToString(BarType::_CLOSE),
             nc);
  
  QList<int> keys = g_symbol->keys();

  for (int pos = 1; pos < keys.size(); pos++)
  {
    Bar *pbar = ohlc->bar(keys.at(pos - 1));
    Bar *bar = ohlc->bar(keys.at(pos));
    
    if (bar->close() > pbar->close())
      bar->setColor(uc);
    else
    {
      if (bar->close() < pbar->close())
        bar->setColor(dc);
    }
  }  
  
  return ohlc;
}

Curve *
OHLC::getMA (Entity *settings, int num)
{
  QString key = "ma" + QString::number(num) + "Input";
  QVariant *input = settings->get(key);
  if (! input)
    return 0;
  
  key = "ma" + QString::number(num) + "Type";
  QVariant *type = settings->get(key);
  if (! type)
    return 0;
  
  key = "ma" + QString::number(num) + "Style";
  QVariant *style = settings->get(key);
  if (! style)
    return 0;
  
  key = "ma" + QString::number(num) + "Width";
  QVariant *width = settings->get(key);
  if (! width)
    return 0;
  
  key = "ma" + QString::number(num) + "Period";
  QVariant *period = settings->get(key);
  if (! period)
    return 0;
  
  key = "ma" + QString::number(num) + "Color";
  QVariant *color = settings->get(key);
  if (! color)
    return 0;
  
  key = "ma" + QString::number(num) + "Label";
  QVariant *label = settings->get(key);
  if (! label)
    return 0;

  MAType mat;
  if (! getMA(input->toString(), label->toString(), mat.stringToIndex(type->toString()), period->toInt()))
    return 0;
  
  Curve *curve = new Curve(QString("CurveLine"));
  curve->setLabel(label->toString());
  CurveLineType clt;
  curve->setStyle(clt.stringToIndex(style->toString()));
  curve->setPen(width->toInt());
  curve->setColor(QColor(color->toString()));
  curve->fill(label->toString(), QString(), QString(), QString(), QColor());

  return curve;
}

int
OHLC::getMA (QString inKey, QString outKey, int type, int period)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "OHLC::getMA: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double v;
    if (! bar->get(inKey, v))
      continue;
    
    input[dpos++] = (TA_Real) v;
  }

  rc = TA_MA(0, dpos - 1, &input[0], period, (TA_MAType) type, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "OHLC::getMA: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(outKey, out[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

int
OHLC::settings (PluginData *pd)
{
  Entity *command = new Entity;
  BarType bt;
  CurveOHLCType ct;
  CurveLineType clt;
  MAType mat;
  
  command->set(QString("plugin"), new QVariant(QString("OHLC")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  
  command->set(QString("style"), new QVariant(ct.indexToString(CurveOHLCType::_OHLC)));
  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("neutralColor"), new QVariant(QString("blue")));
  command->set(QString("label"), new QVariant(QString("OHLC")));

  // ma 1
  command->set(QString("ma1Input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("ma1Type"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("ma1Style"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("ma1Width"), new QVariant(1));
  command->set(QString("ma1Color"), new QVariant(QString("yellow")));
  command->set(QString("ma1Period"), new QVariant(20));
  command->set(QString("ma1Label"), new QVariant(QString("MA1")));

  // ma 2
  command->set(QString("ma2Input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("ma2Type"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("ma2Style"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("ma2Width"), new QVariant(1));
  command->set(QString("ma2Color"), new QVariant(QString("red")));
  command->set(QString("ma2Period"), new QVariant(50));
  command->set(QString("ma2Label"), new QVariant(QString("MA2")));

  // ma 3
  command->set(QString("ma3Input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("ma3Type"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("ma3Style"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("ma3Width"), new QVariant(1));
  command->set(QString("ma3Color"), new QVariant(QString("blue")));
  command->set(QString("ma3Period"), new QVariant(200));
  command->set(QString("ma3Label"), new QVariant(QString("MA3")));
  
  pd->settings = command;
  
  return 1;
}

int
OHLC::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(ohlc, OHLC);
