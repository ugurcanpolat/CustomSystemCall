#ifndef _ASM_X86_HARDIRQ_H
#define _ASM_X86_HARDIRQ_H

#include <linux/threads.h>
#include "linux/percpu-ubuntu.h"

typedef struct {
	u16	     __softirq_pending;
#if IS_ENABLED(CONFIG_KVM_INTEL)
	u8	     kvm_cpu_l1tf_flush_l1d;
#endif
	unsigned int __nmi_count;	/* arch dependent */
#ifdef CONFIG_X86_LOCAL_APIC
	unsigned int apic_timer_irqs;	/* arch dependent */
	unsigned int irq_spurious_count;
	unsigned int icr_read_retry_count;
#endif
#ifdef CONFIG_HAVE_KVM
	unsigned int kvm_posted_intr_ipis;
#endif
	unsigned int x86_platform_ipis;	/* arch dependent */
	unsigned int apic_perf_irqs;
	unsigned int apic_irq_work_irqs;
#ifdef CONFIG_SMP
	unsigned int irq_resched_count;
	unsigned int irq_call_count;
#endif
	unsigned int irq_tlb_count;
#ifdef CONFIG_X86_THERMAL_VECTOR
	unsigned int irq_thermal_count;
#endif
#ifdef CONFIG_X86_MCE_THRESHOLD
	unsigned int irq_threshold_count;
#endif
} ____cacheline_aligned irq_cpustat_t;

DECLARE_PER_CPU_SHARED_ALIGNED(irq_cpustat_t, irq_stat);

/* We can have at most NR_VECTORS irqs routed to a cpu at a time */
#define MAX_HARDIRQS_PER_CPU NR_VECTORS

#define __ARCH_IRQ_STAT

#define inc_irq_stat(member)	this_cpu_inc(irq_stat.member)

#define local_softirq_pending()	this_cpu_read(irq_stat.__softirq_pending)

#define __ARCH_SET_SOFTIRQ_PENDING

#define set_softirq_pending(x)	\
		this_cpu_write(irq_stat.__softirq_pending, (x))
#define or_softirq_pending(x)	this_cpu_or(irq_stat.__softirq_pending, (x))

extern void ack_bad_irq(unsigned int irq);

extern u64 arch_irq_stat_cpu(unsigned int cpu);
#define arch_irq_stat_cpu	arch_irq_stat_cpu

extern u64 arch_irq_stat(void);
#define arch_irq_stat		arch_irq_stat


#if IS_ENABLED(CONFIG_KVM_INTEL)
static inline void kvm_set_cpu_l1tf_flush_l1d(void)
{
	__this_cpu_write(irq_stat.kvm_cpu_l1tf_flush_l1d, 1);
}

static inline void kvm_clear_cpu_l1tf_flush_l1d(void)
{
	__this_cpu_write(irq_stat.kvm_cpu_l1tf_flush_l1d, 0);
}

static inline bool kvm_get_cpu_l1tf_flush_l1d(void)
{
	return __this_cpu_read(irq_stat.kvm_cpu_l1tf_flush_l1d);
}
#else /* !IS_ENABLED(CONFIG_KVM_INTEL) */
static inline void kvm_set_cpu_l1tf_flush_l1d(void) { }
#endif /* IS_ENABLED(CONFIG_KVM_INTEL) */

#endif /* _ASM_X86_HARDIRQ_H */
