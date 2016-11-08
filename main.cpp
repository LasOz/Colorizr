// OpenCV headers
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include <string.h>
#include <unordered_map>
#include "tinyfiledialogs.h"

void extra_filters(cv::Mat &image, char const * folder);

int get_levels(cv::Mat &input);

void palette_reduce(cv::Mat &input, cv::Mat &output, cv::Mat &palette, int max_colours);

int main(int argc, char *argv[])
{
    tinyfd_messageBox("Before we begin...", "You should see a success message, if not then something went badly wrong...", "ok", "info", 1);

    char const * lFilterPatterns[2] = { "*.jpg", "*.png" };
    char const * file_open = tinyfd_openFileDialog("Choose your image", "", 2, lFilterPatterns, "Image files (.jpg, .png)", false);
    char const * folder = tinyfd_selectFolderDialog("Choose folder to save to", "");

    if (!file_open || !folder)
        tinyfd_messageBox("Closing", "The program did not complete successfully, and will now close.", "ok", "info", 1);

    cv::Mat im_gray = cv::imread(file_open, cv::IMREAD_GRAYSCALE);
    cv::Mat im_color;

    char * effect_name[12] = {"Sunset\0", "Platinum\0", "Infrared\0", "Underwater\0", "Nightmare\0", "Midnight\0", "Gameboy\0", "Bonbon\0", "Frozen\0", "Radioactive\0", "Retro\0", "Fire\0"};

    if (!im_gray.empty())
        for (int i = 0; i < 12; i++)
        {
            cv::applyColorMap(im_gray, im_color, i);
            std::stringstream name;
            name << folder << "//" << effect_name[i] << ".png";
            if (!cv::imwrite(name.str(), im_color))
                tinyfd_messageBox("Critical error!", "Something went wrong when trying to save", "ok", "error", 1);
        }
    else
        tinyfd_messageBox("Bad file name!", "Could not read image, due to incorrect file name", "ok", "error", 1);

    im_color = cv::imread(file_open, cv::IMREAD_COLOR);
    extra_filters(im_color, folder);

    //Forgive this concatenation through conversion. Getting undefined behaviour with strcat
    std::string s1("Pictures saved successfully to:\n");
    s1 += std::string(folder);
    tinyfd_messageBox("Saved", s1.c_str(), "ok", "info", 1);

    return EXIT_SUCCESS;
}

void extra_filters(cv::Mat & image, char const * folder)
{
    cv::Mat output, palette_mat;
    cv::resize(image, image, cv::Size(256, 240), 0, 0, cv::INTER_NEAREST);
    std::cout << get_levels(image) << std::endl;
    //palette_reduce(image, output, palette_mat, 25);
}

int get_levels(cv::Mat & input)
{
    int colors = 0;
    std::unordered_map<cv::Vec3b, bool> table;
    for (int j = 0; j < input.rows; ++j)
        for (int i = 0; i < input.cols; ++i)
        {
            if (table[input.at<cv::Vec3b>(j, i)])
            {
                table.insert(std::pair<cv::Vec3b,bool>(input.at<cv::Vec3b>(j, i),true));
                colors++;
            }
        }
    return colors;
}

void palette_reduce(cv::Mat & input, cv::Mat & output, cv::Mat & palette, int max_colours)
{
    //Count colours in original image
    //If greater than max group similar colours together. (k-mediods? / Heirachical with complete linkage?)
    //Use the mediods to change the colour of the image to the nearest mediod.
    //Replace each colour with its nearest neighbour to the palette
}