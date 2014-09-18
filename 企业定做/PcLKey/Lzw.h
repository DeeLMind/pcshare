//////////////////////////////////////////////////////////////////
//																//
//		��; : LZW ѹ���㷨										//
//		���� : [Foolish] / 2001-9-17							//
//		���� : 2002-6-7											//
//		��ҳ :													//
//		���� : crazybit@263.net									//
//									(c) 1999 - 2002 =USTC= ����	//
//////////////////////////////////////////////////////////////////
#ifndef		__FOO_LZW_COMPRESS_H__
#define		__FOO_LZW_COMPRESS_H__
#include <windows.h>
#pragma once

//===================================================================
//	����
//===================================================================
//	��������õ�String Table����󳤶�
#define		LZW_MAX_TABLE_SIZE		4096

//	�������ֻ�õ�Hash��, ����ҪString Table, ��Ϊ������Ҫ֪��String Table�е�����, 
//	ֻ��Ҫ֪��Old + Pixel�ִ��Ƿ��ڱ��кͱ��е�index
//	Hash�����Ϊ  :  (Old << 8) | Pixel ���д�ŵ���String Table��Index
#define		LZW_MAX_HASH_SIZE		0x1000FF // (4096 << 8) + 0xFF

//	��������õ�String Table - string�ṹ
//	ÿ��string�����γ�һ�ö�����, �˶���������һ���ҽڵ�
//	��ΪwPrefix����ָ��String Table�е���һλ��, ��wSuffixָ��0 ~ (clear-1)
typedef struct tagLZW_STRING
{
	WORD	wPrefix ;	// Ϊ Old
	WORD	wSuffix ;	// Ϊ GetFirstChar (Old) �� GetFirstChar (Code)
} LZW_STRING, * PLZW_STRING ;

//===================================================================
//	LZW - ѹ���㷨
//===================================================================
class FCLzw
{
public :
	 FCLzw () ;
	~FCLzw () ;

	/********************************************************************/
	/*	��  �� ��ͨ��LZW����											*/
	/*	˵  �� ��OutBuffer����Ԥ����0									*/
	/*			 �����OutBuffer�� 8 + ...data... ��ʽ�洢				*/
	/********************************************************************/
	DWORD	LZW_Encode (BYTE * InBuffer, DWORD dwLength,
						BYTE * OutBuffer) ;

	/********************************************************************/
	/*	��  �� ��ͨ��LZW����											*/
	/*	˵  �� ��InBuffer���밴 8 + ...data... ��ʽ�洢					*/
	/********************************************************************/
	void	LZW_Decode (BYTE * InBuffer, BYTE * OutBuffer) ;

	/********************************************************************/
	/*	��  �� ������DIBΪGIF											*/
	/*	��  �� ����DIB���Ͻǵ�ָ�봫��DibBuffer							*/
	/*	����ֵ ��д��OutBuffer�е��ֽ���								*/
	/*	˵  �� ��OutBuffer����Ԥ����0									*/
	/*			 ��ʱֻ֧��4, 8λɫλͼ���룬��֧��1λɫλͼ			*/
	/********************************************************************/
	DWORD	LZW_GIF_Encode (BYTE * DibBuffer, BYTE * OutBuffer,
							DWORD dwDibWidth, DWORD dwDibHeight,
							WORD wColorBit) ;

	/********************************************************************/
	/*	��  �� ������GIF��DIB											*/
	/*	��  �� ����DIB���Ͻǵ�ָ�봫��DibBuffer							*/
	/*	˵  �� ������λɫGIFһ�ɽ���Ϊ8λɫDIB, DibBufferΪ8λɫ		*/
	/*			 InBuffer���밴 Min_Code_Length + ...data... ��ʽ�洢	*/
	/*				����GIF����(InBuffer)��Ҫ���						*/
	/********************************************************************/
	void	LZW_GIF_Decode (BYTE * InBuffer, BYTE * DibBuffer,
							DWORD dwDibWidth, DWORD dwDibHeight,
							bool bInterlace) ;
	BOOL PcUnZip(BYTE* pSrcData, BYTE** pDesData, DWORD* nUnZipLen);
	void MyMemSet(BYTE* pData, BYTE ch, DWORD len);

private :
	LZW_STRING	* m_pStrBegin ;	// String Table
	WORD		* m_pHash ;		// Hash Table (16bit���m_pStrBegin��Index)
	WORD		m_LZW_CLEAR, m_LZW_END ; // clear��end��־

	//	����洢��---------------------------------------------------+
	BYTE		m_byInterval ;	// ���ڽ���洢��gif��, ��Ϊm_Interval������, 0xFFΪ�ǽ���洢
	BYTE		m_Interval[7] ; // �������� : 8 8 4 2
	//	����洢��---------------------------------------------------+

	BYTE		* m_pOrigin ;
	BYTE		* m_pCurIn ;
	BYTE		* m_pCurOut ;
	BYTE		m_byInBit ;		// ���뻺��Bitλ��
	BYTE		m_byOutBit ;	// �������Bitλ��

	BYTE		m_byMinCode ;		// ��С�볤�� (��ԭʼ��ɫλ��)
	BYTE		m_byCurrBits ;		// �ֽ׶��볤
	WORD		m_CurrTableIndex ;	// ��ǰ��String Table Index	
	BYTE		m_Padding[2] ;

