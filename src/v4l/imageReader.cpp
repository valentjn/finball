#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <fcntl.h>              /* low-level i/o */
#include <sys/stat.h>
#include <sys/mman.h>

#include <libv4l2.h>
#include <linux/videodev2.h>

#include "imageReader.hpp"


#define CLEAR(x) memset(&(x), 0, sizeof(x))

// ----------------------------------------------------------
// Constructor
// ----------------------------------------------------------

ImageReader::ImageReader() :
    dev_name("/dev/video0"), io(IO_METHOD_MMAP), fd(-1),
    frame_number(70), force_format(1), frame_ctr(0),
    image_width(-1),
    image_height(-1)
{;}


ImageReader::ImageReader(std::string &dev_name) :
    dev_name(dev_name), io(IO_METHOD_MMAP), fd(-1),
    frame_number(70), force_format(1), frame_ctr(0),
	image_width(-1),
	image_height(-1)
{;}


ImageReader::ImageReader(std::string &dev_name, io_method io, int force_format, int frame_number):
    dev_name(dev_name), io(io),
    frame_number(frame_number),
    force_format(force_format),
	image_width(-1),
	image_height(-1)
{;}


// ----------------------------------------------------------
// Helper functions
// ----------------------------------------------------------

void ImageReader::errno_exit(const std::string &s)
{
	std::cerr << s << " error " << errno << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
}

