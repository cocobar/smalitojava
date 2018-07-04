#include "stdafx.h"
#include "JavaInstructItem.h"
#include "JavaClass.h"


CJavaInstructItem::CJavaInstructItem()
{
	bForceOutputCode = false;
	bConstValue = false;
	bValueTypeDefine = false;
	bForceHideIt = false;
}


CJavaInstructItem::~CJavaInstructItem()
{
}

//Lcom/xtc/log/LogUtil;->d(Ljava/lang/String;Ljava/lang/String;)V
// 返回 Class 名
CString CJavaInstructItem::GetClassNameFromInstInvoke(CString strFunction) {
	CString strClassName;
	strFunction.Trim();
	int nDotPos = strFunction.Find("->");
	if (nDotPos > 0) {
		strClassName = strFunction.Left(nDotPos);
		strClassName.Trim();
	}

	strClassName = CJavaClass::GetTypeFromJava(strClassName);

	if (strClassName.Trim().IsEmpty()) {
		printf("格式错误 %s  %d %s\n", strFunction, __LINE__, __FILE__);
	}

	return strClassName;
}

// Landroid/support/multidex/MultiDexExtractor$ExtractedDex;->crc:J
CString CJavaInstructItem::GetClassNameFromInstIput(CString strParam) {
	CString strClassName;
	strParam.Trim();
	int nDotPos = strParam.Find("->");
	if (nDotPos > 0) {
		strClassName = strParam.Left(nDotPos);
		strClassName.Trim();
	}

	strClassName = CJavaClass::GetTypeFromJava(strClassName);

	if (strClassName.Trim().IsEmpty()) {
		printf("格式错误 %s  %d %s\n", strParam, __LINE__, __FILE__);
	}

	return strClassName;
}



//Lcom/xtc/log/LogUtil;->d(Ljava/lang/String;Ljava/lang/String;)V
// 返回 函数 名
CString CJavaInstructItem::GetMethodNameFromInstInvoke(CString strFunction) {
	CString strMethodName;
	strFunction.Trim();
	int nDotPos = strFunction.Find("->");
	if (nDotPos > 0) {
		strMethodName = strFunction.Right(strFunction.GetLength() - nDotPos - 2);
		strMethodName.Trim();
	}

	nDotPos = strMethodName.Find("(");
	if (nDotPos > 0) {
		strMethodName = strMethodName.Left(nDotPos);
		strMethodName.Trim();
	}

	if (strMethodName.Trim().IsEmpty()) {
		printf("格式错误 %s  %d %s\n", strFunction, __LINE__, __FILE__);
	}

	return strMethodName;
}

// Landroid/support/multidex/MultiDexExtractor$ExtractedDex;->crc:J
CString CJavaInstructItem::GetFieldNameFromInstIput(CString strParam) {
	CString strFieldName;
	strParam.Trim();
	int nDotPos = strParam.Find("->");
	if (nDotPos > 0) {
		strFieldName = strParam.Right(strParam.GetLength() - nDotPos - 2);
		strFieldName.Trim();
	}

	nDotPos = strFieldName.Find(":");
	if (nDotPos > 0) {
		strFieldName = strFieldName.Left(nDotPos);
		strFieldName.Trim();
	}

	if (strFieldName.Trim().IsEmpty()) {
		printf("格式错误 %s  %d %s\n", strParam, __LINE__, __FILE__);
	}

	return strFieldName;
}

CString CJavaInstructItem::GetFieldTypeFromInstIput(CString strParam) {
	CString strFieldType;
	strParam.Trim();
	int nDotPos = strParam.Find("->");
	if (nDotPos > 0) {
		strFieldType = strParam.Right(strParam.GetLength() - nDotPos - 2);
		strFieldType.Trim();
	}

	nDotPos = strFieldType.Find(":");
	if (nDotPos > 0) {
		strFieldType = strFieldType.Right(strFieldType.GetLength() - nDotPos - 1);
		strFieldType.Trim();
	}

	if (strFieldType.Trim().IsEmpty()) {
		printf("格式错误 %s  %d %s\n", strParam, __LINE__, __FILE__);
	}

	strFieldType = CJavaClass::GetTypeFromJava(strFieldType);

	return strFieldType;
}

/*
case 'B':   return "byte";
case 'C':   return "char";
case 'D':   return "double";
case 'F':   return "float";
case 'I':   return "int";
case 'J':   return "long";
case 'S':   return "short";
case 'V':   return "void";
case 'Z':   return "boolean";
case 'L':
*/

