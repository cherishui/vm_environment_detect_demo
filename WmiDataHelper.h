#ifndef WmiDataHelper_h__
#define WmiDataHelper_h__

#include <vector>
#include <wbemcli.h>
 
class CWmiDataHelper
{
public:
    CWmiDataHelper();
    virtual ~CWmiDataHelper();
 
    /// @fn        ExecComQuery
    /// @brief    ����SQL���, ����COM��ѯ, ��ѯ�Ķ������, ѹ��vecResult����
    /// @param  IN const wchar_t * pcSqlW, ����COM��ѯ�淶��SQL���
    /// @param  OUT vector<std::wstring> vecResult, ���صĽ����
    /// @return  boolen, true = ��ѯ�ɹ�, false = ��ѯʧ��
    bool ExecComQuery(IN const wchar_t * pcSqlW, IN const wchar_t * pcPropertyValue, OUT std::vector<std::wstring> & vecResult);
 
private:
    /// ����,ȡ�� ���_COM�Ƿ���Ч
    void SetComValid(bool bValid) {m_bIsComValid = bValid;}
    bool GetComValid() {return m_bIsComValid;}
 
    bool SetComSecurity();
    bool CreateComInstance();
    bool ConnectComServer();
 
    bool ProcessWbemClassObject(IN ULONG ulRc, 
                                IN IWbemClassObject ** ppClassObject, 
                                IN const wchar_t * pcPropertyValue,
                                OUT std::vector<std::wstring> & vecResult);
 
    void VariantToString(const LPVARIANT pVar, std::wstring & strRc);
 
    void DataInit();
    void DataUnInit();
 
private:
    bool             m_bIsComValid;     ///< COM�����Ƿ���Ч
    IWbemLocator *   m_pIWbemLocator;
    IWbemServices *  m_pWbemServices;
};



#endif // WmiDataHelper_h__


