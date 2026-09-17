/*
 * SPDX-FileCopyrightText: Copyright The Zephyr Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/fff.h>
#include <zephyr/ztest.h>

#define TEST_NODE DT_NODELABEL(virtio_mmio0)

DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(test_irq_enable, unsigned int);

void __real_arch_irq_enable(unsigned int irq);

void __wrap_arch_irq_enable(unsigned int irq)
{
	if (irq == DT_IRQN(TEST_NODE)) {
		test_irq_enable(irq);
		return;
	}

	__real_arch_irq_enable(irq);
}

ZTEST(virtio_mmio_init_irq, test_failed_init_does_not_enable_irq)
{
	const struct device *dev = DEVICE_DT_GET(TEST_NODE);
	int ret;

	RESET_FAKE(test_irq_enable);
	zassert_false(device_is_ready(dev), "deferred VirtIO MMIO device unexpectedly ready");

	ret = device_init(dev);

	zassert_equal(ret, -EINVAL, "empty VirtIO MMIO slot unexpectedly initialized");
	zassert_false(device_is_ready(dev), "failed VirtIO MMIO transport became ready");
	zassert_equal(test_irq_enable_fake.call_count, 0U,
		      "VirtIO MMIO IRQ enabled despite failed transport initialization");
}

ZTEST_SUITE(virtio_mmio_init_irq, NULL, NULL, NULL, NULL, NULL);
