/*
 * Copyright (c) 2012 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COLORBARSWIDGET_H
#define COLORBARSWIDGET_H

#include <QWidget>
#include "abstractproducerwidget.h"

namespace Ui {
    class ColorBarsWidget;
}

class ColorBarsWidget : public QWidget, public AbstractProducerWidget
{
    Q_OBJECT

public:
    explicit ColorBarsWidget(QWidget *parent = 0);
    ~ColorBarsWidget();

    // AbstractProducerWidget overrides
    Mlt::Producer* producer(Mlt::Profile&);
    Mlt::Properties* getPreset() const;
    void loadPreset(Mlt::Properties&);

private:
    Ui::ColorBarsWidget *ui;
};

#endif // COLORBARSWIDGET_H
