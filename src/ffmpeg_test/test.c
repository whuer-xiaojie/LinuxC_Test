#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <stdio.h>
#include <stdlib.h>

#include "clock.h"
#include "graphics.h"
//x9_opengl context info define in the paint thread
SDL_GLContext gl_context;
GLuint program;    /*the x9 shader program*/
GLint positionLoc; /*the x9 shader variable entrance*/
GLint texCoordLoc;
GLint samplerLoc;
GLuint x9_textureId; /*the x9 paint texture name*/

//the x9 show region size
int screen_Width = 600;
int screen_Height = 400;
int main() {
    const char *input = "test.avi";
    const char *output = "test.rgb";
    init_graphics();
    x9_create_texture();

    av_register_all();

    AVFormatContext *pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, input, NULL, NULL) != 0) {
        printf("%s", "can't open the input file \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("%s", "can't get the stream info \n");
        return;
    }

    int v_stream_idx = -1;
    int i = 0;
    //number of streams
    for (; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_idx = i;
            break;
        }
    }

    if (v_stream_idx == -1) {
        printf("%s", "can't find the stream id\n");
        return;
    }

    AVCodecContext *pCodecCtx = pFormatCtx->streams[v_stream_idx]->codec;

    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        printf("%s", "can't find pCodec\n");
        return;
    }

   
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("%s", "can't open pCodec\n");
        return;
    }

    printf("pFormatCtx->iformat->name::%s\n", pFormatCtx->iformat->name);
    printf("pFormatCtx->duration::%d\n", (pFormatCtx->duration) / 1000000);
    printf("pCodecCtx->width,pCodecCtx->height::%d,%d\n", pCodecCtx->width, pCodecCtx->height);
    printf("pCodec->name::%s\n", pCodec->name);

    AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    AVFrame *pFrame = av_frame_alloc();
    //YUV420
    AVFrame *pFrameYUV = av_frame_alloc();

    uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_RGB24, screen_Width, screen_Height));

    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_RGB24, screen_Width, screen_Height);

    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, screen_Width, screen_Height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
    int got_picture, ret;

    FILE *fp_yuv = fopen(output, "wb+");

    int frame_count = 0;
    struct SwsContext *img_convert_ctx;

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == v_stream_idx) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0) {
                printf("%s", "error1111\n");
                return;
            }

            if (got_picture) {
                //img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                //pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
                sws_scale(sws_ctx, (const uint8_t *const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

                //screen_Width =pCodecCtx->width;
                //screen_Height=pCodecCtx->height;
                int y_size = screen_Width * screen_Height;
                unsigned char *buf = (unsigned char *)malloc(y_size * 3);
                if (buf == NULL) {
                    fprintf(stderr, "%s():malloc for buf failed\n", __FUNCTION__);
                    return 0;
                }
                memcpy(buf, pFrameYUV->data[0], y_size * 3);
                x9_render_buffer_R8G8B8(buf);
                __sleep((pFormatCtx->duration) / 1000000);
                free(buf);
                fwrite(pFrameYUV->data[0], y_size * 3, 1, fp_yuv);
                /*
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);
		        */
                frame_count++;
                //  printf("11111 pCodes the %d frame \n",frame_count);
            }
        }

        
        av_free_packet(packet);
    }

    x9_close_texture();
    close_graphics();
    fclose(fp_yuv);

    av_frame_free(&pFrame);

    avcodec_close(pCodecCtx);

    avformat_free_context(pFormatCtx);
    return 0;
}
