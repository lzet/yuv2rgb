#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yuv2rgb.h"
#include "rgb2yuv.h"
#include "bmp_utils.h"

// yuv422p
int save_bmp422(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    int frameSize = 0; 
    int picSize = 0;
    int ret = 0;
    int i = 0;
    unsigned char* framePtr = NULL;
    unsigned char* rgbPtr = NULL;
    long rgbSize = 0;

    static int yuv_init = 0;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422p: w * h * 2
    // yuv420p: w * h * 3 / 2
    picSize   = frameSize * 2;;

    framePtr = (unsigned char *) malloc(sizeof(unsigned char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (unsigned char *) malloc(sizeof(unsigned char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if (yuv_init == 0)
    {
        init_yuv422p_table();
        yuv_init = 1;
    }

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);

    yuv422p_to_rgb24(framePtr, rgbPtr, width, height);

    // rgb --> bgr
    swap_rgb(rgbPtr, rgbSize);

    // save file
    write_bmp_file(bmpfile, rgbPtr, width, height);

    fclose(fp1);


    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// yuv420
int save_bmp420(const char* yuvfile, const char* bmpfile, int width, int height)
{
    FILE* fp1;
    int frameSize = 0; 
    int picSize = 0;
    int ret = 0;
    unsigned char* framePtr = NULL;
    unsigned char* rgbPtr = NULL;
    long rgbSize = 0;

    rgbSize = width * height * 3;
    frameSize = width * height;

    // yuv422p: w * h * 2
    // yuv420p: w * h * 3 / 2
    picSize   = frameSize * 3 / 2;

    framePtr = (unsigned char *) malloc(sizeof(char) * picSize);
    if (framePtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(framePtr, '\0', picSize);

    rgbPtr = (unsigned char *) malloc(sizeof(unsigned char) * rgbSize);
    if (rgbPtr == NULL)
    {
        printf("malloc failed.\n");
        return -1;
    }
    memset(rgbPtr, '\0', rgbSize);

    if ((fp1 = fopen(yuvfile, "rb")) == NULL )
    {
        printf("open yuv file failed.\n");
        return -1;
    }

    ret = (int)fread(framePtr, 1, picSize, fp1);

    init_yuv420p_table();
    yuv420p_to_rgb24(framePtr, rgbPtr, width, height);

    // rgb --> bgr
    swap_rgb(rgbPtr, rgbSize);

    // save file
    write_bmp_file(bmpfile, rgbPtr, width, height);

    fclose(fp1);

    free(framePtr);
    free(rgbPtr);

    printf("done.\n");
    return 0;
}

// 从多帧YUV文件抽取第一帧
void split_file(const char* src, const char* dst, int width, int height, int type)
{
    FILE* fp1;
    FILE* fp2;
    int file_size;

    fp1 = fopen(src, "rb");
    if (fp1 == NULL)
    {
        printf("open %s failed!\n", src);
        return;
    }
    fp2 = fopen(dst, "wb");
    if (fp2 == NULL)
    {
        printf("open %s failed!\n", dst);
        return;
    }

    if (type == 0)
    {
        file_size = width * height * 3 / 2;
    }
    else if (type == 1)
    {
        file_size = width * height * 2;
    }
    else
    {
        return;
    }

    unsigned char* buffer1 = (unsigned char*)malloc(file_size);

    if ( NULL == buffer1)
    {
        printf("malloc failed!\n");
        return;
    }

    int ret = 0;
    ret = (int)fread(buffer1, 1, file_size, fp1);
    if (ret != file_size)
    {
        printf("fread failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    ret = (int)fwrite(buffer1, 1, file_size, fp2);
    if (ret != file_size)
    {
        printf("fwrite failed! real: %d ret: %d\n", file_size, ret);
        return;
    }

    fclose(fp1);
    fclose(fp2);
}

int main(int argc, char* argv[])
{
    //split_file("yuvfile/tempete_cif.yuv", "tempete_cif_yuv420p_00.yuv", 352, 288, 0);

    // OK
    save_bmp420("yuvfile/suzie_qcif_yuv420p_00.yuv", "suzie_qcif_0.bmp", 176, 144);

    save_bmp422("yuvfile/tulips_yuv422_prog_planar_qcif.yuv", "tulips_yuv422_qcif_0.bmp", 176, 144);

    return 0;
}