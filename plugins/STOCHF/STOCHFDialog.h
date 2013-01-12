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

#ifndef PLUGIN_STOCHF_DIALOG_HPP
#define PLUGIN_STOCHF_DIALOG_HPP

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

class STOCHFDialog : public QDialog
{
  Q_OBJECT

  public:
    STOCHFDialog (QWidget *);
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
    
    ColorButton *_kColor;
    QSpinBox *_kPeriod;
    QSpinBox *_kWidth;
    QComboBox *_kStyle;
    QCheckBox *_kShow;
    
    QSpinBox *_dWidth;
    ColorButton *_dColor;
    QSpinBox *_dPeriod;
    QComboBox *_dStyle;
    QComboBox *_dType;
    QCheckBox *_dShow;
    
    QDoubleSpinBox *_ref1;
    ColorButton *_ref1Color;
    QCheckBox *_ref1Show;
    
    QDoubleSpinBox *_ref2;
    ColorButton *_ref2Color;
    QCheckBox *_ref2Show;
};

#endif
