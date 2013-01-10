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

#include "STOCHFDialog.h"
#include "Doc.h"
#include "Global.h"
#include "MAType.h"
#include "CurveLineType.h"
#include "BarType.h"

#include <QtDebug>
#include <QVariant>
#include <QLayout>
#include <QFormLayout>


STOCHFDialog::STOCHFDialog (QWidget *p) : QDialog (p)
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

void STOCHFDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void STOCHFDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void STOCHFDialog::cancel ()
{
  saveSettings();
  reject();
}

void STOCHFDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("stochf_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("stochf_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void STOCHFDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("stochf_dialog_size", size());
  settings.setValue("stochf_dialog_pos", pos());
}

void STOCHFDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // %K page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // color
  QVariant *set = _settings->get(QString("kColor"));
  
  _kColor = new ColorButton(this, QColor(set->toString()));
  connect(_kColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _kColor);
  
  // period
  set = _settings->get(QString("kPeriod"));

  _kPeriod = new QSpinBox;
  _kPeriod->setRange(1, 999999);
  _kPeriod->setValue(set->toInt());
  connect(_kPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _kPeriod);
  
  // style
  set = _settings->get(QString("kStyle"));

  CurveLineType clt;
  _kStyle = new QComboBox;
  _kStyle->addItems(clt.list());
  _kStyle->setCurrentIndex(_kStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_kStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _kStyle);

  // width
  set = _settings->get(QString("kWidth"));

  _kWidth = new QSpinBox;
  _kWidth->setRange(1, 99);
  _kWidth->setValue(set->toInt());
  connect(_kWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _kWidth);

  // show
  set = _settings->get(QString("kShow"));

  _kShow = new QCheckBox;
  _kShow->setChecked(set->toBool());
  connect(_kShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _kShow);

  _tabs->addTab(w, QString("%K"));

  
  // %D page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  // type
  set = _settings->get(QString("dType"));

  MAType mat;
  _dType = new QComboBox;
  _dType->addItems(mat.list());
  _dType->setCurrentIndex(_dType->findText(set->toString(), Qt::MatchExactly));
  connect(_dType, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _dType);
  
  // style
  set = _settings->get(QString("dStyle"));

  _dStyle = new QComboBox;
  _dStyle->addItems(clt.list());
  _dStyle->setCurrentIndex(_dStyle->findText(set->toString(), Qt::MatchExactly));
  connect(_dStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _dStyle);

  // width
  set = _settings->get(QString("dWidth"));

  _dWidth = new QSpinBox;
  _dWidth->setRange(1, 99);
  _dWidth->setValue(set->toInt());
  connect(_dWidth, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _dWidth);
  
  // color
  set = _settings->get(QString("dColor"));
  
  _dColor = new ColorButton(this, QColor(set->toString()));
  connect(_dColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _dColor);
  
  // period
  set = _settings->get(QString("dPeriod"));

  _dPeriod = new QSpinBox;
  _dPeriod->setRange(1, 999999);
  _dPeriod->setValue(set->toInt());
  connect(_dPeriod, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _dPeriod);
  
  // show
  set = _settings->get(QString("dShow"));

  _dShow = new QCheckBox;
  _dShow->setChecked(set->toBool());
  connect(_dShow, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _dShow);

  _tabs->addTab(w, QString("%D"));
  

  // buy marker page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("buyMarkerColor"));
  
  _ref1Color = new ColorButton(this, QColor(set->toString()));
  connect(_ref1Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ref1Color);

  // value
  set = _settings->get(QString("buyMarkerPrice"));

  _ref1 = new QDoubleSpinBox;
  _ref1->setRange(-99999999.0, 99999999.0);
  _ref1->setValue(set->toDouble());
  connect(_ref1, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Value"), _ref1);

  // show
  set = _settings->get(QString("buyMarkerShow"));

  _ref1Show = new QCheckBox;
  _ref1Show->setChecked(set->toBool());
  connect(_ref1Show, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _ref1Show);

  _tabs->addTab(w, tr("Buy Marker"));
  

  // REF2 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // color
  set = _settings->get(QString("sellMarkerColor"));
  
  _ref2Color = new ColorButton(this, QColor(set->toString()));
  connect(_ref2Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ref2Color);

  // value
  set = _settings->get(QString("sellMarkerPrice"));

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-99999999.0, 99999999.0);
  _ref2->setValue(set->toDouble());
  connect(_ref2, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Value"), _ref2);

  // show
  set = _settings->get(QString("sellMarkerShow"));

  _ref2Show = new QCheckBox;
  _ref2Show->setChecked(set->toBool());
  connect(_ref2Show, SIGNAL(toggled(bool)), this, SLOT(modified()));
  form->addRow(tr("Show"), _ref2Show);

  _tabs->addTab(w, tr("Sell Marker"));
}

void STOCHFDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("kColor"));
  set->setValue(_kColor->color().name());

  set = _settings->get(QString("kPeriod"));
  set->setValue(_kPeriod->value());

  set = _settings->get(QString("kWidth"));
  set->setValue(_kWidth->value());

  set = _settings->get(QString("kStyle"));
  set->setValue(_kStyle->currentText());

  set = _settings->get(QString("kShow"));
  set->setValue(_kShow->isChecked());

  set = _settings->get(QString("dColor"));
  set->setValue(_dColor->color().name());

  set = _settings->get(QString("dPeriod"));
  set->setValue(_dPeriod->value());

  set = _settings->get(QString("dWidth"));
  set->setValue(_dWidth->value());

  set = _settings->get(QString("dStyle"));
  set->setValue(_dStyle->currentText());

  set = _settings->get(QString("dType"));
  set->setValue(_dType->currentText());

  set = _settings->get(QString("dShow"));
  set->setValue(_dShow->isChecked());

  set = _settings->get(QString("buyMarkerColor"));
  set->setValue(_ref1Color->color().name());

  set = _settings->get(QString("buyMarkerPrice"));
  set->setValue(_ref1->value());

  set = _settings->get(QString("buyMarkerShow"));
  set->setValue(_ref1Show->isChecked());

  set = _settings->get(QString("sellMarkerColor"));
  set->setValue(_ref2Color->color().name());

  set = _settings->get(QString("sellMarkerPrice"));
  set->setValue(_ref2->value());

  set = _settings->get(QString("sellMarkerShow"));
  set->setValue(_ref2Show->isChecked());
}

void STOCHFDialog::modified ()
{
  _modified = 1;
}