	DWORD		m_dwDibHeight ;  // DIB�ĸ߶�
	DWORD		m_dwDibWidth ;   // DIB�Ŀ��
	DWORD		m_dwCurrPixel ;	 // ��ǰ��д������ظ���
	DWORD		m_dwCurrHeight ; // ��ǰ�ĸ�
	DWORD		m_dwPitch ;		 // DIBÿ�е��ֽ���

private :
	//	����
	void	Decode_InitStringTable () ;
	WORD	Decode_GetNextCode () ; // �12λ
	bool	Decode_IsInTable (WORD Code) ;
	void	Decode_AddStringToTable (WORD wPrefix, WORD wSuffix) ;
	BYTE	Decode_GetFirstChar (WORD Code) ;
	void	Decode_WriteString_to8 (WORD Code) ; // ���뵽Ŀ��8λɫλͼ
	void	Decode_SwitchToFollowLine () ; // ʹm_pCurOut�л�����һ��
} ;

//===================================================================
//	Implement
//===================================================================
inline  FCLzw::FCLzw () {
	m_Interval[0] = m_Interval[1] = 8 ;
	m_Interval[2] = 4 ; m_Interval[3] = 2 ; // ��������
}
inline  FCLzw::~FCLzw () {
}

inline void FCLzw::MyMemSet(BYTE* pData, BYTE ch, DWORD len)
{
	for(DWORD i = 0; i < len; i++)
	{
		pData[i] = ch;
	}
}

inline void  FCLzw::Decode_InitStringTable () {
	MyMemSet((BYTE*) m_pStrBegin, 0xFF, LZW_MAX_TABLE_SIZE * sizeof(LZW_STRING)) ;
	for (WORD i = 0 ; i < m_LZW_CLEAR ; i++) // ��ʼ��String Table
		m_pStrBegin[i].wSuffix = i ;
	m_CurrTableIndex = m_LZW_END + 1 ;
	m_byCurrBits = m_byMinCode + 1 ;
}
inline void  FCLzw::Decode_SwitchToFollowLine () {
	m_dwCurrPixel = 0 ; // ���п�ͷ
	if (m_byInterval == 0xFF) // �ǽ���洢, ֱ�ӵ���һ��
		m_pCurOut -= m_dwPitch ;
	else
	{
		m_dwCurrHeight += m_Interval[m_byInterval] ; // ����洢��gif, ������һ��
		if (m_dwCurrHeight >= m_dwDibHeight)
			if (++m_byInterval < 4)
				m_dwCurrHeight = m_Interval[m_byInterval]/2 ;
			else
				return ;
		m_pCurOut = m_pOrigin - m_dwCurrHeight * m_dwPitch ;
	}
}
inline WORD  FCLzw::Decode_GetNextCode () {
	// ��Encode_WriteIndex ()�����Ӧ��, �Ϊ12λ(����Խ2-BYTE)
	register DWORD			dwRet = 0 ;
	register unsigned int	uiAdd = m_byInBit + m_byCurrBits ;

	if (uiAdd <= 8) // �ڵ�ǰBYTE��
		dwRet |= * m_pCurIn ;
	else
		if (uiAdd <= 16) // ��1-BYTE
			dwRet |= * (WORD *) m_pCurIn ;
		else // ��2-BYTE
		{
			dwRet |= * (m_pCurIn + 2) ;
			dwRet <<= 16 ;
			dwRet |= * (WORD *) m_pCurIn ;
		}
	m_pCurIn += uiAdd / 8 ;
	m_byInBit = uiAdd % 8 ;
	dwRet <<= 32 - uiAdd ;
	dwRet >>= 32 - m_byCurrBits ; // ��������
	return (WORD)dwRet ;
}
inline bool  FCLzw::Decode_IsInTable (WORD Code) {
	return (Code < m_CurrTableIndex) ;
}
inline BYTE  FCLzw::Decode_GetFirstChar (WORD Code) {
	while (m_pStrBegin[Code].wPrefix != 0xFFFF)
		Code = m_pStrBegin[Code].wPrefix ;
	return (BYTE) m_pStrBegin[Code].wSuffix ;
}
inline void  FCLzw::Decode_AddStringToTable (WORD wPrefix, WORD wSuffix) {
	m_pStrBegin[m_CurrTableIndex  ].wPrefix = wPrefix ;
	m_pStrBegin[m_CurrTableIndex++].wSuffix = wSuffix ;
	if ((m_CurrTableIndex == 0x008) || (m_CurrTableIndex == 0x010) ||
		(m_CurrTableIndex == 0x020) || (m_CurrTableIndex == 0x040) ||
		(m_CurrTableIndex == 0x080) || (m_CurrTableIndex == 0x100) ||
		(m_CurrTableIndex == 0x200) || (m_CurrTableIndex == 0x400) ||
		(m_CurrTableIndex == 0x800))
		m_byCurrBits++ ;
}
inline void  FCLzw::LZW_Decode (BYTE * InBuffer, BYTE * OutBuffer) {
	//	��dwDibWidth����0�򲻻���
	this->LZW_GIF_Decode (InBuffer, OutBuffer, 0, 0, false) ;
}
#endif