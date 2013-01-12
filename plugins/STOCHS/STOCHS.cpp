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
#include "STOCHS.h"
#include "MAType.h"
#include "BarType.h"
#include "CurveLineType.h"
#include "Global.h"
#include "STOCHSDialog.h"


int
STOCHS::command (PluginData *pd)
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
STOCHS::dialog (PluginData *pd)
{
  if (! pd->dialogParent)
  {
    qDebug() << "STOCHS::dialog: invalid parent";
    return 0;
  }
  
  if (! pd->settings)
  {
    qDebug() << "STOCHS::dialog: invalid settings";
    return 0;
  }
  
  STOCHSDialog *dialog = new STOCHSDialog(pd->dialogParent);
  dialog->setGUI(pd->settings);
  pd->dialog = dialog;
  
  return 1;
}

int
STOCHS::run (PluginData *pd)
{
  if (! g_symbol)
    return 0;
  
  QVariant *fk = pd->settings->get(QString("fkPeriod"));
  if (! fk)
    return 0;
  
  QVariant *sk = pd->settings->get(QString("kPeriod"));
  if (! sk)
    return 0;

  MAType mat;
  QVariant *var = pd->settings->get(QString("kType"));
  if (! var)
    return 0;
  int kma = mat.stringToIndex(var->toString());
  
  QVariant *klabel = pd->settings->get(QString("kLabel"));
  if (! klabel)
    return 0;
  
  var = pd->settings->get(QString("kColor"));
  if (! var)
    return 0;
  QColor kcolor(var->toString());
  
  QVariant *kstyle = pd->settings->get(QString("kStyle"));
  if (! kstyle)
    return 0;
  
  QVariant *kwidth = pd->settings->get(QString("kWidth"));
  if (! kwidth)
    return 0;
  
  QVariant *sd = pd->settings->get(QString("dPeriod"));
  if (! sd)
    return 0;
  
  QVariant *dlabel = pd->settings->get(QString("dLabel"));
  if (! dlabel)
    return 0;
  
  var = pd->settings->get(QString("dColor"));
  if (! var)
    return 0;
  QColor dcolor(var->toString());
  
  var = pd->settings->get(QString("dType"));
  if (! var)
    return 0;
  int dma = mat.stringToIndex(var->toString());
  
  QVariant *dstyle = pd->settings->get(QString("dStyle"));
  if (! dstyle)
    return 0;
  
  QVariant *dwidth = pd->settings->get(QString("dWidth"));
  if (! dwidth)
    return 0;
  
  if (! getSTOCHS(fk->toInt(),
                  sk->toInt(),
                  kma,
                  klabel->toString(),
                  sd->toInt(),
                  dma,
                  dlabel->toString()))
    return 0;

  // buy marker
  QVariant *mcolor = pd->settings->get(QString("buyMarkerColor"));
  QColor color(mcolor->toString());
  QVariant *mprice = pd->settings->get(QString("buyMarkerPrice"));
  Marker *m = newMarker(color, mprice->toDouble());
  pd->markers << m;

  // sell marker
  mcolor = pd->settings->get(QString("sellMarkerColor"));
  color = QColor(mcolor->toString());
  mprice = pd->settings->get(QString("sellMarkerPrice"));
  m = newMarker(color, mprice->toDouble());
  pd->markers << m;

  CurveLineType clt;
  Curve *c = new Curve(QString("CurveLine"));
  c->setColor(kcolor);
  c->setLabel(klabel->toString());
  c->setStyle(clt.stringToIndex(kstyle->toString()));
  c->fill(klabel->toString(), QString(), QString(), QString(), kcolor);
  c->setPen(kwidth->toInt());
  pd->curves << c;

  c = new Curve(QString("CurveLine"));
  c->setColor(dcolor);
  c->setLabel(dlabel->toString());
  c->setStyle(clt.stringToIndex(dstyle->toString()));
  c->fill(dlabel->toString(), QString(), QString(), QString(), dcolor);
  c->setPen(dwidth->toInt());
  pd->curves << c;
  
  return 1;
}

int
STOCHS::getSTOCHS (int fk, int sk, int kma, QString kkey, int sd, int dma, QString dkey)
{
  if (! g_symbol)
    return 0;
  
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug() << "STOCHS::STOCHS: error on TA_Initialize";

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

  rc = TA_STOCH(0, dpos - 1, &high[0], &low[0], &close[0], fk, sk, (TA_MAType) kma, sd, (TA_MAType) dma, &outBeg, &outNb, &out[0], &out2[0]);
  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHS::getSTOCHS: TA-Lib error" << rc;
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
STOCHS::newMarker (QColor color, double price)
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
STOCHS::settings (PluginData *pd)
{
  BarType bt;
  CurveLineType clt;
  MAType mat;
  Entity *command = new Entity;
  
  command->set(QString("plugin"), new QVariant(QString("STOCHF")));
  command->set(QString("type"), new QVariant(QString("indicator")));

  command->set(QString("kType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("kStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("kWidth"), new QVariant(1));
  command->set(QString("kColor"), new QVariant(QString("red")));
  command->set(QString("fkPeriod"), new QVariant(5));
  command->set(QString("kPeriod"), new QVariant(3));
  command->set(QString("kLabel"), new QVariant(QString("%K")));

  command->set(QString("dType"), new QVariant(mat.indexToString(MAType::_EMA)));
  command->set(QString("dStyle"), new QVariant(clt.indexToString(CurveLineType::_SOLID)));
  command->set(QString("dWidth"), new QVariant(1));
  command->set(QString("dColor"), new QVariant(QString("yellow")));
  command->set(QString("dPeriod"), new QVariant(3));
  command->set(QString("dLabel"), new QVariant(QString("%D")));
  
  command->set(QString("buyMarkerColor"), new QVariant(QString("green")));
  command->set(QString("buyMarkerPrice"), new QVariant(20.0));

  command->set(QString("sellMarkerColor"), new QVariant(QString("red")));
  command->set(QString("sellMarkerPrice"), new QVariant(80.0));
  
  pd->settings = command;
  
  return 1;
}

int
STOCHS::draw (QPainter *, const QwtScaleMap &, const QwtScaleMap &, const QRect &, void *)
{
  return 0;
}


// do not remove
Q_EXPORT_PLUGIN2(stochs, STOCHS);
