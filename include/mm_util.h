// Utility functions for memory management.

// Return the physical address given the virtual address. Here the virtual
// address is actually linear address, because we config the segmentation so
// that the virtual address is the same as linear address. This function cannot
// be called before the paging enabled.
void * get_phyaddr(void * virtualaddr);
