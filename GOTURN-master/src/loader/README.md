
************************ video.h ***********************************

**** struct Frame ****
  // An image frame and corresponding annotation.
  struct Frame {
    int frame_num;
    BoundingBox bbox;   // helper/boundingbox.h
  };

**** class Video ****
  // Container for video data and the corresponding frame annotations.
  class Video {
  public:
    // For a given annotation index, get the corresponding frame number, image,
    // and bounding box.
	void LoadAnnotation(const int annotation_index, int* frame_num, cv::Mat* image,
                     BoundingBox* box) const;
	
	// Find and return the first frame with an annotation in this video.
    void LoadFirstAnnotation(int* first_frame, cv::Mat* image,
                          BoundingBox* box) const;
						  
	// For a given frame num, find an annotation if it exists, and return true.
	// Otherwise return false.
    // If load_only_annotation = false, also load the image.
    // If draw_bounding_box = true, draw the annotation on the image.
	bool LoadFrame(const int frame_num,
                const bool draw_bounding_box,
                const bool load_only_annotation,
                cv::Mat* image,
                BoundingBox* box) const;
	
	// Show video with all annotations.
    void ShowVideo() const;
	
	// Path to the folder containing the image files for this video.
    std::string path;

    // Name of all image files for this video (must be appended to path).
    std::vector<std::string> all_frames;
	
	// Bounding box annotations for a subset of frames in this video.
    // Note that the length of this vector may be different from the length of the vector of image filenames,
    // if only a strict subset of video frames were labeled.
    std::vector<Frame> annotations;
	
	private:
    // For a given frame num, find an annotation if it exists, and return true.
    // Otherwise return false.
    bool FindAnnotation(const int frame_num, BoundingBox* box) const;
  };

**** struct Category ****
  struct Category {
    std::vector<Video> videos;
  };

*******************************************************************


************************* loader_imagenet_det.h ********************

**** struct Annotation ****
  // An image annotation.
  struct Annotation {
    // Relative path of the image files (must be appended to path).
    std::string image_path;

    // Bounding box annotation for an object in this image.
    BoundingBox bbox;

    // Size of image when the annotation was performed.
    int display_width_;
    int display_height_;
  };

**** class LoaderImagenetDet ****
  // Loads images from the ImageNet object detection challenge.
  public:
    // Load all annotations.
    LoaderImagenetDet(const std::string& image_folder,
                      const std::string& annotations_folder);
	
	// Load the specified image.
    void LoadImage(const size_t image_num, cv::Mat* image) const;
    	
	// Get the annotation and the image.
    void LoadAnnotation(const size_t image_num,
                        const size_t annotation_num,
                        cv::Mat* image,
                        BoundingBox* bbox) const;
    
    // Show just the images (no annotations).
    void ShowImages() const;	
	
    // Show all of the annotations on the images.
    void ShowAnnotations() const;	
	
	// Show random images with their annotations.
    void ShowAnnotationsRand() const;
    
	// Shift the annotations and visualize them (to visualize our
    // artificially shifted training data).
    void ShowAnnotationsShift() const;
	
	// Compute statistics over bounding box sizes on this dataset.
    void ComputeStatistics() const;
	
	const std::vector<std::vector<Annotation> >& get_images() {
      return images_;
    }
	
  private:
    // Read the annotation file, convert to bounding box format, and save.
    void LoadAnnotationFile(const std::string& annotation_file,
                            std::vector<Annotation>* image_annotations);
	
	// Path to the folder containing the image files.
    std::string path_;
	
	// All annotations for all images.
  std::vector<std::vector<Annotation> > images_;
********************************************************************



*********************** video_loader.h *****************************

**** class VideoLoader ****
  public:
    VideoLoader();

    // Show all videos with the annotations.
    // Pause after each video; press any key to continue.
    void ShowVideos() const; 
    
	// Shift the annotations and visualize them (to visualize our
    // artificially shifted training data).
    void ShowVideosShift() const;
  
    std::vector<Video> get_videos() const { return videos_; }
  
  protected:
    std::vector<Video> videos_;  

********************************************************************
	
************************* loader_vot.h ****************************

**** class LoaderVOT : public VideoLoader ****
  public:
    // Loads all images and annotations.
    LoaderVOT(const std::string& vot_folder);
  
*******************************************************************

************************  loader_alov.h ***************************

**** class LoaderAlov : public VideoLoader ****
  // Loads annotations from the ALOV tracking dataset. 
  public:
    // Loads all annotations.
    LoaderAlov(const std::string& images, const std::string& annotations);
    
	// If get_train is true, get the videos in the training set; otherwise,
    // get the videos in the validation set.
    // The training videos (as well as the validation videos) are taken
    // with the same proportion from each category.
    void get_videos(const bool get_train, std::vector<Video>* videos) const;
  
  private:  
    // Make this method private to ensure it doesn't get called.
    const std::vector<Video>& get_videos() const { return videos_; }

    // The videos are organized into categories.
    std::vector<Category> categories_;
  
  
  
  
  
  