#include <iostream>
#include <string>

#ifdef __cplusplus
extern "C"{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include<libavdevice/avdevice.h>
#ifdef __cplusplus
};
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define CHECK_POINTER_NULL(POINTER, FUNC_NAME, END) \
if(POINTER == nullptr){std::string full_message = #FUNC_NAME" failed";std::cout<<full_message<<std::endl;goto END;}

#define CHECK_RETURNED(ret, FUNC_NAME, END) \
if(ret != 0){std::string failed_message = #FUNC_NAME" failed";std::cout<<failed_message<<std::endl;goto END;}

#define JUST_FUN(N) x##N


cv::Mat avFrame2cvMat(AVFrame* frame){
    if(frame == nullptr)return cv::Mat();

    int width = frame->width;
    int height = frame->height;
//    if(frame->format != AVPixelFormat::AV_PIX_FMT_BGR24){
//        std::cout<<"frame format is not correct."<<std::endl;
//        return cv::Mat();
//    }

    cv::Mat result(height, width,CV_8UC3);

//    int line_size = frame->linesize[0];
//    std::cout<<"frame line_size: "<<line_size<<std::endl;

    SwsContext* convert_context = sws_getContext(frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
                                                 width, height, AVPixelFormat::AV_PIX_FMT_BGR24,
                                                 SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
    int line_size[1];
    line_size[0] = result.step1();
    std::cout<<"line_size: "<<result.step1()<<std::endl;

    sws_scale(convert_context, frame->data, frame->linesize, 0, height, reinterpret_cast<uint8_t *const *>(&result.data), line_size);
    sws_freeContext(convert_context);

    return result;
}

#define DSHOW
//#define OPEN_CAMERA

int main() {

    avformat_network_init();
    avdevice_register_all();

    AVFormatContext*  av_format_context;
    av_format_context = avformat_alloc_context();
    CHECK_POINTER_NULL(av_format_context, avformat_alloc_context, THE_END4);
#ifdef OPEN_CAMERA
#ifdef DSHOW
    AVInputFormat* av_input_format = const_cast<AVInputFormat*>(av_find_input_format("dshow"));
#else
    AVInputFormat* av_input_format = const_cast<AVInputFormat*>(av_find_input_format("vfwcap"));
#endif
    CHECK_POINTER_NULL(av_input_format, av_find_input_format, THE_END3);

    int ret = avformat_open_input(&av_format_context, "video=Logitech HD Webcam C310", av_input_format, nullptr);
#else
    int ret = avformat_open_input(&av_format_context, "test.avi", nullptr, nullptr);
#endif

    CHECK_RETURNED(ret, avformat_open_input, THE_END3);

    ret = avformat_find_stream_info(av_format_context, nullptr);
    CHECK_RETURNED(ret, avformat_find_stream_info, THE_END2);

    int video_index = -1;
    std::cout<<"devices num: "<<av_format_context->nb_streams<<std::endl;
    for(int i=0;i<av_format_context->nb_streams;++i){
        if(av_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index = i;
            break;
        }
    }

    if(video_index < 0){
        std::cout<<"No devices can be found."<<std::endl;
        goto THE_END2;
    }

    AVCodecID av_codec_id = av_format_context->streams[video_index]->codecpar->codec_id;
    std::cout<<"av_codec_id is: "<<av_codec_id<<std::endl;

    const AVCodec * av_codec = avcodec_find_decoder(av_codec_id);
    CHECK_POINTER_NULL(av_codec, avcodec_find_decoder, THE_END2);

    AVCodecContext* av_codec_context = const_cast<AVCodecContext*>(avcodec_alloc_context3(av_codec));
    CHECK_POINTER_NULL(av_codec_context, avcodec_alloc_context3, THE_END2);

    ret = avcodec_parameters_to_context(av_codec_context, av_format_context->streams[video_index]->codecpar);
    CHECK_RETURNED(ret, avcodec_parameters_to_context, THE_END2);

//    AVCodecContext* av_codec_context = avcodec_alloc_context3(av_codec);
    ret = avcodec_open2(av_codec_context,  av_codec, nullptr);
    CHECK_RETURNED(ret, avcodec_open2, THE_END2);

    AVPacket* av_packet;
    AVFrame*  av_frame;

    av_packet = av_packet_alloc();
    CHECK_POINTER_NULL(av_packet, av_packet_alloc, THE_END2);
    av_frame = av_frame_alloc();
    CHECK_POINTER_NULL(av_frame, av_frame_alloc, THE_END1);

    cv::namedWindow("ffmpeg");
    while(1){
       while(1){
           if(av_read_frame(av_format_context, av_packet) != 0)
           {
               std::cout<<"av_read_frame failed"<<std::endl;
               goto THE_END0;
           }
           break;
       }

        if(avcodec_send_packet(av_codec_context, av_packet) != 0){
            std::cout<<"avcodec_send_packet failed"<<std::endl;
            goto THE_END0;
        }

        if(avcodec_receive_frame(av_codec_context, av_frame) != 0){
            std::cout<<"avcodec_receive_frame failed."<<std::endl;
            goto THE_END0;
        }

        std::cout<<"frame width:"<<av_frame->width<<", height: "<<av_frame->height<<", channels: "<<av_frame->channels
        <<", format: "<<av_frame->format<<std::endl;

        cv::Mat mat = avFrame2cvMat(av_frame);

        cv::imshow("ffmpeg", mat);
        if(cv::waitKey(1) >= 27)
            break;
    }

    //free
    THE_END0:
    av_frame_free(&av_frame);
    THE_END1:
    av_packet_free(&av_packet);
    THE_END2:
    avformat_close_input(&av_format_context);
    THE_END3:
    avformat_free_context(av_format_context);
    THE_END4:
    return 0;
}