int CJavaInstructItem::GetTotalSizeOfParam(std::vector<CString> listParam) {
	int nTotalSize = 0;

	std::vector<CString>::iterator it;
	for (it = listParam.begin(); it != listParam.end(); it++) {
		if ((*it).Compare("double") == 0) {
			nTotalSize += 2;
		}
		else if ((*it).Compare("long") == 0) {
			nTotalSize += 2;
		}
		else {
			nTotalSize += 1;
		}
	}

	return nTotalSize;
}

//Lcom/xtc/log/LogUtil;->d(Ljava/lang/String;Ljava/lang/String;)V
// 返回参数列表
std::vector<CString> CJavaInstructItem::GetSignatureFromInstInvoke(CString strFunction) {
	std::vector<CString> listString;
	strFunction.Trim();
	listString.clear();
	CString strSignature;
	int nDotPos = strFunction.Find("(");
	if (nDotPos > 0) {
		strSignature = strFunction.Right(strFunction.GetLength() - nDotPos - 1);
	}

	nDotPos = strSignature.Find(")");
	if (nDotPos >= 0) {
		strSignature = strSignature.Left(nDotPos);
	}

	strSignature.Trim();

	CString strArray;
	strArray.Empty();
	while (strSignature.GetLength() > 0) {

		switch (strSignature[0]) {
		case '[':
			strArray += '[';
			strSignature = strSignature.Right(strSignature.GetLength() - 1);
			break;
		case 'B':
		case 'C':
		case 'D':
		case 'F':
		case 'I':
		case 'J':
		case 'S':
		case 'V':
		case 'Z':
		{
			strArray.Trim();
			CString strType = strArray + strSignature.Left(1);
			strArray.Empty();
			strSignature = strSignature.Right(strSignature.GetLength() - 1);

			strType = CJavaClass::GetTypeFromJava(strType);

			listString.push_back(strType);
		}

		break;
		case 'L':
		{
			strArray.Trim();

			nDotPos = strSignature.Find(";");
			CString strType;
			if (nDotPos > 0) {
				strType = strArray + strSignature.Left(nDotPos + 1);
				strSignature = strSignature.Right(strSignature.GetLength() - nDotPos - 1);
			}
			else {
				printf("格式错误 %s %d %s\n", strFunction, __LINE__, __FILE__);
			}
			strArray.Empty();

			strType = CJavaClass::GetTypeFromJava(strType);

			listString.push_back(strType);
		}
		break;
		default:
			printf("格式错误 %s %d %s\n", strFunction, __LINE__, __FILE__);
			break;
		}
	}

	return listString;
}

//Lcom/xtc/log/LogUtil;->d(Ljava/lang/String;Ljava/lang/String;)V
// 返回返回列表
CString CJavaInstructItem::GetReturnTypeFromInstInvoke(CString strFunction) {
	CString strReturnName;
	strFunction.Trim();
	int nDotPos = strFunction.Find(")");
	if (nDotPos > 0) {
		strReturnName = strFunction.Right(strFunction.GetLength() - nDotPos - 1);
		strReturnName.Trim();
	}

	CString strType = CJavaClass::GetTypeFromJava(strReturnName);

	if (strType.Trim().IsEmpty()) {
		printf("格式错误 %s %d %s\n", strFunction, __LINE__, __FILE__);
	}

	return strType;
}

// 返回函数调用的寄存器列表 {v1,v2,v3}
std::vector<CString> CJavaInstructItem::GetMethodCallRegList(CString strCall) {
	std::vector<CString> listString;

	if ((strCall[0] == '{') && (strCall[strCall.GetLength() - 1] == '}')) {

		strCall.Replace('{', ' ');
		strCall.Replace('}', ' ');
		strCall.Trim();

		CString strR = strCall;
		CString strL;

		while (strR.Find(",") >= 0) {
			int nDotPos = strR.Find(",");
			strL = strR.Left(nDotPos);
			strL.Trim();
			listString.push_back(strL);
			strR = strR.Right(strR.GetLength() - nDotPos - 1);
			strR.Trim();
		}
		if (strR.GetLength() > 0) {
			listString.push_back(strR);
		}
	}
	else {
		printf("调用参数列表解析错误 %s\n", strCall);
	}

	return listString;
}

