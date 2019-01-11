// ****************** Lab2.c ***************
// Program written by: put your names here
// Date Created: 1/18/2017 
// Last Modified: 8/23/2018 
// Brief description of the Lab: 

// An embedded system is capturing temperature data from a
// sensor and performing analysis on the captured data.
// The controller part of the system is periodically capturing size
// readings of the temperature sensor. Your task is to write three
// analysis routines to help the controller perform its function
//   The three analysis subroutines are:
//    1. Calculate the mean of the temperature readings 
//       rounded down to the nearest integer
//    2. Convert from Farenheit to Centigrate using integer math 
//    3. Check if the captured readings are a non-decreasing monotonic series
//       This simply means that the readings are sorted in non-decreasing order.
//       We do not say "increasing" because it is possible for consecutive values
//       to be the same, hence the term "non-decreasing". The controller performs 
//       some remedial operation and the desired effect of the operation is to 
//       raise the the temperature of the sensed system. This routine helps 
//       verify whether this has indeed happened
#include <stdint.h>
#define True 1
#define False 0


// Find_Mean - Calculates mean temperature from values in an array
// Inputs: Readings is an array of 16-bit temperature measurements
//         N is the number of elements in the array
// Output: Mean average of the data in Readings array
// Notes: no rounding implemented
int16_t Find_Mean(int16_t const Readings[],int32_t const N)
{
  int32_t sum = 0; // Sum of temperature values
	// Add each value in Readings array to sum
	for (int32_t i = 0; i < N; i++)
	{
		sum += Readings[i];
	}
	// Calculate mean = sum / N
	int32_t mean = sum / N;
	return mean;
}

// FtoC - Converts temperature in Farenheit to temperature in Centigrade
// Inputs: temperature in Farenheit 
// Output: temperature in Centigrade
// Notes: you do not need to implement rounding
int16_t FtoC(int16_t const TinF)
{
	// Calculate temperature in C, C = [(F-32)*5]/9
	int32_t TinC = TinF - 32;
	TinC *= 5;
	TinC /= 9;
  return TinC;
}

// IsMonotonic - Returns True or False based on whether the readings
//               are an increasing monotonic series
// Inputs: Readings is an array of 16-bit temperature measurements
//         N is the number of elements in the array
// Output: True if monotonic increasing, False if nonmonotonic
int IsMonotonic(int16_t const Readings[],int32_t const N)
{
	// Check if values in Readings array are monotonic
	for (int32_t i = 0; i < N - 1; i++)
	{
		if (Readings[i] > Readings[i+1])
		{ // Return False if value is greater than next value, i.e. not monotonic increasing
			return False;
		}
	}
	// Return True if no values were greater than next value
  return True;
}
