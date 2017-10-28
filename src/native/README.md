
trax.h
    |
	-- API to help deal image through trax image data structure and trax region to manipulate
	|
	-- trax_handle
	|
	-- trax_image_deal
	|
	--trax_image_release
	|
	-- trax_region_ploygon
	|
	-- trax_region_rectangle
	|
	-- trax_properties_release
	
vot.h
    |
	-- including Opencv core to enable better Opencv support
	|
	-- Two manner to deal the image and frame of video
    |   |    
	|	-- VOT_RECTANGLE
	|   |    
	|	-- VOT_POLYGON
	|
	-- include class: VOT and VOTRegion
	|    |
	|    -- vot_initialize()
	|    |
	|    -- vot_report()
	|    |
	|    -- vot_quit()
	|    |
	|    -- vot_frame()
	|    |
	|    -- vot_end()
	|
	-- VOT_PREFIX(FUN) VOT::FUN

vot.cpp
    |
	-- VOT_POLYGON / VOT_RECTANGLE
	|   |
	|	-- vot_region_release
	|	|
	|	-- vot_region_creat
	|	|
	|	--vot_region_copy
	|   |
	|   -- _prase_region
	|   
	--  class: VOT
	    |
	    --VOT_PREFIX(vot_initialize) / ..
	
ncc.cpp
    |
	--include "vot.h"
	|
	-- read frame and image by vot
	|
    -- class NCCTracker
	|   |
	|	-- init()
	|	|
	|	-- track
    |	
    -- main()	
	
static.cpp
    |
    -- include "vot.h"
    |
    -- vot.report()	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	