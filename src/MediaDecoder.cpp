#include "MediaDecoder.h"


// http://blog.csdn.net/luotuo44/article/details/54981809

CMediaDecoder::CMediaDecoder()
: hasVideo(false)
, hasAudio(false)
, videoIndex(-1)
, audioIndex(-1)
, videoWidth(-1)
, videoHeight(-1)
, displayWidth(-1)
, displayHeight(-1)
, inputFormatContext(NULL)
, outPixFmt(AV_PIX_FMT_YUV420P)
, swsVideoCtx(NULL)
, swrAudioCtx(NULL)
, url(NULL)
, outChannels(2)
, outSampleFormat(AV_SAMPLE_FMT_S16)
{
    av_register_all();
    avdevice_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_DEBUG);
}

int CMediaDecoder::getVideoWidth()
{
    return videoWidth;
}

int CMediaDecoder::getVideoHeight()
{
    return videoHeight;
}

int CMediaDecoder::getVideoIndex()
{
    return videoIndex;
}

int CMediaDecoder::getAudioIndex()
{
    return audioIndex;
}

int CMediaDecoder::getSampleRate()
{
    return audioSampleRate;
}

void CMediaDecoder::setDataSource(const char* url)
{
    if(this->url) {
        delete this->url;
    }

    this->url = new char[strlen(url) + 1];
    strcpy(this->url, url);

    return;
}

int CMediaDecoder::prepare()
{
    int ret = avformat_open_input(&inputFormatContext, url, NULL, NULL);
    if(ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_open_input error!\n");
        return ret;
    }

    ret = avformat_find_stream_info(inputFormatContext, NULL);
    if(ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_find_stream_info error!\n");
        return -1;
    }

    if(initCodec() <= 0) {
        av_log(NULL, AV_LOG_ERROR, "init codec error!\n");
        return ret;
    }

    displayWidth = videoWidth;
    displayHeight = videoHeight;

    outSampleRate = audioSampleRate;
    outLayout = av_get_default_channel_layout(outChannels);

    initVideoConvert();
    initAudioConvert();

    return 0;
}

AVRational CMediaDecoder::getVideoTimeBase()
{
    return videoTimeBase;
}

int CMediaDecoder::initCodec()
{
    int ret = 0;

    for(int i = 0; i < inputFormatContext->nb_streams; i++) {
        AVStream *inStream = inputFormatContext->streams[i];
        AVCodec *dec = NULL;
        AVCodecContext *dec_ctx = inStream->codec;

        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if(dec == NULL) {
            av_log(NULL, AV_LOG_ERROR, "avcodec_find_decoder find error %d\n", __LINE__);
            return -1;
        }

        ret = avcodec_open2(dec_ctx, dec, NULL);
        if(ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "avcodec_open2 err in line %d\n", __LINE__);
            return -1;
        }

        if(inStream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            hasVideo = true;
            videoIndex = i;
            videoWidth = dec_ctx->width;
            videoHeight = dec_ctx->height;
            videoPixFmt = dec_ctx->pix_fmt;
            videoTimeBase = inStream->time_base;
        } else if(inStream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioSampleRate = dec_ctx->sample_rate;
            audioChannels = dec_ctx->channels;
            audioSampleFormat = dec_ctx->sample_fmt;
            audioLayout = dec_ctx->channel_layout;
            hasAudio = true;
            audioIndex = i;
        }

        av_dump_format(inputFormatContext, i, url, 0);
    }

    av_log(NULL, AV_LOG_DEBUG, "videoIndex %d audioIndex %d\n", videoIndex, audioIndex);

    return 1;
}

int64_t CMediaDecoder::getMsByPts(AVRational time_base, int64_t pts)
{
    return av_rescale_q(pts, time_base, AV_TIME_BASE_Q)/1000;
}

int CMediaDecoder::getPacket(AVPacket *pkt)
{
    int ret = av_read_frame(inputFormatContext, pkt);
    if(ret < 0) {
        char errstr[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errstr, AV_ERROR_MAX_STRING_SIZE, ret);
        av_log(NULL, AV_LOG_ERROR, "av_read_frame error %s\n", errstr);
    }

    return ret;
}

