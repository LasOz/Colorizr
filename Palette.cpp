#include "Palette.hpp"
#include <bitset>

Palette::Palette()
{
    colours = std::vector<cv::Scalar>();
}

Palette::Palette(char const * file_open)
{
    std::string line;
    std::ifstream file(file_open);

    char c1, c2;
    unsigned short r, g, b;

    colours = std::vector<cv::Scalar>();

    if(file)
        while (std::getline(file, line))
        {
            std::istringstream ss(line);
            ss >> r >> c1 >> g >> c2 >> b;
            Palette::colours.push_back(cv::Scalar(b,g,r));
        }

    file.close();
}

void Palette::add(cv::Scalar & entry)
{
    colours.push_back(entry);
}

size_t Palette::size()
{
    return Palette::colours.size();
}

std::vector<cv::Scalar> Palette::palette()
{
    return Palette::colours;
}

cv::Scalar Palette::nearest(cv::Scalar & colour, unsigned short p_norm)
{
    if (Palette::colours.empty())
        return colour;
    unsigned int closest = 0;
    float closest_distance = std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < Palette::colours.size(); i++)
    {
        float dist_x = pow(abs(colours[i][0] - colour[0]), p_norm);
        float dist_y = pow(abs(colours[i][1] - colour[1]), p_norm);
        float dist_z = pow(abs(colours[i][2] - colour[2]), p_norm);
        float dist = pow(dist_x + dist_y + dist_z, 1 / (float)p_norm);
        if (dist < closest_distance)
        {
            closest = i;
            closest_distance = dist;
        }
    }
    return Palette::colours[closest];
}

unsigned int Palette::scalar2int(cv::Scalar & colour)
{
    int result = (uchar)colour[0];
    result = (result << 8) | (uchar)colour[1];
    result = (result << 8) | (uchar)colour[2];
    return result;
}

cv::Scalar Palette::int2scalar(int colour)
{
    return cv::Scalar((uchar)((colour & 16711680) >> 16), (uchar)((colour & 65280 ) >> 8), (uchar)(colour & 255));
}
