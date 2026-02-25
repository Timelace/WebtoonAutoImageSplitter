#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image.h"
#include "stb_image_write.h"
#include <iostream>
#include <cstring>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

#include <FileOpenerDialogBox.h>

int main(int argc, char** argv)
{

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

#ifdef _DEBUG
    std::ofstream debug_file("debug.txt");
    debug_file.flush();
#endif

    std::vector<std::string> input_folders;
    FileSelecter::select_multiple_folders(input_folders);

    for (std::string path : input_folders) {
        std::string out_path = path + " Splits";
        if (!std::filesystem::exists(out_path))
            std::filesystem::create_directories(path + " Splits");

        std::filesystem::directory_iterator it(path);
        int width, height, channels;
        unsigned char* image = stbi_load(it->path().string().c_str(), &width, &height, &channels, 0);
        int begin_index = 0;
        int end_index = width * channels - 1;
        int c_index = 0;
        int split_num = 1;
        int net_size = 0;

        int c_row = 0;
        int c_img_index = 0;

        int color_threshold = 3; // how many values a color can be off from another and still be considered the same color (0 means only exactly the same colors are counted as the same)
        int min_row_threshold = 50; // minimum amount of line necessary to be saved as a frame

        // put files in correct ordering
        std::vector<std::filesystem::directory_entry> files;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry);
        }
        std::sort(files.begin(), files.end(), [](std::filesystem::directory_entry a, std::filesystem::directory_entry b) {
            return std::stoi(a.path().string().substr(a.path().string().find("img") + 4, a.path().string().find("jpeg") - a.path().string().find("img") - 5)) < std::stoi(b.path().string().substr(b.path().string().find("img") + 4, b.path().string().find("jpeg") - b.path().string().find("img") - 5));
            });

        std::vector<char> c_split;

        for (auto i = files.begin(); i != files.end(); i++) {
#ifdef _DEBUG
            debug_file << "moving to file " << (*i).path().string().c_str() << std::endl;
#endif
            unsigned char* image = stbi_load((*i).path().string().c_str(), &width, &height, &channels, 0);
            unsigned char* c_color = new unsigned char[channels];

            while (end_index - net_size < width * height * channels) {
                for (int c = 0; c < channels; c++) c_color[c] = image[c_index + c - net_size];
                c_index += channels;

                for (int col = 1; col < width; col++) {
                    for (int c = 0; c < channels; c++) {
                        if (c_color[c] > image[c_index - net_size] + (color_threshold + image[c_index - net_size] > 255 ? 255 - image[c_index - net_size] : color_threshold) || c_color[c] < image[c_index - net_size] - (image[c_index - net_size] - color_threshold < 0 ? image[c_index - net_size] : color_threshold)) {
#ifdef _DEBUG
                            debug_file << "row " << c_row << " was not the same color" << std::endl;
#endif
                            goto not_the_same;
                        }
                        c_index++;
                    }
                    // check if row is entirely the same color
                }
#ifdef _DEBUG
                debug_file << "row " << c_row << " was the same color" << std::endl;
                debug_file << "\t begin_index: " << begin_index << "\t end_index: " << end_index << std::endl;
#endif
                // all pixels were the same color
                if (begin_index < end_index && end_index != begin_index + (width * channels - 1)) { // 2nd test is just to make sure we are not saving a single line image
                    if (c_split.size() / (channels * width) > min_row_threshold) { // check if were actually saving the image
                        // we need to save the image
                        c_split.insert(c_split.end(), image + begin_index - net_size, image + (end_index - net_size) + 1); // plus 1 for funsies
                        std::string name = out_path + "/split_" + std::to_string(split_num) + ".png";
#ifdef _DEBUG
                        debug_file << "saving image - vector size: " << c_split.size() << "\t png size: " << width * (c_split.size() / (channels * width)) * channels << std::endl;
#endif
                        stbi_write_png(name.c_str(), width, c_split.size() / (channels * width), channels, c_split.data(), width * channels);
                        split_num++;
                        c_split.clear();
                    }
                }
                begin_index = end_index - (width * channels - 1) + (width * channels);

            not_the_same:
#ifdef _DEBUG
                debug_file << "row " << c_row << " has finished processing: begin_index: " << begin_index << " end_index: " << end_index << " net_size: " << net_size << " width: " << width << " height: " << height << " channels: " << channels << std::endl;
                debug_file << "\t" << "img: " << c_img_index << "\t split: " << split_num << std::endl;
#endif

                c_index = end_index + 1;
                end_index += width * channels;

                c_row++;

            }

            delete[] c_color;
            if (begin_index > net_size) {
                // we need to copy only from begin_index
#ifdef _DEBUG
                debug_file << "adding chars from " << begin_index - net_size << " to " << width * height * channels << std::endl;
                debug_file << "\t old vector size: " << c_split.size();
#endif
                c_split.insert(c_split.end(), image + begin_index - net_size, image + (width * height * channels));
#ifdef _DEBUG
                debug_file << "\t new vector size: " << c_split.size() << std::endl;
#endif
            }
            else {
                // we need to copy the full image
#ifdef _DEBUG
                debug_file << "adding chars from " << 0 << " to " << width * height * channels << std::endl;
                debug_file << "\t old vector size: " << c_split.size();
#endif
                c_split.insert(c_split.end(), image, image + (width * height * channels));
#ifdef _DEBUG
                debug_file << "\t new vector size: " << c_split.size() << std::endl;
#endif
            }
            begin_index = end_index - (width * channels - 1);
            net_size += width * height * channels;
            c_img_index++;
            stbi_image_free(image);

        }
    }
#ifdef _DEBUG
    debug_file.close();
#endif

    CoUninitialize();
    
    return 0;
}