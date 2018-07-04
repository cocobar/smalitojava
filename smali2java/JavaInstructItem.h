#pragma once

#include <algorithm>

class CRegVariablePair {

public:
	int belongInstIndex;						// ������ָ������
	CString strRegName;							// �Ĵ�������
	CString strStrongType;						// ǿ����
	std::vector<CString> listWeakType;			// �������б�
	CString strValName;							// ��������
	int nOutregBeQuoteCount;					// ���������õĴ�������������Ĵ�����Ч
	int nOutregAssignedCount;					// ����ֵ���ٴ�

	std::vector<int> listQuotedInstIndex;		// ����Ĵ��������˱��˶��ټĴ����� �����¼������Ĵ����ϣ�ָ������Ĵ������ڵ�ָ����
	std::vector<CString> listQuotePosition;		// ����Ĵ��������õĵط�����¼������Ĵ����ϣ�ָ������Ĵ�����λ��


												// ����Ӧ��ѡ��ʲô���͵ı�������
	CString strGetVarType(void) {
		if (strStrongType.IsEmpty()) {
			sort(listWeakType.begin(), listWeakType.end());
			std::vector<CString>::iterator iter = std::unique(listWeakType.begin(), listWeakType.end());
			listWeakType.erase(iter, listWeakType.end());

			if (listWeakType.size() == 1) {
				return listWeakType[0];
			}
			else if (listWeakType.size() > 1) {
				printf("�ж��ֱ������ͣ�Ҫ����\n");
				while (1) {};
			}
			else {
				printf("û�ж�������������\n");
				while (1) {};
			}
		}
		else {
			return strStrongType;
		}
	}

	CRegVariablePair() {
		belongInstIndex = -1;
		strRegName.Empty();
		strStrongType.Empty();
		listWeakType.clear();
		strValName.Empty();
		listQuotedInstIndex.clear();
		listQuotePosition.clear();
		nOutregBeQuoteCount = 0;
		nOutregAssignedCount = 0;
	};

	~CRegVariablePair() {

	};

	CRegVariablePair(const CRegVariablePair & _c) {
		this->belongInstIndex = _c.belongInstIndex;
		this->strRegName = _c.strRegName;
		this->strStrongType = _c.strStrongType;
		this->listWeakType = _c.listWeakType;
		this->strValName = _c.strValName;
		this->listQuotedInstIndex = _c.listQuotedInstIndex;
		this->listQuotePosition = _c.listQuotePosition;
		this->nOutregBeQuoteCount = _c.nOutregBeQuoteCount;
		this->nOutregAssignedCount = _c.nOutregAssignedCount;
	};

	CRegVariablePair & operator = (const CRegVariablePair & _c) {
		this->belongInstIndex = _c.belongInstIndex;
		this->strRegName = _c.strRegName;
		this->strStrongType = _c.strStrongType;
		this->listWeakType = _c.listWeakType;
		this->strValName = _c.strValName;
		this->listQuotedInstIndex = _c.listQuotedInstIndex;
		this->listQuotePosition = _c.listQuotePosition;
		this->nOutregBeQuoteCount = _c.nOutregBeQuoteCount;
		this->nOutregAssignedCount = _c.nOutregAssignedCount;
		return *this;
	};
};

// ָ��ļĴ�����Ϣ
class CInstructRegInfo {
public:
	CRegVariablePair					outReg;			// ����������Ĵ���
	std::vector<CRegVariablePair>		listIreg;		// ����Ĵ����б�

	CInstructRegInfo() {
		listIreg.clear();
	};
	~CInstructRegInfo() {};

	CInstructRegInfo(const CInstructRegInfo & _c) {
		this->outReg = _c.outReg;
		this->listIreg = _c.listIreg;
	};

	CInstructRegInfo & operator = (const CInstructRegInfo & _c) {
		this->outReg = _c.outReg;
		this->listIreg = _c.listIreg;
		return *this;
	};
};

class CJavaInstructItem
{
public:
	CJavaInstructItem();
	~CJavaInstructItem();

	int nSerialNumber;							// ÿ��ָ���Ψһ���
	CString strInstString;						// ָ������

												// �������µ�ģʽ��
	CInstructRegInfo regInfo;					// �Ĵ�����Ϣ

												// �����Ƿ�����������ɵ�����
	CString strCppExpression;					// ���ʽ
	CString strCppInverseExpression;			// �� ���� �ж� ʹ�õ� �����߼���ֻ�������жϲ�����

	CString strCppCodeOutput;					// ��C������ʾ�õģ��������û��ʲô����


												// ������Ǵ��뷭�����͵�
	bool bForceOutputCode;						// ǿ��������ʽ
	bool bConstValue;							// ����Const����
	bool bValueTypeDefine;						// ���б�������
	bool bForceHideIt;							// ����������


												// ��ʾ�ͷ���ָ���м�����״̬
												// 1����������� �������Ҫʹ�ã� �����ñ��� ��Ҫô�����أ�Ҫô���� ����
												// 2�����������������ֱ������ ���ʽ		 �����ر��ʽ
												// 3�������룬�ޱ��ʽ						 �����������ָ��
												// 4�������룬�б��ʽ						 �����ڲ�ѯ

												// ����������ʽ�������Ƿ��ر��ʽ�����Ƿ��ر���
	CString GetOutExpression(void);

	// �������������Ƿ��Ƿ�Ҫ��ʾ����
	bool GetOutputCanBeShow(void);

	// ��������Ĵ�������Ϣ ��ǰָ�����   ǿ����				������             ������		   �����õ�ָ��
	void SetInputRegInfo(CString regName, CString strongType, CString weakType, CString valName, int nRefedIndex);

