#include "StdAfx.h"
#include "GameLogic.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
bool		CChiHuRight::m_bInit = false;
DWORD		CChiHuRight::m_dwRightMask[MAX_RIGHT_COUNT];

#ifdef CARD_DISPATCHER_CONTROL

void CGameLogic::printLogAnalyseItems(BYTE cbHuCard, CAnalyseItemArray* arr){
	CStringA strLog;
	CStringA temp;
	CStringA strFormat="\n\t\t------printLogAnalyseItems----";

	for(int i=0; i<arr->GetCount(); i++){
		temp.Format("\n\t\t%02x", cbHuCard);
		strFormat += temp;
		for( int j=0; j<MAX_WEAVE; j++){
			BYTE *cardData = arr->GetAt(i).cbCardData[j];
			WORD kind = arr->GetAt(i).wWeaveKind[j];
			if( kind==WIK_GANG || kind==WIK_WIND){
				temp.Format("{%02x, %02x, %02x, %02x}", cardData[0], cardData[1], cardData[2], cardData[3]);
			} else {
				temp.Format("{%02x, %02x, %02x}", cardData[0], cardData[1], cardData[2]);
			}
			strFormat += "-"+temp;
		}
		BYTE bEndCard = arr->GetAt(i).bMagicEye ? SwitchToCardData(m_cbMagicIndex) : arr->GetAt(i).cbCardEye;
		temp.Format("{%02x, %02x}", arr->GetAt(i).cbCardEye, bEndCard);
		strFormat += "-"+temp;
	}
	OutputDebugStringA(strFormat);

}

#endif	//CARD_DISPATCHER_CONTROL

//构造函数
CChiHuRight::CChiHuRight()
{

	ZeroMemory(m_dwRight,sizeof(m_dwRight));

	if(!m_bInit)
	{
		m_bInit = true;
		for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		{
			if(0 == i)
				m_dwRightMask[i] = 0;
			else
				m_dwRightMask[i] = (DWORD(pow(2,i-1)))<<28;
		}
	}
}

//赋值符重载
CChiHuRight & CChiHuRight::operator = (DWORD dwRight)
{

	DWORD dwOtherRight = 0;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		dwRight = ~dwRight;
		dwOtherRight = MASK_CHI_HU_RIGHT;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
			m_dwRight[i] = dwRight&MASK_CHI_HU_RIGHT;
		else m_dwRight[i] = dwOtherRight;
	}

	return *this;
}

//与等于
CChiHuRight & CChiHuRight::operator &= (DWORD dwRight)
{

	bool bNavigate = false;
	//验证权位
	if(!IsValidRight(dwRight))
	{
		//验证取反权位
		ASSERT(IsValidRight(~dwRight));
		if(!IsValidRight(~dwRight)) return *this;
		//调整权位
		DWORD dwHeadRight = (~dwRight)&0xF0000000;
		DWORD dwTailRight = dwRight&MASK_CHI_HU_RIGHT;
		dwRight = dwHeadRight|dwTailRight;
		bNavigate = true;
	}

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] &= (dwRight&MASK_CHI_HU_RIGHT);
		}
		else if(!bNavigate)
			m_dwRight[i] = 0;
	}

	return *this;
}

//或等于
CChiHuRight & CChiHuRight::operator |= (DWORD dwRight)
{

	//验证权位
	if(!IsValidRight(dwRight)) return *this;

	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
	{
		if((dwRight&m_dwRightMask[i]) || (i==0&&dwRight<0x10000000))
		{
			m_dwRight[i] |= (dwRight&MASK_CHI_HU_RIGHT);
			break;
		}
	}

	return *this;
}

//与
CChiHuRight CChiHuRight::operator & (DWORD dwRight)
{

	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//与
CChiHuRight CChiHuRight::operator & (DWORD dwRight) const
{

	CChiHuRight chr = *this;
	return (chr &= dwRight);
}

//或
CChiHuRight CChiHuRight::operator | (DWORD dwRight)
{

	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//或
CChiHuRight CChiHuRight::operator | (DWORD dwRight) const
{

	CChiHuRight chr = *this;
	return chr |= dwRight;
}

//相等
bool CChiHuRight::operator == (DWORD dwRight) const
{

	CChiHuRight chr;
	chr = dwRight;
	return (*this)==chr;
}

//相等
bool CChiHuRight::operator == (const CChiHuRight chr) const
{

	for(WORD i = 0; i < CountArray(m_dwRight); i++)
	{
		if(m_dwRight[i] != chr.m_dwRight[i]) return false;
	}
	return true;
}

//不相等
bool CChiHuRight::operator != (DWORD dwRight) const
{

	CChiHuRight chr;
	chr = dwRight;
	return (*this)!=chr;
}

//不相等
bool CChiHuRight::operator != (const CChiHuRight chr) const
{

	return !((*this)==chr);
}

//是否权位为空
bool CChiHuRight::IsEmpty()
{

	for(BYTE i = 0; i < CountArray(m_dwRight); i++)
		if(m_dwRight[i]) return false;
	return true;
}

//设置权位为空
void CChiHuRight::SetEmpty()
{

	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	return;
}

//获取权位数值
BYTE CChiHuRight::GetRightData(DWORD dwRight[], BYTE cbMaxCount)
{

	ASSERT(cbMaxCount >= CountArray(m_dwRight));
	if(cbMaxCount < CountArray(m_dwRight)) return 0;

	CopyMemory(dwRight,m_dwRight,sizeof(DWORD)*CountArray(m_dwRight));
	return CountArray(m_dwRight);
}

//设置权位数值
bool CChiHuRight::SetRightData(const DWORD dwRight[], BYTE cbRightCount)
{

	ASSERT(cbRightCount <= CountArray(m_dwRight));
	if(cbRightCount > CountArray(m_dwRight)) return false;

	ZeroMemory(m_dwRight,sizeof(m_dwRight));
	CopyMemory(m_dwRight,dwRight,sizeof(DWORD)*cbRightCount);

	return true;
}

//检查仅位是否正确
bool CChiHuRight::IsValidRight(DWORD dwRight)
{
	DWORD dwRightHead = dwRight & 0xF0000000;
	for(BYTE i = 0; i < CountArray(m_dwRightMask); i++)
		if(m_dwRightMask[i] == dwRightHead) return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////

//静态变量

//麻将数据
const BYTE CGameLogic::m_cbCardDataArray[MAX_REPERTORY]=
{
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,						//万子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,						//索子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,						//同子
		0x31,0x31,0x31,0x31,
		0x32,0x32,0x32,0x32,
		0x33,0x33,0x33,0x33,
		0x34,0x34,0x34,0x34,
		0x35,0x35,0x35,0x35,
		0x36,0x36,0x36,0x36,
		0x37,0x37,0x37,0x37
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	InitCustomRule();
	m_cbMagicIndex = INVAILD_CARD_INDEX;
}

//析构函数
CGameLogic::~CGameLogic()
{

}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//混乱准备
	BYTE cbCardDataTemp[CountArray(m_cbCardDataArray)];
	CopyMemory(cbCardDataTemp,m_cbCardDataArray,sizeof(m_cbCardDataArray));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//混乱扑克
VOID CGameLogic::RandCardList(BYTE cbCardData[], BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//混乱准备
	BYTE cbCardDataTemp[MAX_COUNT] = {0};
	CopyMemory(cbCardDataTemp,cbCardData,sizeof(BYTE) * cbBufferCount);

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition]=cbCardDataTemp[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//删除扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		//效验扑克
		ASSERT(IsValidCard(cbRemoveCard[i]));
		ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard[i])]>0);

		//删除扑克
		BYTE cbRemoveIndex=SwitchToCardIndex(cbRemoveCard[i]);
		if (cbCardIndex[cbRemoveIndex]==0)
		{
			//错误断言
			ASSERT(FALSE);

			//还原删除
			for (BYTE j=0;j<i;j++) 
			{
				ASSERT(IsValidCard(cbRemoveCard[j]));
				cbCardIndex[SwitchToCardIndex(cbRemoveCard[j])]++;
			}

			return false;
		}
		else 
		{
			//删除扑克
			--cbCardIndex[cbRemoveIndex];
		}
	}

	return true;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//效验扑克
	ASSERT(IsValidCard(cbRemoveCard));
	ASSERT(cbCardIndex[SwitchToCardIndex(cbRemoveCard)] > 0);

	//删除扑克
	BYTE cbRemoveIndex = SwitchToCardIndex(cbRemoveCard);
	if (cbCardIndex[cbRemoveIndex] > 0)
	{
		cbCardIndex[cbRemoveIndex]--;
		return true;
	}

	//失败效验
	ASSERT(FALSE);

	return false;
}

