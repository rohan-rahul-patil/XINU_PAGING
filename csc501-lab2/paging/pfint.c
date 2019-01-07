/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
SYSCALL pfint()
{

	unsigned long a;
	int pid;

	a = read_cr2();

	pid = getpid();
	
	
	virt_addr_t v;
	v = *(virt_addr_t *)&a;

	pd_t * pd;
	pd = (pd_t *)proctab[pid].pdbr;
	
	if(v.pd_offset < 0 || v.pd_offset >= 1024) {
		kill(pid);
		return(SYSERR);
	}
	

	if(pd[v.pd_offset].pd_pres == 0) {

		int frame;
		
		get_frm(&frame);
	
		frm_tab[frame].fr_status = FRM_MAPPED;
                frm_tab[frame].fr_pid = pid;
                frm_tab[frame].fr_vpno = FRAME0 + frame;
                frm_tab[frame].fr_refcnt = 0;
                frm_tab[frame].fr_type = FR_TBL;
                frm_tab[frame].fr_dirty = 0;
		
	 
		pd[v.pd_offset].pd_pres = 1;
 		pd[v.pd_offset].pd_write = 1;
 		pd[v.pd_offset].pd_user = 0;
		pd[v.pd_offset].pd_pwt = 0;
 		pd[v.pd_offset].pd_pcd = 0;
		pd[v.pd_offset].pd_acc = 0;
 		pd[v.pd_offset].pd_mbz = 0;
 		pd[v.pd_offset].pd_fmb = 0;
 		pd[v.pd_offset].pd_global = 0;
 		pd[v.pd_offset].pd_avail = 0;
 		pd[v.pd_offset].pd_base = FRAME0 + frame;         
	
		pt_t *pt;
                pt = (pt_t *)((FRAME0 + frame) * NBPG);

		int i;
		for(i = 0; i < 1024; i++) { 
			pt->pt_pres  = 0;    
  			pt->pt_write = 0;    
  			pt->pt_user  = 0;    
  			pt->pt_pwt = 0;    
			pt->pt_pcd = 0;    
 			pt->pt_acc = 0;     
 			pt->pt_dirty = 0;     
 			pt->pt_mbz = 0;     
 			pt->pt_global = 0;     
 			pt->pt_avail = 0;     
 			pt->pt_base = 0;    
			pt = pt + 1;
		}
	}
	else {
	//	kprintf("second page fault\n");
		int physical_address, store, pageth, frame;
		pt_t *pt;
		pt = (pt_t *)(pd[v.pd_offset].pd_base * NBPG);

		//kprintf("a = %d, pid = %d\n", a, pid);		

		if(pt[v.pt_offset].pt_pres == 0) {
			if(bsm_lookup(pid, a, &store, &pageth) == SYSERR) {
				return SYSERR;
			}
	
			get_frm(&frame);

			frm_tab[frame].fr_status = FRM_MAPPED;
                        frm_tab[frame].fr_pid = pid;
                        frm_tab[frame].fr_vpno = a / NBPG;
                        frm_tab[frame].fr_refcnt = 0;
                        frm_tab[frame].fr_type = FR_PAGE;
                        frm_tab[frame].fr_dirty = 0;


			frm_tab[pd[v.pd_offset].pd_base - FRAME0].fr_refcnt += 1;

	

			physical_address = (FRAME0 + frame) * NBPG;
                        read_bs((char *)(physical_address), store, pageth);
  			
			
			pt[v.pt_offset].pt_pres  = 1;
                       	pt[v.pt_offset].pt_write = 1;
                        pt[v.pt_offset].pt_user  = 0;
                        pt[v.pt_offset].pt_pwt = 0;
                        pt[v.pt_offset].pt_pcd = 0;
                        pt[v.pt_offset].pt_acc = 0;
                        pt[v.pt_offset].pt_dirty = 0;
                        pt[v.pt_offset].pt_mbz = 0;
                        pt[v.pt_offset].pt_global = 0;
                        pt[v.pt_offset].pt_avail = 0;
                        pt[v.pt_offset].pt_base = FRAME0 + frame;
           
		}
		else {
			return(SYSERR);
		}
	}
			
  return OK;
}