int CMediaDecoder::getFrame(AVPacket *pkt, AVFrame *frame)
{
    int ret = -1;
    int got_frame = 0;
    int avIndex = pkt->stream_index;

    if(avIndex == videoIndex) {
        ret = avcodec_decode_video2(inputFormatContext->streams[avIndex]->codec, frame, &got_frame, pkt);
        av_log(NULL, AV_LOG_DEBUG, "get video frame ret %d got_frame %d\n", ret, got_frame);
    } else if(avIndex == audioIndex) {
        ret = avcodec_decode_audio4(inputFormatContext->streams[avIndex]->codec, frame, &got_frame, pkt);
        av_log(NULL, AV_LOG_DEBUG, "get audio frame ret %d got_frame %d\n", ret, got_frame);
    }

    if(got_frame <= 0) {
        ret = -1;
    }

    return ret;
}

void CMediaDecoder::initVideoConvert()
{
    if(swsVideoCtx) {
        sws_freeContext(swsVideoCtx);
        swsVideoCtx = NULL;
    }

    av_log(NULL, AV_LOG_DEBUG, "videoWidth %d videoHeight %d displayWidth %d displayHeight %d\n",
        videoWidth, videoHeight, displayWidth, displayHeight);

    swsVideoCtx = sws_getContext(videoWidth, videoHeight, videoPixFmt, displayWidth, displayHeight,
                                    outPixFmt, SWS_BILINEAR, NULL, NULL, NULL);
    if(swsVideoCtx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "sws_getContext error!\n");
    }

    return;
}

void CMediaDecoder::initAudioConvert()
{
    if(swrAudioCtx) {
        swr_free(&swrAudioCtx);
    }

    swrAudioCtx = swr_alloc();
    if(!swrAudioCtx) {
        av_log(NULL,AV_LOG_ERROR,  "swr_alloc error !\n");
        return;
    }

    av_opt_set_int(swrAudioCtx, "ich", audioChannels, 0);
    av_opt_set_int(swrAudioCtx, "in_channel_layout", audioLayout, 0);
    av_opt_set_int(swrAudioCtx, "in_sample_rate", audioSampleRate, 0);
    av_opt_set_sample_fmt(swrAudioCtx, "in_sample_fmt", audioSampleFormat, 0);
    av_opt_set_int(swrAudioCtx, "och", outChannels, 0);
    av_opt_set_int(swrAudioCtx, "out_channel_layout", outLayout, 0);
    av_opt_set_int(swrAudioCtx, "out_sample_rate", outSampleRate, 0);
    av_opt_set_sample_fmt(swrAudioCtx, "out_sample_fmt", outSampleFormat, 0);
    if(swr_init(swrAudioCtx) < 0) {
        av_log(NULL,AV_LOG_ERROR,  "swr_init error\n");
        return ;
    }

    av_log(NULL, AV_LOG_ERROR, "swr_init right\n");

    return;
}

AVFrame* CMediaDecoder::convertVideoFrame(AVFrame *src)
{
    if(swsVideoCtx == NULL) {
        av_log(NULL, AV_LOG_ERROR, "swsVideoCtx is null, check it\n");
        return NULL;
    }

    AVFrame *frame = av_frame_alloc();
    int frameSize = av_image_get_buffer_size(outPixFmt, displayWidth, displayHeight, 1);
    unsigned char* frameData = (unsigned char*)av_malloc(frameSize);
    av_image_fill_arrays(frame->data, frame->linesize, frameData, outPixFmt,
                            displayWidth, displayHeight, 1);
    sws_scale(swsVideoCtx, src->data, src->linesize, 0, videoHeight, frame->data, frame->linesize); 

    return frame;
}

int CMediaDecoder::convertAudioFrame(AVFrame *src, AVFrame *outFrame)
{
    if(swrAudioCtx == NULL) {
        return -1;
    }

    int audioNbSamples = src->nb_samples;
    int dstNbSample = av_rescale_rnd(swr_get_delay(swrAudioCtx, outSampleRate) + audioNbSamples,
                                        outSampleRate, audioSampleRate, AV_ROUND_UP);
    int ret = av_samples_alloc(outFrame->data,
                                outFrame->linesize,
                                outChannels,
                                dstNbSample,
                                outSampleFormat, 0);
    if(ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "av_samples_alloc error\n");
        return -1;
    }

    int len = swr_convert(swrAudioCtx, outFrame->data, dstNbSample,
                            (const uint8_t**)src->data, src->nb_samples);
    
    return len;
}


