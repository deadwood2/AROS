#include <stdlib.h>
#include <stdio.h>

#include <png.h>

#include <CUnit/CUnitCI.h>

const char *filename01 = "T:1x1.png";
FILE *file01 = NULL;
png_structp png_ptr = NULL;
png_infop info_ptr = NULL;


CU_SUITE_SETUP()
{
    unsigned char png_image[] = {
        0x89, 0x50, 0x4E, 0x47, // PNG file signature
        0x0D, 0x0A, 0x1A, 0x0A,
        0x00, 0x00, 0x00, 0x0D, // IHDR chunk length
        0x49, 0x48, 0x44, 0x52, // IHDR chunk type
        0x00, 0x00, 0x00, 0x01, // Width: 1 pixel
        0x00, 0x00, 0x00, 0x01, // Height: 1 pixel
        0x08, 0x06, 0x00, 0x00, 0x00, // Bit depth: 8, Color type: RGBA
        0x1F, 0x15, 0xC4, 0x89, // CRC for IHDR chunk
        0x00, 0x00, 0x00, 0x0A, // IDAT chunk length
        0x49, 0x44, 0x41, 0x54, // IDAT chunk type
        0x08, 0xD7, 0x63, 0x60, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, // Compressed image data
        0xE2, 0x21, 0xBC, 0x33, // CRC for IDAT chunk
        0x00, 0x00, 0x00, 0x00, // IEND chunk length
        0x49, 0x45, 0x4E, 0x44, // IEND chunk type
        0xAE, 0x42, 0x60, 0x82  // CRC for IEND chunk
    };
    unsigned int png_image_len = sizeof(png_image);

    file01 = fopen(filename01, "wb");
    if (file01 == NULL)
        return CUE_SINIT_FAILED;

    // Implicit fwrite test :-)
    fwrite(png_image, 1, png_image_len, file01);
    fclose(file01);

    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    if (png_ptr) {
        if (info_ptr) {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        } else {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
        }
    }
    if (file01) fclose(file01);
    remove(filename01);

    return CUE_SUCCESS;
}


static void test_read_info()
{
    png_uint_32 width=0xdeadbeef, height=0xdeadbeef;
    int bit_depth=-1, color_type=-1, interlace_method=-1, compression_method=-1, filter_method=-1;

    file01 = fopen(filename01, "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(file01);

    // Initialize read structure
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(png_ptr);

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    CU_ASSERT_PTR_NOT_NULL_FATAL(info_ptr);

    // Set error handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        CU_ASSERT_FALSE_FATAL(1);
    }

    // Set up input for reading PNG data
    png_init_io(png_ptr, file01);

    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_method, &compression_method, &filter_method);
    CU_ASSERT_EQUAL(width, 1);
    CU_ASSERT_EQUAL(height, 1);
    CU_ASSERT_EQUAL(bit_depth, 8);
    CU_ASSERT_EQUAL(color_type, 6);
    CU_ASSERT_EQUAL(interlace_method, 0);
    CU_ASSERT_EQUAL(compression_method, 0);
    CU_ASSERT_EQUAL(filter_method, 0);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(png_read_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_read_info);
    return CU_CI_RUN_SUITES();
}
