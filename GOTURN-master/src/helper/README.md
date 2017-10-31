************************** helper.h ********************************

**** Number / string conversions ****

  // Conversions from number into a string.
  std::string num2str(const int num);
  std::string num2str(const float num);
  std::string num2str(const double num);
  std::string num2str(const double num, const int decimal_places);
  std::string num2str(const unsigned int num);
  std::string num2str(const size_t num);

  // Conversions from string into a number.
  template<class T>
  T str2num(const std::string& s);

**** File IO ****

  // Find all subfolder of the given folder.
  void find_subfolders(const boost::filesystem::path& folder, std::vector<std::string>* sub_folders);

  // Find all files within a given folder that match a given regex filter.
  void find_matching_files(const boost::filesystem::path& folder, const boost::regex filter,
                         std::vector<std::string>* files);  

**** Probability ****

  // Generate a random number in (0,1)
  double sample_rand_uniform();

  // Sample from an exponential distribution.
  double sample_exp(const double lambda);

  // Sample from a Laplacian distribution, aka two-sided exponential.
  double sample_exp_two_sided(const double lambda);
  
********************************************************************

***************************** bounding_box.h ***********************

// include the vot.h
  class VOTRegion

**** class BoundingBox ****
  
  # Repersents a bounding box on an image, with some additional functionality
  class BoundingBox {
  
    // Convert bounding box into a vector format
	void GetVector(std::vector<float>* bounding_box) const;
	
	// Convert bounding box into VOTRegion format
	void GetRegion(VOtregion* region);
	
	// Print the bounding box coordinates.
	void Print() const;
	
	// Draw a rectangle corresponding to this bbox with the given color
	void Draw(const int r, const int g, const int b, cv::Mat* image) const;
	
	// Draw a white rectangle corresponding to this bbox
	void DrawBoundingBox(cv::Mat* figure_ptr) const;
	
	// Normalize the size of the bounding box based on the size of the image
	void Scale(const cv::Mat& image, BoundingBox* bbox_scaled) const;
	
	// Compute location of bounding box relative to search region
    void Recenter(const BoundingBox& search_location, 
	              const double edge_spacing_x, const double edge_spacing_y, 
				  BoundingBox* bbox_recentered) const;

    // Shift the cropped region of the image to generate a new random training example.
	void Shift(const cv::Mat& image,
	           const double lambda_scale_frac, const double lambda_shift_frac,
			   const double min_scale, const double max_scale,
			   const bool shift_motion_model,
			   BoundingBox* bbox_rand) const;
    
    // Area enclosed by the bounding box.
    double compute_area() const;

    // Area of intersection between two bounding boxes.
    double compute_intersection(const BoundingBox& bbox) const;
	
	// Bounding box coordiantes: top left, bottom right.
    double x1_, y1_, x2_, y2_;

    // Factor to scale the bounding box coordinates before inputting into the neural net.
    double scale_factor_;
  };
  
bounding_box.h
	|
    --class VOTRegion
	|
	-- class BoundingBox
	|    |
	|    | # Three manners of constructure funtion of the class:
	|    -- BoundingBox()
	|    |
	|    | # Convert bounding box into a vector format
	|    -- void GetVector(std::vector<float>* bounding_box) 
	|    |
	|    | # Convert bounding box into VOTRegion format
	|    -- void GetRegion(VOTRegion* region);
	|    |
	|    | # Print the bounding box coordinates
	|    -- void Print() const;
	|    |
	|    | # Draw a rectangle corresponding to this bbox with the given color.
	|    -- void Draw()
	|    |
	|    | # Draw a white rectangle corresponding to this bbox.
	|    -- void DrawBoundingBox(cv::Mat* figure_ptr)
	|    |
	|    | # Normalize the size of the bounding box based on the size of the image.
	|    -- void Scale()
	|    |
	|    | # Unnormalize the size of the bounding box based on the size of the image.
	|    -- void Unscale()
	|    |
	|    | # Compute location of bounding box relative to search region
	|    -- void Recenter()
	|    |
	|    | # Undo the effect of Recenter
	|    -- void Uncenter()
	|    |
	|    | # Shift the cropped region of the image to generate a new random training example
	|    -- void Shift()
	|    |
	|    -- get_scale_factor()
	|    -- get_width()
	|    -- get_height()
	|    | 
	|    | #Compute the boudning box center x and y coordinates
	|    -- get_center_x()
	|    -- get_center_y()
	|    |
	|    | # Get the size of the output image, which is equal to the bounding box with some padding
	|    -- compute_output_height()
	|    -- compute_output_width()
	|    |
	|    | # Get the amount that the output "sticks out" beyond the left and bottom edges of the image.
	     -- edge_spacing_x()
	     -- edge_spacing_y()
	     |
	     | # Area enclosed by the bounding box
	     -- compute_area()
	     |
	     | # Area of intersection between two bounding boxes
	     -- compute_intersection()
		 
********************************************************************

*********************** image_proc.h ******************************

// Functions to process images for tracking.	 
// Crop the image at the bounding box location, plus some additional padding.		 
  void CropPadImage(const BoundingBox& bbox_tight, const cv::Mat& image, cv::Mat* pad_image);
		 
  void CropPadImage(const BoundingBox& bbox_tight, const cv::Mat& image, cv::Mat* pad_image,
                  BoundingBox* pad_image_location, double* edge_spacing_x, double* edge_spacing_y);		 
		 
// Compute the location of the cropped image, which is centered on the bounding box center		 
  void ComputeCropPadImageLocation(const BoundingBox& bbox_tight, const cv::Mat& image, BoundingBox* pad_image_location);
		 
********************************************************************

************************ high_res_timer.h **************************

//A class for measuring how long some piece of code takes to run.		 

**** class HighResTimer ****		 
public:
  void start();
  void stop();
  void reset(const std::string& description);
  void reset();
  double getMicroseconds() const;
  double getMilliseconds() const;
  double getSeconds() const;
  double getMinutes() const;
  double getHours() const;

  std::string report() const;
  std::string reportMicroseconds() const;
  std::string reportMilliseconds() const;
  std::string reportSeconds() const;
  std::string reportMinutes() const;
  std::string reportHours() const;	 
  
private:
  double total_us_;
  timespec start_;
  timespec end_;
  clockid_t clock_;		 
		 
**** class ScopedTimer ****
  public:
  HighResTimer hrt_;
  ScopedTimer(const std::string& description = "ScopedTimer");
  ~ScopedTimer();
		 
		 
		 
		 
		 
		 
		 
		 
		 