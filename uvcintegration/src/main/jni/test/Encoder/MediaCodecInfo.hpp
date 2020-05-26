//
// Created by geier on 25/05/2020.
//

#ifndef FPV_VR_OS_MEDIACODECINFO_HPP
#define FPV_VR_OS_MEDIACODECINFO_HPP

// Values taken from
// https://developer.android.com/reference/android/media/MediaCodecInfo.CodecCapabilities
namespace MediaCodecInfo{
    namespace CodecCapabilities{
        constexpr int COLOR_Format24bitRGB888=12;

        constexpr int COLOR_FormatYUV420Flexible=2135033992;
        constexpr int COLOR_FormatYUV420Planar=19;
        constexpr int COLOR_FormatYUV420PackedPlanar=20;
        constexpr int COLOR_FormatYUV420SemiPlanar=21;
        constexpr int COLOR_FormatYUV420PackedSemiPlanar=39;
        constexpr int COLOR_TI_FormatYUV420PackedSemiPlanar=2130706688;

        constexpr int COLOR_FormatYUV422Flexible= 2135042184;
        constexpr int COLOR_FormatYUV422Planar=22;
        constexpr int COLOR_FormatYUV422PackedPlanar=23;
        constexpr int COLOR_FormatYUV422SemiPlanar=24;
        constexpr int COLOR_FormatYUV422PackedSemiPlanar=40;

        constexpr int COLOR_FormatYUV444Flexible=2135181448;
    }
};

// taken from https://android.googlesource.com/platform/cts/+/3661c33/tests/tests/media/src/android/media/cts/EncodeDecodeTest.java
// and translated to cpp
namespace YUVFrameGenerator{
    boolean isSemiPlanarYUV(const int colorFormat) {
        using namespace MediaCodecInfo::CodecCapabilities;
        switch (colorFormat) {
            case COLOR_FormatYUV420Planar:
            case COLOR_FormatYUV420PackedPlanar:
                return false;
            case COLOR_FormatYUV420SemiPlanar:
            case COLOR_FormatYUV420PackedSemiPlanar:
            case COLOR_TI_FormatYUV420PackedSemiPlanar:
                return true;
            default:
                MLOGE<<"unknown format "<< colorFormat;
                return true;
        }
    }
    constexpr uint8_t TEST_Y = 120;                  // YUV values for colored rect
    constexpr uint8_t TEST_U = 160;
    constexpr uint8_t TEST_V = 200;

    // creates a purple rectangle with w=width/4 and h=height/2 that moves 1 square forward with frameIndex
    void generateFrame(int frameIndex, int colorFormat, uint8_t* frameData,size_t frameDataSize) {
        // Full width/height for luma ( Y )
        constexpr size_t WIDTH=640;
        constexpr size_t HEIGHT=480;
        // Half width / height for chroma (U,V btw Cb,Cr)
        constexpr size_t HALF_WIDTH= WIDTH / 2;
        constexpr size_t HALF_HEIGHT= HEIGHT / 2;

        // For some reason HEIGHT comes before WIDTH here ?!
        // The Y plane has full resolution.
        auto& YPlane = *static_cast<uint8_t (*)[HEIGHT][WIDTH]>(static_cast<void*>(frameData));
        // The CbCrPlane only has half resolution in both x and y direction ( 4:2:0 )
        // CbCrPlane[y][x][0] == Cb (U) value for pixel x,y and
        // CbCrPlane[y][x][1] == Cr (V) value for pixel x,y
        auto& CbCrPlane = *static_cast<uint8_t(*)[HALF_HEIGHT][HALF_WIDTH][2]>(static_cast<void*>(&frameData[WIDTH * HEIGHT]));
        // Check - YUV420 has 12 bit per pixel (1.5 bytes)
        static_assert(sizeof(YPlane)+sizeof(CbCrPlane)==WIDTH*HEIGHT*12 / 8);

        boolean semiPlanar = isSemiPlanarYUV(colorFormat);
        // Set to zero.  In YUV this is a dull green.
        std::memset(frameData,0,frameDataSize);

        constexpr int RECT_W=WIDTH/4;
        constexpr int RECT_H=HEIGHT/2;

        //frameIndex %= 8;
        frameIndex = (frameIndex / 8) % 8;    // use this instead for debug -- easier to see
        int startX;
        int startY;
        if (frameIndex < 4) {
            startX = frameIndex * RECT_W;
            startY = 0;
        } else {
            startX = (7 - frameIndex) * RECT_W;
            startY = HEIGHT / 2;
        }

        for (int x = startX; x <startX + RECT_W;x++) {
            for (int y = startY; y < startY + RECT_H; y++) {
                if (semiPlanar) {
                    // full-size Y, followed by UV pairs at half resolution
                    // e.g. Nexus 4 OMX.qcom.video.encoder.avc COLOR_FormatYUV420SemiPlanar
                    // e.g. Galaxy Nexus OMX.TI.DUCATI1.VIDEO.H264E
                    //        OMX_TI_COLOR_FormatYUV420PackedSemiPlanar
                    //frameData[y * WIDTH + x] = (uint8_t) TEST_Y;
                    YPlane[y][x]=TEST_Y;
                    const bool even=(x % 2) == 0 && (y % 2) == 0;
                    if (even) {
                        CbCrPlane[y/2][x/2][0]=TEST_U;
                        CbCrPlane[y/2][x/2][1]=TEST_V;
                        //frameData[WIDTH * HEIGHT + y * HALF_WIDTH + x] = TEST_U;
                        //frameData[WIDTH * HEIGHT + y * HALF_WIDTH + x + 1] = TEST_V;
                    }
                } else {
                    // full-size Y, followed by quarter-size U and quarter-size V
                    // e.g. Nexus 10 OMX.Exynos.AVC.Encoder COLOR_FormatYUV420Planar
                    // e.g. Nexus 7 OMX.Nvidia.h264.encoder COLOR_FormatYUV420Planar
                    frameData[y * WIDTH + x] = TEST_Y;
                    if ((x & 0x01) == 0 && (y & 0x01) == 0) {
                        frameData[WIDTH * HEIGHT + (y / 2) * HALF_WIDTH + (x / 2)] = TEST_U;
                        frameData[WIDTH * HEIGHT + HALF_WIDTH * (HEIGHT / 2) +
                                  (y / 2) * HALF_WIDTH + (x / 2)] = TEST_V;
                    }
                }
            }
        }
    }




}
#endif //FPV_VR_OS_MEDIACODECINFO_HPP