int ImageReader::xioctl(unsigned long int request, void *arg)
{
    int r;

    do {
        r = v4l2_ioctl(this->fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

void ImageReader::init_read(unsigned int buffer_size)
{
    this->buffers = static_cast<tbuffer*>(calloc(1, sizeof(tbuffer)));

    if (!this->buffers) {
    	std::cerr << "Out of memory" << std::endl;
        // fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    this->buffers[0].length = buffer_size;
    this->buffers[0].start = malloc(buffer_size);

    if (!this->buffers[0].start) {
    	std::cerr << "Out of memory" << std::endl;
        // fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
}

void ImageReader::init_mmap(void)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    std::cout << VIDIOC_REQBUFS << std::endl;

    if (-1 == xioctl(VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            std::cerr << dev_name << "does not support "
                 << "memory mapping" << std::endl;
            // fprintf(stderr, "%s does not support "
            //         "memory mapping\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        std::cerr << "Insufficient buffer memory on " << dev_name << std::endl;
        // fprintf(stderr, "Insufficient buffer memory on %s\n",
        //         dev_name);
        exit(EXIT_FAILURE);
    }

    buffers = static_cast<tbuffer*>(calloc(req.count, sizeof(*buffers)));

    if (!buffers) {
        std::cerr << "Out of memory" << std::endl;
        // fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;

        if (-1 == xioctl(VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
        	v4l2_mmap(NULL /* start anywhere */,
                 buf.length,
                 PROT_READ | PROT_WRITE /* required */,
                 MAP_SHARED /* recommended */,
                 fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

void ImageReader::init_userp(unsigned int buffer_size)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count  = 4;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            std::cerr << dev_name <<  "does not support "
                 << "user pointer i/o" << std::endl;
            // fprintf(stderr, "%s does not support "
            //         "user pointer i/o\n", dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    buffers = static_cast<tbuffer*>(calloc(4, sizeof(*buffers)));

    if (!buffers) {
        std::cerr << "Out of memory" << std::endl;
        // fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
        buffers[n_buffers].length = buffer_size;
        buffers[n_buffers].start = malloc(buffer_size);

        if (!buffers[n_buffers].start) {
            std::cerr << "Out of memory" << std::endl;
            // fprintf(stderr, "Out of memory\n");
            exit(EXIT_FAILURE);
        }
    }
}

// ----------------------------------------------------------

void ImageReader::open_device(void)
{
    struct stat st;

    if (-1 == stat(dev_name.c_str(), &st))
    {
        std::cerr << "Cannot identify '" << dev_name << "': " << errno << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        std::cerr << dev_name << " is no device" << std::endl;
        // fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    this->fd = v4l2_open(dev_name.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd)
    {
        std::cerr << "Cannot open '" << dev_name << "': " << errno << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ImageReader::setVideoInputSize(int i_width, int i_height)
{
	image_width = i_width;
	image_height = i_height;
}

void ImageReader:: init_device(void)
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            std::cerr << dev_name << " is no V4L2 device" << std::endl;
            // fprintf(stderr, "%s is no V4L2 device\n",
            //         dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        std::cerr << dev_name << " is no video capture device" << std::endl;
        // fprintf(stderr, "%s is no video capture device\n",
        //         dev_name);
        exit(EXIT_FAILURE);
    }

    switch (io) {
    case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
            std::cerr << dev_name << " does not support read i/o" << std::endl;
            // fprintf(stderr, "%s does not support read i/o\n",
            //         dev_name);
            exit(EXIT_FAILURE);
        }
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
            std::cerr << dev_name << " does not support streaming i/o" << std::endl;
            // fprintf(stderr, "%s does not support streaming i/o\n",
            //         dev_name);
            exit(EXIT_FAILURE);
        }
        break;
    }


    /* Select video input, video standard and tune here. */


    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(VIDIOC_CROPCAP, &cropcap)) {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(VIDIOC_S_CROP, &crop)) {
            switch (errno) {
            case EINVAL:
                /* Cropping not supported. */
                break;
            default:
                /* Errors ignored. */
                break;
            }
        }
    } else {
        /* Errors ignored. */
    }


    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (force_format) {
    	if (image_width == -1)
    	{
			fmt.fmt.pix.width       = 640;
			fmt.fmt.pix.height      = 480;
    	}
    	else
    	{
    		fmt.fmt.pix.width = image_width;
    		fmt.fmt.pix.height = image_height;
    	}

        // http://linuxtv.org/downloads/v4l-dvb-apis/packed-rgb.html#rgb-formats
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(VIDIOC_S_FMT, &fmt))
            errno_exit("VIDIOC_S_FMT");

    	image_width = fmt.fmt.pix.width;
    	image_height = fmt.fmt.pix.height;

        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(VIDIOC_G_FMT, &fmt))
            errno_exit("VIDIOC_G_FMT");

        image_width = fmt.fmt.pix.width;
        image_height = fmt.fmt.pix.height;
    }

    /* Buggy driver paranoia. */
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    switch (io) {
    case IO_METHOD_READ:
        init_read(fmt.fmt.pix.sizeimage);
        break;

    case IO_METHOD_MMAP:
        init_mmap();
        break;

    case IO_METHOD_USERPTR:
        init_userp(fmt.fmt.pix.sizeimage);
        break;
    }
}


void ImageReader::start_capturing(void)
{
    unsigned int i;
    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < n_buffers; ++i) {
            struct v4l2_buffer buf;

            CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long)buffers[i].start;
            buf.length = buffers[i].length;

            if (-1 == xioctl(VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(VIDIOC_STREAMON, &type))
            errno_exit("VIDIOC_STREAMON");
        break;
    }
}


void ImageReader::stop_capturing(void)
{
    enum v4l2_buf_type type;

    switch (io) {
    case IO_METHOD_READ:
        /* Nothing to do. */
        break;

    case IO_METHOD_MMAP:
    case IO_METHOD_USERPTR:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(VIDIOC_STREAMOFF, &type))
            errno_exit("VIDIOC_STREAMOFF");
        break;
    }
}

int ImageReader::read_frame()
{
    struct v4l2_buffer buf;
    unsigned int i;

    switch (io) {
    case IO_METHOD_READ:
        if (-1 == v4l2_read(fd, buffers[0].start, buffers[0].length)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("read");
            }
        }

        processImage(buffers[0].start, buffers[0].length);
        break;

    case IO_METHOD_MMAP:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
            }
        }

        assert(buf.index < n_buffers);

        processImage(buffers[buf.index].start, buf.bytesused);

        if (-1 == xioctl(VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
        break;

    case IO_METHOD_USERPTR:
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(VIDIOC_DQBUF, &buf)) {
            switch (errno) {
            case EAGAIN:
                return 0;

            case EIO:
                /* Could ignore EIO, see spec. */

                /* fall through */

            default:
                errno_exit("VIDIOC_DQBUF");
            }
        }

        for (i = 0; i < n_buffers; ++i)
            if (buf.m.userptr == (unsigned long)buffers[i].start
                && buf.length == buffers[i].length)
                break;

        assert(i < n_buffers);

        processImage((void *)buf.m.userptr, buf.bytesused);

        if (-1 == xioctl(VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
        break;
    }

    return 1;
}


bool ImageReader::is_input_available()
{
    fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    /* Timeout. */
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    r = select(fd + 1, &fds, NULL, NULL, &tv);

    if (r < 0)
    {
    	std::cerr << "Select error" << std::endl;
    	exit(-1);
    }

    return r > 0;
}

void ImageReader::mainloop()
{
    unsigned int count;
    count = this->frame_number;

    while (count-- > 0) {
        this->frame_ctr = this->frame_number - count;
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            /* Timeout. */
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(fd + 1, &fds, NULL, NULL, &tv);

            if (-1 == r) {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }

            if (0 == r)
            {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }

            if (read_frame())
                break;
            /* EAGAIN - continue select loop. */
        }
    }
}


void ImageReader::uninit_device(void)
{
    unsigned int i;

    switch (io) {
    case IO_METHOD_READ:
        free(buffers[0].start);
        break;

    case IO_METHOD_MMAP:
        for (i = 0; i < n_buffers; ++i)
            if (-1 == v4l2_munmap(buffers[i].start, buffers[i].length))
                errno_exit("munmap");
        break;

    case IO_METHOD_USERPTR:
        for (i = 0; i < n_buffers; ++i)
            free(buffers[i].start);
        break;
    }

    free(buffers);
}


void ImageReader::close_device(void)
{
    if (-1 == v4l2_close(fd))
        errno_exit("close");

    this->fd = -1;
}

// ----------------------------------------------------------
// Get/Set
// ----------------------------------------------------------

std::string &ImageReader::getDeviceName() {
    return(this->dev_name);
}

io_method ImageReader::getIOMethod() {
    return(this->io);
}

int ImageReader::getFileDescriptor() {
    return(this->fd);
}

tbuffer* ImageReader::getBuffers() {
    return(this->buffers);
}

int ImageReader::getNumberOfBuffers() {
    return(this->n_buffers);
}

int ImageReader::getNumberOfFrames() {
    return(this->frame_number);
}
