#include <png.h>
#include <stdio.h>
#include <stdlib.h>

int org_png(char *fileName, unsigned char *buf) {
    if (fileName == NULL || buf == NULL)
        return;
    FILE *fp = NULL;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    if ((fp = fopen(fileName, "rb")) == NULL) {
        fprintf(stderr, "%s():Can't Open File:%s\n", __FUNCTION__, fileName);
        goto __outFailed;
    }

    if ((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0)) == NULL) {
        fprintf(stderr, "%s():png_create_read_struct() Failed!\n", __FUNCTION__);
        goto __outFailed;
    }

    if ((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
        fprintf(stderr, "%s():png_create_info_struct() Failed!\n", __FUNCTION__);
        goto __outFailed;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "%s():setjmp(png_jmpbuf(png_ptr)) Failed!\n", __FUNCTION__);
        goto __outFailed;
    }

    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int colorType = png_get_color_type(png_ptr, info_ptr);
    fprintf(stderr, "%s(): color_type=%d, m_width=%d, m_height=%d\n", __FUNCTION__, colorType, width, height);

    png_bytep *pRows = png_get_rows(png_ptr, info_ptr);
    fprintf(stderr, "%s():colorType=%d\n", __FUNCTION__, colorType);
__outFailed:
    if (fp != NULL)
        fclose(fp);
    if (png_ptr != NULL)
        png_destroy_read_struct(&png_ptr, NULL, NULL);
    if (info_ptr != NULL)
        png_destroy_read_struct(NULL, &info_ptr, NULL);
    return 0;
}

void usage(char *programe) {
    if (programe == NULL)
        return;
    fprintf(stderr, "Usage:%s [filePath]\n", programe);
}

int main(int argc, char **argv) {
    if (argv != 2) {
        usage(argv[0]);
        return 0;
    }

    char *file = argv[1];
    unsigned char buf[1024];
    org_png(file, buf);
    return;
}