// 返回函数调用的寄存器列表 {p0 .. p5}
std::vector<CString> CJavaInstructItem::GetMethodCallRegListRange(CString strCall) {
	std::vector<CString> listString;

	if ((strCall[0] == '{') && (strCall[strCall.GetLength() - 1] == '}')) {

		strCall.Replace('{', ' ');
		strCall.Replace('}', ' ');
		strCall.Trim();

		CString strR = strCall;
		CString strL;

		int nDotPos = strR.Find("..");
		strL = strR.Left(nDotPos);
		strR = strR.Right(strR.GetLength() - nDotPos - 2);
		strL.Trim();
		strR.Trim();

		if ((strL.GetLength() > 0) && (strR.GetLength() > 0)) {
			if (strL[0] == strR[0]) {
				CString strPre = strL.Left(1);
				strL = strL.Right(strL.GetLength() - 1);
				strR = strR.Right(strR.GetLength() - 1);

				int s1 = atoi(strL);
				int s2 = atoi(strR);

				for (; s1 <= s2; s1++) {
					CString strData;
					strData.Format("%s%d", strPre, s1);
					listString.push_back(strData);
				}
			}
			else {
				printf("调用参数列表解析错误 %s\n", strCall);
			}
		}
		else {
			printf("调用参数列表解析错误 %s\n", strCall);
		}
	}
	else {
		printf("调用参数列表解析错误 %s\n", strCall);
	}

	return listString;
}

CString CJavaInstructItem::GetOutExpression(void) {

	if (bForceOutputCode) {					// 要显示代码，那么就返回寄存器的变量
		return regInfo.outReg.strValName;
	}

	// 如果是常量，并且被赋值了小于1次，那么就不显示出来，直接返回表达式
	if (bConstValue) {
		if (regInfo.outReg.nOutregAssignedCount <= 1) {
			return strCppExpression;
		}
	}


	// 如果这个没有返回值，那么肯定就输出表达式，针对函数，对于其它指令，就不应该调用这个函数
	if (regInfo.outReg.strRegName.IsEmpty()) {
		return strCppExpression;
	}
	// 如果有多处引用，那么就要输出变量
	else if (regInfo.outReg.nOutregBeQuoteCount > 1) {
		return regInfo.outReg.strValName;
	}
	else if (regInfo.outReg.nOutregBeQuoteCount <= 1) {
		return strCppExpression;
	}
	else {
		return CString("Unknow!");
	}
}

bool CJavaInstructItem::GetOutputCanBeShow(void) {

	// 故意隐藏
	if (bForceHideIt) {
		return false;
	}

	// 如果是强制输出，那么就输出
	if (bForceOutputCode) {
		return true;
	}

	// 如果是常量，并且没有两次辅助，那么就可以隐藏
	if (bConstValue) {
		if (regInfo.outReg.nOutregAssignedCount <= 1) {
			return false;
		}
	}

	// 如果这个没有返回值，那么肯定就输出表达式，针对函数，对于其它指令，就不应该调用这个函数
	if (regInfo.outReg.strRegName.IsEmpty()) {
		return true;
	}
	// 如果有多处引用，那么就要输出变量
	else if (regInfo.outReg.nOutregBeQuoteCount > 1) {		// 多次使用，也要输出
		return true;
	}
	else if (regInfo.outReg.nOutregBeQuoteCount <= 1) {
		return false;
	}
	else {
		return true;
	}
}

void CJavaInstructItem::SetInputRegInfo(CString regName, CString strongType, CString weakType, CString valName, int nRefedIndex) {
	CRegVariablePair varPair;
	int nFindOverlapIndex = -1;
	for (unsigned int i = 0; i < regInfo.listIreg.size(); i++) {
		if (regInfo.listIreg[i].strRegName.Compare(regName) == 0) {
			varPair = regInfo.listIreg[i];
			nFindOverlapIndex = i;
			break;
		}
	}

	if (nFindOverlapIndex < 0) {
		varPair.strRegName = regName;
		varPair.belongInstIndex = this->nSerialNumber;
	}
	else {
		if (varPair.belongInstIndex != this->nSerialNumber) {
			printf("所属的指令序号不匹配\n");
			while (1) {};
		}
	}

	if ((!varPair.strStrongType.IsEmpty()) && (!strongType.IsEmpty())) {
		if (varPair.strStrongType.Compare(strongType)) {
			printf("两个强类型不一致!\n");
			while (1) {};
		}
	}

	if (!strongType.IsEmpty()) {	// 存在强类型
		if (nFindOverlapIndex < 0) {
			varPair.strStrongType = strongType;
		}
		else {
			if (!varPair.strStrongType.IsEmpty()) {
				if (varPair.strStrongType.Compare(strongType)) {
					printf("两个强类型不一致!\n");
					while (1) {};
				}
			}
			else {
				varPair.strStrongType = strongType;
			}
		}
	}
	else {
		if (!weakType.IsEmpty()) {
			varPair.listWeakType.push_back(weakType);
		}
	}

	if (nFindOverlapIndex < 0) {
		varPair.strValName = valName;
	}
	else {
		if (!varPair.strValName.IsEmpty()) {
			if (varPair.strValName.Compare(valName)) {
				printf("两个变量名不一致!\n");
				while (1) {};
			}
		}
	}

	varPair.listQuotedInstIndex.push_back(nRefedIndex);

	// 是插入还是新建
	if (nFindOverlapIndex >= 0) {
		regInfo.listIreg[nFindOverlapIndex] = varPair;
	}
	else {
		regInfo.listIreg.push_back(varPair);
	}
}



