#pragma once


#include "JavaInstructItem.h"


// 记录 每个 Catch Item 的对象
class CCatchItemNode {
public:
	CCatchItemNode() {};
	~CCatchItemNode() {};
	CString strCatchLabel;
	CString strCatchType;

	CCatchItemNode(const CCatchItemNode & _c) {
		this->strCatchLabel = _c.strCatchLabel;
		this->strCatchType = _c.strCatchType;
	};

	CCatchItemNode & operator = (const CCatchItemNode & _c) {
		this->strCatchLabel = _c.strCatchLabel;
		this->strCatchType = _c.strCatchType;
		return *this;
	};

};

// 记录 Catch 的结构体
class CTryCatchNode {
public:
	CTryCatchNode() {};
	~CTryCatchNode() {};

	CString strName;							// 当前的段名
	CString strStartLabel;						// Try开始的段
	CString strEndLabel;						// Try结束的位置

	std::vector<CCatchItemNode> listCatch;		// Catch的列表
	CString strFinallyLabel;					// FinallyLabel的位置

	CTryCatchNode(const CTryCatchNode & _c) {
		this->strName = _c.strName;
		this->strStartLabel = _c.strStartLabel;
		this->strEndLabel = _c.strEndLabel;
		this->listCatch = _c.listCatch;
		this->strFinallyLabel = _c.strFinallyLabel;
	};

	CTryCatchNode & operator = (const CTryCatchNode & _c) {
		this->strName = _c.strName;
		this->strStartLabel = _c.strStartLabel;
		this->strEndLabel = _c.strEndLabel;
		this->listCatch = _c.listCatch;
		this->strFinallyLabel = _c.strFinallyLabel;
		return *this;
	};

	void clear(void) {
		this->strName.Empty();
		this->strStartLabel.Empty();
		this->strEndLabel.Empty();
		this->listCatch.clear();
		this->strFinallyLabel.Empty();
	}
};

class CJavaMethodSegment
{
public:
	~CJavaMethodSegment() { };
	enum eSegmentNextType {
		nextNormal,		// 通过顺延过去
		nextGoto,		// 通过跳转过去
		nextIf,			// 通过if过去
		nextSwitch,		// 通过switch结构过去
		nextReturn,		// 通过return返回的段
		nextTryCatch,	// 通过TryCatch分支
		nextThrow,		// 这个也是结束代码
	};
	enum eSegmentNextType SegmentNextType;
	std::vector<int> listInstSn;				// 包含的指令集合
	std::vector<CString> listSegName;			// 名字，可能会有多个名字
	std::vector<CString> listStrNext;			// 下一个段的名称

	CJavaMethodSegment()
	{
		listInstSn.clear();
		listStrNext.clear();
		listSegName.clear();
		SegmentNextType = nextNormal;
	};

	CJavaMethodSegment(const CJavaMethodSegment & _c) {
		this->listInstSn = _c.listInstSn;
		this->listStrNext = _c.listStrNext;
		this->listSegName = _c.listSegName;
		this->SegmentNextType = _c.SegmentNextType;
	};

	CJavaMethodSegment & operator = (const CJavaMethodSegment & _c) {
		this->listInstSn = _c.listInstSn;
		this->listStrNext = _c.listStrNext;
		this->listSegName = _c.listSegName;
		this->SegmentNextType = _c.SegmentNextType;
		return *this;
	};

	void clear(void) {
		listInstSn.clear();
		listStrNext.clear();
		listSegName.clear();
		SegmentNextType = nextNormal;
	}
};

class CJavaClass;

class CValueEqString {

public:
	~CValueEqString() {}
	CValueEqString() {}
	CString strValueName;
	CString strExpression;
	CValueEqString(const CValueEqString & _c) {
		this->strValueName = _c.strValueName;
		this->strExpression = _c.strExpression;
	};
	CValueEqString & operator = (const CValueEqString & _c) {
		this->strValueName = _c.strValueName;
		this->strExpression = _c.strExpression;
		return *this;
	};
	void clear() {
		this->strValueName.Empty();
		this->strExpression.Empty();
	}
};


