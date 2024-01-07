#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

class Image
{
public:
	Image(const char* filepath, cv::ImreadModes readmode = cv::IMREAD_GRAYSCALE);
	Image(const Image& other);
	Image(const Image&& other) noexcept;
	Image& operator=(const Image& other);
	Image& operator=(Image&& other) noexcept;
	//~Image();

	uchar* operator[](int r);
	uint8_t at(int r, int c) const;
	void set(int r, int c, uchar value);

	int get_rows() const;
	int get_cols() const;

	void show(std::string window_name = "") const;
private:
	cv::Mat img;
};