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
#include "mainwindow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenuBar>
#include <QTextEdit>
#include <QTreeWidget>

#include <QGlib/Connect>
#include <QGlib/ParamSpec>
#include <QGst/ElementFactory>
#include <QGst/Pipeline>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    //
    // UI
    //

    deviceTree = new QTreeWidget;
    propEdit  = new QTextEdit;
    capsEdit  = new QTextEdit;

    QLayout *layout = new QVBoxLayout();
    layout->addWidget(deviceTree);
    layout->addWidget(propEdit);
    layout->addWidget(capsEdit);
    setLayout(layout);

    deviceTree->setColumnCount(2);
    deviceTree->setColumnWidth(0, 250);
    deviceTree->setHeaderLabels(QStringList(tr("Name")) << tr("Display name"));
    deviceTree->addTopLevelItem(new QTreeWidgetItem(QStringList(tr("Devices"))));
    deviceTree->setMinimumHeight(300);
    connect(deviceTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(deviceTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem*,int)));

    propEdit->setMinimumHeight(60);
    propEdit->setReadOnly(true);

    capsEdit->setMinimumHeight(200);
    capsEdit->setReadOnly(true);
    resize(800, 600);

    //
    // Menu
    //

    QMenuBar *bar = new QMenuBar();
    QMenu *menu = new QMenu(tr("&Device Monitor"));
    bar->addMenu(menu);
    layout->setMenuBar(bar);

    createElementAction = menu->addAction(tr("&Create"), this, SLOT(onCreateElement()), QKeySequence::New);
    menu->addSeparator();
    menu->addAction(tr("&Quit"), qApp, SLOT(quit()), QKeySequence::Quit);

    //
    // DeviceMonitor
    //

    monitor = QGst::DeviceMonitor::create();

    if (!monitor) {
        QMessageBox::warning(this, this->windowTitle(), "Failed to create the device monitor");
        return;
    }

    if (!monitor->start()) {
        QMessageBox::warning(this, this->windowTitle(), "Failed to start the device monitor");
        return;
    }

    QGst::BusPtr bus = monitor->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &MainWindow::onBusMessage);

    // Add all existing devices to the list
    Q_FOREACH (QGst::DevicePtr device, monitor->devices()) {
        onDeviceAdded(device);
    }
}

MainWindow::~MainWindow()
{
    QGst::BusPtr bus = monitor->bus();
    bus->removeSignalWatch();
    QGlib::disconnect(bus, "message", this, &MainWindow::onBusMessage);
    monitor->stop();
}

void MainWindow::onBusMessage(const QGst::MessagePtr& msg)
{
    switch (msg->type()) {
    case QGst::MessageDeviceAdded:
        onDeviceAdded(msg.staticCast<QGst::DeviceAddedMessage>()->device());
        break;
    case QGst::MessageDeviceRemoved:
        onDeviceRemoved(msg.staticCast<QGst::DeviceRemovedMessage>()->device());
        break;
    default:
        qDebug() << msg->typeName() << " " << msg->source()->property("name").toString();
        break;
    }
}

void MainWindow::onDeviceAdded(const QGst::DevicePtr & device)
{
    QTreeWidgetItem *parent = deviceTree->topLevelItem(0);

    // Create the class hierarchy tree
    Q_FOREACH (QString cls, device->deviceClass().split('/')) {
        QTreeWidgetItem* next = NULL;
        for (int idx = 0; idx < parent->childCount(); ++idx) {
            QTreeWidgetItem* item = parent->child(idx);
            if (item->text(0) == cls) {
                next = item;
                break;
            }
        }
        if (!next) {
            next = new QTreeWidgetItem(QStringList(cls));
            parent->addChild(next);
        }
        parent = next;
    }

    // Add newly created item for the device
    QTreeWidgetItem* newItem = new QTreeWidgetItem(QStringList(device->name()) << device->displayName());
    parent->addChild(newItem);
    deviceTree->expandAll();

    // Display all device properties. All of them are internal, undocumented, and should be never used
    QString info;
    Q_FOREACH (const QGlib::ParamSpecPtr & prop, device->listProperties()) {
        if (QGlib::GetType<QGst::Device>().isA(prop->ownerType())) {
            // Filter out all base class properties
            continue;
        }

        info.append(prop->name())
            .append('=')
            .append(device->property(prop->name().toUtf8()).toString())
            .append("\r\n");
    }
    newItem->setData(0, Qt::UserRole, info);
    propEdit->setText(info);

    // Dsiplay the device caps
    QString caps = device->caps()->toString().replace("; ", ";\r\n");
    newItem->setData(1, Qt::UserRole, caps);
    capsEdit->setText(caps);
}

void MainWindow::onDeviceRemoved(const QGst::DevicePtr & device)
{
    Q_FOREACH (QTreeWidgetItem *item, deviceTree->findItems(device->name(), Qt::MatchRecursive)) {
        if (item->isSelected()) {
            // Clear prop & caps
            onCurrentItemChanged(NULL, NULL);
        }
        delete item;
    }
}

// Probe the device. Only Audio|Video/Source|Sink are implemented.
void MainWindow::createElement(const QGst::DevicePtr & device)
{
    QGst::PipelinePtr pipeline = QGst::Pipeline::create();
    QGst::ElementPtr src;
    QGst::ElementPtr sink;

    if (device->hasClasses("Source"))
    {
        src = device->createElement("src");
        if (!src) {
            QMessageBox::critical(this, this->windowTitle(), tr("Failed to create the source element"));
            return;
        }
        sink = QGst::ElementFactory::make(device->hasClasses("Video")? "autovideosink": "autoaudiosink");
        if (!sink) {
            QMessageBox::critical(this, this->windowTitle(), tr("Failed to create the autosink element"));
            return;
        }
    }
    else if (device->hasClasses("Sink")) {
        sink = device->createElement("sink");
        if (!sink) {
            QMessageBox::critical(this, this->windowTitle(), tr("Failed to create the sink element"));
            return;
        }
        src = QGst::ElementFactory::make(device->hasClasses("Video")? "videotestsrc": "audiotestsrc");
        if (!src) {
            QMessageBox::critical(this, this->windowTitle(), tr("Failed to create the test source element"));
            return;
        }
    }
    else {
        QMessageBox::warning(this, this->windowTitle(), tr("Unsupported class '%1'").arg(device->deviceClass()));
        return;
    }

    // Build sample pipeline for testing

    pipeline->add(src, sink);
    src->link(sink);

    pipeline->setState(QGst::StatePlaying);
    if (QGst::StateChangeSuccess != pipeline->getState(NULL, NULL, QGst::ClockTime::fromSeconds(10))) {
        QMessageBox::critical(this, this->windowTitle(), tr("Failed to set the pipeline to playing state"));
    } else {
        QMessageBox::information(this, device->displayName(), tr("It works! Press 'OK' to continue"));
    }
    pipeline->setState(QGst::StateNull);
    pipeline->getState(NULL, NULL, QGst::ClockTime::fromSeconds(10));
}

void MainWindow::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (current) {
        propEdit->setText(current->data(0, Qt::UserRole).toString());
        capsEdit->setText(current->data(1, Qt::UserRole).toString());
        createElementAction->setEnabled(!current->data(0, Qt::UserRole).isNull());
    }
    else {
        propEdit->clear();
        capsEdit->clear();
        createElementAction->setEnabled(false);
    }
}

void MainWindow::onCreateElement()
{
    QTreeWidgetItem *item = deviceTree->currentItem();
    if (item) {
        onItemDoubleClicked(item, 0);
    }
 }

void MainWindow::onItemDoubleClicked(QTreeWidgetItem *item, int)
{
    QString name = item->text(0);
    Q_FOREACH (QGst::DevicePtr device, monitor->devices()) {
        if (device->name() == name) {
            createElement(device);
            break;
        }
    }
}