//删除扑克
bool CGameLogic::RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//检验数据
	ASSERT(cbCardCount<=MAX_COUNT);
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData))
		return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}

	//成功判断
	if (cbDeleteCount!=cbRemoveCount) 
	{
		ASSERT(FALSE);
		return false;
	}

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) 
			cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//财神判断
bool CGameLogic::IsMagicCard(BYTE cbCardData)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	if(m_cbMagicIndex < MAX_INDEX)
		return SwitchToCardIndex(cbCardData) == m_cbMagicIndex;
	return false;
}

//财神判断
bool CGameLogic::IsBaoPaiCard(BYTE cbCardData)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	if(m_cbBaopaiIndex < MAX_INDEX)
		return (m_CustomRule.bEnabled_BaoPai && SwitchToCardIndex(cbCardData)==m_cbBaopaiIndex);
	return false;
}

//花牌判断
bool CGameLogic::IsHuaCard(BYTE cbCardData)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	ASSERT(IsValidCard(cbCardData));

	return cbCardData >= 0x38;
}

//花牌判断
BYTE CGameLogic::IsHuaCard(BYTE cbCardIndex[MAX_INDEX])
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	BYTE cbHuaCardCount = 0;
	for(int i = MAX_INDEX - MAX_HUA_INDEX; i < MAX_INDEX; i++)
	{
		if(cbCardIndex[i] > 0)
		{
			cbHuaCardCount += cbCardIndex[i];
		}
	}

	return cbHuaCardCount;
}

//排序,根据牌值排序
bool CGameLogic::SortCardList(BYTE cbCardData[MAX_COUNT], BYTE cbCardCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//数目过虑
	if (cbCardCount==0||cbCardCount>MAX_COUNT) return false;

	//排序操作
	bool bSorted=true;
	BYTE cbSwitchData=0,cbLast=cbCardCount-1;
	BYTE cbCard1, cbCard2;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<cbLast;i++)
		{
			cbCard1 = cbCardData[i];
			cbCard2 = cbCardData[i+1];
			//如果财神有代替牌，财神与代替牌转换
			if(INDEX_REPLACE_CARD != MAX_INDEX  && m_cbMagicIndex != INDEX_REPLACE_CARD)
			{
				if(SwitchToCardIndex(cbCard1) == INDEX_REPLACE_CARD) cbCard1 = SwitchToCardData(m_cbMagicIndex);
				if(SwitchToCardIndex(cbCard2) == INDEX_REPLACE_CARD) cbCard2 = SwitchToCardData(m_cbMagicIndex);
			}
			if (cbCard1>cbCard2)
			{
				//设置标志
				bSorted=false;

				//扑克数据
				cbSwitchData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=cbSwitchData;
			}	
		}
		cbLast--;
	} while(bSorted==false);

	return true;
}

//动作等级
BYTE CGameLogic::GetUserActionRank(WORD wUserAction)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//胡牌等级
	if (wUserAction&WIK_CHI_HU) { return 4; }

	//杠牌等级
	if (wUserAction&WIK_GANG) { return 3; }

	//碰牌等级
	if (wUserAction&WIK_PENG) { return 2; }

	//上牌等级
	if (wUserAction&(WIK_RIGHT|WIK_CENTER|WIK_LEFT)) { return 1; }

	return 0;
}

//胡牌等级
WORD CGameLogic::GetChiHuActionRank(const CChiHuRight & ChiHuRight)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	return 1;
}

//胡牌倍数
WORD CGameLogic::GetChiHuTime(const CChiHuRight & ChiHuRight)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	WORD wFanShu = 1;//平胡一倍


	if(!(ChiHuRight&CHR_GANG_SHANG_HUA).IsEmpty())//杠上开花
		wFanShu *= 2;
	else if(!(ChiHuRight&CHR_QIANG_GANG_HU).IsEmpty())//抢杠
	{
		wFanShu = 2*wFanShu+1;
	}
	
	if(!(ChiHuRight&CHR_ZI_MO).IsEmpty())
		wFanShu *= 2;


	return wFanShu;
}

//自动出牌
BYTE CGameLogic::AutomatismOutCard(const BYTE cbCardIndex[MAX_INDEX], const BYTE cbEnjoinOutCard[MAX_COUNT], BYTE cbEnjoinOutCardCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	// 先打财神
	if(m_cbMagicIndex != INVAILD_CARD_INDEX)
	{
		if(cbCardIndex[m_cbMagicIndex] > 0)
		{
			return SwitchToCardData(m_cbMagicIndex);
		}
	}

	//而后打字牌，字牌打自己多的，数目一样就按东南西北中发白的顺序
	BYTE cbCardData = 0;
	BYTE cbOutCardIndex  = INVAILD_CARD_INDEX;
	BYTE cbOutCardIndexCount = 0;
	for(int i = MAX_INDEX - 7; i < MAX_INDEX - 1; i++)
	{
		if(cbCardIndex[i] > cbOutCardIndexCount)
		{
			cbOutCardIndexCount = cbCardIndex[i];
			cbOutCardIndex = i;
		}
	}

	if(cbOutCardIndex != INVAILD_CARD_INDEX)
	{
		cbCardData = SwitchToCardData(cbOutCardIndex);
		bool bEnjoinCard = false;
		for(int k = 0; k < cbEnjoinOutCardCount; k++)
		{
			if(cbCardData == cbEnjoinOutCard[k])
			{
				bEnjoinCard = true;
			}
		}
		if(!bEnjoinCard)
		{
			return cbCardData;
		}		
	}

	//没有字牌就打边张，1或9，顺序为万筒索，2，8，而后3，7，而后4，6，而后5
	for(int i = 0; i < 5; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			cbOutCardIndex  = INVAILD_CARD_INDEX;
			if(cbCardIndex[j * 9 + i] > 0)
			{
				cbOutCardIndex = j * 9 + i;
			}
			else if(cbCardIndex[j * 9 + (9 - i - 1)] > 0)
			{
				cbOutCardIndex = j * 9 + (9 - i - 1);
			}

			if(cbOutCardIndex != INVAILD_CARD_INDEX)
			{
				BYTE cbCardDataTemp = SwitchToCardData(cbOutCardIndex);
				bool bEnjoinCard = false;
				for(int k = 0; k < cbEnjoinOutCardCount; k++)
				{
					if(cbCardDataTemp == cbEnjoinOutCard[k])
					{
						bEnjoinCard = true;
					}
				}
				if(!bEnjoinCard)
				{
					return cbCardDataTemp;
				}
				else
				{
					if(cbCardData == 0)
					{
						cbCardData = cbCardDataTemp;
					}
				}
			}
		}
	}
	return cbCardData;
}

//吃牌判断
BYTE CGameLogic::EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	BYTE cbCurrentIndex=SwitchToCardIndex(cbCurrentCard);

	//过滤判断
	if (cbCurrentIndex == m_cbMagicIndex) return WIK_NULL;
	if (cbCurrentIndex == INDEX_REPLACE_CARD && m_cbMagicIndex>=27) return WIK_NULL;
	if (cbCurrentCard >= 0x31 && cbCurrentIndex != INDEX_REPLACE_CARD) return WIK_NULL;

	//变量定义
	BYTE cbExcursion[3]={0,1,2};
	BYTE cbItemKind[3]={WIK_LEFT,WIK_CENTER,WIK_RIGHT};

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//如果有财神
	BYTE cbMagicCardCount = 0;
	if(m_cbMagicIndex != INVAILD_CARD_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//如果财神有代替牌，财神与代替牌转换
		if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}

	//吃牌判断
	BYTE cbEatKind=0,cbFirstIndex=0;
	if (cbCurrentIndex == INDEX_REPLACE_CARD) cbCurrentIndex = m_cbMagicIndex;
	for (BYTE i=0;i<CountArray(cbItemKind);i++)
	{
		BYTE cbValueIndex=cbCurrentIndex%9;
		if ((cbValueIndex>=cbExcursion[i])&&(cbValueIndex<(7+cbExcursion[i])))
		{
			//吃牌判断
			cbFirstIndex=cbCurrentIndex-cbExcursion[i];

			//吃牌不能包含有财神
			if(m_cbMagicIndex != INVAILD_CARD_INDEX &&
				m_cbMagicIndex >= cbFirstIndex && m_cbMagicIndex <= cbFirstIndex+2) continue;

			if ((cbCurrentIndex!=cbFirstIndex)&&(cbMagicCardIndex[cbFirstIndex]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+1))&&(cbMagicCardIndex[cbFirstIndex+1]==0))
				continue;
			if ((cbCurrentIndex!=(cbFirstIndex+2))&&(cbMagicCardIndex[cbFirstIndex+2]==0))
				continue;

			//设置类型
			cbEatKind|=cbItemKind[i];
		}
	}

	return cbEatKind;
}

