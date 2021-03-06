#include "FileGroup.h"
#include "GUI_Private.h"

#define PosCtrlInfo (screen.width()*0.25-15)
#define PosOptions  (screen.width()*0.75-15)
#define PosCursorY 55

static MillisTaskManager mtm_MainMenu(2);
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Ctrl(&screen, (uint8_t*)gImage_Bitmap_Ctrl, 0, 0, 30, 30);
static LightGUI::Bitmap<SCREEN_CLASS> Bitmap_Option(&screen, (uint8_t*)gImage_Bitmap_Option, 0, 0, 30, 30);
static LightGUI::Cursor<SCREEN_CLASS> MenuCursor(&screen, PosCtrlInfo, PosCursorY, 30, 2);

static float CursorPosNow = PosCtrlInfo;
static float CursorPosLast = PosCtrlInfo;
static uint8_t CursorSelect = 0;
static const float CursorTarget[2] = {PosCtrlInfo, PosOptions};
static uint32_t StayPageTimePoint;
static bool IsSpoke;

static void Task_CursorUpdate()
{
    CursorPosNow = CursorTarget[CursorSelect % 2];
    if((CursorPosNow - CursorPosLast) != 0)
        CursorPosLast += (CursorPosNow - CursorPosLast) * 0.2;

    MenuCursor.setPosition(CursorPosLast, PosCursorY);
}

static void Setup()
{
    StayPageTimePoint = millis();
    IsSpoke = false;
    for(float i = 0.7; i < 1.0; i += 0.05)
    {
        Bitmap_Ctrl.setPosition_f(0.5 - 0.25 * i, 0.6);
        Bitmap_Option.setPosition_f(0.5 + 0.25 * i, 0.6);
        if((ecd != 0 || btEcd || btLEFT || btRIGHT))
        {
            i = 1.0;
            Bitmap_Ctrl.setPosition_f(0.25, 0.6);
            Bitmap_Option.setPosition_f(0.75, 0.6);
            break;
        }
    }

    for(float y = screen.height(); y > PosCursorY; y --)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        delay(10);
    }

    mtm_MainMenu.TaskRegister(0, Task_CursorUpdate, 20);
}

static void Loop()
{
    if(!IsSpoke && millis() - StayPageTimePoint > 10000)
    {
        if(BattUsage < 30)
            XFS_Speak("警告：电池电量低。");
        
        XFS_Speak(
            "当前电池电压%0.1f伏，剩余电量%0.0f%%",
            BattVoltage,
            BattUsage
        );
        
        if(BattUsage < 20)
            XFS_Speak("请立即充电");
        else if(BattUsage < 30)
            XFS_Speak("请及时充电");
        
        IsSpoke = true;
    }
    mtm_MainMenu.Running(millis());
}

static void Exit()
{
    for(float y = PosCursorY; y < screen.height() + 1; y ++)
    {
        MenuCursor.setPosition(CursorPosLast, y);
        PageDelay(10);
    }
}

static void ButtonPressEvent()
{
    if(btLEFT && CursorSelect > 0)CursorSelect--;
    if(btRIGHT && CursorSelect < 1)CursorSelect++;

    if((btEcd || btUP || btDOWN))
    {
        if(CursorPosNow == PosCtrlInfo)
        {
            CursorPosLast = PosCtrlInfo;
            if(State_Handshake)
                page.PageChangeTo(PAGE_Handshake);
            else
                page.PageChangeTo(PAGE_CtrlInfo);
        }
        else if(CursorPosNow == PosOptions)
        {
            CursorPosLast = PosOptions;
            page.PageChangeTo(PAGE_Options);
        }
    }

}

static void ButtonLongPressEvent()
{

}

static void EncoderEvent()
{
    if(ecd < 0 && CursorSelect > 0)CursorSelect--;
    if(ecd > 0 && CursorSelect < 1)CursorSelect++;
}

void PageRegister_MainMenu(uint8_t ThisPage)
{
    page.PageRegister_Basic(ThisPage, Setup, Loop, Exit);
    page.PageRegister_Event(ThisPage, EVENT_ButtonPress, ButtonPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_ButtonLongPress, ButtonLongPressEvent);
    page.PageRegister_Event(ThisPage, EVENT_EncoderRotate, EncoderEvent);
}
