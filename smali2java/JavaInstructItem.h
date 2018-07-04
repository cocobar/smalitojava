#pragma once

#include <algorithm>

class CRegVariablePair {

public:
	int belongInstIndex;						// 所属的指令索引
	CString strRegName;							// 寄存器名称
	CString strStrongType;						// 强类型
	std::vector<CString> listWeakType;			// 弱类型列表
	CString strValName;							// 变量名称
	int nOutregBeQuoteCount;					// 变量被引用的次数，仅对输出寄存器有效
	int nOutregAssignedCount;					// 被赋值多少次

	std::vector<int> listQuotedInstIndex;		// 这个寄存器引用了别人多少寄存器？ 这个记录在输入寄存器上，指向输出寄存器所在的指令编号
	std::vector<CString> listQuotePosition;		// 输出寄存器被引用的地方，记录在输出寄存器上，指向输入寄存器的位置


												// 决策应该选择什么类型的变量出来
	CString strGetVarType(void) {
		if (strStrongType.IsEmpty()) {
			sort(listWeakType.begin(), listWeakType.end());
			std::vector<CString>::iterator iter = std::unique(listWeakType.begin(), listWeakType.end());
			listWeakType.erase(iter, listWeakType.end());

			if (listWeakType.size() == 1) {
				return listWeakType[0];
			}
			else if (listWeakType.size() > 1) {
				printf("有多种变量类型，要决策\n");
				while (1) {};
			}
			else {
				printf("没有定义弱变量类型\n");
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

// 指令的寄存器信息
class CInstructRegInfo {
public:
	CRegVariablePair					outReg;			// 单个的输出寄存器
	std::vector<CRegVariablePair>		listIreg;		// 输入寄存器列表

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

	int nSerialNumber;							// 每条指令的唯一编号
	CString strInstString;						// 指令内容

												// 下面是新的模式的
	CInstructRegInfo regInfo;					// 寄存器信息

												// 下面是翻译过程中生成的数据
	CString strCppExpression;					// 表达式
	CString strCppInverseExpression;			// 给 条件 判断 使用的 反向逻辑，只有条件判断才有用

	CString strCppCodeOutput;					// 给C语言显示用的，这个好像没有什么用了


												// 用来标记代码翻译类型的
	bool bForceOutputCode;						// 强制输出表达式
	bool bConstValue;							// 属于Const定义
	bool bValueTypeDefine;						// 进行变量定义
	bool bForceHideIt;							// 故意隐藏他


												// 显示和翻译指令有几种种状态
												// 1、翻译出来， 后面如果要使用， 就引用变量 ：要么不返回，要么返回 变量
												// 2、不翻译出来，后面直接引用 表达式		 ：返回表达式
												// 3、不翻译，无表达式						 ：这个是特殊指令
												// 4、不翻译，有表达式						 ：用于查询

												// 返回输出表达式，决定是返回表达式，还是返回变量
	CString GetOutExpression(void);

	// 这里决定翻译的是否是否要显示代码
	bool GetOutputCanBeShow(void);

	// 设置输入寄存器的信息 当前指令序号   强类型				弱类型             变量名		   被引用的指令
	void SetInputRegInfo(CString regName, CString strongType, CString weakType, CString valName, int nRefedIndex);

	// 复用的模式
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

	// 强制检测和更新一次类型
	void CheckInputRegStrongType(CString regName, CString strTypeName) {
		for (unsigned int i = 0; i < this->regInfo.listIreg.size(); i++) {
			if (this->regInfo.listIreg[i].strRegName == regName) {
				if (!this->regInfo.listIreg[i].strStrongType.IsEmpty()) {
					if (this->regInfo.listIreg[i].strStrongType != strTypeName) {

						if (this->regInfo.listIreg[i].strStrongType.Compare("this") == 0) {
							// 这个可能是使用super的类型，需要去查表
						}
						else {
							printf("输入寄存器返回为 %s 的类型不匹配, 原类型为 %s\n", strTypeName, this->regInfo.listIreg[i].strStrongType);
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

	// 强制检测和更新一次类型
	void CheckOutputRegStrongType(CString regName, CString strTypeName) {
		if (this->regInfo.outReg.strRegName.Compare(regName) == 0) {
			if (!this->regInfo.outReg.strStrongType.IsEmpty()) {
				if (this->regInfo.outReg.strStrongType.Compare(strTypeName)) {
					printf("输出寄存器返回为 %s 的类型不匹配, 原类型为 %s\n", strTypeName, this->regInfo.outReg.strStrongType);
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

	// 将指令按 分号，切割成段，方便使用，里面要跳过字符串
	static std::vector<CString> GetListInstructionSymbol(CString strInst);

	// 获得函数调用的寄存器列表
	static std::vector<CString> GetMethodCallRegList(CString strCall);

	// 返回函数调用的寄存器列表 {p0 .. p5}
	static std::vector<CString> GetMethodCallRegListRange(CString strCall);

	// 从函数调用的汇编上获得完整参数和类型
	static CString GetClassNameFromInstInvoke(CString strFunction);
	static std::vector<CString> GetSignatureFromInstInvoke(CString strFunction);
	static CString GetReturnTypeFromInstInvoke(CString strFunction);
	static CString GetMethodNameFromInstInvoke(CString strFunction);

	// 从Field中获得完整的参数了类型
	static CString GetClassNameFromInstIput(CString strParam);
	static CString GetFieldNameFromInstIput(CString strParam);
	static CString GetFieldTypeFromInstIput(CString strParam);

	// 通过参数表计算出来函数调用的寄存器个数，因为存在long和double两个符号，占用2个寄存器
	static int GetTotalSizeOfParam(std::vector<CString> listParam);
};

