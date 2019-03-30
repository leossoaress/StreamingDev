#include "UdpSocket.h"

#define AV_OUTPUT_CODEC  "libx264"
#define AV_OUTPUT_BITRATE 6000000

int IOWriteCustom(void *opaque, uint8_t *buf, int bufsize)
{
    size_t size = 0;
    if(!opaque) return -1;
    
    UdpSocket *server = (UdpSocket *) opaque;
    server->SendDatagrama(buf, size, bufsize);
    
    return size;
}

void Erro(std::string msg)
{
    std::clog << "Error: " << msg << std::endl;
    exit(0);
}

int main(int argc, char const *argv[])
{
    if(argc != 3) 
    {
        std::cout << "Usage: " << argv[0] << "<ip_address> <port>" << std::endl;
        std::cout << "For example: " << std::endl;
        std::cout << argv[0] << "127.0.0.1 6000" << std::endl;
        return 1;
    }

    // register all codecs/devices/filters
    av_register_all();
    avdevice_register_all();
    avformat_network_init();
    
    UdpSocket server(argv[1], atoi(argv[2]), 1);
    int ret = 0, videoId = -1;
    ff_input_t input;
    ff_output_t output;
    
    input.pInFmt = av_find_input_format("v4l2");
    if(!input.pInFmt) Erro("Input format could not be found");

    AVDictionary *inputOpts = nullptr;
    av_dict_set(&inputOpts, "video_size", "uhd2160", 0);
    av_dict_set(&inputOpts, "framerate", "30", 0);
    av_dict_set(&inputOpts, "preset", "fast", 0);

    ret = avformat_open_input(&input.pFmtCtx, "/dev/video0", input.pInFmt, &inputOpts);
    if(ret < 0) Erro("Could not open input");

    ret = avformat_find_stream_info(input.pFmtCtx, nullptr);
    if(ret < 0) Erro("Could not find stream info");

    av_dump_format(input.pFmtCtx, 0, "/dev/video0", 0);
    std::cout << std::endl;

    AVCodecParameters *pCodecAux = nullptr;

    for(unsigned int i = 0; i < input.pFmtCtx->nb_streams; ++i) {
        pCodecAux = input.pFmtCtx->streams[i]->codecpar;
        if(pCodecAux->codec_type == AVMEDIA_TYPE_VIDEO) {
            input.pCodec = avcodec_find_decoder(pCodecAux->codec_id);
            videoId = i;
            break;
        }
    }

    input.pCodecCtx = avcodec_alloc_context3(input.pCodec);
    if(!input.pCodecCtx) Erro("Could not alloc input codec context");

    ret = avcodec_parameters_to_context(input.pCodecCtx, pCodecAux);
    if(ret < 0) Erro("Could not copy parameters to context");

    const int INPUT_WIDTH = input.pCodecCtx->width;
    const int INPUT_HEIGHT = input.pCodecCtx->height;
    const int OUTPUT_WIDTH = INPUT_WIDTH;
    const int OUTPUT_HEIGHT = INPUT_HEIGHT;

    ret = avcodec_open2(input.pCodecCtx, input.pCodec, nullptr);
    if(ret < 0) Erro("Could not open codec");

    input.pFrame = av_frame_alloc();
    input.pPacket = av_packet_alloc();
    if(!input.pFrame || !input.pPacket) Erro("Could not alloc frame/packet");

    uint8_t *IOBuffer = (uint8_t *) av_malloc(1316);

    ret = avformat_alloc_output_context2(&output.pFmtCtx, nullptr, "mpegts", nullptr);
    if(ret < 0) Erro("Alloc output format");
    
    AVIOContext * avio = avio_alloc_context(IOBuffer, BUFFER_SIZE, 1, &server, nullptr, IOWriteCustom, nullptr);
    if(!avio) Erro("Avio alloc context");
    
    output.pFmtCtx->pb = avio;
    
    //Find encoder
    output.pCodec = avcodec_find_encoder_by_name(AV_OUTPUT_CODEC);
    if(!output.pCodec) Erro("Codec not found");

    //Create and add new stream
    output.pStream = avformat_new_stream(output.pFmtCtx, output.pCodec);
    if(!output.pStream) Erro("Could not create the output stream");

     //Alloc codec context
    output.pCodecCtx = avcodec_alloc_context3(output.pCodec);
    if(!output.pCodecCtx) Erro("Could not alloc codec context");

    output.pCodecCtx->codec_tag = 0;
    output.pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    output.pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    output.pCodecCtx->codec_id = AV_CODEC_ID_H264;
    output.pCodecCtx->bit_rate = AV_OUTPUT_BITRATE;
    output.pCodecCtx->width = OUTPUT_WIDTH;
    output.pCodecCtx->height = OUTPUT_HEIGHT;
    output.pCodecCtx->time_base.den = 20;
    output.pCodecCtx->time_base.num = 1;

    if(output.pFmtCtx->flags & AVFMT_GLOBALHEADER) {
        output.pCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    
    //Initialize codec stream
    ret = avcodec_parameters_from_context(output.pStream->codecpar, output.pCodecCtx);
    if(ret < 0) Erro("Could not initialize stream codec");

    //Set open codec configuration
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "preset", "ultrafast", 0);
    av_dict_set(&opts, "crf", "22", 0);
    av_dict_set(&opts, "tune", "zerolatency", 0);
    av_dict_set(&opts, "profile", "high", 0);

    av_opt_set(output.pCodecCtx->priv_data, "preset", "ultrafast", 0);
    av_opt_set(output.pCodecCtx->priv_data, "crf", "22", 0);

    //Open video encoder
    ret = avcodec_open2(output.pCodecCtx, output.pCodec, &opts);
    if(ret < 0) Erro("Could nt open output codec");

    output.pStream->codecpar->extradata = output.pCodecCtx->extradata;
    output.pStream->codecpar->extradata_size = output.pCodecCtx->extradata_size;
    
    //Print some information about output
    av_dump_format(output.pFmtCtx, 0, "", 1);
    
    //Initialize converter
    SwsContext *swsctx = sws_alloc_context();
    swsctx = sws_getContext(INPUT_WIDTH, INPUT_HEIGHT, input.pCodecCtx->pix_fmt,
                            OUTPUT_WIDTH, OUTPUT_HEIGHT, AV_PIX_FMT_YUV420P,
                            SWS_BICUBIC,nullptr, nullptr, nullptr);

    //Alloc and initialize frame buffer
    output.pFrame = av_frame_alloc();
    if(!output.pFrame) Erro("Could not alloc frame");

    std::vector<uint8_t> framebuf(av_image_get_buffer_size(output.pCodecCtx->pix_fmt, OUTPUT_WIDTH, OUTPUT_HEIGHT, 1));
    av_image_fill_arrays(output.pFrame->data, output.pFrame->linesize, framebuf.data(), output.pCodecCtx->pix_fmt, OUTPUT_WIDTH, OUTPUT_HEIGHT, 1);

    //Write header
    ret = avformat_write_header(output.pFmtCtx, &opts);
    if(ret < 0) Erro("Fail to write outstream header");

    while(av_read_frame(input.pFmtCtx, input.pPacket) >= 0) {

        if(input.pPacket->size == 0) continue;
        if(input.pPacket->stream_index == videoId) {
            ret = avcodec_send_packet(input.pCodecCtx, input.pPacket);
            if(ret < 0) Erro("Could not send packet to decoder");

            ret = avcodec_receive_frame(input.pCodecCtx, input.pFrame);
            if(ret < 0) Erro("Could not receive frame from decoder");

            //Convert YUV422 to YUV420P
            sws_scale(swsctx, input.pFrame->data, input.pFrame->linesize, 0, OUTPUT_HEIGHT, output.pFrame->data, output.pFrame->linesize);

            //Set frame config
            output.pFrame->width = OUTPUT_WIDTH;
            output.pFrame->height = OUTPUT_HEIGHT;
            output.pFrame->format = AV_PIX_FMT_YUV420P;
            output.pFrame->pts += av_rescale_q(1, output.pCodecCtx->time_base, output.pStream->time_base);

            //Alloc and initialize packet
            output.pPacket = av_packet_alloc();
            av_init_packet(output.pPacket);

            //Send frame to encoder
            ret = avcodec_send_frame(output.pCodecCtx, output.pFrame);
            if(ret < 0) Erro("Error sending frame to codec context");

            //Receive packet from encoder
            ret = avcodec_receive_packet(output.pCodecCtx, output.pPacket);
            if(ret < 0) Erro("Error receiving packet from codec context");

            //Write frame
            av_interleaved_write_frame(output.pFmtCtx, output.pPacket);
            av_packet_unref(output.pPacket);
            av_packet_unref(input.pPacket);
        }
    }

    return 0;
}
