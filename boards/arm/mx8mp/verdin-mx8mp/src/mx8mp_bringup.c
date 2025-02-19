/****************************************************************************
 * boards/arm/mx8mp/verdin-mx8mp/src/mx8mp_bringup.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <debug.h>
#include <errno.h>
#include <sys/types.h>

#include "verdin-mx8mp.h"

#ifdef CONFIG_USERLED
#  include <nuttx/leds/userled.h>
#endif

#ifdef CONFIG_SENSORS_INA219
#  include "mx8mp_ina219.h"
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mx8mp_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

int mx8mp_bringup(void)
{
  int ret = OK;

#ifdef CONFIG_USERLED
  /* Register the LED driver */

  ret = userled_lower_initialize("/dev/userleds");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: userled_lower_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount procfs at /proc: %d\n", ret);
    }
#endif

#ifdef CONFIG_MX8MP_I2C_DRIVER
  /* Initialize I2C buses */

  ret = mx8mp_i2cdev_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: mx8mp_i2cdev_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_INA219
  /* Configure and initialize the INA219 sensor in I2C4 */

  ret = board_ina219_initialize(4);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: mx8mp_ina219_initialize() failed: %d\n", ret);
    }
#endif

  return ret;
}
