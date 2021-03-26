#ifndef __FRAME_IMAGE_HPP__
#define __FRAME_IMAGE_HPP__

#include <string.h>

namespace diff_image_video
{
	class frame_image
	{
	public:
		frame_image() { data = NULL; };
		frame_image(unsigned int width, unsigned int height, unsigned int channels, unsigned char* data)
		{
			if (width > 0 && height > 0 && channels > 0 && data != NULL)
			{
				this->width = width;
				this->height = height;
				this->channels = channels;

				this->data = new unsigned char[width * height * channels];
				memcpy(this->data, data, width * height * channels);
			}
		}

		~frame_image()
		{
			dispose();
		}

		void dispose()
		{
			if (data != NULL)
			{
				delete data;
				data = NULL;
			}
		}

		frame_image& operator=(frame_image& tmp)
		{
			dispose();

			this->width = tmp.width;
			this->height = tmp.height;
			this->channels = tmp.channels;

			if (tmp.data != NULL)
			{
				this->data = new unsigned char[tmp.width * tmp.height * tmp.channels];
				memcpy(this->data, tmp.data, tmp.width * tmp.height * tmp.channels);
			}
			else
			{
				this->data = NULL;
			}

			return *this;
		}

	public:
		unsigned char* data;
		unsigned int width;
		unsigned int height;
		unsigned int channels;
	};
}

#endif // !__FRAME_IMAGE_HPP__

