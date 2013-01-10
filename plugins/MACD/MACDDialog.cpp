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

#include "MACDDialog.h"
#include "Doc.h"
#include "Global.h"
#include "MAType.h"
#include "CurveLineType.h"
#include "BarType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


MACDDialog::MACDDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void MACDDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void MACDDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void MACDDialog::cancel ()
{
  saveSettings();
  reject();
}

void MACDDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("macd_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("macd_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void MACDDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("macd_dialog_size", size());
  settings.setValue("macd_dialog_pos", pos());
}

void MACDDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // MACD page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // input
  QVariant *set = _settings->get(QString("input"));

  BarType bt;
  _input = new QComboBox;
  _input->addItems(bt.list());
  _input->setCurrentIndex(_input->findText(set->toString(), Qt::MatchExactly));
  connect(_input, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Input"), _input);
  
  // color
  set = _settings->get(QString("macdColor"));
  
  _macdColor = new ColorButton(this, QColor(set->toString()));
  connect(_macdColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _macdColor);
  
  // fast period
  set = _settings->get(QString("fast"));

  _fast = new QSpinBox;
  _fast->setRange(1, 999999);
  _fast->setValue(set->toInt());
  connect(_fast, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Fast Period"), _fast);
  
  // slow period
  set = _settings->get(QString("slow"));

  _slow = new QSpinBox;
  _slow->setRange(1, 999999);
  _slow->setValue(set->toInt());
  connect(_slow, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Slow Period"), _slow);

  // ma fast type
  set = _settings->get(QString("fastMA"));

  MAType mat;
  _fastMA = new QComboBox;
  _fastMA->addItems(mat.list());
  _fastMA->setCurrentIndex(_fastMA->findText(set->toString(), Qt::MatchExactly));
  connect(_fastMA, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Fast MA"), _fastMA);

  // ma slow type
  set = _settings->get(QString("slowMA"));

  _slowMA = new QComboBox;
  _slowMA->addItems(mat.list());
  _slowMA->setCurrentIndex(_slowMA->findText(set->toString(), Qt::MatchExactly));
  connect(_slowMA, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Slow MA"), _slowMA);
  
  // line style
  set = _settings->get(QString("macdStyle"));

  CurveLineType clt;
  _macdStyle = new QComboBox;
  _macdStyle->addItems(clt.list());
  _macdStyle->setCurrentIndex(_macdStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_macdStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _macdStyle);

  // width
  set = _settings->get(QString("macdWidth"));

  _macdWidth = new QSpinBox;
  _macdWidth->setRange(1, 99);
  _macdWidth->setValue(set->toInt());
  connect(_macdWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _macdWidth);

  // show
  set = _settings->get(QString("macdShow"));

  _macdShow = new QCheckBox;
  _macdShow->setChecked(set->toBool());
  connect(_macdShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _macdShow);
  
  _tabs->addTab(w, QString("MACD"));

  // SIGNAL page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // style
  set = _settings->get(QString("signalStyle"));

  _signalStyle = new QComboBox;
  _signalStyle->addItems(clt.list());
  _signalStyle->setCurrentIndex(_signalStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_signalStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _signalStyle);

  // width
  set = _settings->get(QString("signalWidth"));

  _signalWidth = new QSpinBox;
  _signalWidth->setRange(1, 99);
  _signalWidth->setValue(set->toInt());
  connect(_signalWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _signalWidth);
  
  // color
  set = _settings->get(QString("signalColor"));
  
  _signalColor = new ColorButton(this, QColor(set->toString()));
  connect(_signalColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _signalColor);

  // ma type
  set = _settings->get(QString("signalMA"));

  _signalMA = new QComboBox;
  _signalMA->addItems(mat.list());
  _signalMA->setCurrentIndex(_fastMA->findText(set->toString(), Qt::MatchExactly));
  connect(_signalMA, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("MA"), _signalMA);
  
  // period
  set = _settings->get(QString("signalPeriod"));

  _signalPeriod = new QSpinBox;
  _signalPeriod->setRange(1, 999999);
  _signalPeriod->setValue(set->toInt());
  connect(_signalPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _signalPeriod);
  
  // show
  set = _settings->get(QString("signalShow"));

  _signalShow = new QCheckBox;
  _signalShow->setChecked(set->toBool());
  connect(_signalShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _signalShow);

  _tabs->addTab(w, tr("Signal"));

  // HIST page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // color
  set = _settings->get(QString("histColor"));
  
  _histColor = new ColorButton(this, QColor(set->toString()));
  connect(_histColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _histColor);
  
  // show
  set = _settings->get(QString("histShow"));

  _histShow = new QCheckBox;
  _histShow->setChecked(set->toBool());
  connect(_histShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _histShow);

  _tabs->addTab(w, tr("Histogram"));
}

void MACDDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("macdColor"));
  set->setValue(_macdColor->color().name());

  set = _settings->get(QString("fast"));
  set->setValue(_fast->value());

  set = _settings->get(QString("slow"));
  set->setValue(_slow->value());

  set = _settings->get(QString("fastMA"));
  set->setValue(_fastMA->currentText());

  set = _settings->get(QString("slowMA"));
  set->setValue(_slowMA->currentText());

  set = _settings->get(QString("macdStyle"));
  set->setValue(_macdStyle->currentText());

  set = _settings->get(QString("macdWidth"));
  set->setValue(_macdWidth->value());

  set = _settings->get(QString("macdShow"));
  set->setValue(_macdShow->isChecked());

  set = _settings->get(QString("signalColor"));
  set->setValue(_signalColor->color().name());

  set = _settings->get(QString("signalPeriod"));
  set->setValue(_signalPeriod->value());

  set = _settings->get(QString("signalWidth"));
  set->setValue(_signalWidth->value());

  set = _settings->get(QString("signalStyle"));
  set->setValue(_signalStyle->currentText());

  set = _settings->get(QString("signalShow"));
  set->setValue(_signalShow->isChecked());

  set = _settings->get(QString("histColor"));
  set->setValue(_histColor->color().name());

  set = _settings->get(QString("histShow"));
  set->setValue(_histShow->isChecked());
}

void MACDDialog::modified ()
{
  _modified = 1;
}
