/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
**
**    GNU Lesser General Public License
**    This file may be used under the terms of the GNU Lesser
**    General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.LGPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU Lesser General Public License requirements
**    will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

/**
* \class GPUQueue
* \ingroup HatchitGraphics
*
* \brief An abstraction of a Queue used by a graphics language
*
* This a very thing abstraction of a very thin wrapper. Extend this
* only to create a very simple wrapper around your graphics language's
* concept of a queue
*/

#pragma once

#include <ht_platform.h>    //HT_API

namespace Hatchit
{
    namespace Graphics
    {
        enum class QueueType
        {
            GRAPHICS,
            COMPUTE,
            COPY
        };

        class HT_API GPUQueue
        {
        public:
            virtual ~GPUQueue() {};

            QueueType GetQueueType() const;

        protected:
            QueueType m_queueType;
        };
    }
}