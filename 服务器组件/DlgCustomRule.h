#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"


#define DEFAULT_INNINGS_COUNT       16
#define INNINGS_COUNT_2             24

//////////////////////////////////////////////////////////////////////////////////

//配置结构
struct tagCustomRule
{
	//其他定义
	BYTE							cbMaCount;							//码数，1：一码全中，2-6：对应码数 // no need in Chaoyang
	BYTE							cbPlayerCount;						//人数 2-4						   //Always 4 in Chaoyang

	//时间定义
	BYTE							cbTimeOutCard;						//出牌时间
	BYTE							cbTimeOperateCard;					//操作时间
	BYTE							cbTimeStartGame;					//开始时间	
	
	// Added custome game settings for Chaoyang
	BYTE							cbInningsCount_cy;
	bool							bEnabled_DianPao;
	bool							bEnabled_FengGang;
	bool							bEnabled_HuiPai;
	bool							bEnabled_BaoPai;
	bool							bEnabled_ZhanLiHu;
	bool							bEnabled_JiaHu;
	bool							bEnabled_ChangMaoGang;
};

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class CDlgCustomRule : public CDialog
{
	//配置变量
protected:
	tagCustomRule					m_CustomRule;						//配置结构

	//函数定义
public:
	//构造函数
	CDlgCustomRule();
	//析构函数
	virtual ~CDlgCustomRule();

	//重载函数
protected:
	//配置函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

private:
	VOID CDlgCustomRule::OnClickChechBox(UINT nIDbutton);

	//功能函数
public:
	//更新控件
	bool FillDataToControl();
	//更新数据
	bool FillControlToData();

	//配置函数
public:
	//读取配置
	bool GetCustomRule(tagCustomRule & CustomRule);
	//设置配置
	bool SetCustomRule(tagCustomRule & CustomRule);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif