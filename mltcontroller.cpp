/*
 * Copyright (c) 2011 Meltytech, LLC
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

#include "mltcontroller.h"
#include <QWidget>
#include <QPalette>
#include <QMetaType>
#include <Mlt.h>
#ifndef Q_WS_WIN
#include "glwidget.h"
#endif
#include "sdlwidget.h"

namespace Mlt {

QFrame::QFrame(QObject *parent)
    : QObject(parent)
    , m_frame(0)
{}

QFrame::QFrame(const Frame& frame)
    : QObject(0)
{
    Frame* f = const_cast<Frame*>(&frame);
    m_frame = new Frame(f->get_frame());
}

QFrame::QFrame(const QFrame& qframe)
    : QObject(0)
{
    Frame* frame = qframe.frame();
    m_frame = new Frame(frame->get_frame());
}

QFrame::~QFrame() {
    delete m_frame;
}

Frame* QFrame::frame() const {
    return m_frame;
}

QImage QFrame::image()
{
    if (m_frame) {
        int width = 0;
        int height = 0;
        // TODO: change the format if using a pixel shader
        mlt_image_format format = mlt_image_rgb24a;
        const uint8_t* image = m_frame->get_image(format, width, height);
        QImage qimage(width, height, QImage::Format_ARGB32);
        memcpy(qimage.scanLine(0), image, width * height * 4);
        return qimage;
    }
    else {
        return QImage();
    }
}

Controller::Controller()
    : m_repo(Mlt::Factory::init())
    , m_producer(0)
    , m_consumer(0)
    , m_profile(new Mlt::Profile)
{
}

Controller* Controller::createWidget(QWidget* parent)
{
    qRegisterMetaType<QFrame>("Mlt::QFrame");
#ifdef Q_WS_MAC
    return new GLWidget(parent);
#else
    return new SDLWidget(parent);
#endif
}

Controller::~Controller()
{
    close();
    delete m_profile;
    // TODO: this is commented out because it causes crash on closing queued QFrames.
//    Mlt::Factory::close();
}

int Controller::open(Mlt::Producer* producer)
{
    int error = 0;

    if (producer != m_producer)
        close();
    if (producer && producer->is_valid()) {
        m_producer = producer;
    }
    else {
        // Cleanup on error
        error = 1;
        delete producer;
    }
    return error;
}

int Controller::open(const char* url)
{
    int error = 0;

    close();
    m_producer = new Mlt::Producer(profile(), url);
    if (m_producer->is_valid()) {
        double fps = profile().fps();
        if (!profile().get_profile()->is_explicit)
            profile().from_producer(*m_producer);
        if (profile().fps() != fps) {
            // reopen with the correct fps
            delete m_producer;
            m_producer = new Mlt::Producer(profile(), url);
        }
    }
    else {
        delete m_producer;
        m_producer = 0;
        error = 1;
    }
    return error;
}

void Controller::close()
{
    if (m_consumer)
        m_consumer->stop();
    delete m_consumer;
    m_consumer = 0;
    delete m_producer;
    m_producer = 0;
}

void Controller::play(double speed)
{
    if (m_producer)
        m_producer->set_speed(speed);
    // If we are paused, then we need to "unlock" sdl_still.
    if (m_consumer) {
        m_consumer->start();
        m_consumer->set("refresh", 1);
    }
}

void Controller::pause()
{
    if (m_producer)
        m_producer->pause();
}

void Controller::setVolume(double volume)
{
    if (m_consumer)
        m_consumer->set("volume", volume);
}

void Controller::onWindowResize()
{
    if (m_consumer)
        // When paused this tells sdl_still to update.
        m_consumer->set("refresh", 1);
}

void Controller::seek(int position)
{
    if (m_producer)
        m_producer->seek(position);
    if (m_consumer) // need to refresh consumer when paused
        m_consumer->set("refresh", 1);
}

} // namespace
