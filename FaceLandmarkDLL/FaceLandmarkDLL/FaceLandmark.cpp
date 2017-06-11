#include "FaceLandmark.h"



FaceLandmark::FaceLandmark(std::string imageName)
{
	// We need a face detector.  We will use this to get bounding boxes for
	// each face in an image.
	faceDetector = get_frontal_face_detector();
	// And we also need a shape_predictor.  This is the tool that will predict face
	// landmark positions given an image and face bounding box.
	deserialize("ShapePredictor.dat") >> shapePredictor;
	dlib::load_image(image, imageName + ".jpg");

}


FaceLandmark::FaceLandmark(cv::Mat imageMat)
{
	// We need a face detector.  We will use this to get bounding boxes for
	// each face in an image.
	faceDetector = get_frontal_face_detector();
	// And we also need a shape_predictor.  This is the tool that will predict face
	// landmark positions given an image and face bounding box.
	deserialize("ShapePredictor.dat") >> shapePredictor;

	//Convert cvMat to arrady2d
	dlib::assign_image(image, dlib::cv_image<bgr_pixel>(imageMat));


}

FaceLandmark::~FaceLandmark()
{
}



void FaceLandmark::detect_face_and_features()
{
	// Now tell the face detector to give us a list of bounding boxes
	// around all the faces in the image.
	std::vector<rectangle> dets = faceDetector(image);
	std::cout << "Number of faces detected: " << dets.size() << std::endl;
	// Now we will go ask the shape_predictor to tell us the pose of
	// each face we detected.

	for (unsigned long j = 0; j < dets.size(); ++j)
	{
		full_object_detection shape = shapePredictor(image, dets[j]);

		// rysowanie wyznaczonych punktów charakterystycznych
		for (unsigned long k = 0; k < shape.num_parts(); k++)
		{
			dlib::point p = shape.part(k);
			draw_solid_circle(image, p, 2, dlib::rgb_pixel(0, 255, 255));
		}


		std::cout << "number of parts: " << shape.num_parts() << std::endl;
		std::cout << "pixel position of first part:  " << shape.part(0) << std::endl;
		std::cout << "pixel position of second part: " << shape.part(1) << std::endl;
		// You get the idea, you can get all the face part locations if
		// you want them.  Here we just store them in shapes so we can
		// put them on the screen.
		faces.push_back(shape);
	}

}

void FaceLandmark::show_detected_faces()
{
	win.clear_overlay();
	win.set_image(image);
	win.add_overlay(render_face_detections(faces));
}

void FaceLandmark::show_face_chips()
{
	dlib::array<array2d<rgb_pixel> > face_chips;
	extract_image_chips(image, get_face_chip_details(faces), face_chips);
	winFaces.set_image(tile_images(face_chips));

}

void FaceLandmark::save_processed_file(std::string fileName)
{
	//by³o save_bmp
	save_jpeg(image, fileName + ".jpg");
}

dlib::point FaceLandmark::extract_specified_point_from_detected_landmarks(int pointNumber)
{
	return  faces[0].part(pointNumber);
}

dlib::point FaceLandmark::get_righteye_point()
{
	if (faces.empty())
	{
		std::cout << "You need to detect face first use detect_face methood on loaded image" << std::endl;
		return dlib::point(0, 0);
	}
	else
	{
		dlib::vector<double, 2> rightEyePoints;
		double cnt = 0;
		// Find the center of the right eye by averaging the points around 
		// the eye. Iterations are based on face model where points are numered(see points_onFace image)
		for (unsigned long i = 36; i <= 41; ++i)
		{
			rightEyePoints += faces[0].part(i);
			++cnt;
		}
		//rightEyePoints /= cnt;
		dlib::dpoint rightEyeAveragePosition = rightEyePoints / cnt;

		return rightEyeAveragePosition;

	}






}

dlib::point FaceLandmark::get_lefteye_point()
{
	if (faces.empty())
	{
		std::cout << "You need to detect face first _ use detect_face methood on loaded image" << std::endl;
		return dlib::point(0, 0);
	}
	else
	{
		dlib::vector<double, 2> leftEyePoints;
		double cnt = 0;
		// Find the center of the left eye by averaging the points around 
		// the eye. Iterations are based on face model where points are numered(see points_onFace image)
		for (unsigned long i = 42; i <= 47; ++i)
		{
			leftEyePoints += faces[0].part(i);
			++cnt;
		}
		//leftEyePoints /= cnt;
		dlib::dpoint leftEyeAveragePosition = leftEyePoints / cnt;

		return leftEyeAveragePosition;

	}



}

double FaceLandmark::calculate_face_width()
{
	if (faces.empty())
	{
		std::cout << "You need to detect face first _ use detect_face methood on loaded image" << std::endl;
		return 1;
	}
	else
	{
		return length(faces[0].part(16) - faces[0].part(0));
	}

}

double FaceLandmark::calculate_temple_distance()
{
	if (faces.empty())
	{
		std::cout << "You need to detect face first _ use detect_face methood on loaded image" << std::endl;
		return 1;
	}
	else
	{
		return length(faces[0].part(26) - faces[0].part(17));
	}

}

std::vector<double> FaceLandmark::calculate_eye_nose_distance()
{
	std::vector<double> calculatedDistances;
	double rightEyeToNoseDistance = length(faces[0].part(27) - get_righteye_point());
	double leftEyeToNoseDistance = length(faces[0].part(27) - get_lefteye_point());
	calculatedDistances.push_back(rightEyeToNoseDistance);
	calculatedDistances.push_back(leftEyeToNoseDistance);

	return calculatedDistances;
}


