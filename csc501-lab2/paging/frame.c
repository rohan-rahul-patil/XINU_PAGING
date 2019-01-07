/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */


SYSCALL init_frm()
{
	int frame_no;
	for(frame_no = 0; frame_no < NFRAMES; frame_no++) {		
		frm_tab[frame_no].fr_status = FRM_UNMAPPED;
		frm_tab[frame_no].fr_pid = -1;
  		frm_tab[frame_no].fr_vpno = -1;
 		frm_tab[frame_no].fr_refcnt = 0;
  		frm_tab[frame_no].fr_type = -1;              
		frm_tab[frame_no].fr_dirty = 0;

	}
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
	int pid, frame_no;
	
	static int c_queue_head = 0;

	for(frame_no = 0; frame_no < NFRAMES; frame_no++) {
		if(frm_tab[frame_no].fr_status == FRM_UNMAPPED) {
			*avail = frame_no;
			return(OK);
		}
	}

	int vpno;
	unsigned long virtual_address;
	pd_t *pd;
	pt_t *pt;
	
	int i;	
	frame_no = c_queue_head;

        for(i = 0; i < NFRAMES + 1; i++) {
                if(frame_no == NFRAMES) {
                        frame_no = 0;
                }
		
		if(frm_tab[frame_no].fr_type != FR_PAGE) {
                        frame_no = frame_no + 1;
                        continue;
                }

                vpno = frm_tab[frame_no].fr_vpno;
                virtual_address = vpno * NBPG;

                virt_addr_t v;
                v = *(virt_addr_t *)&virtual_address;


		pid = frm_tab[frame_no].fr_pid;

                pd = (pd_t *)proctab[pid].pdbr;
		
                pt = (pt_t *)(pd[v.pd_offset].pd_base * NBPG);

                if(pt[v.pt_offset].pt_acc == 1) {
                        pt[v.pt_offset].pt_acc = 0;

                }
                else {
                        c_queue_head = frame_no + 1;
			
			if(c_queue_head == NFRAMES) {
				c_queue_head = 0;
			}
	
			pt[v.pt_offset].pt_pres = 0;
			
			if(pid == getpid()){
				//to be implemented
			}

			frm_tab[pd[v.pd_offset].pd_base - FRAME0].fr_refcnt -= 1;

			if(frm_tab[pd[v.pd_offset].pd_base - FRAME0].fr_refcnt <= 0) {
				pd[v.pd_offset].pd_pres = 0;
			}
			
			if(pt[v.pt_offset].pt_dirty == 1){
				int store, pageth;
				int physical_address;

				if(bsm_lookup(pid, virtual_address, &store, &pageth) == SYSERR){
					kprintf("lookup failed in frame.c\n");
					return(SYSERR);
				}

				physical_address = (FRAME0 + frame_no) * NBPG;
				write_bs((char *)(physical_address), store, pageth);
				
			}

			*avail = frame_no;
			return OK;
                } 
                frame_no += 1;
     	}

	return(SYSERR);
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	 frm_tab[i].fr_status = FRM_UNMAPPED;
         frm_tab[i].fr_pid = -1;
         frm_tab[i].fr_vpno = -1;
         frm_tab[i].fr_refcnt = 0;
         frm_tab[i].fr_type = -1;
         frm_tab[i].fr_dirty = 0;

	  return OK;
}



