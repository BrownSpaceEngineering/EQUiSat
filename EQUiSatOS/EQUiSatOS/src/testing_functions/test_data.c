/*
 * test_data.c
 *
 * Created: 3/1/2017 9:54:37 PM
 *  Author: rj16
 */ 

#include "test_data.h"

void fill_test(test_data* id, uint16_t val) 
{
	for (int i = 0; i < 6; i++)
	{
		fill_one(&(id->one_data[i]), val);
	}
	
	for (int i = 0; i < 6; i++)
	{
		fill_two(&(id->two_data[i]), val);
	}
	
	for (int i = 0; i < 6; i++)
	{
		fill_three(&(id->three_data[i]), val);
	}
}