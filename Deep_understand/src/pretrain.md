第一步： 
    argc > 13

第二步：
    caffe::Caffe::set_random_seed(random_seed); 
	
第三步：
     CPU_ONLY 或 GPU 的判断

第四步： Load the ImageNet data.
     LoaderImagenetDet image_loader(images_folder_imagenet, annotations_folder_imagenet);
	 const std::vector<std::vector<Annotation> >& train_images = image_loader.get_images();
	 
第五步： Create an example generator.
	 ExampleGenerator example_generator(lambda_shift, lambda_scale,
                                        min_scale, max_scale);
     
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
	 
	 
第六步： 创建 Regressor
      RegressorTrain regressor_train(deploy_proto, caffe_model, mean_file,
                                   gpu_id, solver_file);
	 
	 6.1 RegressorTrain::RegressorTrain(const std::string& deploy_proto,
							   const std::string& caffe_model,
							   const int gpu_id,
							   const string& solver_file,
							   const bool do_train)
	: Regressor(deploy_proto, caffe_model, gpu_id, kNumInputs, do_train),
      RegressorTrainBase(solver_file)
    {
	    solver_.set_net(net_);
    }
	 
	6.2 regressor_train.set_test_net(test_proto); 
	 void RegressorTrain::set_test_net(const std::string& test_proto)
    {
	    printf("Setting test net to: %s\n", test_proto.c_str());
	    test_net_.reset(new caffe::Net<float>(test_proto, caffe::TEST));
	    solver_.set_test_net(test_net_);
    }

	 
	 
	 
第七步： 创建 Tracker Trainer
       TrackerTrainer tracker_trainer(&example_generator, &regressor_train);
	 
第八步： Train on a random image.
     for (int i = 0; i < kNumIters; ++i) {
         preTrainImage(image_loader, train_images, &tracker_trainer);
     }        
	 
	 8.1 void preTrainImage(const LoaderImagenetDet& image_loader,
				   const std::vector<std::vector<Annotation> >& images,
				   TrackerTrainer* tracker_trainer)
	 
	 8.2 // Load the image.
	    cv::Mat image;
	    image_loader.getImage(image_num, &image);
	 
	 8.3 // Train on this example
	     tracker_trainer->train(image, image, bbox, bbox);
	    
第九步： TrackerTrainer 详解
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















