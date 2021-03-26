#ifndef __FILE_NAME_SORT_HPP__
#define __FILE_NAME_SORT_HPP__

#include <string>
#include <stdio.h>

namespace diff_image_video
{
	class FileNameSort
	{
	public:
		FileNameSort(std::string image_name_format)
		{
			this->image_name_format = image_name_format;
		}

		~FileNameSort() { }

		bool operator()(std::string str1, std::string str2)
		{
			int num1 = 0;
			int num2 = 0;
			num1 = get_file_name_number(str1, image_name_format);
			num2 = get_file_name_number(str2, image_name_format);

			return num1 < num2;
		}

	private:
		int get_file_name_number(const std::string& file_path, const std::string& image_name_format)
		{
			int number = 0;

			std::string file_name;
			size_t name_point = 0;
			size_t p_point = file_path.find_last_of('.');
			size_t name_point_1 = file_path.find_last_of('/');
			size_t name_point_2 = file_path.find_last_of('\\');
			
			if (name_point_1 != std::string::npos && name_point_2 != std::string::npos) {
				name_point = name_point_1 > name_point_2 ? name_point_1 : name_point_2;
			} else if (name_point_1 != std::string::npos && name_point_2 == std::string::npos) {
				name_point = name_point_1;
			} else if (name_point_1 == std::string::npos && name_point_2 != std::string::npos) {
				name_point = name_point_2;
			}

			file_name = file_path.substr(name_point + 1, p_point - name_point - 1);

#if _MSC_VER > 1600
			sscanf_s(file_name.c_str(), image_name_format.c_str(), &number);
#else
			sscanf(file_name.c_str(), image_name_format.c_str(), &number);
#endif
			return number;
		}
	private:
		std::string image_name_format;
	};

}
#endif // !__FILE_NAME_SORT_HPP__

