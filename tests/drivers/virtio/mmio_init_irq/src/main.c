/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/irq.h>
#include <zephyr/ztest.h>

#define TEST_NODE DT_NODELABEL(virtio_mmio0)

ZTEST(virtio_mmio_init_irq, test_failed_init_does_not_enable_irq)
{
	const struct device *dev = DEVICE_DT_GET(TEST_NODE);
	const unsigned int irq = DT_IRQN(TEST_NODE);
	int ret;

	zassert_false(device_is_ready(dev), "deferred VirtIO MMIO device unexpectedly ready");
	zassert_false(irq_is_enabled(irq), "deferred VirtIO MMIO IRQ unexpectedly enabled");

	ret = device_init(dev);

	zassert_equal(ret, -EINVAL, "empty VirtIO MMIO slot unexpectedly initialized");
	zassert_false(device_is_ready(dev), "failed VirtIO MMIO transport became ready");
	zassert_false(irq_is_enabled(irq),
		      "VirtIO MMIO IRQ enabled despite failed transport initialization");
}

ZTEST_SUITE(virtio_mmio_init_irq, NULL, NULL, NULL, NULL, NULL);