//for the 手把一
WORD CGameLogic::possibleAction(const tagWeaveItem arrWeavItem[MAX_INDEX], BYTE cbWeaveCount, WORD wAction)
{
	if( !m_CustomRule.bEnabled_JiaHu ) return wAction;

	if( cbWeaveCount < (MAX_INDEX-1) ) return wAction;

	if( wAction&WIK_PENG ){
		for( int i=0; i<cbWeaveCount; i++){
			if( arrWeavItem[i].wWeaveKind != WIK_PENG ){
				wAction ^= WIK_PENG;
				break;
			}
		}
	}

	if( wAction&WIK_LEFT)
		wAction ^= WIK_LEFT;
	if( wAction&WIK_CENTER)
		wAction ^= WIK_CENTER;
	if( wAction&WIK_RIGHT)
		wAction ^= WIK_RIGHT;

	if( wAction&WIK_GANG )
		wAction ^= WIK_GANG;

	if( wAction&WIK_ARROW )
		wAction ^= WIK_ARROW;
	if( wAction&WIK_WIND )
		wAction ^= WIK_WIND;
	
	
	/* // no need
	if( wAction&WIK_CHASEARROW )
	{
		if( cbWeaveCount == MAX_INDEX )
			wAction ^= WIK_CHASEARROW;
	}
	if( wAction&WIK_CHASEWIND )
	{
		if( cbWeaveCount == MAX_INDEX )
			wAction ^= WIK_CHASEWIND;
	}*/

	return wAction;
}

//碰牌判断
BYTE CGameLogic::EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//碰牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]>=2)?WIK_PENG:WIK_NULL;
}

//杠牌判断
BYTE CGameLogic::EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//参数效验
	ASSERT(IsValidCard(cbCurrentCard));

	//过滤判断
	if (IsMagicCard(cbCurrentCard) || IsHuaCard(cbCurrentCard)) 
		return WIK_NULL;

	//杠牌判断
	return (cbCardIndex[SwitchToCardIndex(cbCurrentCard)]==3)?WIK_GANG:WIK_NULL;
}

//杠牌分析
WORD CGameLogic::AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//设置变量
	WORD wActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex) continue;
		if (cbCardIndex[i]==4)
		{
			wActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].wWeaveKind==WIK_PENG)
		{
			if (cbCardIndex[SwitchToCardIndex(WeaveItem[i].cbCenterCard)]==1)
			{
				wActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	return wActionMask;
}

WORD CGameLogic::AnalyseGangCardEx(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], 
								   BYTE cbWeaveCount,BYTE cbProvideCard, 
								   tagGangCardResult & GangCardResult, BYTE cbOutCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//设置变量
	WORD wActionMask=WIK_NULL;
	ZeroMemory(&GangCardResult,sizeof(GangCardResult));

	//手上杠牌
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex) continue;
		if (cbCardIndex[i]==4)
		{
			wActionMask|=WIK_GANG;
			GangCardResult.cbCardData[GangCardResult.cbCardCount++]=SwitchToCardData(i);
		}
	}

	//组合杠牌
	for (BYTE i=0;i<cbWeaveCount;i++)
	{
		if (WeaveItem[i].wWeaveKind==WIK_PENG)
		{
			if (WeaveItem[i].cbCenterCard==cbProvideCard)//之后抓来的的牌才能和碰组成杠
			{
				wActionMask|=WIK_GANG;
				GangCardResult.cbCardData[GangCardResult.cbCardCount++]=WeaveItem[i].cbCenterCard;
			}
		}
	}

	bool bXuanFeng = false;
	bool bChangMao = false;
	if( m_CustomRule.bEnabled_FengGang && cbOutCount<1){
		bXuanFeng = true;
	}
	if( m_CustomRule.bEnabled_ChangMaoGang){
		bChangMao = true;
	}

	if (bXuanFeng && IsSpGangOK(cbCardIndex, WIK_WIND))
	{
		wActionMask |= WIK_WIND;
	}
	if (bXuanFeng && IsSpGangOK(cbCardIndex, WIK_ARROW))
	{
		wActionMask |= WIK_ARROW;
	}

	BYTE cbProvideCardIndex = INVAILD_CARD_INDEX;
	if( IsValidCard(cbProvideCard) ) cbProvideCardIndex = SwitchToCardIndex(cbProvideCard);

	if ( bChangMao && IsPossibleChaseArrow(cbCardIndex,WeaveItem,cbWeaveCount,WIK_CHASEWIND))
	{
		wActionMask |= WIK_CHASEWIND;
	}
	if ( bChangMao && IsPossibleChaseArrow(cbCardIndex,WeaveItem,cbWeaveCount,WIK_CHASEARROW))			//m_CustomRule.cbZhuiFeng && 
	{
		wActionMask |= WIK_CHASEARROW;
	}

	return wActionMask;
}

//吃胡分析
BYTE CGameLogic::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], 
								  const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, 
								  BYTE cbCurrentCard, CChiHuRight &ChiHuRight
								  /*, bool bTingStatus=false*/)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//变量定义
	BYTE cbChiHuKind=WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	//设置变量
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();

	//构造扑克
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	//cbCurrentCard一定不为0			!!!!!!!!!
	ASSERT(cbCurrentCard != INVAILD_CARD_DATA);
	if(cbCurrentCard == INVAILD_CARD_DATA) return WIK_NULL;

	//插入扑克
	cbCardIndexTemp[SwitchToCardIndex(cbCurrentCard)]++;

		
	//分析扑克
	AnalyseCard(cbCurrentCard, cbCardIndexTemp,WeaveItem,cbWeaveCount,AnalyseItemArray);

	//胡牌分析
	if (AnalyseItemArray.GetCount()>0)
	{


#ifdef CARD_DISPATCHER_CONTROL
		if( AnalyseItemArray.GetCount()>1 ){	// for the TEST
			printLogAnalyseItems(cbCurrentCard, &AnalyseItemArray);
		} else {
			printLogAnalyseItems(cbCurrentCard, &AnalyseItemArray);
		}
#endif // CARD_DISPATCHER_CONTROL

		//牌型分析
		if( m_CustomRule.bEnabled_ZhanLiHu || isOpenedKaimen(WeaveItem,cbWeaveCount)) {
			ChiHuRight |= CHR_PING_HU;
			
			if( IsQingYiSe(&AnalyseItemArray[0]) ) {
				ChiHuRight |= CHR_QING_YI_SE;
			}

			if( IsShouBaYi(cbCurrentCard, &AnalyseItemArray[0], cbWeaveCount) ){
				ChiHuRight |= CHR_SHOU_BA_YI;
			}
			
			if( IsShiSanYao(cbCardIndex, cbWeaveCount) ){
				ChiHuRight |= CHR_SHI_SAN_YAO;
			}

			if( IsJiaHuFormat(cbCurrentCard, &AnalyseItemArray[0], cbWeaveCount) ){
				ChiHuRight |= CHR_JIA_HU;
			}
		}
	}

	if(!ChiHuRight.IsEmpty())
		cbChiHuKind = WIK_CHI_HU;

	return cbChiHuKind;
}

//听牌分析
BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],BYTE cbWeaveCount)
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if(cbCardCount% 3 == 2)
	{
		for(BYTE i = 0; i < MAX_INDEX - MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			cbCardIndexTemp[i]--;

			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbTestCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, cbTestCard, chr)){
					if( !isPossibleTing(cbCardIndexTemp, WeaveItem, cbWeaveCount) ) continue;
					return WIK_LISTEN;
				}
			}

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
		{
			BYTE cbCurrentCard = SwitchToCardData(j);
			if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbCurrentCard,chr)){
				if( !isPossibleTing(cbCardIndexTemp, WeaveItem, cbWeaveCount) ) continue;
				return WIK_LISTEN;
			}
		}
	}

	return WIK_NULL;
}

