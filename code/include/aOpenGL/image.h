#pragma once
#include <string>
#include <map>
#include <memory>

namespace a::gl {

/**
 * @brief 이미지 데이터 관리 class. 한번 읽은 데이터는 m_images 멤버 변수에 저장해둠.
 * @author ckm
 * @since Wed Aug 12 2020
 */
class Image
{
public:
    struct Data
    {
        std::string path;
        unsigned char* image; // image data. don't delete this.
        int channel, height, width;
    };
    using spData = std::shared_ptr<Data>;

    struct DataHDR
    {
        std::string path;
        float* image;
        int channel, height, width;
    };
    using spDataHDR = std::shared_ptr<DataHDR>;

    /**
     * call 할 경우 path 정보를 key 값으로 _images에 저장. 
     * 만약 같은 path를 다시 call 할 경우 _images에서 가져옴.
     */
    static Image::spData load(std::string path, int channel = 0)
    {
        return instance()->_load(path, channel);
    }

    /**
     * @return shared pointer of Image::DataHDR
     */
    static Image::spDataHDR load_hdr(std::string path)
    {
        return instance()->_load_hdr(path);
    }

    static Image::spData create(std::string name, unsigned char* img, int width, int height, int channel)
    {
        return instance()->_create(name, img, width, height, channel);
    }

    static void free_image(std::string path)
    {
        instance()->_free_image(path);
    }

    static void save_image(std::string save_name, spData data, bool flip = false);
    static void save_image(std::string save_name, const char* data, int width, int height, int channel, bool flip = false);

private:
    static Image* instance()
    {
        static Image* loader = new Image;
        return loader;
    }

    Image();
    Image::spData _load(std::string path, int channel);
    Image::spData _create(std::string path, unsigned char* img, int width, int height, int channel); // create empty image
    Image::spDataHDR _load_hdr(std::string path);
    void _free_image(std::string path);
    
    std::map<std::string, Image::spData> m_images;
    std::map<std::string, Image::spDataHDR> m_hdrs;
};

}