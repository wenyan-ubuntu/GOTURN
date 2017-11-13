#include "vot.h"

#include <cfloat>

#define VOT_RECTANGLE

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#ifndef VOT_RECTANGLE
#define VOT_POLYGON
#endif

#ifdef VOT_POLYGON
void vot_region_release(vot_region** region) 
{
	if (!(*region)) return;
	
	if ((*region)->x) {
		free((*region)->x);
		(*region)->x = NULL;
	}
	
	if ((*region)->y) {
		free((*region)->y);
		(*region)->y = NULL;
	}
	
	free(*region);
	
	*region = NULL;
}

vot_region* vot_region_create(int n)
{
	vot_region* region = (vot_region *) malloc(sizeof(vot_region));
	region->x = (float *) malloc(sizeof(float) * n);
	region->y = (float *) malloc(sizeof(float) * n);
	
	memset(region->x, 0, sizeof(float) * n);
	memset(region->y, 0, sizeof(float) * n);
	region->count = n;
	
	return region;
}

vot_region* vot_region_copy(const vot_region* region)
{
	vot_region* copy = vot_region_create(region->count);
	
	int i;
	for (i = 0; i < region->count; i++) {
		copy->x[i] = region->x[i];
		copy->y[i] = region->y[i];
	}

	return copy;
}
	
#else

void vot_region_release(vot_region** region) 
{
	if (!(*region)) return;
	
	free(*region);
	
	*region = NULL;
}
	
vot_region* vot_region_create() 
{
	vot_region* region = (vot_region*) malloc(sizeof(vot_region));
	
	region->x = 0;
	region->y = 0;
	region->width = 0;
	region->height = 0;
	
	return region;
}

vot_region* vot_region_copy(const vot_region* region)
{
	vot_region* copy = vot_region_create();
	
	copy->x = region->x;
	copy->y = region->y;
	copy->width = region->width;
	copy->height = region->height;
	
	return copy;
}

#endif // VOT_POLYGON

vot_region* _parse_region(char* buffer)
{
	int i;
	float* numbers = (float*) malloc(sizeof(float) * (strlen(buffer) / 2));
	char* pch = strtok(buffer, ",");
	
	for (i = 0; ; i++) {
		if (!pch) break;
		numbers[i] = (float) atof(pch);
		pch = strtok(NULL, ",");
	}
	
	vot_region* region;

#ifdef VOT_POLYGON
	{
	    // Check if region is actually a rectangle and convert it 
	    if (i == 4) {
			region = vot_region_create(4);
			region->cout = 4;
			
			region->x[0] = numbers[0];
			region->x[1] = numbers[0] + numbers[2];
			region->x[2] = numbers[0] + numbers[2];
			region->x[3] = numbers[0];
			
			region->y[0] = numbers[1];
			region->y[1] = numbers[1];
			region->y[2] = numbers[1] + numbers[3];
			region->y[3] = numbers[1] + numbers[3];
		} else {
			int count = i / 2;
			assert(count >= 3);
			
			region = vot_region_create(count);
			
			for (i = 0; i < count; i++) {
				region->x[i] = numbers[i*2];
				region->y[i] = numbers[i*2+1];
			}
			
			region->count = count;
		}
    }
#else
	{
	    assert(i > 3);
	
	    region = vot_region_create();
	
    	// Check if the input region is actually a polygon and convert it
	    if (i > 6) {
			int j;
			float top = FLT_MAX;
			float bottom = FLT_MIN;
			float left = FLT_MAX;
			float right = FLT_MIN;
			
			for (j = 0; j < i/2; j++) {
				top = MIN(top, numbers[j * 2 + 1]);
				bottom = MAX(bottom, numbers[j * 2 + 1]);
				left = MIN(left, numbers[j * 2]);
				right = MAX(right, numbers[j * 2]);
			}
			
			region->x = left;
			region->y = top;
			region->width = right - left;
			region->height = bottom - top;
		} else {
			//region = vot_region_create();
			region->x = numbers[0];
			region->y = numbers[1];
			region->width = numbers[2];
			region->height = nmbers[3];
		}
    }
#endif // VOT_POLYGON
	
	free(numbers);
	
	return region;
}
			
		
			
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	