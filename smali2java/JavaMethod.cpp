#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>

CJavaMethod::CJavaMethod()
{
	nSerialNumberCnt = 1;
	listOriginalInstruct.clear();
}


CJavaMethod::~CJavaMethod()
{
	nSerialNumberCnt = 0;
	listOriginalInstruct.clear();
}

// ������������þ��ǽ�����Method�Ĵ���ȫ���ֶ�
void CJavaMethod::ProcessInstList(CJavaClass * pClass, std::vector<CString> listMethodInst)
{
	// �����ֶ�
	CreateSegmentList(listMethodInst);

	// ����ѭ����¼�������
	CreateSegmentLoopData();

	// ��ú���ȫ������,������ָ������
	this->listFullRunVector = GetFullRunVector();
	this->listFullRunInstructionVector.clear();
	for (unsigned int i = 0; i < this->listFullRunVector.size(); i++) {
		for (unsigned int j = 0; j < this->listMethodSegment[listFullRunVector[i]].listInstSn.size(); j++) {
			this->listFullRunInstructionVector.push_back(this->listMethodSegment[listFullRunVector[i]].listInstSn[j]);
		}
	}

#if 1
	for (unsigned int i = 0; i < this->listMethodSegment.size(); i++) {
		printf("------------------------------------------------------------------------------------------>\n");
		printf("%4d ������", i);
		for (unsigned int j = 0; j < this->listMethodSegment[i].listSegName.size(); j++) {
			printf(" %s, ", this->listMethodSegment[i].listSegName[j]);
		}
		printf("\n");
		for (unsigned int j = 0; j < this->listMethodSegment[i].listInstSn.size(); j++) {
			printf("%4d %-128s :", this->listMethodSegment[i].listInstSn[j], GetOriginalInstruct(this->listMethodSegment[i].listInstSn[j])->strInstString);



			printf("\n");
		}
		printf("-->������ :");
		for (unsigned int j = 0; j < this->listMethodSegment[i].listStrNext.size(); j++) {
			printf(" %s ", this->listMethodSegment[i].listStrNext[j]);
		}
		printf("\n");
		printf("------------------------------------------------------------------------------------------<\n");
	}
#endif

	// �ݹ鷽ʽ����������ָ��ļĴ�����
	// ������мĴ���֮���������ϵ��
	// ����Ҫ����ÿ���Ĵ�����ǿ���ͺ������ͣ��������Ծ��ߺ��������
	SegmentVariableDefine();

	// ��������
	SegmentVariableSpread();

	// ������ṹ
	int nMacroCodeRoot = CreateMacroCodeStruction();

	// �������б�
	this->listCppStringCode.clear();

	//this->strMethodName = this->GetMethodName(strLine);
	//this->strReturnType = this->GetReturnTypeFromMethods(strLine);
	//this->listMethodAttr = this->GetMethodAttribute(strLine);
	//this->listParamType = this->GetSignatureFromMethods(strLine);
	//this->listInputParam

	// ���뺯������
	CString strFunctionDefine;

	if (this->strMethodName.Find("<init>") == 0) {
		strFunctionDefine.Format("%s::%s(", this->strClassName, this->strClassName);
	}
	else {
		strFunctionDefine.Format("%s %s::%s(", this->strReturnType, this->strClassName, this->strMethodName);
	}

	int nStartParamIndex = 1;
	if (find(listMethodAttr.begin(), listMethodAttr.end(), CString("static")) != listMethodAttr.end()) {
		nStartParamIndex = 0;
	}
	for (unsigned int i = nStartParamIndex; i < this->listInputParam.size(); i++) {
		strFunctionDefine += (this->listInputParam[i].strTypeName + CString(" ") + this->listInputParam[i].strValueName);
		if ((i + 1) != this->listInputParam.size()) {
			strFunctionDefine += CString(",");
		}
	}
	strFunctionDefine += CString(")");
	this->listCppStringCode.push_back(strFunctionDefine);

	this->listCppStringCode.push_back(CString("{"));

	// ��ʼ�����ṹ,�洢�� listCppStringCode ��
	TranslateMacroCodeStruct(nMacroCodeRoot);

	this->listCppStringCode.push_back(CString("}"));

	printf("��ʼ��ʾ��ɵ�C++�ļ�����\n");




	// �������ڱ���, ���Ҫ���뺯����ʼ
	std::vector<CString>::iterator itString = listCppStringCode.begin();
	std::vector<CString> strVariableDefine;
	strVariableDefine.clear();
	for (unsigned int i = 0; i < listVariableItem.size(); i++) {
		if (listVariableItem[i].nShowedCount > 0) {
			CString tempStr;
			tempStr.Format("\t%s %s;", listVariableItem[i].strTypeName, listVariableItem[i].strName);
			strVariableDefine.push_back(tempStr);
		}
	}
	if (strVariableDefine.size() > 0) {
		strVariableDefine.push_back(CString("\r"));
	}
	itString++; itString++;
	listCppStringCode.insert(itString, strVariableDefine.begin(), strVariableDefine.end());

	for (unsigned int i = 0; i < this->listCppStringCode.size(); i++) {
		printf("%s\n", this->listCppStringCode[i]);
	}

	printf("��ǰ������������\n");
	getchar();
}
