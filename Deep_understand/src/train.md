第一步： 
    argc > 13

第二步：
    caffe::Caffe::set_random_seed(random_seed); 
	
第三步：
     CPU_ONLY 或 GPU 的判断

第四步： Load the ImageNet data.
     LoaderImagenetDet image_loader(images_folder_imagenet, annotations_folder_imagenet);
	 const std::vector<std::vector<Annotation> >& train_images = image_loader.get_images();

第五步： Load the video data.
     LoaderAlov alov_video_loader(alov_videos_folder, alov_annotations_folder);
     const bool get_train = true;
     std::vector<Video> train_videos;
     alov_video_loader.get_videos(get_train, &train_videos);
	 
第六步：Create an ExampleGenerator to generate training examples.
      ExampleGenerator example_generator(lambda_shift, lambda_scale,
                                     min_scale, max_scale);

第七步： Set up network.
    RegressorTrain regressor_train(train_proto, caffe_model,
                                 gpu_id, solver_file);

第八步： Set up trainer.
     TrackerTrainer tracker_trainer(&example_generator, &regressor_train);

第九步： Train tracker.
     // Train on an image example.
     1 train_image(image_loader, train_images, &tracker_trainer);
	  
	 // Train on a video example.
     2 train_video(train_videos, &tracker_trainer);
	 
第十步 Train 详解

1 Train image

// Train on a random image.
void train_image(const LoaderImagenetDet& image_loader,
           const std::vector<std::vector<Annotation> >& images,
           TrackerTrainer* tracker_trainer) {
  // Get a random image.
  const int image_num = rand() % images.size();
  const std::vector<Annotation>& annotations = images[image_num];

  // Choose a random annotation.
  const int annotation_num = rand() % annotations.size();

  // Load the image with its ground-truth bounding box.
  cv::Mat image;
  BoundingBox bbox;
  image_loader.LoadAnnotation(image_num, annotation_num, &image, &bbox);

  // Train on this example
  tracker_trainer->Train(image, image, bbox, bbox);
}


2 Train video
// Train on all annotated frames in the set of videos.
void train_video(const std::vector<Video>& videos, TrackerTrainer* tracker_trainer) {
  // Get a random video.
  const int video_num = rand() % videos.size();
  const Video& video = videos[video_num];

  // Get the video's annotations.
  const std::vector<Frame>& annotations = video.annotations;

  // We need at least 2 annotations in this video for this to be useful.
  if (annotations.size() < 2) {
    printf("Error - video %s has only %zu annotations\n", video.path.c_str(),
           annotations.size());
    return;
  }

  // Choose a random annotation.
  const int annotation_index = rand() % (annotations.size() - 1);

  // Load the frame's annotation.
  int frame_num_prev;
  cv::Mat image_prev;
  BoundingBox bbox_prev;
  video.LoadAnnotation(annotation_index, &frame_num_prev, &image_prev, &bbox_prev);

  // Load the next frame's annotation.
  int frame_num_curr;
  cv::Mat image_curr;
  BoundingBox bbox_curr;
  video.LoadAnnotation(annotation_index + 1, &frame_num_curr, &image_curr, &bbox_curr);

  // Train on this example
  tracker_trainer->Train(image_prev, image_curr, bbox_prev, bbox_curr);

  // Save
  frame_num_prev = frame_num_curr;
  image_prev = image_curr;
  bbox_prev = bbox_curr;
}
  

第十一步： TrackerTrainer 详解 
    void TrackerTrainer::Train(const cv::Mat& image_prev, const cv::Mat& image_curr,
                           const BoundingBox& bbox_prev, const BoundingBox& bbox_curr)
	 
	9.1 Set up example generator.
	example_generator_->Reset(bbox_prev,
							  bbox_curr, 
							  image_prev,
							  image_curr);
		
	9.2 Make training examples. 
	    MakeTrainingExamples(&images, &targets, &bboxes_gt_scaled);
	
	    void TrackerTrainer::MakeTrainingExamples(std::vector<cv::Mat>* images,
										  std::vector<cv::Mat>* targets,
										  std::vector<BoundingBox>* bboxes_gt_scaled)
	    {
	        // Generator true example
	        cv::Mat image;
	        cv::Mat target;
	        BoundingBox bbox_gt_scaled;
	        example_generator_->MakeTrueExample(&image, &target, &bbox_gt_scaled);
	        images->push_back(image);
	        targets->push_back(target);
	        bboxes_gt_scaled->push_back(bbox_gt_scaled);
	
	        // Generate additional training examples through synthetic transformations.
            example_generator_->MakeTrainingExamples(kGeneratedExamplesPerImage, images,
                                              targets, bboxes_gt_scaled);
        }
	 
	 9.2.1   Focus the current image at the location of the previous frame's bounding box (true motion).
	    example_generator_->MakeTrueExample(&image, &target, &bbox_gt_scaled);
	    
		void ExampleGenerator::MakeTrueExample(cv::Mat* curr_search_region,
									   cv::Mat* target_pad,
									   BoundingBox* bbox_gt_scaled) const
		{
	        *target_pad = target_pad_;
	
	        // Get a tight prior prediction of the target's location in the current image,
	        // For simplicity, we use the object's previous location as out guess.
	        // TODO - use a motion model ?
	        const BoundingBox& curr_prior_tight = bbox_prev_gt_;
	
	        // Crop the current image based on the prior estimate, with some padding
	        // To define a search region within the current image.
	        BoundingBox curr_search_location;
	        double edge_spacing_x, edge_spacing_y;
	        CropPadImage(curr_prior_tight, image_curr_, curr_search_region, 
				 &curr_search_location, &edge_spacing_x, &edge_spacing_y);
	
	        // Recenter the ground-truth bbox relative to the search location.
	        BoundingBox bbox_gt_recentered;
	        bbox_curr_gt_.Recenter(curr_search_location, edge_spacing_x, edge_spacing_y, 
						   &bbox_gt_recentered);
	
	        // Scale the bounding box relative to current crop.
	        bbox_gt_recentered.Scale(*curr_search_region, bbox_gt_scaled);
       }
	 
	 9.2.2 Make batch_size training examples according to the input parameters.
	       void MakeTrainingExamples(const int num_examples, std::vector<cv::Mat>* images,
	                          std::vector<cv::Mat>* targets,
							  std::vector<BoundingBox>* bboxes_gt_scaled);
           {

               // Make training example by synthetically shifting and scaling the image,
		       // creating an apparent translation and scale changeof the target object
		       MakeTrainingExampleBBShift(&image_rand_focus, &target_pad, &bbox_gt_scaled);
            }
	
	9.3 void TrackerTrainer::Train(const cv::Mat& image_prev, const cv::Mat& image_curr,
                           const BoundingBox& bbox_prev, const BoundingBox& bbox_curr)

    9.4 void RegressorTrain::Train(const std::vector<cv::Mat>& images,
						   const std::vector<cv::Mat>& targets,
						   const std::vector<BoundingBox>& bboxes_gt) 
		{
	         assert(net_->phase() == caffe::TRAIN);
	
	         if (images.size() != targets.size()) {
		        printf("Error - %zu images but %zu targets\n", images.size(), targets.size());
	         }
	
	         if (images.size() != bboxes_gt.size()) {
		        printf("Error - %zu images but %zu bboxes_gt", images.size(), bboxes_gt.size());
	         }
	
	         // Normally to track we just estimate the bbox location; if we need to backprop,
             // we also need to input the ground-truth bounding boxes.
             set_bboxes_gt(bboxes_gt);
	
         	// Set the image and target.
	        SetImages(images, targets);

            // Train the network.
	        Step();
    }






















