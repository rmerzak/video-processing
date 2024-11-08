extern "C" {
    #include<libavformat/avformat.h>
    #include<libavcodec/avcodec.h>
    #include<inttypes.h>
    #include<libswscale/swscale.h>
}
#include <stdio.h>

bool load_frames(const char* filename, int *width, int *height, unsigned char** data) {
    AVFormatContext* av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        printf("Failed to allocate memory for format context\n");
        return false;
    }

    // Open the input video file
    if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
        printf("Couldn't open video file\n");
        return false;
    }

    // if (avformat_find_stream_info(av_format_ctx, NULL) < 0) {
    //     printf("Couldn't find stream information\n");
    //     return false;
    // }

    // // Find the video stream
    // int video_stream_index = -1;
    // for (int i = 0; i < av_format_ctx->nb_streams; i++) {
    //     if (av_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
    //         video_stream_index = i;
    //         printf("Found video stream at index %d\n", video_stream_index);
    //         break;
    //     }
    // }

    // if (video_stream_index == -1) {
    //     printf("Couldn't find a video stream\n");
    //     return false;
    // }
    AVCodecParameters* av_codec_params;
    const AVCodec* av_codec;
    int video_stream_index = -1;
    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
        auto stream = av_format_ctx->streams[i];
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);
        if (!av_codec) {
            printf("Failed to find decoder for codec id %d\n", av_codec_params->codec_id);
           continue;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            printf("Found video stream at index %d\n", video_stream_index);
            break;
        }
    }
    // set up codec context for the decoder
    AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        printf("Failed to allocate memory for codec context\n");
        return false;
    }
    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        printf("Failed to copy codec parameters to codec context\n");
        return false;
    }
    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
        printf("Failed to open codec\n");
        return false;
    }
    AVFrame *av_frame = av_frame_alloc();
    AVPacket *av_packet = av_packet_alloc();
    if (!av_frame || !av_packet) {
        printf("Failed to allocate memory for frame or packet\n");
        return false;
    }
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index == video_stream_index) {
            if (avcodec_send_packet(av_codec_ctx, av_packet) < 0) {
                printf("Failed to send packet to decoder\n");
                return false;
            }
            if (avcodec_receive_frame(av_codec_ctx, av_frame) < 0) {
                printf("Failed to receive frame from decoder\n");
                return false;
            }
            *width = av_frame->width;
            *height = av_frame->height;
            *data = new unsigned char[av_frame->width * av_frame->height * 3];
            for (int y = 0; y < av_frame->height; ++y) {
                for (int x = 0; x < av_frame->width; ++x) {
                    (*data)[y * av_frame->width * 3 + x * 3 + 0] = av_frame->data[0][y * av_frame->linesize[0] + x * 3 + 0];
                    (*data)[y * av_frame->width * 3 + x * 3 + 1] = av_frame->data[0][y * av_frame->linesize[0] + x * 3 + 1];
                    (*data)[y * av_frame->width * 3 + x * 3 + 2] = av_frame->data[0][y * av_frame->linesize[0] + x * 3 + 2];
                }
            }
            break;
        }
        av_packet_unref(av_packet);
    }

    uint8_t* local_data = new uint8_t[av_frame->width * av_frame->height * 4];

    SwsContext *sws_ctx = sws_getContext(av_frame->width, av_frame->height, av_codec_ctx->pix_fmt, av_frame->width, av_frame->height, AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        printf("Failed to create sws context\n");
        return false;
    }
    uint8_t* dest[4] = {local_data,NULL,NULL,NULL};
    int dest_linesize[4] = {av_frame->width * 4,0,0,0};
    sws_scale(sws_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    sws_freeContext(sws_ctx);

    *width = av_frame->width;
    *height = av_frame->height;
    *data = local_data;


    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    av_packet_free(&av_packet);
    av_frame_free(&av_frame);
    
    return true;
}