// 分析指令格式  XXX AA,BB,CC
std::vector<CString> CJavaInstructItem::GetListInstructionSymbol(CString strInst) {
	std::vector<CString> listString;

	listString.clear();

	int nSpacePos = strInst.Find(" ");

	if (nSpacePos > 0) {
		CString strL = strInst.Left(nSpacePos);
		CString strR = strInst.Right(strInst.GetLength() - nSpacePos - 1);
		strR.Trim();

		strL.Trim();
		listString.push_back(strL);


		if (strR[0] == '{') {
			int nDotPos = strR.Find("}");
			if (nDotPos >= 0) {
				strL = strR.Left(nDotPos + 1);
				strR = strR.Right(strR.GetLength() - nDotPos - 1);

				strL.Trim();
				listString.push_back(strL);

				strR.Trim();

				if (strR.GetLength() > 0) {
					if (strR[0] == ',') {
						strR = strR.Right(strR.GetLength() - 1);
						strR.Trim();
					}
				}
			}
		}

		while (strR.Find(",") >= 0) {
			int nDotPos = strR.Find(",");
			strL = strR.Left(nDotPos);
			strL.Trim();
			listString.push_back(strL);
			strR = strR.Right(strR.GetLength() - nDotPos - 1);
			strR.Trim();
		}

		if (strR.GetLength() > 0) {
			listString.push_back(strR);
		}
	}
	else {
		// 单目指令，例如 nop
		listString.push_back(strInst);
	}

	return listString;
}


/*
1、解决指令的输入输出寄存器
2、解决寄存器类型
*/

// smali 汇编指令 https://blog.csdn.net/hejjunlin/article/details/48394559

