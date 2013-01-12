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

#include "OHLCDialog.h"
#include "Doc.h"
#include "Global.h"
#include "CurveOHLCType.h"
#include "BarType.h"
#include "MAType.h"
#include "CurveLineType.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


OHLCDialog::OHLCDialog (QWidget *p) : QDialog (p)
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

void OHLCDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void OHLCDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void OHLCDialog::cancel ()
{
  saveSettings();
  reject();
}

void OHLCDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("ohlc_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("ohlc_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void OHLCDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("ohlc_dialog_size", size());
  settings.setValue("ohlc_dialog_pos", pos());
}

void OHLCDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // OHLC page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // style
  QVariant *set = _settings->get(QString("style"));
  
  CurveOHLCType ct;
  _ohlcStyle = new QComboBox;
  _ohlcStyle->addItems(ct.list());
  _ohlcStyle->setCurrentIndex(_ohlcStyle->findText(set->toString()));
  connect(_ohlcStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _ohlcStyle);
  
  // up color
  set = _settings->get(QString("upColor"));
  
  _upColor = new ColorButton(this, QColor(set->toString()));
  connect(_upColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Up Color"), _upColor);
  
  // down color
  set = _settings->get(QString("downColor"));
  
  _downColor = new ColorButton(this, QColor(set->toString()));
  connect(_downColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Down Color"), _downColor);
  
  // neutral color
  set = _settings->get(QString("neutralColor"));
  
  _neutralColor = new ColorButton(this, QColor(set->toString()));
  connect(_neutralColor, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Neutral Color"), _neutralColor);
  
  _tabs->addTab(w, QString("OHLC"));

  // MA 1 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);

  BarType bt;
  QStringList il;
  il << bt.indexToString(BarType::_OPEN);
  il << bt.indexToString(BarType::_HIGH);
  il << bt.indexToString(BarType::_LOW);
  il << bt.indexToString(BarType::_CLOSE);

  // input
  set = _settings->get(QString("ma1Input"));

  _ma1Input = new QComboBox;
  _ma1Input->addItems(il);
  _ma1Input->setCurrentIndex(_ma1Input->findText(set->toString(), Qt::MatchExactly));
  connect(_ma1Input, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Input"), _ma1Input);
  
  // type
  set = _settings->get(QString("ma1Type"));

  MAType mat;
  _ma1Type = new QComboBox;
  _ma1Type->addItems(mat.list());
  _ma1Type->setCurrentIndex(_ma1Type->findText(set->toString(), Qt::MatchExactly));
  connect(_ma1Type, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _ma1Type);
  
  // style
  set = _settings->get(QString("ma1Style"));

  CurveLineType clt;
  _ma1Style = new QComboBox;
  _ma1Style->addItems(clt.list());
  _ma1Style->setCurrentIndex(_ma1Style->findText(set->toString(), Qt::MatchExactly));
  connect(_ma1Style, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _ma1Style);

  // width
  set = _settings->get(QString("ma1Width"));

  _ma1Width = new QSpinBox;
  _ma1Width->setRange(1, 99);
  _ma1Width->setValue(set->toInt());
  connect(_ma1Width, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _ma1Width);
  
  // color
  set = _settings->get(QString("ma1Color"));
  
  _ma1Color = new ColorButton(this, QColor(set->toString()));
  connect(_ma1Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ma1Color);
  
  // period
  set = _settings->get(QString("ma1Period"));

  _ma1Period = new QSpinBox;
  _ma1Period->setRange(1, 999999);
  _ma1Period->setValue(set->toInt());
  connect(_ma1Period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _ma1Period);
  
  _tabs->addTab(w, QString("MA 1"));
  
  // MA 2 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // input
  set = _settings->get(QString("ma2Input"));

  _ma2Input = new QComboBox;
  _ma2Input->addItems(il);
  _ma2Input->setCurrentIndex(_ma1Input->findText(set->toString(), Qt::MatchExactly));
  connect(_ma2Input, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Input"), _ma2Input);
  
  // type
  set = _settings->get(QString("ma2Type"));

  _ma2Type = new QComboBox;
  _ma2Type->addItems(mat.list());
  _ma2Type->setCurrentIndex(_ma1Type->findText(set->toString(), Qt::MatchExactly));
  connect(_ma2Type, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _ma2Type);
  
  // style
  set = _settings->get(QString("ma2Style"));

  _ma2Style = new QComboBox;
  _ma2Style->addItems(clt.list());
  _ma2Style->setCurrentIndex(_ma2Style->findText(set->toString(), Qt::MatchExactly));
  connect(_ma2Style, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _ma2Style);

  // width
  set = _settings->get(QString("ma2Width"));

  _ma2Width = new QSpinBox;
  _ma2Width->setRange(1, 99);
  _ma2Width->setValue(set->toInt());
  connect(_ma2Width, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _ma2Width);
  
  // color
  set = _settings->get(QString("ma2Color"));
  
  _ma2Color = new ColorButton(this, QColor(set->toString()));
  connect(_ma2Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ma2Color);
  
  // period
  set = _settings->get(QString("ma2Period"));

  _ma2Period = new QSpinBox;
  _ma2Period->setRange(1, 999999);
  _ma2Period->setValue(set->toInt());
  connect(_ma2Period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _ma2Period);
  
  _tabs->addTab(w, QString("MA 2"));

  // MA 3 page
  w = new QWidget;
  
  form = new QFormLayout;
  w->setLayout(form);
  
  // input
  set = _settings->get(QString("ma3Input"));

  _ma3Input = new QComboBox;
  _ma3Input->addItems(il);
  _ma3Input->setCurrentIndex(_ma1Input->findText(set->toString(), Qt::MatchExactly));
  connect(_ma3Input, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Input"), _ma3Input);
  
  // type
  set = _settings->get(QString("ma3Type"));

  _ma3Type = new QComboBox;
  _ma3Type->addItems(mat.list());
  _ma3Type->setCurrentIndex(_ma1Type->findText(set->toString(), Qt::MatchExactly));
  connect(_ma3Type, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Type"), _ma3Type);
  
  // style
  set = _settings->get(QString("ma3Style"));

  _ma3Style = new QComboBox;
  _ma3Style->addItems(clt.list());
  _ma3Style->setCurrentIndex(_ma3Style->findText(set->toString(), Qt::MatchExactly));
  connect(_ma3Style, SIGNAL(currentIndexChanged(int)), this, SLOT(modified()));
  form->addRow(tr("Style"), _ma3Style);

  // width
  set = _settings->get(QString("ma3Width"));

  _ma3Width = new QSpinBox;
  _ma3Width->setRange(1, 99);
  _ma3Width->setValue(set->toInt());
  connect(_ma3Width, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Width"), _ma3Width);
  
  // color
  set = _settings->get(QString("ma3Color"));
  
  _ma3Color = new ColorButton(this, QColor(set->toString()));
  connect(_ma3Color, SIGNAL(valueChanged(QColor)), this, SLOT(modified()));
  form->addRow(tr("Color"), _ma3Color);
  
  // period
  set = _settings->get(QString("ma3Period"));

  _ma3Period = new QSpinBox;
  _ma3Period->setRange(1, 999999);
  _ma3Period->setValue(set->toInt());
  connect(_ma3Period, SIGNAL(valueChanged(const QString &)), this, SLOT(modified()));
  form->addRow(tr("Period"), _ma3Period);
  
  _tabs->addTab(w, QString("MA 3"));
}

void OHLCDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("style"));
  set->setValue(_ohlcStyle->currentText());
  
  set = _settings->get(QString("upColor"));
  set->setValue(_upColor->color().name());

  set = _settings->get(QString("downColor"));
  set->setValue(_downColor->color().name());

  set = _settings->get(QString("neutralColor"));
  set->setValue(_neutralColor->color().name());

  set = _settings->get(QString("ma1Input"));
  set->setValue(_ma1Input->currentText());

  set = _settings->get(QString("ma1Type"));
  set->setValue(_ma1Type->currentText());

  set = _settings->get(QString("ma1Style"));
  set->setValue(_ma1Style->currentText());

  set = _settings->get(QString("ma1Width"));
  set->setValue(_ma1Width->value());

  set = _settings->get(QString("ma1Color"));
  set->setValue(_ma1Color->color().name());

  set = _settings->get(QString("ma1Period"));
  set->setValue(_ma1Period->value());

  set = _settings->get(QString("ma2Input"));
  set->setValue(_ma2Input->currentText());

  set = _settings->get(QString("ma2Type"));
  set->setValue(_ma2Type->currentText());

  set = _settings->get(QString("ma2Style"));
  set->setValue(_ma2Style->currentText());

  set = _settings->get(QString("ma2Width"));
  set->setValue(_ma2Width->value());

  set = _settings->get(QString("ma2Color"));
  set->setValue(_ma2Color->color().name());

  set = _settings->get(QString("ma2Period"));
  set->setValue(_ma2Period->value());

  set = _settings->get(QString("ma3Input"));
  set->setValue(_ma3Input->currentText());

  set = _settings->get(QString("ma3Type"));
  set->setValue(_ma3Type->currentText());

  set = _settings->get(QString("ma3Style"));
  set->setValue(_ma3Style->currentText());

  set = _settings->get(QString("ma3Width"));
  set->setValue(_ma3Width->value());

  set = _settings->get(QString("ma3Color"));
  set->setValue(_ma3Color->color().name());

  set = _settings->get(QString("ma3Period"));
  set->setValue(_ma3Period->value());
}

void OHLCDialog::modified ()
{
  _modified = 1;
}
