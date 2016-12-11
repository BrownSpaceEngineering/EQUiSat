/*
 * EquiStacks.h
 *
 * Created: 11/1/2016 9:45:00 PM
 *  Author: jleiken
 */ 


#ifndef EQUISTACKS_H_
#define EQUISTACKS_H_

#define MUTEX_WAIT_TIME_TICKS 10

#include "boot_Stack.h"
#include "flash_Stack.h"
#include "idle_Stack.h"
#include "low_power_Stack.h"
#include "Num_Stack.h"
#include "stack_tests.h"

void clear_existing_data(void* ptr, size_t slot_size);

#endif /* EQUISTACKS_H_ */