class CSortNumber {
public:
	CSortNumber() {};
	~CSortNumber() {};
	int nData;			// 这个是数据
	int nKey;			// 这个适用于排序的Key
	CSortNumber(const CSortNumber & _c) {
		this->nData = _c.nData;
		this->nKey = _c.nKey;
	};
	CSortNumber & operator = (const CSortNumber & _c) {
		this->nData = _c.nData;
		this->nKey = _c.nKey;
		return *this;
	};
};


class CJavaSwitchItem {
public:
	CJavaSwitchItem() { };
	~CJavaSwitchItem() {	};
	unsigned int nData;
	CString strSegmentName;
	CJavaSwitchItem(const CJavaSwitchItem & _c) {
		this->nData = _c.nData;
		this->strSegmentName = _c.strSegmentName;
	};
	CJavaSwitchItem & operator = (const CJavaSwitchItem & _c) {
		this->nData = _c.nData;
		this->strSegmentName = _c.strSegmentName;
		return *this;
	};
};



class CJavaPackedSwitchStruct
{
public:
	CJavaPackedSwitchStruct() {};
	~CJavaPackedSwitchStruct() {};
	CString strSwitchName;
	std::vector<CJavaSwitchItem> listItem;
	CJavaPackedSwitchStruct(const CJavaPackedSwitchStruct & _c) {
		this->listItem = _c.listItem;
		this->strSwitchName = _c.strSwitchName;
	};
	CJavaPackedSwitchStruct & operator = (const CJavaPackedSwitchStruct & _c) {
		this->listItem = _c.listItem;
		this->strSwitchName = _c.strSwitchName;
		return *this;
	}
};

class CLoopPair {
public:
	int nLoopStart;		// loop 开始的Index;
	int nLoopEnd;		// loop 结束的Index;
	CLoopPair() {};
	~CLoopPair() {};
	CLoopPair(const CLoopPair & _c) {
		this->nLoopStart = _c.nLoopStart;
		this->nLoopEnd = _c.nLoopEnd;
	};
	CLoopPair & operator = (const CLoopPair & _c) {
		this->nLoopStart = _c.nLoopStart;
		this->nLoopEnd = _c.nLoopEnd;
		return *this;
	};
};

class CMethodParam {
public:
	CString strReg;
	CString strTypeName;
	CString strValueName;

	CMethodParam() {};
	~CMethodParam() {};

	CMethodParam(const CMethodParam & _c) {
		this->strReg = _c.strReg;
		this->strTypeName = _c.strTypeName;
		this->strValueName = _c.strValueName;
	};

	CMethodParam & operator = (const CMethodParam & _c) {
		this->strReg = _c.strReg;
		this->strTypeName = _c.strTypeName;
		this->strValueName = _c.strValueName;
		return *this;
	};
};

// 宏指令列表
class CMarcoCodeItem {

public:
	static int nSeed;
	static int GetSeed(void) {
		return nSeed++;
	}
	static void ResetSeed(void) {
		nSeed = 10000;
	}
	int nMacroIndex;
	std::vector<CString> listMacro;
	CMarcoCodeItem() {};
	~CMarcoCodeItem() {};
	CMarcoCodeItem(const CMarcoCodeItem & _c) {
		this->nMacroIndex = _c.nMacroIndex;
		this->listMacro = _c.listMacro;
	};
	CMarcoCodeItem & operator = (const CMarcoCodeItem & _c) {
		this->nMacroIndex = _c.nMacroIndex;
		this->listMacro = _c.listMacro;
		return *this;
	};
	void clear(void) {
		nMacroIndex = GetSeed();
	}
};

class CVariableItem {
public:
	CString strTypeName;			// 类型名称
	CString strName;				// 变量的名字
	int nShowedCount;				// 被代码显示部分展示了多少次？
	int nFirstDefinedInstIndex;		// 被确认的需要被定义在哪个指令上？

