/*
 * Copyright (C) 2015  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "filecontentindexer.h"
#include "filecontentindexerprovider.h"
#include "extractorprocess.h"

#include <QEventLoop>

using namespace Baloo;

FileContentIndexer::FileContentIndexer(FileContentIndexerProvider* provider)
    : m_provider(provider)
    , m_stop(0)
{
    m_process = new ExtractorProcess();

    Q_ASSERT(provider);
}

void FileContentIndexer::run()
{
    while (m_provider->size() && !m_stop.load()) {
        //
        // WARNING: This will go mad, if the Extractor does not commit after 40 files
        // cause then we will keep fetching the same 40 again and again.
        //
        QVector<quint64> idList = m_provider->fetch(40);

        QEventLoop loop;
        connect(m_process, &ExtractorProcess::done, &loop, &QEventLoop::quit);

        m_process->index(idList);
        loop.exec();
    }
}