static const char* gOpNames[] = {
	// BEGIN(libdex-opcode-names); GENERATED AUTOMATICALLY BY opcode-gen
	"nop",
	"move",
	"move/from16",
	"move/16",
	"move-wide",
	"move-wide/from16",
	"move-wide/16",
	"move-object",
	"move-object/from16",
	"move-object/16",
	"move-result",
	"move-result-wide",
	"move-result-object",
	"move-exception",
	"return-void",
	"return",
	"return-wide",
	"return-object",
	"const/4",
	"const/16",
	"const",
	"const/high16",
	"const-wide/16",
	"const-wide/32",
	"const-wide",
	"const-wide/high16",
	"const-string",
	"const-string/jumbo",
	"const-class",
	"monitor-enter",
	"monitor-exit",
	"check-cast",
	"instance-of",
	"array-length",
	"new-instance",
	"new-array",
	"filled-new-array",
	"filled-new-array/range",
	"fill-array-data",
	"throw",
	"goto",
	"goto/16",
	"goto/32",
	"packed-switch",
	"sparse-switch",
	"cmpl-float",
	"cmpg-float",
	"cmpl-double",
	"cmpg-double",
	"cmp-long",
	"if-eq",
	"if-ne",
	"if-lt",
	"if-ge",
	"if-gt",
	"if-le",
	"if-eqz",
	"if-nez",
	"if-ltz",
	"if-gez",
	"if-gtz",
	"if-lez",
	"unused-3e",
	"unused-3f",
	"unused-40",
	"unused-41",
	"unused-42",
	"unused-43",
	"aget",
	"aget-wide",
	"aget-object",
	"aget-boolean",
	"aget-byte",
	"aget-char",
	"aget-short",
	"aput",
	"aput-wide",
	"aput-object",
	"aput-boolean",
	"aput-byte",
	"aput-char",
	"aput-short",
	"iget",
	"iget-wide",
	"iget-object",
	"iget-boolean",
	"iget-byte",
	"iget-char",
	"iget-short",
	"iput",
	"iput-wide",
	"iput-object",
	"iput-boolean",
	"iput-byte",
	"iput-char",
	"iput-short",
	"sget",
	"sget-wide",
	"sget-object",
	"sget-boolean",
	"sget-byte",
	"sget-char",
	"sget-short",
	"sput",
	"sput-wide",
	"sput-object",
	"sput-boolean",
	"sput-byte",
	"sput-char",
	"sput-short",
	"invoke-virtual",
	"invoke-super",
	"invoke-direct",
	"invoke-static",
	"invoke-interface",
	"unused-73",
	"invoke-virtual/range",
	"invoke-super/range",
	"invoke-direct/range",
	"invoke-static/range",
	"invoke-interface/range",
	"unused-79",
	"unused-7a",
	"neg-int",
	"not-int",
	"neg-long",
	"not-long",
	"neg-float",
	"neg-double",
	"int-to-long",
	"int-to-float",
	"int-to-double",
	"long-to-int",
	"long-to-float",
	"long-to-double",
	"float-to-int",
	"float-to-long",
	"float-to-double",
	"double-to-int",
	"double-to-long",
	"double-to-float",
	"int-to-byte",
	"int-to-char",
	"int-to-short",
	"add-int",
	"sub-int",
	"mul-int",
	"div-int",
	"rem-int",
	"and-int",
	"or-int",
	"xor-int",
	"shl-int",
	"shr-int",
	"ushr-int",
	"add-long",
	"sub-long",
	"mul-long",
	"div-long",
	"rem-long",
	"and-long",
	"or-long",
	"xor-long",
	"shl-long",
	"shr-long",
	"ushr-long",
	"add-float",
	"sub-float",
	"mul-float",
	"div-float",
	"rem-float",
	"add-double",
	"sub-double",
	"mul-double",
	"div-double",
	"rem-double",
	"add-int/2addr",
	"sub-int/2addr",
	"mul-int/2addr",
	"div-int/2addr",
	"rem-int/2addr",
	"and-int/2addr",
	"or-int/2addr",
	"xor-int/2addr",
	"shl-int/2addr",
	"shr-int/2addr",
	"ushr-int/2addr",
	"add-long/2addr",
	"sub-long/2addr",
	"mul-long/2addr",
	"div-long/2addr",
	"rem-long/2addr",
	"and-long/2addr",
	"or-long/2addr",
	"xor-long/2addr",
	"shl-long/2addr",
	"shr-long/2addr",
	"ushr-long/2addr",
	"add-float/2addr",
	"sub-float/2addr",
	"mul-float/2addr",
	"div-float/2addr",
	"rem-float/2addr",
	"add-double/2addr",
	"sub-double/2addr",
	"mul-double/2addr",
	"div-double/2addr",
	"rem-double/2addr",
	"add-int/lit16",
	"rsub-int",
	"mul-int/lit16",
	"div-int/lit16",
	"rem-int/lit16",
	"and-int/lit16",
	"or-int/lit16",
	"xor-int/lit16",
	"add-int/lit8",
	"rsub-int/lit8",
	"mul-int/lit8",
	"div-int/lit8",
	"rem-int/lit8",
	"and-int/lit8",
	"or-int/lit8",
	"xor-int/lit8",
	"shl-int/lit8",
	"shr-int/lit8",
	"ushr-int/lit8",
	"+iget-volatile",
	"+iput-volatile",
	"+sget-volatile",
	"+sput-volatile",
	"+iget-object-volatile",
	"+iget-wide-volatile",
	"+iput-wide-volatile",
	"+sget-wide-volatile",
	"+sput-wide-volatile",
	"^breakpoint",
	"^throw-verification-error",
	"+execute-inline",
	"+execute-inline/range",
	"+invoke-object-init/range",
	"+return-void-barrier",
	"+iget-quick",
	"+iget-wide-quick",
	"+iget-object-quick",
	"+iput-quick",
	"+iput-wide-quick",
	"+iput-object-quick",
	"+invoke-virtual-quick",
	"+invoke-virtual-quick/range",
	"+invoke-super-quick",
	"+invoke-super-quick/range",
	"+iput-object-volatile",
	"+sget-object-volatile",
	"+sput-object-volatile",
	"unused-ff",
	// END(libdex-opcode-names)
};

