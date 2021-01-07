/********************************************************************
*  Name: shm.h
*
*  Purpose: Header for utility functions for creating/modifying
*           shared memory.
*
*  Author: Will Merges
*
*  RIT Launch Initiative
*********************************************************************/
#include "common/types.h"
#include <stdint.h>
#include <stdlib.h>

// NOTE: need to create this file on the system first (current issue)
#define SHM_FILE "/gsw/shmfile" // TODO this can't be a fixed location (or can it? idk)
// probably need to do this anyways

namespace shm {
    // used for generating shm id
    const int id = 65; // random number
    const int info_id = 23;

    // set the size of the block
    RetType set_shmem_size(size_t size);

    // get the size of the block
    size_t get_shmem_size();

    // attach the current process to the shared memory block
    RetType attach_to_shm();

    // detach the current process to the shared memory block
    RetType detach_from_shm();

    // destroy the current shared memory
    RetType destroy_shm();

    // write to shared memory
    // returns failure if not all bytes were able to be written
    RetType write_to_shm(void* src, size_t size);

    // read from shared memory, size is max size to read
    // returns failure if not all bytes were able to be read
    RetType read_from_shm(void* dst, size_t size);

    // create shared memory
    RetType create_shm();

    // set all shared memory to zero
    RetType clear_shm();
}
