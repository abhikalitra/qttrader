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

#include "ta_libc.h"
#include "STOCHF.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "Global.h"
#include "STOCHFDialog.h"


int
STOCHF::command (PluginData *pd)
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
STOCHF::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "STOCHF::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "STOCHF::dialog: invalid settings";
    return 0;
  }
  
  STOCHFDialog *dialog = new STOCHFDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
STOCHF::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *kperiod = pd->settings->get(QString("kPeriod"));
  if (! kperiod)
    return 0;
  
  QVariant *klabel = pd->settings->get(QString("kLabel"));
  if (! klabel)
    return 0;
  
  QVariant *dperiod = pd->settings->get(QString("dPeriod"));
  if (! dperiod)
    return 0;
  
  QVariant *dlabel = pd->settings->get(QString("dLabel"));
  if (! dlabel)
    return 0;
  
  MAType mat;
  QVariant *var = pd->settings->get(QString("dType"));
  if (! var)
    return 0;
  int ma = mat.stringToIndex(var->toString());
  
  if (! getSTOCHF(kperiod->toInt(),
                  klabel->toString(),
                  ma,
                  dperiod->toInt(),
                  dlabel->toString()))
    return 0;

  // %K
  QVariant *show = pd->settings->get(QString("kShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("kColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
  
    QVariant *style = pd->settings->get(QString("kStyle"));
    if (! style)
      return 0;
  
    QVariant *width = pd->settings->get(QString("kWidth"));
    if (! width)
      return 0;
    
    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(klabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(klabel->toString(), QString(), QString(), QString(), color);
    c->setPen(width->toInt());
    pd->curves << c;
  }

  // %D
  show = pd->settings->get(QString("dShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("dColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
  
    QVariant *style = pd->settings->get(QString("dStyle"));
    if (! style)
      return 0;
  
    QVariant *width = pd->settings->get(QString("dWidth"));
    if (! width)
      return 0;

    CurveLineType clt;
    Curve *c = new Curve(QString("CurveLine"));
    c->setColor(color);
    c->setLabel(dlabel->toString());
    c->setStyle(clt.stringToIndex(style->toString()));
    c->fill(dlabel->toString(), QString(), QString(), QString(), color);
    c->setPen(width->toInt());
    pd->curves << c;
  }
  
  // buy marker
  show = pd->settings->get(QString("buyMarkerShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("buyMarkerColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
    
    QVariant *price = pd->settings->get(QString("buyMarkerPrice"));
    if (! price)
      return 0;
    
    Marker *m = newMarker(color, price->toDouble());
    if (! m)
      return 0;
    
    pd->markers << m;
  }

  // sell marker
  show = pd->settings->get(QString("sellMarkerShow"));
  if (! show)
    return 0;
  
  if (show->toBool())
  {
    QVariant *var = pd->settings->get(QString("sellMarkerColor"));
    if (! var)
      return 0;
    QColor color(var->toString());
    
    QVariant *price = pd->settings->get(QString("sellMarkerPrice"));
    if (! price)
      return 0;
    
    Marker *m = newMarker(color, price->toDouble());
    if (! m)
      return 0;
    
    pd->markers << m;
  }
  
  return 1;
}

int
STOCHF::getSTOCHF (int pk, QString kkey, int ma, int pd, QString dkey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "STOCHF::STOCHF: error on TA_Initialize";

  QList<int> keys = g_symbol->keys();

  int size = keys.size();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  BarType bt;
  QString highKey = bt.indexToString(BarType::_HIGH);
  QString lowKey = bt.indexToString(BarType::_LOW);
  QString closeKey = bt.indexToString(BarType::_CLOSE);
  
  int dpos = 0;
  for (int kpos = 0; kpos < keys.size(); kpos++)
  {
    CBar *bar = g_symbol->bar(keys.at(kpos));
    
    double h;
    if (! bar->get(highKey, h))
      continue;
    
    double l;
    if (! bar->get(lowKey, l))
      continue;
    
    double c;
    if (! bar->get(closeKey, c))
      continue;
    
    high[dpos] = (TA_Real) h;
    low[dpos] = (TA_Real) l;
    close[dpos++] = (TA_Real) c;
  }

  rc = TA_STOCHF(0, dpos - 1, &high[0], &low[0], &close[0], pk, pd, (TA_MAType) ma, &outBeg, &outNb, &out[0], &out2[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHF::getSTOCHF: TA-Lib error" << rc;
    return 0;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CBar *bar = g_symbol->bar(keys.at(keyLoop));
    bar->set(kkey, out[outLoop]);
    bar->set(dkey, out2[outLoop]);
    keyLoop--;
    outLoop--;
  }
  
  return 1;
}

Marker *
STOCHF::newMarker (QColor color, double price)
{
  QString id = QUuid::createUuid().toString();
  id.remove("{");
  id.remove("}");
  id.remove("-");
  
  Marker *m = new Marker(QString("MarkerHLine"));
  m->setID(id);  
  m->setReadOnly(TRUE);
  
  Entity *e = m->settings();
  
  QVariant *tset = e->get(QString("color"));
  tset->setValue(color.name());
  
  tset = e->get(QString("price"));
  tset->setValue(price);

  return m;
}

int
STOCHF::settings (PluginData *pd)
{
  BarType bt;
  CurveLineType clt;
  MAType mat;
  Entity *command = new Entity;
  
  command->set(QString("plugin"), new QVariant(QString("STOCHF")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("kStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("kWidth"), new QVariant(1));
  command->set(QString("kColor"), new QVariant(QString("red")));
  command->set(QString("kPeriod"), new QVariant(5));
  command->set(QString("kLabel"), new QVariant(QString("%K")));
  command->set(QString("kShow"), new QVariant(TRUE));

  command->set(QString("dType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("dStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("dWidth"), new QVariant(1));
  command->set(QString("dColor"), new QVariant(QString("yellow")));
  command->set(QString("dPeriod"), new QVariant(3));
  command->set(QString("dLabel"), new QVariant(QString("%D")));
  command->set(QString("dShow"), new QVariant(TRUE));
  
  command->set(QString("buyMarkerColor"), new QVariant(QString("green")));
  command->set(QString("buyMarkerPrice"), new QVariant(20.0));
  command->set(QString("buyMarkerShow"), new QVariant(TRUE));

  command->set(QString("sellMarkerColor"), new QVariant(QString("red")));
  command->set(QString("sellMarkerPrice"), new QVariant(80.0));
  command->set(QString("sellMarkerShow"), new QVariant(TRUE));
  
  pd->settings = command;
  
  return 1;
}

int
STOCHF::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(stochf, STOCHF);
