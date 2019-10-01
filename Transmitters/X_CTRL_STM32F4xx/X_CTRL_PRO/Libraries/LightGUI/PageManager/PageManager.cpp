#include "PageManager.h"

#define IS_PAGE(page)   (page<MaxPage)

/**
  * @brief  ��ʼ��ҳ�������
  * @param  pageMax: ҳ���������
  * @param  eventMax: �¼��������
  * @retval ��
  */
PageManager::PageManager(uint8_t pageMax)
{
    MaxPage = pageMax;
    NewPage = 0;
    OldPage = 0xFF;

    /* �����ڴ棬����б� */
    PageList = new PageList_TypeDef[MaxPage];
    for(uint8_t page = 0; page < MaxPage; page++)
    {
        PageClear(page);
    }
}

/**
  * @brief  ���һ��ҳ��
  * @param  pageID: ҳ����
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageClear(uint8_t pageID)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = 0;
    PageList[pageID].LoopCallback = 0;
    PageList[pageID].ExitCallback = 0;
    PageList[pageID].EventCallback = 0;

    return true;
}

/**
  * @brief  ע��һ������ҳ�棬����һ����ʼ��������ѭ���������˳��������¼�����
  * @param  pageID: ҳ����
  * @param  setupCallback: ��ʼ�������ص�
  * @param  loopCallback: ѭ�������ص�
  * @param  exitCallback: �˳������ص�
  * @param  eventCallback: �¼������ص�
  * @retval true:�ɹ� false:ʧ��
  */
bool PageManager::PageRegister(uint8_t pageID,
                                     CallbackFunction_t setupCallback,
                                     CallbackFunction_t loopCallback,
                                     CallbackFunction_t exitCallback,
                                     EventFunction_t eventCallback)
{
    if(!IS_PAGE(pageID))
        return false;

    PageList[pageID].SetupCallback = setupCallback;
    PageList[pageID].LoopCallback = loopCallback;
    PageList[pageID].ExitCallback = exitCallback;
    PageList[pageID].EventCallback = eventCallback;
    return true;
}

/**
  * @brief  ҳ���¼�����
  * @param  eventID: �¼����
  * @param  param: �¼�����
  * @retval ��
  */
void PageManager::PageEventTransmit(int event, void* param)
{
    if(PageList[NowPage].EventCallback)
        PageList[NowPage].EventCallback(event, param);
}

/**
  * @brief  �л���ָ��ҳ��
  * @param  pageID: ҳ����
  * @retval ��
  */
void PageManager::PageChangeTo(uint8_t pageID)
{
    if(!IsPageBusy)
    {
        if(IS_PAGE(pageID))
        {
            NextPage = NewPage = pageID;
        }

        IsPageBusy = true;
    }
}

/**
  * @brief  ҳ�������״̬��
  * @param  ��
  * @retval ��
  */
void PageManager::Running()
{
    /*ҳ���л��¼�*/
    if(NewPage != OldPage)
    {
        IsPageBusy = true;

        if(PageList[OldPage].ExitCallback && IS_PAGE(OldPage))
            PageList[OldPage].ExitCallback();
        
        LastPage = OldPage;

        if(PageList[NewPage].SetupCallback && IS_PAGE(NewPage))
            PageList[NewPage].SetupCallback();

        NowPage = OldPage = NewPage;
    }
    else
    {
        IsPageBusy = false;
        if(PageList[NowPage].LoopCallback && IS_PAGE(NowPage))
            PageList[NowPage].LoopCallback();
    }
}