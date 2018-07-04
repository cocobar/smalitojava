#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// 向原始指令列表，添加指令，并生成序号，返回当前指令序号
int CJavaMethod::AddInstructToOriginalList(CString strInst) {
	CJavaInstructItem smaliInstruct;
	smaliInstruct.nSerialNumber = nSerialNumberCnt++;
	smaliInstruct.strInstString = strInst;

	this->listOriginalInstruct.push_back(smaliInstruct);
	return smaliInstruct.nSerialNumber;
}

// 通过指令的编号，找到指令
std::vector<CJavaInstructItem>::iterator CJavaMethod::GetOriginalInstruct(int nIndex) {
	std::vector<CJavaInstructItem>::iterator itOi;
	for (itOi = listOriginalInstruct.begin(); itOi != listOriginalInstruct.end(); itOi++) {
		if ((*itOi).nSerialNumber == nIndex) {
			//printf("指令 %s\n", (*itOi).strInstString);
			return itOi;
		}
	}
	return listOriginalInstruct.end();
}


//.method private createBindNumberBitmap(Ljava/lang/String;)Landroid/graphics/Bitmap;
// 获得函数的修饰
std::vector<CString> CJavaMethod::GetMethodAttribute(CString strFunction) {
	std::vector<CString> listAtt;

	CString strLeft = strFunction;

	strLeft.Trim();
	if (strLeft.Find(".method") == 0) {
		strLeft = strLeft.Right(strLeft.GetLength() - 7);
		strLeft.Trim();
	}

	while ((strLeft.Find(" ") > 0)) {
		int nFindSpace = strLeft.Find(" ");
		CString strL = strLeft.Left(nFindSpace);
		strLeft = strLeft.Right(strLeft.GetLength() - nFindSpace - 1);
		strL.Trim(); strLeft.Trim();
		listAtt.push_back(strL);
	}

	return listAtt;
}

// 获得Method的名字
CString CJavaMethod::GetMethodName(CString strFunction) {

	CString strLeft = strFunction;

	strLeft.Trim();
	if (strLeft.Find(".method") >= 0) {
		strLeft.Right(strLeft.GetLength() - 7);
		strLeft.Trim();
	}

	while ((strLeft.Find(" ") > 0)) {
		int nFindSpace = strLeft.Find(" ");
		CString strL = strLeft.Left(nFindSpace);
		strLeft = strLeft.Right(strLeft.GetLength() - nFindSpace - 1);
		strL.Trim(); strLeft.Trim();
	}

	int nFindDot = strLeft.Find("(");

	if (nFindDot > 0) {
		strLeft = strLeft.Left(nFindDot);
		strLeft.Trim();
	}

	return strLeft;
}

//.method private createBindNumberBitmap(Ljava/lang/String;)Landroid/graphics/Bitmap;
// 返回函数的参数列表
std::vector<CString> CJavaMethod::GetSignatureFromMethods(CString strFunction) {
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

//.method private createBindNumberBitmap(Ljava/lang/String;)Landroid/graphics/Bitmap;
// 返回函数的返回类型
CString CJavaMethod::GetReturnTypeFromMethods(CString strFunction) {
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

// 从 0xXXXXXX 的 16 进制文本获取 10 进制的数
unsigned int CJavaMethod::GetDataFromHex(CString strHex)
{
	strHex.Trim();

	if ((strHex.Find("0x") == 0) || (strHex.Find("0X") == 0)) {
		unsigned int nData = 0;
		CString strBuf = strHex.Right(strHex.GetLength() - 2);
		strBuf.Trim();

		for (int i = 0; i < strBuf.GetLength(); i++) {
			int nNumber = -1;
			char _c = strBuf[i];

			nData *= 16;

			if ((_c >= '0') && (_c <= '9')) {
				nNumber = (_c - '0');
			}
			else if ((_c >= 'A') && (_c <= 'F')) {
				nNumber = (_c - 'A' + 10);
			}
			else if ((_c >= 'a') && (_c <= 'f')) {
				nNumber = (_c - 'a' + 10);
			}
			else {
				printf("未知的 16 进制符号 %c %s %d\n", _c, __FILE__, __LINE__);
			}

			if (nNumber >= 0) {
				nData += nNumber;
			}
		}

		return nData;
	}
	else {
		return 0;
	}
}
