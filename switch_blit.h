#ifndef _g_blit_h_
#define _g_blit_h_

#include "include/GTypes.h"
#include "include/GShader.h"
#include "include/GBlendMode.h"
#include "utils.h"

void kClearBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        row[i] = 0;
    }
}

void kSrcBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        row[i] = src;
    }
}

void kSrcPlusDstBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        row[i] = src + dst;    
    }
}

void kSrcOverBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - GPixel_GetA(src))) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - GPixel_GetA(src))) << 8) 
                    + src;
    }
}

void kDstOverBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8) 
                    + dst;       
    }

}

void kSrcInBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * Da) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * Da) << 8);        
    }    
}

void kDstInBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * GPixel_GetA(src)) 
                    + (divisionHax32(((dst >> 8) & 0x00FF00FF) * GPixel_GetA(src)) << 8);    
    }
}

void kSrcOutBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8);     
    }
}

void kDstOutBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - GPixel_GetA(src))) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - GPixel_GetA(src))) << 8);      
        }
}

void kSrcATopBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * Da) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * Da) << 8)
                    + divisionHax32((dst & 0x00FF00FF) * (255 - GPixel_GetA(src))) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - GPixel_GetA(src))) << 8);    
        }
}

void kDstATopBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((dst & 0x00FF00FF) * GPixel_GetA(src)) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * GPixel_GetA(src)) << 8)
                    + divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8) ;             
        }
}

void kXorBlitShader(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list) {
    for(int i = left; i < right; i++) {
        GPixel src = src_list[i - left];
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - GPixel_GetA(src))) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - GPixel_GetA(src))) << 8)
                    + divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8);
                
    }
}


void kClearBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        row[i] = 0;
    }
}

void kSrcBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        row[i] = src;
    }
}

void kSrcPlusDstBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        row[i] = src + dst;    
    }
}

void kSrcOverBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - Sa)) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - Sa)) << 8) 
                    + src;
    }
}

void kDstOverBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8) 
                    + dst;       
    }

}

void kSrcInBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * Da) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * Da) << 8);        
    }    
}

void kDstInBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * Sa) 
                    + (divisionHax32(((dst >> 8) & 0x00FF00FF) * Sa) << 8);    
    }
}

void kSrcOutBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8);     
    }
}

void kDstOutBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - Sa)) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - Sa)) << 8);      
        }
}

void kSrcATopBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((src & 0x00FF00FF) * Da) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * Da) << 8)
                    + divisionHax32((dst & 0x00FF00FF) * (255 - Sa)) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - Sa)) << 8);    
        }
}

void kDstATopBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((dst & 0x00FF00FF) * Sa) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * Sa) << 8)
                    + divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8) ;             
        }
}

void kXorBlit(GPixel src, int Sa, int left, int right, GPixel* row) {
    for(int i = left; i < right; i++) {
        GPixel dst = row[i];
        int Da = GPixel_GetA(dst);
        row[i] = divisionHax32((dst & 0x00FF00FF) * (255 - Sa)) 
                    + (divisionHax32(((dst & 0xFF00FF00) >> 8) * (255 - Sa)) << 8)
                    + divisionHax32((src & 0x00FF00FF) * (255 - Da)) 
                    + (divisionHax32(((src & 0xFF00FF00) >> 8) * (255 - Da)) << 8);
                
    }
}


int switchMode(int a, 
                GShader* shader, 
                GBlendMode& mode,
                void (*&blit)(GPixel src, int Sa, int left, int right, GPixel* row),
                void (*&blit_shader)(GShader* shader, int left, int right, GPixel* row, std::vector<GPixel>& src_list)
) {
    if (a == 0 && shader == nullptr) {
        // Sa = 0;
        switch(mode){
            case(GBlendMode::kClear):
                blit = kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kSrc):
                blit =  kSrcBlit;
                blit_shader = kSrcBlitShader;
                break;
            case(GBlendMode::kDst):
                return -1;
            case(GBlendMode::kSrcOver):
                return -1;
            case(GBlendMode::kDstOver):
                return -1;
            case(GBlendMode::kSrcIn):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kDstIn):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kSrcOut):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kDstOut):
                return -1;
            case(GBlendMode::kSrcATop):
                return -1;
            case(GBlendMode::kDstATop):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kXor):
                return -1;
        }
    } else if((a == 255 && shader == nullptr) || (shader != nullptr && shader->isOpaque())) {
        // Sa = 1; 
        switch(mode){
            case(GBlendMode::kClear):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kSrc):
                blit =  kSrcBlit;
                blit_shader = kSrcBlitShader;
                break;
            case(GBlendMode::kDst):
                return -1;
            case(GBlendMode::kSrcOver):
                blit =  kSrcBlit;
                blit_shader = kSrcBlitShader;
                break;
            case(GBlendMode::kDstOver):
                blit =  kDstOverBlit;
                blit_shader = kDstOverBlitShader;
                break;
            case(GBlendMode::kSrcIn):
                blit =  kSrcInBlit;
                blit_shader = kSrcInBlitShader;
                break;
            case(GBlendMode::kDstIn):
                return -1;
            case(GBlendMode::kSrcOut):
                blit =  kSrcOutBlit;
                blit_shader = kSrcOutBlitShader;
                break;
            case(GBlendMode::kDstOut):
                blit =  kClearBlit;
                blit_shader = kClearBlitShader;
                break;
            case(GBlendMode::kSrcATop):
                blit =  kSrcInBlit;
                blit_shader = kSrcInBlitShader;
                break;
            case(GBlendMode::kDstATop):
                blit =  kDstOverBlit;
                blit_shader = kDstOverBlitShader;
                break;
            case(GBlendMode::kXor):
                blit =  kSrcOutBlit;
                blit_shader = kSrcOutBlitShader;
                break;
        }
    } else {
            switch(mode){
                case(GBlendMode::kClear):
                    blit =  kClearBlit;
                    blit_shader = kClearBlitShader;
                    break;
                case(GBlendMode::kSrc):
                    blit =  kSrcBlit;
                    blit_shader = kSrcBlitShader;
                    break;
                case(GBlendMode::kDst):
                    return -1;
                case(GBlendMode::kSrcOver):
                    blit =  kSrcOverBlit;
                    blit_shader = kSrcOverBlitShader;
                    break;
                case(GBlendMode::kDstOver):
                    blit =  kDstOverBlit;
                    blit_shader = kDstOverBlitShader;
                    break;
                case(GBlendMode::kSrcIn):
                    blit =  kSrcInBlit;
                    blit_shader = kSrcInBlitShader;
                    break;
                case(GBlendMode::kDstIn):
                    blit =  kDstInBlit;
                    blit_shader = kDstInBlitShader;
                    break;
                case(GBlendMode::kSrcOut):
                    blit =  kSrcOutBlit;
                    blit_shader = kSrcOutBlitShader;
                    break;
                case(GBlendMode::kDstOut):
                    blit =  kDstOutBlit;
                    blit_shader = kDstOutBlitShader;
                    break;
                case(GBlendMode::kSrcATop):
                    blit =  kSrcATopBlit;
                    blit_shader = kSrcATopBlitShader;
                    break;
                case(GBlendMode::kDstATop):
                    blit =  kDstATopBlit;
                    blit_shader = kDstATopBlitShader;
                    break;
                case(GBlendMode::kXor):
                    blit =  kXorBlit;
                    blit_shader = kXorBlitShader;
                    break;
            }
    } 
    
    return 0;
}


#endif