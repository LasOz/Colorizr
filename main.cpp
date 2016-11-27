// OpenCV headers
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include <string.h>
#include <unordered_map>
#include <algorithm>

#include "tinyfiledialogs.h"
#include "palette.hpp"

struct colourbin {
    cv::Scalar colour;
    unsigned int count;
};

void assign_palette(cv::Mat & image, Palette & palette, unsigned short p_norm);

bool sort(colourbin first, colourbin second) { return first.count > second.count; }

void extra_filters(cv::Mat &image, char const * folder);

std::vector<colourbin> NN_Palette_Reduction(cv::Mat &image, Palette &palette, unsigned short p_norm);

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
    std::stringstream name;
    name << folder << "//" << "test" << ".png";
    if (!cv::imwrite(name.str(), im_color))
        tinyfd_messageBox("Critical error!", "Something went wrong when trying to save", "ok", "error", 1);

    //Forgive this concatenation through conversion. Getting undefined behaviour with strcat
    std::string s1("Pictures saved successfully to:\n");
    s1 += std::string(folder);
    tinyfd_messageBox("Saved", s1.c_str(), "ok", "info", 1);

    return EXIT_SUCCESS;
}

void assign_palette(cv::Mat & image, Palette & palette, unsigned short p_norm)
{
    for (int j = 0; j < image.rows; ++j)
        for (int i = 0; i < image.cols; ++i)
        {
            cv::Vec3b image_pixel_colour = image.at<cv::Vec3b>(j, i);
            cv::Scalar nearest = palette.nearest(cv::Scalar(image_pixel_colour), p_norm);
            image.at<cv::Vec3b>(j, i)[0] = nearest[0];
            image.at<cv::Vec3b>(j, i)[1] = nearest[1];
            image.at<cv::Vec3b>(j, i)[2] = nearest[2];
        }
}

void extra_filters(cv::Mat & image, char const * folder)
{
    cv::Mat output, palette_mat;
    cv::resize(image, image, cv::Size(256, 240), 0, 0, cv::INTER_NEAREST);

    char const * lFilterPatterns[2] = { "*.csv", "*.txt" };
    char const * file_open = tinyfd_openFileDialog("Choose your palette", "", 2, lFilterPatterns, "Text files (.csv, .txt)", false);

    Palette NES(file_open);

    std::vector<colourbin> used_palette = NN_Palette_Reduction(image, NES, 2);
    // Change colours to nearest NES Counter parts (Patlette Reduce), Pixel by pixel using NN assign.
    // Return the palette that is used.

    std::sort(used_palette.begin(), used_palette.end(), sort);
    // Sort the palette from most used to least used (requires making a structure to hold a colour histogram)

    Palette new_palette;

    for (int i = 0; (i < used_palette.size()) && (i < 25); i++)
        new_palette.add(used_palette[i].colour);
    
    std::cout << "New palette size: " << new_palette.size();

    // Count colours, if >25 (of some other number) reduce by using only the 25 most used colours from previous palette
    assign_palette(image, new_palette, 2);

    // Go into each 8x8 space and reduce colours to 4

    //palette_reduce(image, output, palette_mat, 25);
}

std::vector<colourbin> NN_Palette_Reduction(cv::Mat & image, Palette & palette, unsigned short p_norm)
{
    std::unordered_map<int, int> count_map(palette.size());

    for (int j = 0; j < image.rows; ++j)
        for (int i = 0; i < image.cols; ++i)
        {
            cv::Vec3b image_pixel_colour = image.at<cv::Vec3b>(j, i);
            count_map[Palette::scalar2int(palette.nearest(cv::Scalar(image_pixel_colour), p_norm))]++;
        }

    std::vector<cv::Scalar> colours = palette.palette();
    std::vector<colourbin> output;

    for (int i = 0; i < colours.size(); i++)
        if (count_map[Palette::scalar2int(colours[i])] != 0)
        {
            colourbin entry;
            entry.colour = colours[i];
            entry.count = count_map[Palette::scalar2int(colours[i])];
            output.push_back(entry);
        }

    return output;
}
