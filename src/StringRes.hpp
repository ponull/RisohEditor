// StringRes.hpp --- String Resources
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_RES_HPP_
#define STRING_RES_HPP_

#include "MByteStreamEx.hpp"
#include "MString.hpp"
#include "ConstantsDB.hpp"
#include <map>

//////////////////////////////////////////////////////////////////////////////

struct STRING_ENTRY
{
    WCHAR StringID[128];
    WCHAR StringValue[512];
};

BOOL StrDlg_GetEntry(HWND hwnd, STRING_ENTRY& entry, ConstantsDB& db);
void StrDlg_SetEntry(HWND hwnd, STRING_ENTRY& entry, ConstantsDB& db);

//////////////////////////////////////////////////////////////////////////////

class StringRes
{
public:
    typedef std::wstring string_type;
    typedef std::map<WORD, string_type> map_type;

    StringRes()
    {
    }

    BOOL LoadFromStream(const MByteStreamEx& stream, WORD wName)
    {
        for (WORD i = 0; i < 16; ++i)
        {
            m_map.erase((wName - 1) * 16 + i);
        }

        WORD wLen;
        for (WORD i = 0; i < 16; ++i)
        {
            if (!stream.ReadWord(wLen))
                break;

            if (wLen > 0)
            {
                string_type str(wLen, 0);
                if (!stream.ReadData(&str[0], wLen * sizeof(WCHAR)))
                    break;

                m_map[(wName - 1) * 16 + i] = str;
            }
        }

        return TRUE;
    }

    BOOL SaveToStream(MByteStreamEx& stream, WORD wName)
    {
        WORD first, last;
        GetIdRange(wName, first, last);

        for (WORD i = first; i <= last; ++i)
        {
            const string_type& str = m_map[i];
            WORD wLen = WORD(str.size());
            if (!stream.WriteWord(wLen) ||
                !stream.WriteData(&str[0], wLen * sizeof(WCHAR)))
                return FALSE;
        }

        return TRUE;
    }

    string_type Dump(const ConstantsDB& db, WORD wName)
    {
        string_type ret;

        ret += L"STRINGTABLE\r\n";
        ret += L"{\r\n";

        WORD first, last;
        GetIdRange(wName, first, last);
        for (WORD i = first; i <= last; ++i)
        {
            if (m_map[i].empty())
                continue;

            ret += L"    ";
            if (0)
            {
                ret += mstr_dec_word(i);
            }
            else
            {
                ret += db.GetNameOfResID(IDTYPE_STRING, i);
            }

            ret += L", \"";
            ret += mstr_escape(m_map[i]);
            ret += L"\"\r\n";
        }
        ret += L"}\r\n";

        return ret;
    }

    string_type Dump(const ConstantsDB& db)
    {
        string_type ret;

        ret += L"STRINGTABLE\r\n";
        ret += L"{\r\n";

        map_type::iterator it, end = m_map.end();
        for (it = m_map.begin(); it != end; ++it)
        {
            if (it->second.empty())
                continue;

            ret += L"    ";
            if (0)
            {
                ret += mstr_dec_word(it->first);
            }
            else
            {
                ret += db.GetNameOfResID(IDTYPE_STRING, it->first);
            }

            ret += L", \"";
            ret += mstr_escape(it->second);
            ret += L"\"\r\n";
        }

        ret += L"}\r\n";

        return ret;
    }

    map_type& map()
    {
        return m_map;
    }
    const map_type& map() const
    {
        return m_map;
    }

    void GetIdRange(WORD Name, WORD& first, WORD& last) const
    {
        first = (Name - 1) * 16;
        last = first + 16 - 1;
    }

protected:
    WORD        m_wName;
    map_type    m_map;
};

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef STRING_RES_HPP_
