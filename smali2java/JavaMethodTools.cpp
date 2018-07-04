#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// ��ԭʼָ���б����ָ���������ţ����ص�ǰָ�����
int CJavaMethod::AddInstructToOriginalList(CString strInst) {
	CJavaInstructItem smaliInstruct;
	smaliInstruct.nSerialNumber = nSerialNumberCnt++;
	smaliInstruct.strInstString = strInst;

	this->listOriginalInstruct.push_back(smaliInstruct);
	return smaliInstruct.nSerialNumber;
}

// ͨ��ָ��ı�ţ��ҵ�ָ��
std::vector<CJavaInstructItem>::iterator CJavaMethod::GetOriginalInstruct(int nIndex) {
	std::vector<CJavaInstructItem>::iterator itOi;
	for (itOi = listOriginalInstruct.begin(); itOi != listOriginalInstruct.end(); itOi++) {
		if ((*itOi).nSerialNumber == nIndex) {
			//printf("ָ�� %s\n", (*itOi).strInstString);
			return itOi;
		}
	}
	return listOriginalInstruct.end();
}


//.method private createBindNumberBitmap(Ljava/lang/String;)Landroid/graphics/Bitmap;
// ��ú���������
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

// ���Method������
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
// ���غ����Ĳ����б�
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
				printf("��ʽ���� %s %d %s\n", strFunction, __LINE__, __FILE__);
			}
			strArray.Empty();

			strType = CJavaClass::GetTypeFromJava(strType);

			listString.push_back(strType);
		}
		break;
		default:
			printf("��ʽ���� %s %d %s\n", strFunction, __LINE__, __FILE__);
			break;
		}
	}

	return listString;
}

//.method private createBindNumberBitmap(Ljava/lang/String;)Landroid/graphics/Bitmap;
// ���غ����ķ�������
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
		printf("��ʽ���� %s %d %s\n", strFunction, __LINE__, __FILE__);
	}

	return strType;
}

// �� 0xXXXXXX �� 16 �����ı���ȡ 10 ���Ƶ���
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
				printf("δ֪�� 16 ���Ʒ��� %c %s %d\n", _c, __FILE__, __LINE__);
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
