/****************************************************************************
 * boards/arm/mx8mp/verdin-mx8mp/src/verdin-mx8mp.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __BOARDS_ARM_MX8MP_MX8MP_VERDIN_SRC_VERDIN_MX8MP_H
#define __BOARDS_ARM_MX8MP_MX8MP_VERDIN_SRC_VERDIN_MX8MP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: mx8mp_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

int mx8mp_bringup(void);

/****************************************************************************
 * Name: mx8mp_i2cdev_initialize
 *
 * Description:
 *   Called to configure all i2c
 *
 ****************************************************************************/

int mx8mp_i2cdev_initialize(void);

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_MX8MP_MX8MP_VERDIN_SRC_VERDIN_MX8MP_H */
