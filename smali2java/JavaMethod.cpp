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

// 这个函数的作用就是将整个Method的代码全部分段
void CJavaMethod::ProcessInstList(CJavaClass * pClass, std::vector<CString> listMethodInst)
{
	// 创建分段
	CreateSegmentList(listMethodInst);

	// 创建循环记录体的数据
	CreateSegmentLoopData();

	// 获得函数全局向量,并保存指令向量
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
		printf("%4d 段名：", i);
		for (unsigned int j = 0; j < this->listMethodSegment[i].listSegName.size(); j++) {
			printf(" %s, ", this->listMethodSegment[i].listSegName[j]);
		}
		printf("\n");
		for (unsigned int j = 0; j < this->listMethodSegment[i].listInstSn.size(); j++) {
			printf("%4d %-128s :", this->listMethodSegment[i].listInstSn[j], GetOriginalInstruct(this->listMethodSegment[i].listInstSn[j])->strInstString);



			printf("\n");
		}
		printf("-->结束段 :");
		for (unsigned int j = 0; j < this->listMethodSegment[i].listStrNext.size(); j++) {
			printf(" %s ", this->listMethodSegment[i].listStrNext[j]);
		}
		printf("\n");
		printf("------------------------------------------------------------------------------------------<\n");
	}
#endif

	// 递归方式，解析所有指令的寄存器，
	// 解决所有寄存器之间的依赖关系，
	// 里面要设置每个寄存器的强类型和弱类型，这样可以决策后面的类型
	SegmentVariableDefine();

	// 变量传播
	SegmentVariableSpread();

	// 构建宏结构
	int nMacroCodeRoot = CreateMacroCodeStruction();

	// 清空输出列表
	this->listCppStringCode.clear();

	//this->strMethodName = this->GetMethodName(strLine);
	//this->strReturnType = this->GetReturnTypeFromMethods(strLine);
	//this->listMethodAttr = this->GetMethodAttribute(strLine);
	//this->listParamType = this->GetSignatureFromMethods(strLine);
	//this->listInputParam

	// 插入函数声明
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

	// 开始翻译宏结构,存储到 listCppStringCode 中
	TranslateMacroCodeStruct(nMacroCodeRoot);

	this->listCppStringCode.push_back(CString("}"));

	printf("开始显示完成的C++文件定义\n");




	// 处理函数内变量, 这个要插入函数开始
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

	printf("当前函数分析结束\n");
	getchar();
}
