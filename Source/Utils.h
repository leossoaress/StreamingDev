#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <string>
#include <vector>

#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

extern "C"
{
    #include <libavdevice/avdevice.h>
    #include <libavfilter/avfilter.h>
    #include <libavformat/avformat.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
}

typedef struct {
    AVFormatContext *pFmtCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVInputFormat *pInFmt = nullptr;
    AVStream *pStream = nullptr;
    AVPacket *pPacket = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
} ff_input_t;

typedef struct {
    AVFormatContext *pFmtCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVStream *pStream = nullptr;
    AVPacket *pPacket = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
} ff_output_t;

#endif //UTILS_H_