BYTE CGameLogic::AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,
								 BYTE& cbOutCardCount,BYTE cbOutCardData[],
								 BYTE cbHuCardCount[],BYTE cbHuCardData[][28])
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//复制数据
	BYTE cbOutCount = 0;
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if(cbCardCount%3 == 2)
	{
		for(BYTE i = 0; i < MAX_INDEX-MAX_HUA_INDEX; i++)
		{
			if(cbCardIndexTemp[i] == 0) continue;
			if(cbCardIndexTemp[i] == m_cbMagicIndex) continue;
			cbCardIndexTemp[i]--;

			bool bAdd=false;
			BYTE nCount=0;
			for(BYTE j = 0; j < MAX_INDEX-MAX_HUA_INDEX; j++)
			{
				BYTE cbTestCard = SwitchToCardData(j);
				if(WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbTestCard,chr))
				{
					if( !isPossibleTing(cbCardIndexTemp, WeaveItem, cbWeaveCount) ) continue;
					if(bAdd==FALSE)
					{
						bAdd=true;
						cbOutCardData[cbOutCount++] = SwitchToCardData(i);
					}
					cbHuCardData[cbOutCount-1][nCount++]=SwitchToCardData(j);
				}
			}
			if(bAdd)
				cbHuCardCount[cbOutCount-1]=nCount;

			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		BYTE nCount=0;
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbTestCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbTestCard,chr) )
			{
				if( !isPossibleTing(cbCardIndexTemp, WeaveItem, cbWeaveCount) ) continue;
				cbHuCardData[0][nCount++] = cbTestCard;
			}
		}
		cbHuCardCount[0]=nCount;
		cbOutCount = nCount>0 ? 1 : 0;
	}

	cbOutCardCount = cbOutCount;
	return cbOutCount;
}

// Check Ting condition
bool CGameLogic::isPossibleTing(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount){
	
	bool isContainChiPeng = false;
	for(int i=0; i<cbWeaveCount; i++){
		if( WeaveItem[i].wWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT|WIK_PENG) ){
			isContainChiPeng = true;
			break;
		}
	}

	return isContainChiPeng;
}

// Check Kaimen condition 开门
bool CGameLogic::isOpenedKaimen(const tagWeaveItem WeaveItem[], BYTE cbWeaveCount){
	//Peng, Chi, Gang(except FengGang & AnGang)
	for(int i=0; i<cbWeaveCount; i++){
		if( WeaveItem[i].wWeaveKind&(WIK_LEFT|WIK_CENTER|WIK_RIGHT|WIK_PENG) )
			return true;
		if( WeaveItem[i].wWeaveKind==WIK_GANG && (WeaveItem[i].cbParam!=WIK_AN_GANG) )
			return true;
	}
	return false;
}

BYTE CGameLogic::GetHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount,BYTE cbHuCardData[])
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//复制数据
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory( cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp) );

	BYTE nCount = 0;
	ZeroMemory(cbHuCardData,sizeof(cbHuCardData));

	BYTE cbCardCount = GetCardCount(cbCardIndexTemp);
	CChiHuRight chr;

	if( cbCardCount%3 != 2 )
	{
		for( BYTE j = 0; j < MAX_INDEX; j++ )
		{
			BYTE cbTestCard = SwitchToCardData(j);
			if( WIK_CHI_HU == AnalyseChiHuCard(cbCardIndexTemp,WeaveItem,cbWeaveCount,cbTestCard,chr) )
			{
				cbHuCardData[nCount++] = cbTestCard;
			}
		}
		return nCount;
	}
	return 0;
}

BYTE CGameLogic::GetRandHuiPaiCardIndex( )
{
	OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	BYTE indexHuiPai = rand()%CountArray(m_cbCardDataArray); 
	return SwitchToCardIndex(m_cbCardDataArray[indexHuiPai]);
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(BYTE cbCardIndex)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	ASSERT(cbCardIndex<MAX_INDEX);
	//if( cbCardIndex>=MAX_INDEX ) return INVAILD_CARD_DATA;

	if(cbCardIndex < 27)
		return ((cbCardIndex/9)<<4)|(cbCardIndex%9+1);
	else return (0x30|(cbCardIndex-27+1));
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(BYTE cbCardData)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	ASSERT(IsValidCard(cbCardData));
	//if( !IsValidCard(cbCardData) ) return INVAILD_CARD_INDEX;

	return ((cbCardData & MASK_COLOR) >> 4) * 9 + (cbCardData & MASK_VALUE) - 1;
}

//扑克转换
BYTE CGameLogic::SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT])
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//转换扑克
	BYTE cbPosition=0;
	//财神
	if(m_cbMagicIndex != INVAILD_CARD_INDEX)
	{
		for(BYTE i = 0; i < cbCardIndex[m_cbMagicIndex]; i++)
			cbCardData[cbPosition++] = SwitchToCardData(m_cbMagicIndex);
	}
	for (BYTE i=0;i<MAX_INDEX;i++)
	{
		if(i == m_cbMagicIndex && m_cbMagicIndex != INDEX_REPLACE_CARD) 
		{
			//如果财神有代替牌，则代替牌代替财神原来的位置
			if(INDEX_REPLACE_CARD != MAX_INDEX)
			{
				for(BYTE j = 0; j < cbCardIndex[INDEX_REPLACE_CARD]; j++)
					cbCardData[cbPosition++] = SwitchToCardData(INDEX_REPLACE_CARD);
			}
			continue;
		}
		if(i == INDEX_REPLACE_CARD) continue;
		if (cbCardIndex[i]!=0)
		{
			for (BYTE j=0;j<cbCardIndex[i];j++)
			{
				ASSERT(cbPosition<MAX_COUNT);
				cbCardData[cbPosition++]=SwitchToCardData(i);
			}
		}
	}

	return cbPosition;
}

//扑克转换
BYTE CGameLogic::SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX])
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//设置变量
	ZeroMemory(cbCardIndex,sizeof(BYTE)*MAX_INDEX);

	//转换扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		ASSERT(IsValidCard(cbCardData[i]));
		cbCardIndex[SwitchToCardIndex(cbCardData[i])]++;
	}

	return cbCardCount;
}


//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	BYTE cbValue = (cbCardData & MASK_VALUE);
	BYTE cbColor = (cbCardData & MASK_COLOR) >> 4;
	return (((cbValue >= 1) && (cbValue <= 9) && (cbColor <= 2)) || ((cbValue >= 1) && (cbValue <= (7 + MAX_HUA_INDEX)) && (cbColor == 3)));
}

//扑克数目
BYTE CGameLogic::GetCardCount(const BYTE cbCardIndex[MAX_INDEX])
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//数目统计
	BYTE cbCardCount=0;
	for (BYTE i=0;i<MAX_INDEX;i++) 
		cbCardCount+=cbCardIndex[i];

	return cbCardCount;
}

//获取组合
BYTE CGameLogic::GetWeaveCard(WORD wWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4])
{
	//OutputDebugStringA("\n");OutputDebugStringA(__FUNCTION__);
	//组合扑克
	switch (wWeaveKind)
	{
	case WIK_LEFT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard+1;
			cbCardBuffer[2]=cbCenterCard+2;

			return 3;
		}
	case WIK_RIGHT:		//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-2;
			cbCardBuffer[1]=cbCenterCard-1;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_CENTER:	//上牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard-1;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard+1;

			return 3;
		}
	case WIK_PENG:		//碰牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;

			return 3;
		}
	case WIK_GANG:		//杠牌操作
		{
			//设置变量
			cbCardBuffer[0]=cbCenterCard;
			cbCardBuffer[1]=cbCenterCard;
			cbCardBuffer[2]=cbCenterCard;
			cbCardBuffer[3]=cbCenterCard;

			return 4;
		}

	case WIK_WIND:		//东南西北
		{
			//设置变量
			cbCardBuffer[0]=0x31;
			cbCardBuffer[1]=0x32;
			cbCardBuffer[2]=0x33;
			cbCardBuffer[3]=0x34;

			return 4;
		}
	case WIK_ARROW:		//中发白
		{
			//设置变量
			cbCardBuffer[0]=0x35;
			cbCardBuffer[1]=0x36;
			cbCardBuffer[2]=0x37;

			return 3;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return 0;
}

