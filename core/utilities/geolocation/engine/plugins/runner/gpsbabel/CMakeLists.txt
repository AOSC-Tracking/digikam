#
# SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

set(gpsbabel_SRCS GpsbabelPlugin.cpp GpsbabelRunner.cpp)

marble_add_plugin(NAME    GpsbabelPlugin
                  SOURCES ${gpsbabel_SRCS})

