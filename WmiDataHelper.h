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
    /// @brief    按照SQL语句, 进行COM查询, 查询的多条结果, 压入vecResult返回
    /// @param  IN const wchar_t * pcSqlW, 负荷COM查询规范的SQL语句
    /// @param  OUT vector<std::wstring> vecResult, 返回的结果集
    /// @return  boolen, true = 查询成功, false = 查询失败
    bool ExecComQuery(IN const wchar_t * pcSqlW, IN const wchar_t * pcPropertyValue, OUT std::vector<std::wstring> & vecResult);
 
private:
    /// 设置,取得 标记_COM是否有效
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
    bool             m_bIsComValid;     ///< COM环境是否有效
    IWbemLocator *   m_pIWbemLocator;
    IWbemServices *  m_pWbemServices;
};



#endif // WmiDataHelper_h__


