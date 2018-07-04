#pragma once


#include "JavaInstructItem.h"


// ��¼ ÿ�� Catch Item �Ķ���
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

// ��¼ Catch �Ľṹ��
class CTryCatchNode {
public:
	CTryCatchNode() {};
	~CTryCatchNode() {};

	CString strName;							// ��ǰ�Ķ���
	CString strStartLabel;						// Try��ʼ�Ķ�
	CString strEndLabel;						// Try������λ��

	std::vector<CCatchItemNode> listCatch;		// Catch���б�
	CString strFinallyLabel;					// FinallyLabel��λ��

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
		nextNormal,		// ͨ��˳�ӹ�ȥ
		nextGoto,		// ͨ����ת��ȥ
		nextIf,			// ͨ��if��ȥ
		nextSwitch,		// ͨ��switch�ṹ��ȥ
		nextReturn,		// ͨ��return���صĶ�
		nextTryCatch,	// ͨ��TryCatch��֧
		nextThrow,		// ���Ҳ�ǽ�������
	};
	enum eSegmentNextType SegmentNextType;
	std::vector<int> listInstSn;				// ������ָ���
	std::vector<CString> listSegName;			// ���֣����ܻ��ж������
	std::vector<CString> listStrNext;			// ��һ���ε�����

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
	int nData;			// ���������
	int nKey;			// ��������������Key
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
	int nLoopStart;		// loop ��ʼ��Index;
	int nLoopEnd;		// loop ������Index;
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

// ��ָ���б�
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
	CString strTypeName;			// ��������
	CString strName;				// ����������
	int nShowedCount;				// ��������ʾ����չʾ�˶��ٴΣ�
	int nFirstDefinedInstIndex;		// ��ȷ�ϵ���Ҫ���������ĸ�ָ���ϣ�

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

	CString strClassName;									// ������Class����
	CString strSuperClass;									// ����

	CString strMethodInfo;									// Method������Ϣ

	CString strMethodName;									// ����������
	CString strReturnType;									// �������ز���
	std::vector<CString> listParamType;						// ��������б�

	std::vector<CMethodParam> listInputParam;				// ��������嵥

	std::vector<CString> listMethodAttr;					// ��������

	std::vector<CVariableItem> listVariableItem;			// ������ʾ

	std::vector<CLoopPair> listLooper;                      // ѭ����

	std::vector<CJavaInstructItem> listOriginalInstruct;	// ԭʼָ���б�

	std::vector<CJavaMethodSegment> listMethodSegment;		// ָ����б�
	std::vector<CJavaPackedSwitchStruct> listPackedSwitch;	// Switch�ṹ
	std::vector<CTryCatchNode> listCatchData;				// Try�ṹ

	std::vector<int> listFullRunVector;						// ����������ָ��������ϵ
	std::vector<int> listFullRunInstructionVector;			// ���е�����ָ������˳��

	std::vector<CString> listCppStringCode;					// �����������

	void InsertCppStringCode(int nLevel, CString strCode) {
		CString strTmpCode;
		for (int i = 0; i < nLevel; i++) {
			strTmpCode += CString("\t");
		}
		strTmpCode += strCode;
		listCppStringCode.push_back(strTmpCode);
	}

	// ��ָ��
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

	// �����������������ʾ����, û���ҵ�Ҳ����ν
	void TouchTheVariableItem(CString strVariableName) {
		for (unsigned int i = 0; i < listVariableItem.size(); i++) {
			if (listVariableItem[i].strName == strVariableName) {
				listVariableItem[i].nShowedCount++;
				break;
			}
		}
	}
};