	// ���õ�ģʽ
	void SetInputRegInfo(CString regName, CString strongType, std::vector<CString> weakType, CString valName, int nRefedIndex) {
		SetInputRegInfo(regName, strongType, "", valName, nRefedIndex);
		for (unsigned int i = 0; i < regInfo.listIreg.size(); i++) {
			if (regInfo.listIreg[i].strRegName.Compare(regName) == 0) {
				if (weakType.size() > 0) {
					regInfo.listIreg[i].listWeakType.insert(regInfo.listIreg[i].listWeakType.end(), weakType.begin(), weakType.end());
				}
				break;
			}
		}
	}

	// ǿ�Ƽ��͸���һ������
	void CheckInputRegStrongType(CString regName, CString strTypeName) {
		for (unsigned int i = 0; i < this->regInfo.listIreg.size(); i++) {
			if (this->regInfo.listIreg[i].strRegName == regName) {
				if (!this->regInfo.listIreg[i].strStrongType.IsEmpty()) {
					if (this->regInfo.listIreg[i].strStrongType != strTypeName) {

						if (this->regInfo.listIreg[i].strStrongType.Compare("this") == 0) {
							// ���������ʹ��super�����ͣ���Ҫȥ���
						}
						else {
							printf("����Ĵ�������Ϊ %s �����Ͳ�ƥ��, ԭ����Ϊ %s\n", strTypeName, this->regInfo.listIreg[i].strStrongType);
							printf("%4d %s\n", this->nSerialNumber, this->strInstString);
							getchar();
						}
					}
				}
				else {
					this->regInfo.listIreg[i].strStrongType = strTypeName;
				}
			}
		}
	}



	void SetOutputRegInfo(CString regName, CString strongType, CString weakType, CString valName) {
		regInfo.outReg.strRegName = regName;
		regInfo.outReg.belongInstIndex = this->nSerialNumber;
		if (!strongType.IsEmpty()) {
			regInfo.outReg.strStrongType = strongType;
		}
		else {
			if (!weakType.IsEmpty()) {
				regInfo.outReg.listWeakType.push_back(weakType);
			}
		}
		regInfo.outReg.strValName = valName;
	}

	void SetOutputRegInfo(CString regName, CString strongType, std::vector<CString> weakType, CString valName) {
		SetOutputRegInfo(regName, strongType, "", valName);
		if (weakType.size() > 0) {
			regInfo.outReg.listWeakType.insert(regInfo.outReg.listWeakType.begin(), weakType.begin(), weakType.end());
		}
	}

	// ǿ�Ƽ��͸���һ������
	void CheckOutputRegStrongType(CString regName, CString strTypeName) {
		if (this->regInfo.outReg.strRegName.Compare(regName) == 0) {
			if (!this->regInfo.outReg.strStrongType.IsEmpty()) {
				if (this->regInfo.outReg.strStrongType.Compare(strTypeName)) {
					printf("����Ĵ�������Ϊ %s �����Ͳ�ƥ��, ԭ����Ϊ %s\n", strTypeName, this->regInfo.outReg.strStrongType);
					printf("%4d %s\n", this->nSerialNumber, this->strInstString);
					while (1) {};
				}
			}
			else {
				this->regInfo.outReg.strStrongType = strTypeName;
			}
		}
	}


	CJavaInstructItem(const CJavaInstructItem & _c) {
		this->nSerialNumber = _c.nSerialNumber;
		this->strInstString = _c.strInstString;
		this->regInfo = _c.regInfo;
		this->strCppExpression = _c.strCppExpression;
		this->bForceOutputCode = _c.bForceOutputCode;
		this->strCppCodeOutput = _c.strCppCodeOutput;
		this->bConstValue = _c.bConstValue;
		this->strCppInverseExpression = _c.strCppInverseExpression;
		this->bValueTypeDefine = _c.bValueTypeDefine;
		this->bForceHideIt = _c.bForceHideIt;
	};

	CJavaInstructItem & operator = (const CJavaInstructItem & _c) {
		this->nSerialNumber = _c.nSerialNumber;
		this->strInstString = _c.strInstString;
		this->regInfo = _c.regInfo;
		this->strCppExpression = _c.strCppExpression;
		this->bForceOutputCode = _c.bForceOutputCode;
		this->strCppCodeOutput = _c.strCppCodeOutput;
		this->bConstValue = _c.bConstValue;
		this->strCppInverseExpression = _c.strCppInverseExpression;
		this->bValueTypeDefine = _c.bValueTypeDefine;
		this->bForceHideIt = _c.bForceHideIt;
		return *this;
	};

	// ��ָ� �ֺţ��и�ɶΣ�����ʹ�ã�����Ҫ�����ַ���
	static std::vector<CString> GetListInstructionSymbol(CString strInst);

	// ��ú������õļĴ����б�
	static std::vector<CString> GetMethodCallRegList(CString strCall);

	// ���غ������õļĴ����б� {p0 .. p5}
	static std::vector<CString> GetMethodCallRegListRange(CString strCall);

	// �Ӻ������õĻ���ϻ����������������
	static CString GetClassNameFromInstInvoke(CString strFunction);
	static std::vector<CString> GetSignatureFromInstInvoke(CString strFunction);
	static CString GetReturnTypeFromInstInvoke(CString strFunction);
	static CString GetMethodNameFromInstInvoke(CString strFunction);

	// ��Field�л�������Ĳ���������
	static CString GetClassNameFromInstIput(CString strParam);
	static CString GetFieldNameFromInstIput(CString strParam);
	static CString GetFieldTypeFromInstIput(CString strParam);

	// ͨ���������������������õļĴ�����������Ϊ����long��double�������ţ�ռ��2���Ĵ���
	static int GetTotalSizeOfParam(std::vector<CString> listParam);
};