	CVariableItem() {
		strTypeName.Empty();
		strName.Empty();
		nShowedCount = 0;
		nFirstDefinedInstIndex = -1;
	};
	void clear(void) {
		strTypeName.Empty();
		strName.Empty();
		nShowedCount = 0;
		nFirstDefinedInstIndex = -1;
	}
	~CVariableItem() {};
	CVariableItem(const CVariableItem & _c) {
		this->strTypeName = _c.strTypeName;
		this->strName = _c.strName;
		this->nShowedCount = _c.nShowedCount;
		this->nFirstDefinedInstIndex = _c.nFirstDefinedInstIndex;
	};
	CVariableItem & operator = (const CVariableItem & _c) {
		this->strTypeName = _c.strTypeName;
		this->strName = _c.strName;
		this->nShowedCount = _c.nShowedCount;
		this->nFirstDefinedInstIndex = _c.nFirstDefinedInstIndex;
		return *this;
	};
};

class CJavaMethod
{
public:
	CJavaMethod();
	~CJavaMethod();

	int nSerialNumberCnt;

	CString strClassName;									// 所属的Class名称
	CString strSuperClass;									// 父类

	CString strMethodInfo;									// Method定义信息

	CString strMethodName;									// 函数方法名
	CString strReturnType;									// 函数返回参数
	std::vector<CString> listParamType;						// 输入参数列表

	std::vector<CMethodParam> listInputParam;				// 输出参数清单

	std::vector<CString> listMethodAttr;					// 方法属性

	std::vector<CVariableItem> listVariableItem;			// 变量表示

	std::vector<CLoopPair> listLooper;                      // 循环体

	std::vector<CJavaInstructItem> listOriginalInstruct;	// 原始指令列表

	std::vector<CJavaMethodSegment> listMethodSegment;		// 指令段列表
	std::vector<CJavaPackedSwitchStruct> listPackedSwitch;	// Switch结构
	std::vector<CTryCatchNode> listCatchData;				// Try结构

	std::vector<int> listFullRunVector;						// 整个函数的指令依赖关系
	std::vector<int> listFullRunInstructionVector;			// 运行的完整指令依赖顺序

	std::vector<CString> listCppStringCode;					// 代码生成输出

	void InsertCppStringCode(int nLevel, CString strCode) {
		CString strTmpCode;
		for (int i = 0; i < nLevel; i++) {
			strTmpCode += CString("\t");
		}
		strTmpCode += strCode;
		listCppStringCode.push_back(strTmpCode);
	}

	// 宏指令
	std::vector<CMarcoCodeItem> listMarco;

	CJavaMethod(const CJavaMethod & _c) {
		this->listMethodAttr = _c.listMethodAttr;
		this->listParamType = _c.listParamType;
		this->strReturnType = _c.strReturnType;
		this->strMethodName = _c.strMethodName;
		this->listMethodSegment = _c.listMethodSegment;
		this->listPackedSwitch = _c.listPackedSwitch;
		this->listLooper = _c.listLooper;
		this->strSuperClass = _c.strSuperClass;
		this->strClassName = _c.strClassName;
		this->listMarco = _c.listMarco;
		this->listInputParam = _c.listInputParam;
		this->listOriginalInstruct = _c.listOriginalInstruct;
		this->listCatchData = _c.listCatchData;
		this->listCppStringCode = _c.listCppStringCode;
		this->strMethodInfo = _c.strMethodInfo;
		this->listFullRunVector = _c.listFullRunVector;
		this->listFullRunInstructionVector = _c.listFullRunInstructionVector;
		this->listVariableItem = _c.listVariableItem;
	};

