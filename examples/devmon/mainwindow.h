/*
    Copyright (C) 2015  Pavel Bludov <pbludov@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <QGst/DeviceMonitor>
#include <QGst/Message>

class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void onCreateElement();
    void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem*);
    void onItemDoubleClicked(QTreeWidgetItem *item, int);

private:
    QAction *createElementAction;
    QTreeWidget *deviceTree;
    QTextEdit *propEdit;
    QTextEdit *capsEdit;
    QGst::DeviceMonitorPtr monitor;

    void onBusMessage(const QGst::MessagePtr & msg);
    void onDeviceAdded(const QGst::DevicePtr & device);
    void onDeviceRemoved(const QGst::DevicePtr & device);
    void createElement(const QGst::DevicePtr & device);
};

#endif // MAINWINDOW_H
