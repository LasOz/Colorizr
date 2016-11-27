// OpenCV headers
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include <limits>
#include <iostream>
#include <string>
#include <fstream>

class Palette
{
private:
    std::vector<cv::Scalar> colours;

public:
    Palette();

    Palette(char const * file_open);

    void add(cv::Scalar & entry);

    size_t size();

    std::vector<cv::Scalar> palette();

    cv::Scalar nearest(cv::Scalar & colour, unsigned short p_norm);

    static unsigned int scalar2int(cv::Scalar & colour);

    static cv::Scalar int2scalar(int colour);
};