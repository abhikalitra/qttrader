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

#include <QtGui>
#include "ta_libc.h"
#include "MACD.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "CurveHistogramType.h"
#include "Global.h"
#include "MACDDialog.h"


int
MACD::command (PluginData *pd)
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
MACD::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "MACD::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "MACD::dialog: invalid settings";
    return 0;
  }
  
  MACDDialog *dialog = new MACDDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
MACD::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *input = pd->settings->get(QString("input"));
  if (! input)
    return 0;
  
  QVariant *fp = pd->settings->get(QString("fast"));
  if (! fp)
    return 0;
  
  QVariant *sp = pd->settings->get(QString("slow"));
  if (! sp)
    return 0;
  
  QVariant *fma = pd->settings->get(QString("fastMA"));
  if (! fma)
    return 0;
  
  QVariant *sma = pd->settings->get(QString("slowMA"));
  if (! sma)
    return 0;
  
  QVariant *mlabel = pd->settings->get(QString("macdLabel"));
  if (! mlabel)
    return 0;
  
  QVariant *siglabel = pd->settings->get(QString("signalLabel"));
  if (! siglabel)
    return 0;
  
  QVariant *histlabel = pd->settings->get(QString("histLabel"));
  if (! histlabel)
    return 0;
  
  QVariant *sigma = pd->settings->get(QString("signalMA"));
  if (! sigma)
    return 0;
  
  QVariant *sigperiod = pd->settings->get(QString("signalPeriod"));
  if (! sigperiod)
    return 0;
  
  if (! getMACD(input->toString(),
                fp->toInt(),
                sp->toInt(),
                fma->toInt(),
                sma->toInt(),
                sigperiod->toInt(),
                sigma->toInt(),
                mlabel->toString(),
                siglabel->toString(),
                histlabel->toString()))
    return 0;
  
  // hist
  QVariant *show = pd->settings->get(QString("histShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("histColor"));
    if (! var)
      return 0;
    QColor color(var->toString());

    Curve *c = new Curve(QString("CurveHistogram"));
    c->setColor(color);
    c->setLabel(histlabel->toString());
    c->setStyle(CurveHistogramType::_BAR);
    c->fill(histlabel->toString(), QString(), QString(), QString(), color);
    pd->curves << c;
  }
  
  // macd
  show = pd->settings->get(QString("macdShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("macdColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
  
    QVariant *width = pd->settings->get(QString("macdWidth"));
    if (! width)
      return 0;
  
    QVariant *style = pd->settings->get(QString("macdStyle"));
    if (! style)
      return 0;
    
    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(mlabel->toString());
    c->setPen(width->toInt());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(mlabel->toString(), QString(), QString(), QString(), QColor());
    pd->curves << c;
  }
  
  // signal
  show = pd->settings->get(QString("signalShow"));
  if (! show)
    return 0;

  if (show->toBool())
  {
    QVariant *width = pd->settings->get(QString("signalWidth"));
    if (! width)
      return 0;
  
    QVariant *var = pd->settings->get(QString("signalColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
  
    QVariant *style = pd->settings->get(QString("signalStyle"));
    if (! style)
      return 0;
  
    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setPen(width->toInt());
    c->setLabel(siglabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(siglabel->toString(), QString(), QString(), QString(), QColor());
    pd->curves << c;
  }
  
  return 1;
}

int
MACD::getMACD (QString inKey, int fp, int sp, int fma, int sma, int sigp,
               int sigma, QString mkey, QString skey, QString hkey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "MACD::MACD: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Real out3[size];
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

  rc = TA_MACDEXT(0,
                  dpos - 1,
                  &input[0],
                  fp,
                  (TA_MAType) fma,
                  sp,
                  (TA_MAType) sma,
                  sigp,
                  (TA_MAType) sigma,
                  &outBeg,
                  &outNb,
                  &out[0],
                  &out2[0],
                  &out3[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MACD::getMACD: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    
    bar->set(mkey, out[outLoop]);
    bar->set(skey, out2[outLoop]);
    bar->set(hkey, out3[outLoop]);
    
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

int
MACD::settings (PluginData *pd)
{
  Entity *command = new Entity;
  BarType bt;
  MAType mat;
  CurveHistogramType cht;
  CurveLineType clt;
  
  command->set(QString("plugin"), new QVariant(QString("MACD")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  
  command->set(QString("input"), new QVariant(bt.indexToString(BarType::_CLOSE)));
  command->set(QString("fast"), new QVariant(12));
  command->set(QString("slow"), new QVariant(24));
  command->set(QString("fastMA"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("slowMA"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("macdColor"), new QVariant(QString("red")));
  command->set(QString("macdLabel"), new QVariant(QString("MACD")));
  command->set(QString("macdStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("macdWidth"), new QVariant(1));
  command->set(QString("macdShow"), new QVariant(TRUE));

  command->set(QString("signalMA"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("signalPeriod"), new QVariant(9));
  command->set(QString("signalColor"), new QVariant(QString("yellow")));
  command->set(QString("signalLabel"), new QVariant(tr("Signal")));
  command->set(QString("signalStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("signalWidth"), new QVariant(1));
  command->set(QString("signalShow"), new QVariant(TRUE));

  command->set(QString("histColor"), new QVariant(QString("blue")));
  command->set(QString("histLabel"), new QVariant(QString("HIST")));
  command->set(QString("histShow"), new QVariant(TRUE));
  
  pd->settings = command;
  
  return 1;
}

int
MACD::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(macd, MACD);