bool CGameLogic::AddKindItem(tagKindItem &TempKindItem, tagKindItem KindItem[], BYTE &cbKindItemCount, bool &bMagicThree)
{
	TempKindItem.cbMagicCount =  
		(m_cbMagicIndex == TempKindItem.cbValidIndex[0] ? 1 : 0) + 
		(m_cbMagicIndex == TempKindItem.cbValidIndex[1] ? 1 : 0) +
		(m_cbMagicIndex == TempKindItem.cbValidIndex[2] ? 1 : 0) ;

	if(TempKindItem.cbMagicCount>=3) 
	{
		if(!bMagicThree)
		{
			bMagicThree = true;
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 2)
	{
		BYTE cbNoMagicIndex = 0;
		BYTE cbNoTempMagicIndex = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex)
			{
				cbNoTempMagicIndex = TempKindItem.cbValidIndex[i];
				break;
			}
		}
		bool bFind = false;
		for(int j = 0; j < cbKindItemCount; j++)
		{
			for(int i = 0; i < 3; i++)
			{
				if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex)
				{
					cbNoMagicIndex = KindItem[j].cbValidIndex[i];
					break;
				}
			}
			if(cbNoMagicIndex == cbNoTempMagicIndex && cbNoMagicIndex != 0)
			{
				bFind = true;
			}
		}

		if(!bFind)
		{
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
	else if(TempKindItem.cbMagicCount == 1)
	{
		BYTE cbTempCardIndex[2] = {0};
		BYTE cbCardIndex[2] = {0};
		BYTE cbCardCount = 0;
		for(int i = 0; i < 3; i++)
		{
			if(TempKindItem.cbValidIndex[i] != m_cbMagicIndex)
			{
				cbTempCardIndex[cbCardCount++] = TempKindItem.cbValidIndex[i];
			}
		}
		ASSERT(cbCardCount == 2);

		for(int j = 0; j < cbKindItemCount; j++)
		{
			if(1 == KindItem[j].cbMagicCount)
			{
				cbCardCount = 0;
				for(int i = 0; i < 3; i++)
				{
					if(KindItem[j].cbValidIndex[i] != m_cbMagicIndex)
					{
						cbCardIndex[cbCardCount++] = KindItem[j].cbValidIndex[i];
					}
				}
				ASSERT(cbCardCount == 2);

				if(cbTempCardIndex[0] == cbCardIndex[0] && cbTempCardIndex[1] == cbCardIndex[1])
				{
					return false;
				}
			}
		}


		CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
		return true;
	}
	else
	{
		bool findSame = false;
		for(int j = 0; j < cbKindItemCount; j++)
		{
			findSame = true;
			for(int i = 0; i < 3; i++)
			{
				if(KindItem[j].cbValidIndex[i] != TempKindItem.cbValidIndex[i])
				{
					findSame = false;
				}
			}
			if( findSame )
				break;
		}

		if(!findSame)
		{
			CopyMemory(&KindItem[cbKindItemCount++],&TempKindItem,sizeof(TempKindItem));
			return true;
		}
		return false;
	}
}



