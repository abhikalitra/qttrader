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

#include "Volume.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "CurveHistogramType.h"
#include "Global.h"
#include "ta_libc.h"
#include "VolumeDialog.h"


int
Volume::command (PluginData *pd)
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
Volume::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "Volume::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "Volume::dialog: invalid settings";
    return 0;
  }
  
  VolumeDialog *dialog = new VolumeDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
Volume::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *var = pd->settings->get(QString("upColor"));
  if (! var)
    return 0;
  QColor uc(var->toString());
  
  var = pd->settings->get(QString("downColor"));
  if (! var)
    return 0;
  QColor dc(var->toString());
  
  var = pd->settings->get(QString("neutralColor"));
  if (! var)
    return 0;
  QColor nc(var->toString());
  
  QVariant *label = pd->settings->get(QString("label"));
  if (! label)
    return 0;

  Curve *vol = new Curve(QString("CurveHistogram"));
  vol->setStyle(CurveHistogramType::_BAR);
  vol->setLabel(label->toString());
  
  BarType bt;
  QList<int> keys = g_symbol->keys();
  vol->fill(bt.indexToString(BarType::_VOLUME), QString(), QString(), QString(), nc);
  
  for (int pos = 1; pos < keys.size(); pos++)
  {
    CBar *pbar = g_symbol->bar(keys.at(pos - 1));
    CBar *bar = g_symbol->bar(keys.at(pos));
    Bar *vbar = vol->bar(keys.at(pos));

    double yc = 0;
    if (! pbar->get(bt.indexToString(BarType::_CLOSE), yc))
      continue;

    double c = 0;
    if (! bar->get(bt.indexToString(BarType::_CLOSE), c))
      continue;
    
    if (c > yc)
      vbar->setColor(uc);
    else
    {
      if (c < yc)
        vbar->setColor(dc);
    }
  }  

  pd->curves << vol;
  
  Curve *ma = getMA(pd->settings);
  if (ma)
    pd->curves << ma;
  
  return 1;
}

Curve *
Volume::getMA (Entity *settings)
{
  QVariant *type = settings->get(QString("maType"));
  if (! type)
    return 0;
  
  QVariant *period = settings->get(QString("maPeriod"));
  if (! period)
    return 0;
  
  QVariant *var = settings->get(QString("maColor"));
  if (! var)
    return 0;
  QColor color(var->toString());
  
  QVariant *label = settings->get(QString("maLabel"));
  if (! label)
    return 0;
  
  QVariant *style = settings->get(QString("maStyle"));
  if (! style)
    return 0;
  
  QVariant *width = settings->get(QString("maWidth"));
  if (! width)
    return 0;

  BarType bt;
  if (! getMA(bt.indexToString(BarType::_VOLUME), label->toString(), type->toInt(), period->toInt()))
    return 0;
  
  Curve *curve = new Curve(QString("CurveLine"));
  curve->setLabel(label->toString());
  CurveLineType clt;
  curve->setStyle(clt.stringToIndex(style->toString()));
  curve->setPen(width->toInt());
  curve->setColor(color);
  curve->fill(label->toString(), QString(), QString(), QString(), color);

  return curve;
}

int
Volume::getMA (QString inKey, QString outKey, int type, int period)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "Volume::getMA: error on TA_Initialize";

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
    qDebug() << "Volume::getMA: TA-Lib error" << rc;
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
Volume::settings (PluginData *pd)
{
  Entity *command = new Entity;
  
  // plugin
  command->set(QString("plugin"), new QVariant(QString("Volume")));
  command->set(QString("type"), new QVariant(QString("indicator")));
  command->set(QString("upColor"), new QVariant(QString("green")));
  command->set(QString("downColor"), new QVariant(QString("red")));
  command->set(QString("neutralColor"), new QVariant(QString("blue")));
  command->set(QString("label"), new QVariant(QString("VOL")));

  MAType mat;
  command->set(QString("maType"), new QVariant(mat.indexToString(MAType::_EMA)));

  // style
  CurveLineType clt;
  command->set(QString("maStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  
  command->set(QString("maWidth"), new QVariant(1));
  command->set(QString("maColor"), new QVariant(QString("yellow")));
  command->set(QString("maPeriod"), new QVariant(10));
  command->set(QString("maLabel"), new QVariant(QString("MA")));
  
  pd->settings = command;
  
  return 1;
}

int
Volume::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(volume, Volume);
