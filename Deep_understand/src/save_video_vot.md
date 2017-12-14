
第一步：
     argc > 6

第二步：
     boost::filesystem::create_directories(output_folder);

第三步： 创建 Regressor regressor(test_proto, caffe_model, gpu_id, do_train);
    Regressor::Regressor(const string& deploy_proto,
					 const string& caffe_model,
					 const int gpu_id,
					 const bool do_train)
	: num_inputs_(kNumInputs),
      caffe_model_(caffe_model),
      modified_params_(false)
    {
	    SetupNetwork(deploy_proto, caffe_model, gpu_id, do_train);
    }
    
    void Regressor::SetupNetwork(const string& deploy_proto,
							 const string& caffe_model,
							 const int gpu_id,
							 const bool do_train) 
   {
	
#ifdef CPU_ONLY
	printf("Setting up Caffe in CPU model\n");
	caffe::Caffe::set_model(caffe::Caffe::CPU);
#else
	printf("Setting up Caffe in GPU model with ID: %d\n", gpu_id);
	caffe::Caffe::SetDevice(gpu_id);
	caffe::Caffe::set_mode(caffe::Caffe::GPU);
#endif
	
	if (do_train) {
		printf("Setting phase to train\n");
		net_.reset(new Net<float>(deploy_proto, caffe::TRAIN));
	} else {
		printf("Setting phase to test\n");
		net_.reset(new Net<float>(deploy_proto, caffe::TEST));
	}
	
	if (caffe_model != "NONE") {
		net_->CopyTrainedLayersFrom(caffe_model_);
	} else {
		printf("Not initializing network from pre-trained model\n");
	}
	
	//CHECK_EQ(net_->num_inputs(), num_inputs_) << "Network should have exactly " << num_inputs_ << " inputs.";
	CHECK_EQ(net_->num_outputs(), 1) << "Net work should have exactly one output.";
	
	Blob<float>* input_layer = net_->input_blobs()[0];
	
	printf("Network image size: %d, %d\n", input_layer->width(), input_layer->height());
	
	num_channels_ = input_layer->channels();
	CHECK(num_channels_ == 3 || num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
	input_geometry_ = cv::Size(input_layer->width(), input_layer->height());
	
	// Load the binaryproto mean file.
	SetMean();
    }

    void Regressor::SetMean() 
    {
	    // Set the mean image.
	    mean_ = cv::Mat(input_geometry_, CV_32FC3, cv::Scalar(104, 117, 123));
    }

第四步： 创建 Videos 并加载 LoaderVOT数据集
    std::vector<Video> videos;
    LoaderVOT loader(videos_folder);
    videos = loader.get_videos();
    
    其中利用 find_subfolder() 对 videos 进行处理， 得到Loader::vector<Video> videos_;

第五步： 创建 tracker
    const bool show_intermediate_output = false;
    Tracker tracker(show_intermediate_output);
    
    Tracker::Tracker(const bool show_tracking):
    show_tracking_(show_tracking)
	{}

第六步：Track all object in all videos and save the output
     const bool save_videos = true;
     TrackerTesterAlov tracker_tester(videos, save_videos, &regressor, &tracker, output_folder);    
 
     TrackerTesterAlov::TrackerTesterAlov(const std::vector<Video>& videos,
									 const bool save_videos,
									 RegressorBase* regressor, Tracker* tracker,
									 const std::string& output_folder)
	: TrackerManager(videos, regressor, tracker),
      output_folder_(output_folder),
      hrt_("Tracker"),
      total_ms_(0),
      num_frames_(0),
      save_videos_(save_videos)
	  {}
      
     TrackerManager::TrackerManager(const std::vector<Video>& videos,
							   RegressorBase* regressor, Tracker* tracker)
	: videos_(videos),
      regressor_(regressor),
      tracker_(tracker)
	{}
    
第七步： Track all
     tracker_tester.TrackAll();

         // Save tracking output and video; rcord timing.
         class TrackerTesterAlov : public TrackerManager

         void TrackerManager::TrackAll(const size_t start_video_num, const int pause_val)

第八步： Tracker 详解
       8.1 // Iterator over all videos and track the target object in each.
	       for (size_t video_num = start_video_num; video_num < videos_.size(); ++video_num)
			 
	   8.2 // Get the video.
	       const Video& video = videos_[video_num];
       
       8.3 // perform any pre-processing steps on this video.
		   VideoInit(video, video_num);

       8.4 // Get the first frame of this video with the initial ground-truth bounding box
           int first_frame;
		   cv::Mat image_curr;
		   BoundingBox bbox_gt;
		   video.LoadFirstAnnotation(&first_frame, &image_curr, &bbox_gt);
           
           8.4.1 void Video::LoadFirstAnnotation(int* first_frame, cv::Mat* image,
							    BoundingBox* box) const
		   {
			   LoadAnnotation(0, first_frame, image, box);
		   }

           8.4.2 void Video::LoadAnnotation(const int annotation_index,
						   int* frame_num,
						   cv::Mat* image,
						   BoundingBox* box) const
	
       8.5 // Initialize the tracker.
           tracker_->Init(image_curr, bbox_gt, regressor_);

           8.5.1 void Tracker::Init(const cv::Mat& image, const BoundingBox& bbox_gt,
				   RegressorBase* regressor)
		   {
			   // Initialize the neural network.
	           regressor->Init();
		   }
           
           8.5.2 // If the parameters of the network have been modified, reinitialize 
			     // the parameters to their original values.
			   void Regressor::Init() 
		       {
			     if (modified_params_) {
		           printf("Reloading new params\n");
		           net_->CopyTrainedLayersFrom(caffe_model_);
		           modified_params_ = false;
				 }
	           }
           
           8.5.3 // A neural network for the tracker must inherit from this class.
               class Regressor : public RegressorBase
			   {
			       ...
			   };
			   
			   class RegressorBase {
		         ...
			   protected:
                   boost::shared_ptr<caffe::Net<float> > net_;	 
		       };

        8.6 // Iterate over the remain frames of the video.
            cv::Mat image_curr;
			BoundingBox bbox_gt;
            bool has_annotation = video.LoadFrame(frame_num,
												  draw_bounding_box,
												  load_only_annotation,
												  &image_curr, &bbox_gt);
        8.7 // Get ready top track the object.
			SetupEstimate();
            void TrackerTesterAlov::SetupEstimate() {
                // Record the time before starting to track.
                hrt_.reset();
                hrt_.start();
            }
        
         8.8 // Track and estimate the target's bounding box location in the current image.
			 // Important: this method cann't receive bbox_gt 
			 // (The ground-truth bounding box) as an input.
			 BoundingBox bbox_estimate_uncentered;
			 tracker_->Track(image_curr, regressor_, &bbox_estimate_uncentered);
         
         8.9 // Estimate the location of the target object in the current image.
		     void Tracker::Track(const cv::Mat& image_curr, RegressorBase* regressor,
				    BoundingBox* bbox_estimate_uncentered)
		 
		 8.10 // Process the output (e.g. visualize / save results)
              ProcessTrackOutput(frame_num, image_curr, has_annotation, bbox_gt,
							   bbox_estimate_uncentered, pause_val);
        
         8.11 
			 // Called after finishing tracking a video. (Used by subclasses) 
			 void PostProcessVideo();  
             
             // Called after finishing tracking all videos. (Used by subclasses)
             void PostProcessAll() {}










