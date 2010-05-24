#ifdef FEATURE_BT_EXTPF_AV
/* This is the body of the generic version of OI_SBC_ComputeJointScalefactorsAndDoMidSide
* It is designed to be #included into a function as follows:

PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide4(OI_CODEC_SBC_COMMON_CONTEXT *common)
#define NROF_SUBBANDS 4
#include "computejoint.inc"
#undef NROF_SUBBANDS

PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide8(OI_CODEC_SBC_COMMON_CONTEXT *common)
#define NROF_SUBBANDS 8
#include "computejoint.inc"
#undef NROF_SUBBANDS

or, to make a generic version:
PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;

#define NROF_SUBBANDS nrof_subbands
#include "computejoint.inc"
#undef NROF_SUBBANDS
}


*/
{
    OI_UINT blk;
    OI_UINT nrof_blocks = common->frameInfo.nrof_blocks;
    OI_UINT16 joinBit = (1 << NROF_SUBBANDS);
    OI_UINT sb = 0;
    OI_UINT join = 0;

    do {
        OI_INT32 util_l = 0;
        OI_INT32 util_r = 0;
        OI_INT32 util_m = 0;
        OI_INT32 util_s = 0;
        OI_INT32 *sl = &common->subdata[sb];
        OI_INT32 *sr = &common->subdata[sb + NROF_SUBBANDS];
        OI_INT32 left;
        OI_INT32 right;
        OI_INT32 mid;
        OI_INT32 side;

        joinBit >>= 1;

        /*
         * Note that per the SBC spec the last suband is always left/right,
         */
        /*
         * First pass determines computes left/right and mid/side bit utilitization. This has to
         * be computed across all blocks in the frame because they share the same join mask.
         */
        for (blk = 0; blk < nrof_blocks; blk++) {
            left = *sl;
            right = *sr;
            mid = (left + right);
            side = (left - right);
            util_l |= ABS(left);
            util_r |= ABS(right);
            util_m |= (ABS(mid) >> 1);
            util_s |= (ABS(side) >> 1);
            sl += 2 * NROF_SUBBANDS;
            sr += 2 * NROF_SUBBANDS;
        }
        /*
         * If mid/side is preferred translate the sub-band data. 
         */
        if ((joinBit > 1) && ((util_m + util_s) < (util_l + util_r))) {
            join |= joinBit;
            for (blk = 0; blk < nrof_blocks; blk++) {
                sl -= 2 * NROF_SUBBANDS;
                sr -= 2 * NROF_SUBBANDS;
                left = *sl;
                right = *sr;
                mid = (left + right) / 2;
                side = (left - right) / 2;
                *sl = mid;
                *sr = side;
            }
            util_l = util_m;
            util_r = util_s;
        }

        common->scale_factor[sb] = SCALEFAC(util_l);
        common->scale_factor[NROF_SUBBANDS + sb] = SCALEFAC(util_r);
        ++sb;

    } while (joinBit > 1);
    common->frameInfo.join = join;
}
#endif /* FEATURE_BT_EXTPF_AV */
