/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-03-22
 * Description : a Mediawiki C++ interface
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Joris Munoz <munozjoris at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mediawiki_page.h"

// C++ includes

#include <algorithm>

// Qt includes


namespace MediaWiki
{

class Q_DECL_HIDDEN Page::Private
{
public:

    unsigned int m_pageid       = 0;
    unsigned int m_ns           = 0;
    unsigned int m_lastrevid    = 0;
    unsigned int m_counter      = 0;
    unsigned int m_length       = 0;
    unsigned int m_talkid       = 0;
    QString      m_edittoken;
    QString      m_title;
    QString      m_readable;
    QString      m_preload;
    QUrl         m_fullurl;
    QUrl         m_editurl;
    QDateTime    m_touched;
    QDateTime    m_starttimestamp;
};

Page::Page()
    : d(new Private)
{
}

Page::~Page()
{
    delete d;
}

Page::Page( const Page& other)
    : d(new Private(*(other.d)))
{
}

Page& Page::operator=(const Page& other)
{
    *d = *other.d;

    return *this;
}

bool Page::operator==(const Page& other) const
{
    return (
            (pageId()             == other.pageId())        &&
            (pageTitle()          == other.pageTitle())     &&
            (pageNs()             == other.pageNs())        &&
            (pageLastRevId()      == other.pageLastRevId()) &&
            (pageCounter()        == other.pageCounter())   &&
            (pageLength()         == other.pageLength())    &&
            (pageEditToken()      == other.pageEditToken()) &&
            (pageTalkid()         == other.pageTalkid())    &&
            (pageFullurl()        == other.pageFullurl())   &&
            (pageEditurl()        == other.pageEditurl())   &&
            (pageReadable()       == other.pageReadable())  &&
            (pagePreload()        == other.pagePreload())   &&
            (pageTouched()        == other.pageTouched())   &&
            (pageStarttimestamp() == other.pageStarttimestamp())
           );
}

void Page::setPageId(unsigned int id)
{
    d->m_pageid = id;
}

unsigned int Page::pageId() const
{
    return d->m_pageid;
}

void Page::setTitle(const QString& title)
{
    d->m_title = title;
}

QString Page::pageTitle() const
{
    return d->m_title;
}

void Page::setNs(unsigned int ns) const
{
    d->m_ns = ns;
}

unsigned int Page::pageNs() const
{
    return d->m_ns;
}

void Page::setLastRevId(unsigned int lastRevId) const
{
    d->m_lastrevid = lastRevId;
}

unsigned int Page::pageLastRevId() const
{
    return d->m_lastrevid;
}

void Page::setCounter(unsigned int counter) const
{
    d->m_counter = counter;
}

unsigned int Page::pageCounter() const
{
    return d->m_counter;
}

void Page::setLength(unsigned int length) const
{
     d->m_length = length;
}

unsigned int Page::pageLength() const
{
     return d->m_length;
}

void Page::setEditToken(const QString& editToken)
{
    d->m_edittoken = editToken;
}

QString Page::pageEditToken() const
{
    return d->m_edittoken;
}

void Page::setTalkid(unsigned int talkid) const
{
     d->m_talkid = talkid;
}

unsigned int Page::pageTalkid() const
{
     return d->m_talkid;
}

void Page::setFullurl(const QUrl& fullurl)
{
    d->m_fullurl = fullurl;
}

QUrl Page::pageFullurl() const
{
    return d->m_fullurl;
}

void Page::setEditurl(const QUrl& editurl)
{
    d->m_editurl = editurl;
}

QUrl Page::pageEditurl() const
{
    return d->m_editurl;
}

void Page::setReadable(const QString& readable)
{
    d->m_readable = readable;
}

QString Page::pageReadable() const
{
    return d->m_readable;
}

void Page::setPreload(const QString& preload)
{
    d->m_preload = preload;
}

QString Page::pagePreload() const
{
    return d->m_preload;
}

void Page::setTouched(const QDateTime& touched)
{
    d->m_touched = touched;
}

QDateTime Page::pageTouched() const
{
    return d->m_touched;
}

void Page::setStarttimestamp(const QDateTime& starttimestamp)
{
    d->m_starttimestamp = starttimestamp;
}

QDateTime Page::pageStarttimestamp() const
{
    return d->m_starttimestamp;
}

} // namespace MediaWiki;