//分析扑克
bool CGameLogic::AnalyseCard(BYTE cbCurrentCard, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, CAnalyseItemArray & AnalyseItemArray)
{
	//计算数目
	BYTE cbCardCount=GetCardCount(cbCardIndex);

	//效验数目
	ASSERT((cbCardCount>=2)&&(cbCardCount<=MAX_COUNT)&&(cbCardCount%3==2));
	if ((cbCardCount<2)||(cbCardCount>MAX_COUNT)||(cbCardCount%3!=2))
		return false;

	//变量定义
	BYTE cbKindItemCount=0;
	tagKindItem KindItem[27*2+28+16];
	ZeroMemory(KindItem,sizeof(KindItem));
	tagKindItem TempKindItem;
	ZeroMemory(&TempKindItem,sizeof(TempKindItem));
	bool bMagicThree=false;

	//需求判断
	BYTE cbLessKindItem=(cbCardCount-2)/3;
	ASSERT((cbLessKindItem+cbWeaveCount)==MAX_WEAVE);

	//单吊判断
	if (cbLessKindItem==0)
	{
		//效验参数
		ASSERT((cbCardCount==2)&&(cbWeaveCount==MAX_WEAVE));

		//牌眼判断
		for (BYTE i=0;i<MAX_INDEX;i++)
		{
			// isValidDuiZi
			if (cbCardIndex[i]==2 || (IsValidCard(SwitchToCardData(m_cbMagicIndex)) && i != m_cbMagicIndex && cbCardIndex[m_cbMagicIndex]+cbCardIndex[i]==2))
			{
				//变量定义
				tagAnalyseItem AnalyseItem;
				ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

				//设置结果
				for (BYTE j=0;j<cbWeaveCount;j++)
				{
					AnalyseItem.wWeaveKind[j]=WeaveItem[j].wWeaveKind;
					AnalyseItem.cbCenterCard[j]=WeaveItem[j].cbCenterCard;
					CopyMemory(AnalyseItem.cbCardData[j],WeaveItem[j].cbCardData,sizeof(WeaveItem[j].cbCardData));
				}
				if(cbCardIndex[i] < 2 || i == m_cbMagicIndex)
					AnalyseItem.bMagicEye = true;
				else AnalyseItem.bMagicEye = false;
				AnalyseItem.cbCardEye=cbCardIndex[i]==0?SwitchToCardData(cbCardIndex[m_cbMagicIndex]):SwitchToCardData(i);

				if( isPossibleHu(cbCurrentCard, &AnalyseItem, cbWeaveCount) ) {
					//插入结果
					AnalyseItemArray.Add(AnalyseItem);

					return true;
				}
			}
		}

		return false;
	}

	//拆分分析
	BYTE cbMagicCardIndex[MAX_INDEX];
	CopyMemory(cbMagicCardIndex,cbCardIndex,sizeof(cbMagicCardIndex));

	//如果有财神
	BYTE cbMagicCardCount = 0;
	BYTE cbTempMagicCount = 0;

	if(m_cbMagicIndex != INVAILD_CARD_INDEX)
	{
		cbMagicCardCount = cbCardIndex[m_cbMagicIndex];
		//如果财神有代替牌，财神与代替牌转换
		if(INDEX_REPLACE_CARD != MAX_INDEX)
		{
			cbMagicCardIndex[m_cbMagicIndex] = cbMagicCardIndex[INDEX_REPLACE_CARD];
			cbMagicCardIndex[INDEX_REPLACE_CARD] = cbMagicCardCount;
		}
	}

	if (cbCardCount>=3)
	{
		for (BYTE i=0;i<MAX_INDEX-MAX_HUA_INDEX;i++)
		{
			//同牌判断
			//如果是财神,并且财神数小于3,则不进行组合
			if(cbMagicCardIndex[i] >= 3 || (cbMagicCardIndex[i]+cbMagicCardCount >= 3 &&
				((INDEX_REPLACE_CARD!=MAX_INDEX && i != INDEX_REPLACE_CARD) || (INDEX_REPLACE_CARD==MAX_INDEX && i != m_cbMagicIndex))))
			{
				int nTempIndex = cbMagicCardIndex[i];
				do
				{
					ASSERT(cbKindItemCount < CountArray(KindItem));
					BYTE cbIndex = i;
					BYTE cbCenterCard = SwitchToCardData(i);
					//如果是财神且财神有代替牌,则换成代替牌
					if(i == m_cbMagicIndex && INDEX_REPLACE_CARD != MAX_INDEX)
					{
						cbIndex = INDEX_REPLACE_CARD;
						cbCenterCard = SwitchToCardData(INDEX_REPLACE_CARD);
					}
					TempKindItem.wWeaveKind=WIK_PENG;
					TempKindItem.cbCenterCard=cbCenterCard;
					TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
					TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
					TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
					AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);


					//当前索引牌未与财神组合 且财神个数不为0 
					if(nTempIndex>=3 && cbMagicCardCount >0)
					{
						--nTempIndex;
						//1个财神与之组合
						TempKindItem.wWeaveKind=WIK_PENG;
						TempKindItem.cbCenterCard=cbCenterCard;
						TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
						TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
						TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
						AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);

						//两个财神与之组合
						if(cbMagicCardCount>1)
						{
							TempKindItem.wWeaveKind=WIK_PENG;
							TempKindItem.cbCenterCard=cbCenterCard;
							TempKindItem.cbValidIndex[0] = nTempIndex>0?cbIndex:m_cbMagicIndex;
							TempKindItem.cbValidIndex[1] = nTempIndex>1?cbIndex:m_cbMagicIndex;
							TempKindItem.cbValidIndex[2] = nTempIndex>2?cbIndex:m_cbMagicIndex;
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}

						++nTempIndex;
					}

					//如果是财神,则退出
					if(i == INDEX_REPLACE_CARD || ((i == m_cbMagicIndex) && INDEX_REPLACE_CARD == MAX_INDEX))
						break;

					nTempIndex -= 3;
					//如果刚好搭配全部，则退出
					if(nTempIndex == 0) break;

				}while(nTempIndex+cbMagicCardCount >= 3);
			}

			//连牌判断 // 顺字牌
			if (i<3*9 && ((i%9)<7))
			{
				//只要财神牌数加上3个顺序索引的牌数大于等于3,则进行组合
				if(cbMagicCardCount+cbMagicCardIndex[i]+cbMagicCardIndex[i+1]+cbMagicCardIndex[i+2] >= 3)
				{
					BYTE cbIndex[3] = { cbMagicCardIndex[i],cbMagicCardIndex[i+1],cbMagicCardIndex[i+2] };

					if(cbIndex[0]+cbIndex[1]+cbIndex[2]==0) continue;

					int nMagicCountTemp;
					nMagicCountTemp = cbMagicCardCount;

					BYTE cbValidIndex[3];
					while(nMagicCountTemp+cbIndex[0]+cbIndex[1]+cbIndex[2] >= 3)
					{
						for(BYTE j = 0; j < CountArray(cbIndex); j++)
						{
							if(cbIndex[j] > 0) 
							{
								cbIndex[j]--;
								cbValidIndex[j] = ((i+j==m_cbMagicIndex) && INDEX_REPLACE_CARD!=MAX_INDEX)?INDEX_REPLACE_CARD:i+j;
							}
							else 
							{
								nMagicCountTemp--;
								cbValidIndex[j] = m_cbMagicIndex;														
							}
						}
						if(nMagicCountTemp >= 0)
						{
							ASSERT(cbKindItemCount < CountArray(KindItem));
							TempKindItem.wWeaveKind=WIK_LEFT;
							TempKindItem.cbCenterCard=SwitchToCardData(i);
							CopyMemory(TempKindItem.cbValidIndex,cbValidIndex,sizeof(cbValidIndex));
							AddKindItem(TempKindItem, KindItem, cbKindItemCount, bMagicThree);
						}
						else break;
					}
				}
			}
		}
	}

	//组合分析
	if (cbKindItemCount>=cbLessKindItem)
	{
		ASSERT(27*2+28+16 >= cbKindItemCount);
		//变量定义
		BYTE cbCardIndexTemp[MAX_INDEX];
		ZeroMemory(cbCardIndexTemp,sizeof(cbCardIndexTemp));

		//变量定义
		BYTE cbIndex[MAX_WEAVE];
		for(BYTE i = 0; i < CountArray(cbIndex); i++)
			cbIndex[i] = i;

		tagKindItem * pKindItem[MAX_WEAVE];
		ZeroMemory(&pKindItem,sizeof(pKindItem));

		tagKindItem KindItemTemp[CountArray(KindItem)];

		//开始组合
		do
		{
			//如果四个组合中的混牌大于手上的混牌个数则重置索引
			cbTempMagicCount = 0;
			for(int i=0;i<cbLessKindItem;i++) cbTempMagicCount +=  KindItem[cbIndex[i]].cbMagicCount; 
			if(cbTempMagicCount <= cbMagicCardCount)
			{
				//设置变量
				CopyMemory(cbCardIndexTemp,cbCardIndex,sizeof(cbCardIndexTemp));
				CopyMemory(KindItemTemp,KindItem,sizeof(KindItem));
	
				for (BYTE i=0;i<cbLessKindItem;i++)
					pKindItem[i]=&KindItemTemp[cbIndex[i]];


				//数量判断
				bool bEnoughCard=true;

				for (BYTE i=0;i<cbLessKindItem*3;i++)
				{
					//存在判断
					BYTE cbCardIndex=pKindItem[i/3]->cbValidIndex[i%3]; 
					if (cbCardIndexTemp[cbCardIndex]==0)
					{
						if(m_cbMagicIndex != INVAILD_CARD_INDEX && cbCardIndexTemp[m_cbMagicIndex] > 0)
						{											
							pKindItem[i/3]->cbValidIndex[i%3] = m_cbMagicIndex;
							cbCardIndexTemp[m_cbMagicIndex]--;
						}
						else
						{
							bEnoughCard=false;
							break;
						}
					}
					else cbCardIndexTemp[cbCardIndex]--;
				}

				//胡牌判断
				if (bEnoughCard==true)
				{
					//牌眼判断
					BYTE cbCardEye=0;
					bool bMagicEye = false;
					if(GetCardCount(cbCardIndexTemp) == 2)
					{
						if(m_cbMagicIndex != INVAILD_CARD_INDEX && cbCardIndexTemp[m_cbMagicIndex]==2)
						{
							cbCardEye = SwitchToCardData(m_cbMagicIndex);
							bMagicEye = true;
						}
						else
						{
							for (BYTE i=0;i<MAX_INDEX;i++)
							{
								if (cbCardIndexTemp[i]==2)
								{
									cbCardEye=SwitchToCardData(i);
									if(m_cbMagicIndex != INVAILD_CARD_INDEX && i == m_cbMagicIndex) 
									{
										bMagicEye = true;
									}
									break;
								}
								else if(i!=m_cbMagicIndex && m_cbMagicIndex != INVAILD_CARD_INDEX && cbCardIndexTemp[i]+cbCardIndexTemp[m_cbMagicIndex]==2)
								{
									cbCardEye = SwitchToCardData(i);
									bMagicEye = true;
									break;
								}
							}
						}
					}

					//组合类型
					if (cbCardEye!=0)
					{
						//变量定义
						tagAnalyseItem AnalyseItem;
						ZeroMemory(&AnalyseItem,sizeof(AnalyseItem));

						//设置组合
						for (BYTE i=0;i<cbWeaveCount;i++)
						{
							AnalyseItem.wWeaveKind[i]=WeaveItem[i].wWeaveKind;
							AnalyseItem.cbCenterCard[i]=WeaveItem[i].cbCenterCard;
							CopyMemory(AnalyseItem.cbCardData[i],WeaveItem[i].cbCardData,sizeof(WeaveItem[i].cbCardData));
							//GetWeaveCard(WeaveItem[i].wWeaveKind,WeaveItem[i].cbCenterCard,AnalyseItem.cbCardData[i]);
						}

						//设置牌型
						for (BYTE i=0;i<cbLessKindItem;i++) 
						{
							AnalyseItem.wWeaveKind[i+cbWeaveCount]=pKindItem[i]->wWeaveKind;
							AnalyseItem.cbCenterCard[i+cbWeaveCount]=pKindItem[i]->cbCenterCard;
							AnalyseItem.cbCardData[cbWeaveCount+i][0] = SwitchToCardData(pKindItem[i]->cbValidIndex[0]);
							AnalyseItem.cbCardData[cbWeaveCount+i][1] = SwitchToCardData(pKindItem[i]->cbValidIndex[1]);
							AnalyseItem.cbCardData[cbWeaveCount+i][2] = SwitchToCardData(pKindItem[i]->cbValidIndex[2]);
						}

						//设置牌眼
						AnalyseItem.cbCardEye=cbCardEye;
						AnalyseItem.bMagicEye = bMagicEye;

						if( isPossibleHu(cbCurrentCard, &AnalyseItem, cbWeaveCount) ) {
							//插入结果
							AnalyseItemArray.Add(AnalyseItem);
						}
					}
				}
			}

			//设置索引
			if (cbIndex[cbLessKindItem-1]==(cbKindItemCount-1))
			{
				BYTE i;
				for (i=cbLessKindItem-1;i>0;i--)
				{
					if ((cbIndex[i-1]+1)!=cbIndex[i])
					{
						BYTE cbNewIndex=cbIndex[i-1];
						for (BYTE j=(i-1);j<cbLessKindItem;j++) 
							cbIndex[j]=cbNewIndex+j-i+2;
						break;
					}
				}
				if (i==0)
					break;
			}
			else
				cbIndex[cbLessKindItem-1]++;

		} while (true);
	}

	if(AnalyseItemArray.GetCount()>0){
		int i=0;
	}
	return (AnalyseItemArray.GetCount()>0);
}

