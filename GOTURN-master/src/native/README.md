
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
	
******************************** trax.h **************************
**** trax_image ****
# A trax image data struct structure, at the moment only the path format is supported .
# so a lot of fields are empty 
# Use trax image_* function to access the data

  typedef struct trax_image {
    short type;
    int width;
    int height;
    char* data;
  } trax_image;

**** trax_region ****
# A placeholder for region structure. Use the trax_region_* function to manipulate the data
  
  typedef void trax_region;

**** trax_properties ****
# A placeholder for properties structure. Use the trax_properties_* function to manipulate the data
  
  typedef struct trax_properties trax_properties;

**** trax_configuration ****
# Some basic configuration data used to set up the server
  
  typedef struct trax_configuration{
    int format_region;
    int format_image;
  } trax_configuration;

**** trax_handle ****
# Core object of the protocol. Do not manipulate it directly    
  
  typedef struct trax_handle{
    int flags;
    int version;
    void *stream;
    trax_properties* properties;
    FILE* log;
    trax_configuration config;
  } trax_handle;  
  

******************************************************************
/**
    client 
        |
		| # Setups the protocol state object for the client and results a handle object.	
	    -- trax_client_setup_file
        |
        |
        -- trax_client_setup_scoket
        |
	    |
		-- trax_client_wait
		|
		|
		-- trax_client_initialize
		|
		|
		-- trax_client_frame
	    |
		|
	server		
	    -- trax_server_setup
		|
		|
		-- trax_server_setup_file
        |
		|
		-- trax_server_wait
		|   
		|
		-- trax_server_rely
		|
		|
	client and server
        -- trax_cleanup
        |
        |
        -- trax_set_parameter
        |
        |
        -- trax_get_parameter
        |
        |
	image
        -- trax_image_release
        |
        |		
        -- trax_image_creat_path
		|
		|
		-- trax_image_get_path
        |
        |
    region 
        -- trax_region_release
        |
        |
        -- trax_region_get_type
        |
        |
        --trax_region_create_special
        |
        |
        -- trax_region_set_special	
        |
		| 
		-- trax_region_get_special
		|
		|
	rectangle
		-- trax_region_create_rectangle
		|
		| 
		-- trax_region_set_rectangel
		|
		|
		-- trax_region_get_rectangle
		|
		|
	polygon
	    -- trax_region_creat_polygon
		|
		|
		-- trax_region_get_polygon_point
		|
		|
		-- trax_region_set_polygon_point
		|
		|
		-- trax_region_get_polygon_count
		|
		|
	bounds
	    -- trax_region_get_bounds
	    |
		|
	properties
	    -- trax_properties_release
		|
		|
		-- trax_properties_clear
		|
		|
		-- trax_properties_creat
		|
		|
		-- trax_properties_set
		|
		|
		-- trax_properties_get
		|
		|
		-- trax_properties_enumerate
			
**/	

******************************* vot.h ******************************

**** vot_region ****
  #ifdef VOT_POLYGON
  
  typedef struct vot_region {
  	float* x;
  	float* y;
  	int count;
  } vot_region;
  
  #else
  typedef struct vot_region{
  	float x;
  	float y;
  	float width;
  	float height;
  } vot_region;
  
  #endif // VOT_POLYGON

**** vot_region_creat ****
  vot_region* _parse_region(char* buffer);
  void vot_region_release(vot_region** region);
  vot_region* vot_region_copy(const vot_region* region);

  #ifdef VOT_POLYGON
    vot_region* vot_region_create(int n);
  #else
    vot_region* vot_region_create();
  #endif
   
**** class VOTRegion ****
  #ifdef VOT_POLYGON
    void set(int i, float x, float y);
    float get_x(int i) const;
    float get_y(int i) const
  #else
    float get_x() const;
    float get_y() const;
    float get_width();
    float get_height();
    
    float set_x(float x);
    float set_y(float y);
    float set_width(float width);
    float set_height(float height);
  #endif // VOT_POLYGON
  
  protected:
    vot_region* _region;  
	
**** VOT ****
public:
  const VOTRegion region();
  void report(const VOTRegion& region);
  const string frame();
  bool end();
 
protected:
  vot_region* vot_initialize()
  void vot_quit()
  const char* vot_frame();
  void ct_report(vot_region* region);
  int vot_end();
  vot_region* _region;
  
  // Current position in the sequence
  int _vot_sequence_position;
  // Size of the sequence
  int _vot_sequence_size;
  // List of image file names
  //char** _vot_sequence = NULL;
  char** _vot_sequence; //DH
  // List of results
  vot_region** _vot_result; //DH
 
********************************************************************
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
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	