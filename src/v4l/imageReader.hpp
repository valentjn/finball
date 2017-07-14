#ifndef IMAGEREADER_HPP
#define IMAGEREADER_HPP

#include <string>

typedef enum io_method {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR
} io_method;

typedef struct buffer {
    void *start;
    size_t length;
} tbuffer;


class ImageReader
{
protected:
	std::string dev_name;
	io_method io;
	int fd;
	tbuffer *buffers;
	unsigned int n_buffers;
	int frame_number;
	int force_format;
	int frame_ctr;

    // Kommentar

	void errno_exit(const std::string &s);
	void init_read(unsigned int buffer_size);
	void init_mmap(void);
	void init_userp(unsigned int buffer_size);
	int xioctl(unsigned long int request, void *arg);

	int read_frame();

public:

	ImageReader();
	ImageReader(std::string &dev_name);
	ImageReader(std::string &dev_name, io_method io, int force_format, int frame_number);

	~ImageReader() {;}

	void open_device(void);
	void init_device(void);
	void start_capturing(void);
	bool is_input_available();
	void mainloop();
	void stop_capturing(void);
	void uninit_device(void);
	void close_device(void);

	void setVideoInputSize(int i_width, int i_height);

	std::string &getDeviceName();
	io_method getIOMethod();
	int getFileDescriptor();
	tbuffer* getBuffers();
	int getNumberOfBuffers();
	int getNumberOfFrames();

	virtual void processImage(const void* p, int size) = 0;

	int image_width;
	int image_height;
};

#endif /* IMAGEREADER_HPP */
