#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <vector>

#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class VOTRegion;

// Represents a bounding box on an image, with some additional functionality.
class BoundingBox{	
	public:
	    BoundingBox();
		BoundingBox(const std::vector<float>& bounding_box);
		
}










































#endif