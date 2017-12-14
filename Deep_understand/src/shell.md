
第一步：
  ExampleGenerator example_generator(5, 5, -0.4, 0.4);
     
     ExampleGenerator::ExampleGenerator(const double lambda_shift,
								   const double lambda_scale,
								   const double min_scale,
								   const double max_scale)
         : lambda_shift_(lambda_shift),
           lambda_scale_(lambda_scale),
           min_scale_(min_scale),
           max_scale_(max_scale)
          {
          }

第二步：生成 image_curr, target_pad_, bbox_curr_gt = bbox_prev_gt_
   //Set up the example generator with this annotation
   example_generator.Reset(bbox, bbox, image, image);

      void ExampleGenerator::Reset(const BoundingBox& bbox_prev,
							 const BoundingBox& bbox_curr,
							 const cv::Mat& image_prev,
							 const cv::Mat& image_curr)
      {
	      // Get padded target from previous image to feed the network.
	      CropPadImage(bbox_prev, image_prev, &target_pad_);
	
	      // Save the current image.
	      image_curr_ = image_curr;
	
	       // Save the current ground-truth bounding box.
	       bbox_curr_gt_ = bbox_curr;
	
	       // Save the previous ground-truth bounding box.
	       bbox_prev_gt_ = bbox_prev;
       }
       
       2.1  从 image 中生成 target_pad_
		    CropPadImage(bbox_prev, image_prev, &target_pad_);
            void CropPadImage(const BoundingBox& bbox_tight, const cv::Mat& image, 
							  cv::Mat* pad_image)
            {
	            BoundingBox pad_image_location;
	            double edge_spacing_x, edge_spacing_y;
	
	            CropPadImage(bbox_tight, image, pad_image, &pad_image_location, 
				        &edge_spacing_x, &edge_spacing_y);
            }
            
            2.1.1 利用 bbox, image生成 pad_image (target_pad_) 和 target_pad_ 的 location
			      void CropPadImage(const BoundingBox& bbox_tight, const cv::Mat& image, cv::Mat* pad_image
                                    BoundingBox* pad_image_location, double* edge_spacing_x, double* edge_spacing_y)
                 {
	                 // Crop the image based on the bounding box location, adding some padding.
	
	                 // Get the location of the cropped and padded image.
	                 ComputeCropPadImageLocation(bbox_tight, image, pad_image_location);
	
	                 ......
	                 // Set the output.
	                 *pad_image = output_image;
                  }
                  
                  2.1.1.1 由 ComputeCropPadImageLocation 函数生成 pad_image_location
				      void ComputeCropPadImageLocation(const BoundingBox& bbox_tight, const cv::Mat& image, 
								 BoundingBox* pad_image_location)
                      {
	                      ...
	
	                      // Set the output image corrdinates.
	                      pad_image_location->x1_ = roi_left;
	                      pad_image_location->y1_ = roi_bottom;
	                      pad_image_location->x2_ = roi_left + roi_width;
	                      pad_image_location->y2_ = roi_bottom + roi_height;
                      }

第三步：设置当前的视频编号和帧编号    
       // Save the image and annotation index so we can given this image a simple identifder
	   example_generator.set_indices(i, j);
       
       void set_indices(const int video_index, const int frame_index) {
           video_index_ = video_index; frame_index_ = frame_index;
       }

第四步：
      
	  cv::Mat image_rand_focus;
	  cv::Mat target_pad;
	  BoundingBox bbox_gt_scaled;
      const bool visualize = true;
      
      example_generator.MakeTrainingExampleBBShift(visualize, &image_rand_focus,
                                                     &target_pad, &bbox_gt_scaled);
      4.1
		 void ExampleGenerator::MakeTrainingExampleBBShift(
	                  const bool visualize_example, cv::Mat* image_rand_focus,
	                  cv::Mat* target_pad, BoundingBox* bbox_gt_scaled) const
	     {
		     .... 
			 // Generate training examples.
	          MakeTrainingExampleBBShift(visualize_example, default_bb_params,
							   image_rand_focus, target_pad, bbox_gt_scaled);
	     }
         
         4.1.1 首先：bbox_curr_gt_.Shift ==> bbox_curr_shift
			  void ExampleGenerator::MakeTrainingExampleBBShift(const bool visualize_example,
												  const BBParams& bbparams,
												  cv::Mat* rand_search_region,
												  cv::Mat* target_pad,
												  BoundingBox* bbox_gt_scaled) const
		      {
			      *target_pad = target_pad_;
				  // Randomly transform the current image (translation and scale changes).
	              BoundingBox bbox_curr_shift;
				  
				  1`移动 BoundingBox
				  // bbox_curr_gt_.Shift ==> bbox_curr_shift
				  bbox_curr_gt_.Shift(image_curr_, bbparams.lambda_scale, bbparams.lambda_shift,
                        bbparams.min_scale, bbparams.max_scale,
						shift_motion_model = true,
						&bbox_curr_shift);
				  ...
				  2` 根据 BoundingBox 剪切
				  // 利用 bbox_curr_shift, image_curr ==> rand_search_region 和 rand_search_location  
				  BoundingBox rand_search_location;
	              CropPadImage(bbox_curr_shift, image_curr_, rand_search_region, &rand_search_location,
				               &edge_spacing_x, &edge_spacing_y);
				  
				  ......
				  // 利用 生成的 rand_search_loaction ==> bbox_gt_recentered
				  BoundingBox bbox_gt_recentered;
	              bbox_curr_gt_.Recenter(rand_search_location, edge_spacing_x, edge_spacing_y, 
						              &bbox_gt_recentered);  
				  
				  // 利用生成的bbox_gt_recentered.Scale 对 bbox_gt_scaled 进行 scaled
				  // Scale the ground-truth bounding box relative to the random transformation.
	              bbox_gt_recentered.Scale(*rand_search_region, bbox_gt_scaled);
				  
		      }
              
              4.1.1.1 
				  