bool CGameLogic::isPossibleHu(BYTE cbHuCard, const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount) {
	if( !CheckHuFormatStyle(pAnalyseItem) ) return false;

	if( !CheckYaoJiuFormat(pAnalyseItem)) return false;

	if( m_CustomRule.bEnabled_JiaHu ){
		if( !CheckJiaHuStyle(cbHuCard, pAnalyseItem, cbWeaveCount) )
			return false;
	}
	return true;
}

bool CGameLogic::CheckYaoJiuFormat(const tagAnalyseItem *pAnalyseItem){
	if( isYaoJiuCard(pAnalyseItem->cbCardEye) ) return true;
	if( isYaoJiuSubstitute(pAnalyseItem->cbCardEye) ) return true;

	for( int i=0; i<MAX_WEAVE; i++){
		if( pAnalyseItem->wWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT) ){
			for( int j=0; j<3; j++){
				if( isYaoJiuCard(pAnalyseItem->cbCardData[i][j]) ) return true;
			}
		} else {
			if( isYaoJiuCard(pAnalyseItem->cbCenterCard[i]) ) return true;
			if( isYaoJiuSubstitute(pAnalyseItem->cbCenterCard[i]) ) return true;
		}
	}

	BYTE magicCard = SwitchToCardData(m_cbMagicIndex);

	//⑤	当自己手中有会牌时，如果缺少幺九，可以代替幺九，但则不再可以替代其他牌。 
	bool substituteYaoJiuFlag = false;
	if( pAnalyseItem->bMagicEye && pAnalyseItem->cbCardEye!=magicCard) return false;
	if( pAnalyseItem->bMagicEye && pAnalyseItem->cbCardEye==magicCard) substituteYaoJiuFlag = true;
	for( int i=0; i<MAX_WEAVE; i++){
		if( pAnalyseItem->cbCardData[i][2]==magicCard ) {
			if( pAnalyseItem->cbCardData[i][0]==magicCard && pAnalyseItem->cbCardData[i][1]==magicCard ){ substituteYaoJiuFlag = true; continue;}

			if( (pAnalyseItem->cbCardData[i][0]&MASK_VALUE)==0x02 && (pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x03){ substituteYaoJiuFlag = true; continue;}
			if( (pAnalyseItem->cbCardData[i][0]&MASK_VALUE)==0x03 && (pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x02){ substituteYaoJiuFlag = true; continue;}
			if( (pAnalyseItem->cbCardData[i][0]&MASK_VALUE)==0x07 && (pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x08){ substituteYaoJiuFlag = true; continue;}
			if( (pAnalyseItem->cbCardData[i][0]&MASK_VALUE)==0x08 && (pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x07){ substituteYaoJiuFlag = true; continue;}

			return false;
		}
	}

	return substituteYaoJiuFlag;
}

bool CGameLogic::isYaoJiuCard(BYTE card){
	//if( card == SwitchToCardData(m_cbMagicIndex) ) return false;

	switch( card ){
		case 0x01:
		case 0x11:
		case 0x21:
		case 0x09:
		case 0x19:
		case 0x29:
			return true;
	}

	return false;
}

bool CGameLogic::isYaoJiuSubstitute(BYTE card){
	if( card == SwitchToCardData(m_cbMagicIndex) ) return false;

	switch( card ){
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			return true;
	}

	return false;
}

bool CGameLogic::CheckHuFormatStyle(const tagAnalyseItem *pAnalyseItem){
	bool bOnlyShunzi = true;

	// 111 111 111 111 11
	// 123 111 111 111 11
	// 123 123 111 111 11
	// 123 123 123 111 11
	for(int i=0; i<MAX_WEAVE; i++){
		if( !(pAnalyseItem->wWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))!=0 ){
			bOnlyShunzi = false;
			return true;
		}
	}

	{ //  123 123 123 123 11 ( in this case, 11 must be Duizi (对子）of 中， 发， 白  123-Shunzi(顺子) )
		if( bOnlyShunzi && (pAnalyseItem->cbCardEye==SwitchToCardData(m_cbMagicIndex)) )
			return true;

		if( bOnlyShunzi && (pAnalyseItem->cbCardEye>=0x35 && pAnalyseItem->cbCardEye<=0x37) ){ // 中， 发， 白
			return true;
		}
	}

	return false;
}

bool CGameLogic::CheckJiaHuStyle(BYTE cbHuCard, const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount){
	if( IsShouBaYi(cbHuCard, pAnalyseItem, cbWeaveCount) ) return true;
	if( IsJiaHuFormat(cbHuCard, pAnalyseItem, cbWeaveCount)) return true;

	return false;
}

/*
// 胡法分析函数
*/

//碰碰和
bool CGameLogic::IsPengPeng(const tagAnalyseItem *pAnalyseItem)
{
	for(BYTE i = 0; i < CountArray(pAnalyseItem->wWeaveKind); i++)
	{
		if(pAnalyseItem->wWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT))
			return false;
	}
	return true;
}

//是否麻七系列
bool CGameLogic::IsMaQi(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	if(cbWeaveCount!=0)
		return false;

	BYTE cbGang = 0;
	BYTE cbMagicCount = m_cbMagicIndex != INVAILD_CARD_INDEX ? cbCardIndex[m_cbMagicIndex] : 0;
	//变量定义
	for(BYTE i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i] != 0 && i != m_cbMagicIndex)
		{			
			if(cbCardIndex[i] % 2 == 1)
			{
				if(cbMagicCount >= 1)
				{
					cbMagicCount--;
				}
				else
				{
					return false;//有非对子，跳出
				}
			}

			cbGang+= cbCardIndex[i] / 4;
		}
	}

	return false;
}
//十三烂系列
bool CGameLogic::IsShiSanYao(const BYTE cbCardIndex[MAX_INDEX],BYTE cbWeaveCount)
{
	//组合判断
	if (cbWeaveCount!=0) return false;

	for(int i=0;i<MAX_INDEX;i++)
	{
		if(cbCardIndex[i]>=2)//不能有重复牌
			return false;
	}

	for(int j=0;j<3;j++)
	{
		for(int i=0;i<9-2;i++)
		{
			int index = j*9+i;
			if(cbCardIndex[index]+cbCardIndex[index+1]+cbCardIndex[index+2]>1)
			{
				//if(cbCardIndex[index+1]>0 || cbCardIndex[index+2]>0)//间隔必须>=3
					return false;
			}
		}
	}

	return true;
}

//手把一
bool CGameLogic::IsShouBaYi(BYTE cbHuPai, const tagAnalyseItem *pAnalyseItem, BYTE nWeaveCount)
{
	if( nWeaveCount < 4 ) return false;

	for( int i=0; i<nWeaveCount; i++){
		if( pAnalyseItem->wWeaveKind[i] != WIK_PENG ){
			return false;
		}
	}

	return true;
}

