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

#ifndef PLUGIN_MACD_DIALOG_HPP
#define PLUGIN_MACD_DIALOG_HPP

#include <QDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QStringList>
#include <QTabWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>

#include "Entity.h"
#include "ColorButton.h"

class MACDDialog : public QDialog
{
  Q_OBJECT

  public:
    MACDDialog (QWidget *);
    void setGUI (Entity *);

  public slots:
    void done ();
    void cancel ();
    void help ();
    void loadSettings ();
    void saveSettings ();
    void modified ();
    void save ();

  protected:
    Entity *_settings;
    QTabWidget *_tabs;
    QDialogButtonBox *_buttonBox;
    QPushButton *_okButton;
    QPushButton *_cancelButton;
    QString _helpFile;
    int _modified;
    
    QSpinBox *_macdWidth;
    ColorButton *_macdColor;
    QSpinBox *_fast;
    QSpinBox *_slow;
    QComboBox *_macdStyle;
    QComboBox *_fastMA;
    QComboBox *_slowMA;
    QComboBox *_input;
    QCheckBox *_macdShow;
    
    QSpinBox *_signalWidth;
    ColorButton *_signalColor;
    QSpinBox *_signalPeriod;
    QComboBox *_signalStyle;
    QComboBox *_signalMA;
    QCheckBox *_signalShow;
    
    ColorButton *_histColor;
    QCheckBox *_histShow;
};

#endif
