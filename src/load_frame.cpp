extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <inttypes.h>
}

bool load_frame(const char* filename, int* width, int* height, unsigned char** data){

    int k = 0;
    AVFormatContext* av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        printf("Couldn't create AVFormatContext\n");
        return false;
    }

    if(avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
        printf("Couldn't open video file\n");
        return false;
    }

    int video_stream_index = -1;
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;

    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
        AVStream *stream = av_format_ctx->streams[i];
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        printf("Couldn't find a video stream inside the file\n");
        return false;
    }

    av_codec = avcodec_find_decoder(av_codec_params->codec_id);
    if (!av_codec) {
        printf("Counldn't load codec");
        return false;
    }

    //set up a codec context for the decoder
    AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        printf("Couldn't create av codec context\n");
        return false;
    }

    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        printf("Couldn't initialize av codec context from av codec parameters");
        return false;
    }

    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
        printf("Couldn't open codec\n");
        return false;
    }

    AVFrame* av_frame = av_frame_alloc();
    AVPacket* av_packet = av_packet_alloc();

    //Read packets from av format context
    int response;
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) {
            continue;
        }
        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if ( response < 0 ) {
            printf("Couldn't decode packet: %s\n", av_err2str(response));
            return false;
        }
        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            continue;
        } else if (response < 0) {
            printf("Counldn't decode packet: %s\n", av_err2str(response));
            return false;
        }
    }

    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    
    return false;
}
