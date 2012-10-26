#ifndef DISPLAYREND_UTILS_H
#define DISPLAYREND_UTILS_H
typedef enum
{
    DISPLAYREND_TYPE_FADE = 0,      // 渐变
    DISPLAYREND_TYPE_SHOW_HORZ,     // 水平显示
    DISPLAYREND_TYPE_SHOW_VERT,     // 垂直显示
    DISPLAYREND_TYPE_MOVE_HORZ,     // 水平移动
    DISPLAYREND_TYPE_MOVE_VERT,     // 垂直移动
    #if !defined(FEATURE_VERSION_W516) && !defined(FEATURE_VERSION_W208S) && !defined(FEATURE_VERSION_W027)
    DISPLAYREND_TYPE_FLY_HORZ,      // 水平飞行
    DISPLAYREND_TYPE_FLY_VERT,      // 垂直飞行
    #endif
    DISPLAYREND_TYPE_ROTAT_HORZ,    // 水平旋转
    DISPLAYREND_TYPE_ROTAT_VERT,    // 垂直旋转
    DISPLAYREND_TYPE_BLINDS_VERT,   // 垂直百叶窗
    DISPLAYREND_TYPE_BLINDS_INTER,  // 交叉百叶窗
    DISPLAYREND_TYPE_GROUP,         // 分组的最大值
    DISPLAYREND_TYPE_RAND,          // 随机
    DISPLAYREND_TYPE_ONEBYONE,      // 逐个
    DISPLAYREND_TYPE_MAX,
}DisplayRendType;

extern void DisplayRend_Enable(boolean bEnable);
extern void DisplayRend_Push(void);
extern void DisplayRend_Start(boolean bIn);
extern void DisplayRend_StartEx(DisplayRendType eType, boolean bIn, int nDelay);
extern void DisplayRend_SetConfig(DisplayRendType eType);
extern void DisplayRend_Done(void);
#endif //DISPLAYREND_UTILS_H