//夹胡
bool CGameLogic::IsJiaHuFormat(BYTE cbHuPai, const tagAnalyseItem *pAnalyseItem, BYTE nWeaveCount)
{
	for(BYTE i = nWeaveCount; i < MAX_WEAVE; i++)
	{
		if(pAnalyseItem->wWeaveKind[i]&(WIK_LEFT|WIK_CENTER|WIK_RIGHT)){
			if( cbHuPai == pAnalyseItem->cbCardData[i][1] )
				return true;

			if( (cbHuPai&MASK_VALUE)==0x03 ){
				if( ((pAnalyseItem->cbCardData[i][0]&MASK_VALUE)==0x01)
					&& ((pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x02)
					&& (pAnalyseItem->cbCardData[i][2]==cbHuPai))
					return true;
			}

			if( (cbHuPai&MASK_VALUE)==0x09 ){
				if( (pAnalyseItem->cbCardData[i][0]==cbHuPai)
					&& ((pAnalyseItem->cbCardData[i][1]&MASK_VALUE)==0x08)
					&& ((pAnalyseItem->cbCardData[i][2]&MASK_VALUE)==0x09))
					return true;
			}
		}
	}

	return false;
}

//鸡胡
bool CGameLogic::IsJiHu(const tagAnalyseItem *pAnalyseItem)
{
	bool bPeng = false,bLian = false;
	for(BYTE i = 0; i < CountArray(pAnalyseItem->wWeaveKind); i++)
	{
		if(pAnalyseItem->wWeaveKind[i]&(WIK_PENG|WIK_GANG))
			bPeng = true;
		else bLian = true;
	}

	return bPeng&&bLian;
}

//平胡
bool CGameLogic::IsPingHu(const tagAnalyseItem *pAnalyseItem)
{

	//检查组合
	for(BYTE i = 0; i < CountArray(pAnalyseItem->wWeaveKind); i++)
	{
		if(pAnalyseItem->wWeaveKind[i]&(WIK_PENG|WIK_GANG)) return false;
	}

	return true;
}

//清一色
bool CGameLogic::IsQingYiSe(const tagAnalyseItem * pAnalyseItem)
{

	//参数校验
	if(pAnalyseItem==NULL) return false;

	//变量定义
	BYTE  cbCardColor=pAnalyseItem->cbCardEye&MASK_COLOR;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		if((pAnalyseItem->cbCenterCard[i]&MASK_COLOR) != cbCardColor) 
		{
			return false;
		}
	}

	return true;
}

//混一色
bool CGameLogic::IsHunYiSe(const tagAnalyseItem * pAnalyseItem)
{

	//参数校验
	if(pAnalyseItem==NULL) return false;

	//变量定义
	BYTE  cbCardColor=(pAnalyseItem->cbCardEye&MASK_COLOR)>>4;
	ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
	BYTE cbColorCount[4] = {0};
	cbColorCount[cbCardColor] = 1;
	for(BYTE i=0;i<MAX_WEAVE;i++)
	{
		cbCardColor=((pAnalyseItem->cbCenterCard[i])&MASK_COLOR)>>4;
		ASSERT(cbCardColor >= 0 && cbCardColor <= 3);
		if(0 == cbColorCount[cbCardColor])
		{
			cbColorCount[cbCardColor] = 1;
		}
	}

	if(cbColorCount[0] + cbColorCount[1] + cbColorCount[2] == 1 && cbColorCount[3] == 1)
	{
		return true;
	}

	return false;
}


//验证某玩家是否可以做特殊杠操作
bool CGameLogic::IsSpGangOK(const BYTE cbCardIndex[MAX_INDEX], DWORD dwOpCode)
{
	switch (dwOpCode)
	{
	case WIK_WIND:
		{
			int nWindCondition;
			BYTE cbWindArray[] = {0x31, 0x32, 0x33, 0x34};	//【东】【南】【西】【北】
			nWindCondition = 0;
			for (int i=0; i<4; i++)
			{
				if(SwitchToCardIndex(cbWindArray[i]) == m_cbMagicIndex) {
					nWindCondition--;
					break;
				}
				if (cbCardIndex[SwitchToCardIndex(cbWindArray[i])]<1)
				{
					nWindCondition--;
					break;
				}
			}
			return (nWindCondition>=0);
		}
	case WIK_ARROW:
		{
			int nArrowCondition;
			BYTE cbArrowArray[] = {0x35, 0x36, 0x37};		//【中】【发】【白】
			nArrowCondition = 0;
			for (int i=0; i<3; i++)
			{
				if(SwitchToCardIndex(cbArrowArray[i]) == m_cbMagicIndex) {
					nArrowCondition--;
					break;
				}
				if (cbCardIndex[SwitchToCardIndex(cbArrowArray[i])]<1)
				{
					nArrowCondition--;
					break;
				}
			}
			return (nArrowCondition>=0);
		}
	}

	return false;
}

bool CGameLogic::IsChaseArrow(BYTE cbProvidedCardIndex,const tagWeaveItem WeaveItem[], BYTE cbWeaveICount,DWORD dwOpCode)
{
	switch (dwOpCode)
	{
	case WIK_CHASEWIND:											//【东】【南】【西】【北】
		{
			for (int i = 0;i<cbWeaveICount;i++)
			{
				if (WeaveItem[i].wWeaveKind ==WIK_WIND  && (cbProvidedCardIndex>=27 && cbProvidedCardIndex<=30))
				{
					return true;
				}
			}
			break;
		}
	case WIK_CHASEARROW:											//【中】【发】【白】
		{
			for (int i = 0;i<cbWeaveICount;i++)
			{
				if (WeaveItem[i].wWeaveKind == WIK_ARROW && (cbProvidedCardIndex>=31 && cbProvidedCardIndex<=33))	
				{
					return true;
				}
			}
			break;
		}
	default:
		{
			break;
		}
	}
	return false;
}

bool CGameLogic::IsPossibleChaseArrow(const BYTE cbHandCards[MAX_INDEX],const tagWeaveItem WeaveItem[], BYTE cbWeaveICount,DWORD dwOpCode){
	switch (dwOpCode)
	{
	case WIK_CHASEWIND:											//【东】【南】【西】【北】
		{
			for (int i = 0;i<cbWeaveICount;i++)
			{
				if (WeaveItem[i].wWeaveKind ==WIK_WIND) {
					if( cbHandCards[27] > 0
						|| cbHandCards[28] > 0
						|| cbHandCards[29] > 0
						|| cbHandCards[30] > 0)
						return true;
					else
						return false;
				}
			}
			break;
		}
	case WIK_CHASEARROW:											//【中】【发】【白】
		{
			for (int i = 0;i<cbWeaveICount;i++)
			{
				if (WeaveItem[i].wWeaveKind == WIK_ARROW)	
				{
					if( cbHandCards[31] > 0
						|| cbHandCards[32] > 0
						|| cbHandCards[33] > 0)
						return true;
					else
						return false;
				}
			}
			break;
		}
	}
	return false;
}

//从手中扣掉特殊杠的牌组
bool CGameLogic::TakeOutSpGang(BYTE cbCardIndex[MAX_INDEX], DWORD dwOpCode)
{
	if (!IsSpGangOK(cbCardIndex, dwOpCode))
	{
		return false;
	}

	switch (dwOpCode)
	{
	case WIK_WIND:
		{
			BYTE cbWindArray[] = {0x31, 0x32, 0x33, 0x34};	//【东】【南】【西】【北】
			for (int i=0; i<4; i++)
			{
				cbCardIndex[SwitchToCardIndex(cbWindArray[i])]--;
			}
			return true;
		}
	case WIK_ARROW:
		{
			BYTE cbArrowArray[] = {0x35, 0x36, 0x37};		//【中】【发】【白】
			for (int i=0; i<3; i++)
			{
				cbCardIndex[SwitchToCardIndex(cbArrowArray[i])]--;
			}
			return true;
		}

	default:
		{
			break;
		}
	}

	return false;
}

// 从手中扣掉长毛杠的牌组
bool CGameLogic::TakeOutCHMGang(BYTE cbCardIndex[MAX_INDEX],BYTE cbCernterCard)
{
	cbCardIndex[SwitchToCardIndex(cbCernterCard)]--;
	if (cbCardIndex[SwitchToCardIndex(cbCernterCard)]<0)
	{
		return false;
	}
	return true;
}

//按默认值初始化定制规则
void CGameLogic::InitCustomRule()
{
	m_CustomRule.cbTimeOutCard=20;
	m_CustomRule.cbTimeStartGame=30;
	m_CustomRule.cbTimeOperateCard=10;

	//游戏控制
	m_CustomRule.cbMaCount=2;
	m_CustomRule.cbPlayerCount=4;

	m_CustomRule.cbInningsCount_cy	= DEFAULT_INNINGS_COUNT;
	m_CustomRule.bEnabled_DianPao	= true;
	m_CustomRule.bEnabled_FengGang= true;
	m_CustomRule.bEnabled_HuiPai	= true;
	m_CustomRule.bEnabled_BaoPai	= true;
	m_CustomRule.bEnabled_ZhanLiHu= true;
	m_CustomRule.bEnabled_JiaHu	= true;
	m_CustomRule.bEnabled_ChangMaoGang = true;
}

//设置定制规则
void CGameLogic::SetCustomRule(tagCustomRule *pRule)
{
	if (pRule == NULL)
	{
		return;
	}

	CopyMemory(&m_CustomRule, pRule, sizeof(tagCustomRule));
}

//////////////////////////////////////////////////////////////////////////////////
