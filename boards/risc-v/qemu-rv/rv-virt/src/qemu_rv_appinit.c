/****************************************************************************
 * boards/risc-v/qemu-rv/rv-virt/src/qemu_rv_appinit.c
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

#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <errno.h>

#include <nuttx/board.h>
#include <nuttx/virtio/virtio-mmio.h>

#include <sys/mount.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define QEMU_VIRTIO_MMIO_BASE    0x10001000
#define QEMU_VIRTIO_MMIO_REGSIZE 0x1000
#ifdef CONFIG_ARCH_USE_S_MODE
#  define QEMU_VIRTIO_MMIO_IRQ   26
#else
#  define QEMU_VIRTIO_MMIO_IRQ   28
#endif
#define QEMU_VIRTIO_MMIO_NUM     8

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: qemu_virtio_register_mmio_devices
 ****************************************************************************/

#ifdef CONFIG_DRIVERS_VIRTIO_MMIO
static void qemu_virtio_register_mmio_devices(void)
{
  uintptr_t virtio = (uintptr_t)QEMU_VIRTIO_MMIO_BASE;
  size_t size = QEMU_VIRTIO_MMIO_REGSIZE;
  int irq = QEMU_VIRTIO_MMIO_IRQ;
  int i;

  for (i = 0; i < QEMU_VIRTIO_MMIO_NUM; i++)
    {
      virtio_register_mmio_device((FAR void *)(virtio + size * i), irq + i);
    }
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Perform architecture specific initialization
 *
 * Input Parameters:
 *   arg - The boardctl() argument is passed to the board_app_initialize()
 *         implementation without modification.  The argument has no
 *         meaning to NuttX; the meaning of the argument is a contract
 *         between the board-specific initialization logic and the
 *         matching application logic.  The value could be such things as a
 *         mode enumeration value, a set of DIP switch switch settings, a
 *         pointer to configuration data read from a file or serial FLASH,
 *         or whatever you would like to do with it.  Every implementation
 *         should accept zero/NULL as a default configuration.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is returned on
 *   any failure to indicate the nature of the failure.
 *
 ****************************************************************************/

int board_app_initialize(uintptr_t arg)
{
#ifdef CONFIG_BOARD_LATE_INITIALIZE
  /* Board initialization already performed by board_late_initialize() */

  return OK;
#else
  /* Perform board-specific initialization */

#ifdef CONFIG_NSH_ARCHINIT

  mount(NULL, "/proc", "procfs", 0, NULL);

#endif

#ifdef CONFIG_DRIVERS_VIRTIO_MMIO
  qemu_virtio_register_mmio_devices();
#endif

  return OK;
#endif
}
