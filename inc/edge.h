#include "board.h"
#include <stdio.h>
#include "i2c.h"


static int MAX_GRADIENT_VAL = 20;
static int MIN_GRADIENT_VAL = 8;
static int MAX_DIFF = 50;
static int  SIZE = 38; // BUT EFFECTIVE SIZE IS 40 DUE TO NEGLECTING BOUNDARY CONDITIONS
static int dist_vec[38];
static int dist_vec_filtered[38];
int size_of_vec = 0;


int edge(uint16_t distance)
{
	int i;

	if(size_of_vec >= SIZE - 1)
	{

		for(i = 0 ; i <= SIZE - 2; i++)
		{
			dist_vec[i] = dist_vec[i+1]; // SHIFT EVERYTHING LEFT
		}

		dist_vec[SIZE - 1] = (int)distance;//*DISTANCE_MEAUSREMENT;


	}
	else
	{
		dist_vec[size_of_vec] = (int)distance;
		size_of_vec = size_of_vec + 1;
	}


		//  MOVED ---------------------------------------------------



		// NOISE REDUCTION KERNEL, 1x7 GAUSSIAN KERNEL // (std deviation temporarily set to 1 pixel) e^(-x^2/(2*sigma^2))*1/(sqrt(2*pi))

		//stdev 3

		//int gauss_kernel[7] =	{106595,140367,165569,174938,165569,140367,106595};

		// stdev = 2.5
		//int gauss_kernel[7] = {92904,137865,174704,189054,174704,137865,92904}; //multiplied kernel by 1000000 in order to get ints

		// stdev = 1.5
		//int gauss_kernel[7] = {38735, 113085, 215007, 266346, 215007,113085,38735};

		//stdev = 1
		int gauss_kernel[7] = {5980,60626,241843,383103,241843,60626,5980};

		for(i = 3; i <= SIZE - 4; i++)
		{
			int sum = 0;
			int k = 0;
			int j = 0;
			for(j = i - 3; j <= i + 3; j++)
			{
				sum = sum + dist_vec[j]*gauss_kernel[k];
				k++;
			}

				dist_vec_filtered[i] = (sum)/(1000000);

		}


		dist_vec_filtered[0] = 0;  // BOUNDARIES ARE NEGLECTED
		dist_vec_filtered[1] = 0;
		dist_vec_filtered[2] = 0;
		dist_vec_filtered[SIZE - 3] = 0;
		dist_vec_filtered[SIZE - 2] = 0;
		dist_vec_filtered[SIZE - 1] = 0;


		int cent_derivative[3] = {-1, 0, 1};
		int dist_vec_derivative[38];



		// GRADIENT FIELD
		for(i = 4 /*1*/; i < SIZE - 4 /*1*/ ; i++)
		{
			int sum = 0;
			sum = cent_derivative[0]*dist_vec_filtered[i-1] + cent_derivative[1]*dist_vec_filtered[i];
			sum = sum + cent_derivative[2]*dist_vec_filtered[i+1];
			dist_vec_derivative[i] = sum;
		}

		// IF DERIVATIVE IS POSITIVE THAN THE GRADIENT IS DIRECTED ON THE RIGHT SIDE OF DIST_VEC, OTHERWISE
		// IT IS TOWARDD THE LEFT SIDE

		//NON-MAXIMUM SUPRESSION
			dist_vec_derivative[0] = 0;
			dist_vec_derivative[1] = 0;
			dist_vec_derivative[2] = 0;
			dist_vec_derivative[3] = 0;
			dist_vec_derivative[SIZE - 4] = 0;
			dist_vec_derivative[SIZE - 3] = 0;
			dist_vec_derivative[SIZE - 2] = 0;
			dist_vec_derivative[SIZE - 1] = 0;

		int dist_vec_supressed[38];

		for(i = 5  ; i < SIZE - 5 ; i++)
		{
			int current = dist_vec_derivative[i];
			if(current < 0) current = -1*current;
			int next = dist_vec_derivative[i+1];
			if(next < 0) next = -1*next;
			int previous = dist_vec_derivative[i-1];
			if(previous < 0) previous = -1*previous;

			if( (current > next) && (current > previous) )
				dist_vec_supressed[i] = dist_vec_derivative[i];
			else dist_vec_supressed[i] = 0;
		}

		dist_vec_supressed[0] = 0;
		dist_vec_supressed[1] = 0;
		dist_vec_supressed[2] = 0;
		dist_vec_supressed[3] = 0;
		dist_vec_supressed[4] = 0;
		dist_vec_supressed[SIZE - 5] = 0;
		dist_vec_supressed[SIZE - 4] = 0;
		dist_vec_supressed[SIZE - 3] = 0;
		dist_vec_supressed[SIZE - 2] = 0;
		dist_vec_supressed[SIZE - 1] = 0;

		//ASSUMING LAST 50 MEASUREMENTS APPROXIMATES SMALL CHANGES IN USER'S HAND!! (can always modify the size we check for measurements)


		int edge = 0;
		int index = 0;


		for(i = 5 ; i < SIZE - 5 ; i++)
		{
			int diff = dist_vec_filtered[i+1] - dist_vec_filtered[i-1];
			if(diff < 0) diff = -1*diff;

			if(dist_vec_supressed[i] < 0) dist_vec_supressed[i] = dist_vec_supressed[i]*(-1);

			if( dist_vec_supressed[i] >= MAX_GRADIENT_VAL)
			{
				edge = 1;
				index = i;

			}
			else if(dist_vec_supressed[i] < MIN_GRADIENT_VAL) {}
		}

		if((edge == 1) && (index == 20))
		{
			//haptic_set_waveform_queue(15);
			//haptic_notify();
			return 1;
		}

		return 0;
}
