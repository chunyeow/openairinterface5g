#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4d6582e4, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0xf9a482f9, "msleep" },
	{ 0xb8957094, "mem_map" },
	{ 0x15692c87, "param_ops_int" },
	{ 0x69a358a6, "iomem_resource" },
	{ 0x763c5db, "dma_set_mask" },
	{ 0x184a8a78, "pci_disable_device" },
	{ 0x38620fb, "__register_chrdev" },
	{ 0x3573f04f, "x86_dma_fallback_dev" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xb74d03f5, "pci_set_master" },
	{ 0x2bc95bd4, "memset" },
	{ 0x50eedeb8, "printk" },
	{ 0xae88b44d, "ipipe_percpu" },
	{ 0xfaef0ed, "__tasklet_schedule" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x7e61808f, "dma_release_from_coherent" },
	{ 0x2072ee9b, "request_threaded_irq" },
	{ 0x98fb1f75, "dma_alloc_from_coherent" },
	{ 0xa8a6f639, "__check_region" },
	{ 0x19c39034, "pci_bus_read_config_word" },
	{ 0x7c61340c, "__release_region" },
	{ 0x37a0cba, "kfree" },
	{ 0xf85828f1, "remap_pfn_range" },
	{ 0xec591975, "dma_supported" },
	{ 0xedc03953, "iounmap" },
	{ 0x7628f3c7, "this_cpu_off" },
	{ 0x94828082, "pci_get_device" },
	{ 0x471d2cd9, "pci_iomap" },
	{ 0x436c2179, "iowrite32" },
	{ 0xc1a6619, "pci_enable_device" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xf358d5ac, "dma_ops" },
	{ 0xe484e35f, "ioread32" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "BAA63F2436D299D87A90400");
