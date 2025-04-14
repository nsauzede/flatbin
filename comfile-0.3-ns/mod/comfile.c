/* comfile.c: Loadable kernel module implementing command executables.
 * Copyright 2019 Brian Raiter <breadbox@muppetlabs.com>.
 * License: GNU GPL version 2 or later.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/binfmts.h>
#include <linux/personality.h>
#include <linux/processor.h>
#include <linux/ptrace.h>
#include <linux/sched/task_stack.h>
#include <linux/uaccess.h>

MODULE_DESCRIPTION("Linux command executable files");
MODULE_AUTHOR("Brian Raiter <breadbox@muppetlabs.com>");
MODULE_VERSION("0.3");
MODULE_LICENSE("GPL");

static struct linux_binfmt comfile_fmt;

/* Given argc + envc strings above the top of the stack, construct the
 * argv and envp arrays in the memory preceding, and then push argc,
 * argv, and envp onto the stack. Also reserve a final slot for the
 * top function return address, to be set by the caller.
 * Set up rsi/rdi/rdx/rax registers. Return the new stack top address.
 */
static unsigned long make_arrays(struct linux_binprm const *lbp)
{
    void* __user *sp;
    char* __user *argv;
    char* __user *envp;
    char __user *p;
    int i;

    p = (char __user *)lbp->p;
    envp = (char* __user *)ALIGN(lbp->p, sizeof *envp);
    envp = envp - (lbp->envc + 1);
    argv = envp - (lbp->argc + 1);
    sp = (void* __user *)argv - 4;      // leave room for: func ret, argv, argv, envp

    current->mm->arg_start = (unsigned long)p;
    for (i = 0 ; i < lbp->argc ; ++i) {
        put_user(p, argv + i);
        p += strnlen_user(p, MAX_ARG_STRLEN);
    }
    put_user(NULL, argv + i);
    current->mm->arg_end = (unsigned long)p;

    current->mm->env_start = (unsigned long)p;
    for (i = 0 ; i < lbp->envc ; ++i) {
        put_user(p, envp + i);
        p += strnlen_user(p, MAX_ARG_STRLEN);
    }
    put_user(NULL, envp + i);
    current->mm->env_end = (unsigned long)p;

    put_user((void*)(unsigned long)lbp->argc, sp + 1);
    put_user(argv, sp + 2);
    put_user(envp, sp + 3);
    // func ret will be put_user by our caller

    // also setup registers
    struct pt_regs *regs = current_pt_regs();
    regs->di = lbp->argc;
    regs->si = (unsigned long)argv;
    regs->dx = (unsigned long)envp;
    regs->ax = 0;

    return (unsigned long)sp;
}

/* Map a binary file into memory, initialize the heap and stack, and
 * schedule the process.
 */
static int load_comfile_binary(struct linux_binprm *lbp)
{
    long const loadaddr = 0x00010000;

    char const *ext;
    loff_t filesize, allocsize;
    int r;

    ext = strrchr(lbp->filename, '.');
    if (!ext || strcmp(ext, ".com"))
        return -ENOEXEC;

    //r = flush_old_exec(lbp);  // removed from recent kernels ?
    r = begin_new_exec(lbp);
    if (r)
        return r;
    set_personality(PER_LINUX);
    set_binfmt(&comfile_fmt);
    setup_new_exec(lbp);

    filesize = generic_file_llseek(lbp->file, 0, SEEK_END);
    generic_file_llseek(lbp->file, 0, SEEK_SET);
    allocsize = PAGE_ALIGN(filesize + 16); // ensure we have at least 16 bytes after filesize, to store epilog (calling exit)

    current->mm->start_code = loadaddr;
    current->mm->end_code = current->mm->start_code + filesize;
    current->mm->start_data = current->mm->end_code;
    current->mm->end_data = loadaddr + allocsize;
    current->mm->start_brk = current->mm->end_data;
    current->mm->brk = current->mm->start_brk;

    r = setup_arg_pages(lbp, STACK_TOP, EXSTACK_DEFAULT);
    if (r)
        return r;
    current->mm->start_stack = make_arrays(lbp);

    r = vm_mmap(lbp->file, loadaddr, allocsize,         // allocsize>filesize => extra space will be 0s
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_FIXED | MAP_PRIVATE, 0);
    if (r < 0)
        return r;

    void* __user *sp = (void* __user *)current->mm->start_stack;        // top of stack contains the function return address
    void* __user *epi = (void* __user *)(loadaddr + filesize);          // when main() returns, it will go back to the epilog
    put_user((void *)(unsigned long)epi, sp);
/*      4889C7            mov rdi,rax
        B03C              mov al,0x3c
        0F05              syscall
        CC                int3          */
    put_user((void *)(unsigned long)0xCC050f3cb0c78948, epi);           // the epilog calls sys_exit() with main() return code

    //r = vm_brk(current->mm->start_brk, 0);    // removed from recent kernels ?
    r = vm_brk_flags(current->mm->start_brk, 0, 0);
    if (r < 0)
        return r;

    //install_exec_creds(lbp);  // removed from recent kernels ?
    finalize_exec(lbp);         // supported in recent kernels, could be uncommented
    printk(KERN_INFO "Starting thread at 0x%016lx\n", loadaddr);
    start_thread(current_pt_regs(), loadaddr, current->mm->start_stack);
    return 0;
}

static struct linux_binfmt comfile_fmt = {
    .module = THIS_MODULE,
    .load_binary = load_comfile_binary,
    .load_shlib = NULL,
    .core_dump = NULL,
    .min_coredump = 0
};

static int __init comfile_start(void)
{
    register_binfmt(&comfile_fmt);
    return 0;
}

static void __exit comfile_end(void)
{
    unregister_binfmt(&comfile_fmt);
}

module_init(comfile_start);
module_exit(comfile_end);