	int AddInstructToOriginalList(CString strInst);
	void ProcessInstList(CJavaClass * pClass, std::vector<CString> listMethodInst);
	CString GetValueNameExpression(std::vector<CValueEqString> & listValueEqString, CString strValueName);
	CString SegmentNextGetOneLabel(std::vector<CString> listStrNext, CString strType);
	CRegVariablePair SegmentListRetrodictGetRegInfo(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex);
	void SegmentCodeTranslate(int nLevel, std::vector<int> listInstIndex, std::vector<int> listNewInst);
	CString SegmentListRetrodictGetExpression(std::vector<int> listInstIndex, int nInsArryIndex, CString strRegName, int nInstIndex);

	std::vector<CJavaInstructItem>::iterator GetOriginalInstruct(int nIndex);

	std::vector<CString> GetSignatureFromMethods(CString strFunction);
	CString GetReturnTypeFromMethods(CString strFunction);
	std::vector<CString> GetMethodAttribute(CString strFunction);
	CString GetMethodName(CString strFunction);

	void CreateSegmentLoopData(void);
	bool SegmentLoopRecursion(std::vector<int> listSegIndex, int nIndex);
	void SegmentVariableDefine(void);
	bool SegmentVariableRecursion(std::vector<int> listSegIndex, int nIndex);
	bool SegmentVariableSpreadRecursion(std::vector<int> listSegIndex, int nIndex);
	bool SegmentListInstRebuild(std::vector<int> listSegIndex);
	bool SegmentListInstSpread(std::vector<int> listSegIndex);

	std::vector<int> GetThreadFullRunningList(int nStartSegment, std::vector<int> listIndexSort, int nSegIndex);
	bool GetThreadFullRunningListRecursion(int nSegIndex, std::vector<std::vector<int>> & listArrayRunningIndex, std::vector<int> listRunningIndex);
	int MacroStructureRecursion(int nStartSeg, std::vector<int> listIndexSort, int nEndSeg);
	void TranslateMacroStructRecursion(std::vector<int> listOldInstList, int nLevel, int nMacroIndex);
	void ShowAbsMacroStructRecursion(std::vector<int> listOldInstList, int nLevel, int nMacroIndex);
	bool GetFullRunVectorRecursion(std::vector<int> listSegIndex, std::vector<std::vector<int>> & listArrayRunningIndex, int nIndex);

	void SegmentVariableSpread(void);
	CString GetCatchValueName(int nMacroIndex);

	void CreateSegmentList(std::vector<CString> listMethodInst);
	int CreateMacroCodeStruction(void);
	int GetSegmentIndexFromName(CString strSegmentName);
	void TranslateMacroCodeStruct(int nMacroIndex);
	static unsigned int GetDataFromHex(CString strHex);

	std::vector<int> GetFullRunVector(void);

	CRegVariablePair GetRegVariablePair(CString strRegInfo);


	CJavaMethod & operator = (const CJavaMethod & _c) {
		this->listMethodAttr = _c.listMethodAttr;
		this->listParamType = _c.listParamType;
		this->strReturnType = _c.strReturnType;
		this->strMethodName = _c.strMethodName;
		this->listMethodSegment = _c.listMethodSegment;
		this->listPackedSwitch = _c.listPackedSwitch;
		this->listLooper = _c.listLooper;
		this->strSuperClass = _c.strSuperClass;
		this->strClassName = _c.strClassName;
		this->listMarco = _c.listMarco;
		this->listInputParam = _c.listInputParam;
		this->listOriginalInstruct = _c.listOriginalInstruct;
		this->listCatchData = _c.listCatchData;
		this->listCppStringCode = _c.listCppStringCode;
		this->strMethodInfo = _c.strMethodInfo;
		this->listFullRunVector = _c.listFullRunVector;
		this->listFullRunInstructionVector = _c.listFullRunInstructionVector;
		this->listVariableItem = _c.listVariableItem;
		return *this;
	};

	// 设置这个变量可以显示计数, 没有找到也无所谓
	void TouchTheVariableItem(CString strVariableName) {
		for (unsigned int i = 0; i < listVariableItem.size(); i++) {
			if (listVariableItem[i].strName == strVariableName) {
				listVariableItem[i].nShowedCount++;
				break;
			}
		}
	}
};

