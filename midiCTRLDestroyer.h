/****************************************************************************
**
** Copyright (C) 2007~2020 Colin Willcocks.
** All rights reserved.
** This file is part of "KATANAFxFloorBoard".
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#ifndef MIDICTRLDESTROYER_H
#define MIDICTRLDESTROYER_H

#include "midiCTRL.h"

class midiCTRLDestroyer
{

public:
    midiCTRLDestroyer(midiCTRL* s = 0);
    ~midiCTRLDestroyer();

    void SetmidiCTRL(midiCTRL* s);

private:
    midiCTRL* _midiCTRL;
};

#endif // MIDICTRLDESTROYER_H
