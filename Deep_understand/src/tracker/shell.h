tracker 工作方式

第一步： Initialize 模型

   1.1 void Tracker::Init(const std::string& image_curr_path, const VOTRegion& region,
                   RegressorBase* regressor) 
      {
         // Read the given image.
         const cv::Mat& image = cv::imread(image_curr_path);

         // Convert the VOT region into a bounding box.
         BoundingBox bbox_gt(region);

         // Initialize the tracker.
         Init(image, bbox_gt, regressor);
      }
     
    1.2 Initialize tracker.
        void Tracker::Init(const cv::Mat& image, const BoundingBox& bbox_gt,
                   RegressorBase* regressor) 
        {
            image_prev_ = image;
			bbox_prev_tight_ = bbox_gt;
			bbox_curr_prior_tight_ = bbox_gt;
			...
			regressor->Init();
		}
    
    1.3 Initialize regressor 
	    // Called at the beginning of tracking a new object to initialize the network.
        void Regressor::Init() 
		{ 
			if (modified_params_ ) {
			  ...
			  net_->CopyTrainedLayersFrom(caffe_model_);
			  ...
			}
		}

第二步： Define the work of Tracker
     
	  1.1 void Tracker::Track(const cv::Mat& image_curr, RegressorBase* regressor,
                    BoundingBox* bbox_estimate_uncentered) 
	  {
	       // Get target from previous image.
           cv::Mat target_pad;
           CropPadImage(bbox_prev_tight_, image_prev_, &target_pad);
		   
		   // Crop the current image based on predicted prior location of target.
		   ...
		   CropPadImage(bbox_curr_prior_tight_, image_curr, &curr_search_region, 
			   &search_location, &edge_spacing_x, &edge_spacing_y);
		   
		   // Estimate the bounding box location of the target, centered and scaled relative 
		   // to the cropped image.
		   BoundingBox bbox_estimate;		   
		   regressor->Regress(image_curr, curr_search_region, target_pad, &bbox_estimate);		   
		   ....
	   }
       
       1.2 regressor Regress
	       void Regressor::Regress(const cv::Mat& image_curr,
                        const cv::Mat& image, const cv::Mat& target,
                        BoundingBox* bbox)
		   {
			   ...
			   // Estimate the bounding box location of the target object in the current image.
			   std::vector<float> estimation;
			   Estimate(image, target, &estimation);
			   
			   ....
		   }

        1.3 Estimate
		    void Regressor::Estimate(const cv::Mat& image, 
						 const cv::Mat& target, std::vector<float>* output)
			{
				...
					
			    // Process the inputs so we can set them
				...
				WrapInputLayer(&target_channels, &image_channels);
				
				// Set the inputs to the network.
				Preprocess(image, &image_channels);
				Preprocess(target, &target_channels);
				
				// Perform a forward-pass in the network.				
				net_->ForwardPrefilled();
				
				// Get the network output.
				GetOutput(output);
			}
			
















