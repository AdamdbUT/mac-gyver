#ifndef __webcam__
#define __webcam__

#include <stdint.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

#include "common.hpp"

class webcam_t
{
    public:
    webcam_t(const std::string& dev,size_t width,size_t height);
    ~webcam_t();
    
    void start();
    void capture(image_t& img);// image must already have the good size
    void stop();
    
    size_t get_width();
    size_t get_height();
    
    protected:
    int xioctl(int request,void *arg);
    void open(const std::string& dev);
    bool init();
    bool enqueue_buffer();
    bool dequeue_buffer();
    bool start_capturing();
    void stop_capturing();
    bool video_get_integer_control(__u32 cid,int& v);
    bool video_set_boolean_control(__u32 cid,bool b);
    bool video_set_integer_control(__u32 cid,int v);
    bool video_capture();
    void deinit();
    bool process_capture_to_image(image_t& img);
    
    enum capture_method_t
    {
        VIDEO_READ_WRITE,
        VIDEO_MMAP,
        VIDEO_USERP
    };
    
    int m_fd;
    size_t m_width,m_height;
    size_t m_video_bytes_per_line;
    size_t m_video_image_size;
    bool m_read_write_supported;
    bool m_streaming_supported;
    capture_method_t m_capture_method;
    uint8_t *m_video_buffer;
    size_t m_mmap_buffer_length;
    __u32 m_video_pixel_format;
    enum v4l2_colorspace m_video_colorspace;
};

#endif // __webcam__
