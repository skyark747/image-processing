#include "Image.h"

Image::Image(const char* filepath, cv::ImreadModes readmode)
	:img(cv::imread(filepath, readmode)) {}

Image::Image(const Image& other) :img(other.img.clone()) {}

Image::Image(const Image&& other) noexcept
	:img(std::move(other.img)) {
}

Image& Image::operator=(const Image& other) {
	if (this != &other) {
		img = other.img.clone();
	}
	return *this;
}

Image& Image::operator=(Image&& other) noexcept {
	img = std::move(other.img);
	return *this;
}

//Image::~Image() {
//	img.release();
//}

uint8_t Image::at(int r, int c) const {
	return static_cast<uint8_t>(img.at<uchar>(r, c));
}

void Image::set(int r, int c, uchar value) {
	img.at<uchar>(r, c) = value;
}

uchar* Image::operator[](int r) {
	return img.ptr<uchar>(r);
}

int Image::get_rows() const {
	return img.rows;
}
int Image::get_cols() const {
	return img.cols;
}

void Image::show(std::string window_name) const {
	window_name = (window_name == "" ? std::to_string((size_t)this) : window_name);
	cv::namedWindow(window_name, cv::WINDOW_NORMAL);
	cv::imshow(window_name, this->img);
	cv::waitKey(0);
	//cv::destroyWindow(window_name);
}