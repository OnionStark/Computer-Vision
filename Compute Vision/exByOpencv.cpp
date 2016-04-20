#include"exByOpencv.h"
#include"ReadCfg.h"
#include"general.h"

//不知道为什么会触发一个神奇的bug，却是和vc 的版本相关…………
//这个bug 没法de，我暂时认输了

void showImg(string name, Mat& img, bool is = false)
{

	if (is)
	{
		if (img.rows > 1024 || img.cols > 1024)
			namedWindow(name, WINDOW_NORMAL);
		else
			namedWindow(name, WINDOW_AUTOSIZE);
		imshow(name, img);
	}
	string path = "img/result/";
	path += string(name)+".jpg";
	imwrite(path, img);
}

string getname(string path)
{
	int i = path.find_first_of('/')+1;
	int index = path.find_last_of('.');
	return path.substr(i, index-i);
}

void sift()
{
	//从文件中读入图像
	if (ReadCfg::data.size() < 2)
	{
		cerr << "输入的图像参数不够，请补充图像的路径名（in img.cfg)" << endl;
		return;
	}
	Mat input1 = imread(ReadCfg::data[0].path, 1);
	Mat input2 = imread(ReadCfg::data[1].path, 1);
	if (!input1.data || !input2.data)
	{
		cerr << "error input" << endl;
		return;
	}

	SiftFeatureDetector detector;
	vector<KeyPoint> keypoint1, keypoint2;
	Mat output1;
	Mat output2;
	SiftDescriptorExtractor extractor;
	Mat descriptor1, descriptor2;
	BruteForceMatcher<L2<float>> matcher;

	vector<DMatch> matches;
	Mat img_matches;

	detector.detect(input1, keypoint1);//有问题


	drawKeypoints(input1, keypoint1, output1);
	showImg(getname(ReadCfg::data[0].path)+"SIFT特征点", output1);

	detector.detect(input2, keypoint2);
	drawKeypoints(input2, keypoint2, output2);

	showImg(getname(ReadCfg::data[1].path) + "SIFT特征点", output2);

	extractor.compute(input1, keypoint1, descriptor1);
	extractor.compute(input2, keypoint2, descriptor2);

	matcher.match(descriptor1, descriptor2, matches);

	drawMatches(input1, keypoint1, input2, keypoint2, matches, img_matches);
	showImg(getname(ReadCfg::data[0].path) + getname(ReadCfg::data[1].path) + "sift匹配",
		img_matches, true);

	waitKey();
}


void surf()
{
	Mat img_object = imread(ReadCfg::data[0].path, CV_LOAD_IMAGE_GRAYSCALE);
	Mat img_scene = imread(ReadCfg::data[1].path, CV_LOAD_IMAGE_GRAYSCALE);

	if (!img_object.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return;
	}
	Mat img_ouput1, img_ouput2;
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints_object, keypoints_scene;

	detector.detect(img_object, keypoints_object);
	detector.detect(img_scene, keypoints_scene);
	drawKeypoints(img_object, keypoints_object, img_ouput1);
	drawKeypoints(img_scene, keypoints_object, img_ouput2);
	showImg(getname(ReadCfg::data[0].path) + "SURF特征点", img_ouput1);
	showImg(getname(ReadCfg::data[1].path) + "SURF特征点", img_ouput2);

	//-- Step 2: Calculate descriptors (feature vectors)
	SurfDescriptorExtractor extractor;

	Mat descriptors_object, descriptors_scene;

	extractor.compute(img_object, keypoints_object, descriptors_object);
	extractor.compute(img_scene, keypoints_scene, descriptors_scene);

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, CV_RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	//-- Show detected matches
	showImg(getname(ReadCfg::data[0].path) + getname(ReadCfg::data[1].path) + "SUFT匹配",
		img_matches, true);

	//waitKey(0);
}

void orb()
{
	Mat img_1 = imread(ReadCfg::data[0].path);
	Mat img_2 = imread(ReadCfg::data[1].path);
	if (!img_1.data || !img_2.data)
	{
		cerr << "error input!" << endl;
		return;
	}

	// -- Step 1: Detect the keypoints using STAR Detector 
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	ORB orb;
	orb.detect(img_1, keypoints_1);
	orb.detect(img_2, keypoints_2);

	Mat ouput1, ouput2;
	drawKeypoints(img_1, keypoints_1, ouput1);
	drawKeypoints(img_2, keypoints_2, ouput2);
	showImg(getname(ReadCfg::data[0].path) + "ORB特征点", ouput1);
	showImg(getname(ReadCfg::data[1].path) + "ORB特征点", ouput1);

	// -- Stpe 2: Calculate descriptors (feature vectors) 
	Mat descriptors_1, descriptors_2;
	orb.compute(img_1, keypoints_1, descriptors_1);
	orb.compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors with a brute force matcher 
	BruteForceMatcher<L2<float>> matcher;
	vector<DMatch> mathces;
	matcher.match(descriptors_1, descriptors_2, mathces);
	// -- dwaw matches 
	Mat img_mathes;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, mathces, img_mathes);
	// -- show 
	showImg(getname(ReadCfg::data[0].path) + getname(ReadCfg::data[1].path) + "orb匹配",
		img_mathes, true);

	waitKey(0);
}

void fast()
{
	Mat temp1 = imread(ReadCfg::data[0].path);
	Mat temp2 = imread(ReadCfg::data[1].path);
	if (!temp1.data || !temp2.data)
	{
		cerr << "error input!" << endl;
		return;
	}

	Mat image1, image2;
	if (temp1.channels() == 3)
	{
		converToGray(image1, temp1);
	}
	else image1 = temp1;
	if (temp2.channels() == 3)
	{
		converToGray(image2, temp2);
	}
	else image2 = temp2;
	vector<KeyPoint> key1, key2;

	FastFeatureDetector fast(40);//阈值=40   
	fast.detect(image1, key1);//检测特征点
	fast.detect(image2, key2);//检测特征点

	Mat ouput1, ouput2;
	//画出特征点
	drawKeypoints(image1, key1, ouput1);
	drawKeypoints(image2, key2, ouput2);
	showImg(getname(ReadCfg::data[0].path) + "FAST特征点", ouput1);
	showImg(getname(ReadCfg::data[1].path) + "FAST特征点", ouput2);

	//计算描述符
	SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	extractor.compute(image1, key1, descriptors1);
	extractor.compute(image2, key2, descriptors2);

	//匹配
	BruteForceMatcher<L2<float>> matcher;
	std::vector<DMatch> mathces;
	matcher.match(descriptors1, descriptors2, mathces);
	// -- dwaw matches 
	Mat img_mathes;
	drawMatches(image1, key1, image2, key2, mathces, img_mathes);
	// -- show 
	showImg(getname(ReadCfg::data[0].path) + getname(ReadCfg::data[1].path) + "fast匹配",
		img_mathes, true);
	waitKey(0);
}