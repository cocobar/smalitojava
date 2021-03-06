#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"


CJavaClass::CJavaClass()
{
}


CJavaClass::~CJavaClass()
{
}

BOOL CJavaClass::AddImportClassToList(CString strFullClass) {

	if (strFullClass.Find("[") > 0) {
		strFullClass = strFullClass.Left(strFullClass.Find("["));
	}

	if ((strFullClass == CString("byte")) || (strFullClass == CString("char"))
		|| (strFullClass == CString("double")) || (strFullClass == CString("float"))
		|| (strFullClass == CString("int")) || (strFullClass == CString("long"))
		|| (strFullClass == CString("short")) || (strFullClass == CString("void"))
		|| (strFullClass == CString("boolean"))) {
		return FALSE;
	}

	if (strFullClass == this->strFullClassName) {
		return FALSE;
	}

	CString strPackage;
	CString strShortClass;
	GetPackageAndClassName(strFullClass, strPackage, strShortClass);

	if (strPackage == CString("java.lang")) {
		return FALSE;
	}

	if (strPackage == this->strPackageName) {
		return FALSE;
	}

	if (find(listStrImportClass.begin(), listStrImportClass.end(), strFullClass) == listStrImportClass.end()) {
		listStrImportClass.push_back(strFullClass);
		return TRUE;
	}

	return FALSE;
}



BOOL CJavaClass::GetPackageAndClassName(CString strFullClassName, CString &strPackageName, CString &strClassName) {

	CString strTmpPackageName;
	strTmpPackageName.Empty();
	int nFindDot = strFullClassName.Find(".");

	while (nFindDot >= 0) {
		strTmpPackageName += strFullClassName.Left(nFindDot + 1);
		strFullClassName = strFullClassName.Right(strFullClassName.GetLength() - nFindDot - 1);

		nFindDot = strFullClassName.Find(".");
	}

	// 去掉最后一个小数点
	if (strTmpPackageName.GetLength() > 0) {
		strTmpPackageName = strTmpPackageName.Left(strTmpPackageName.GetLength() - 1);
	}

	strPackageName = strTmpPackageName;
	strClassName = strFullClassName;

	return TRUE;
}

CString CJavaClass::GetTypeFromJava(CString strType) {
	int nArrayCount = 0;
	CString strTypeString;

	if (strType.Find("[") == 0) {
		for (int i = 0; i < strType.GetLength(); i++) {
			if (strType[i] == '[') {
				nArrayCount++;
			}
			else {
				strType = strType.Right(strType.GetLength() - nArrayCount);
				break;
			}
		}
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
	if (strType.Compare("B") == 0) {
		strTypeString = CString("byte");
	}
	else if (strType.Compare("C") == 0) {
		strTypeString = CString("char");
	}
	else if (strType.Compare("D") == 0) {
		strTypeString = CString("double");
	}
	else if (strType.Compare("F") == 0) {
		strTypeString = CString("float");
	}
	else if (strType.Compare("I") == 0) {
		strTypeString = CString("int");
	}
	else if (strType.Compare("J") == 0) {
		strTypeString = CString("long");
	}
	else if (strType.Compare("S") == 0) {
		strTypeString = CString("short");
	}
	else if (strType.Compare("V") == 0) {
		strTypeString = CString("void");
	}
	else if (strType.Compare("Z") == 0) {
		strTypeString = CString("boolean");
	}
	else if (strType.Find("L") == 0) {
		strTypeString = strType;
		strTypeString.Trim();
		strTypeString = strTypeString.Right(strTypeString.GetLength() - 1);
	}
	else {
		printf("未知的类型 %s \n", strType);
		strTypeString = CString("UnknowType");
	}

	strTypeString.Replace("/", ".");
	strTypeString.Replace(";", "");
	strTypeString.Trim();


	while (nArrayCount--) {
		strTypeString += CString("[]");
	}

	return strTypeString;
}

// 返回类型列表
std::vector<CString> CJavaClass::GetFieldSymbolList(CString strLine) {
	std::vector<CString> listSymbol;

	BOOL bInString = false;
	BOOL bChangeSymbol = false;
	CString strNote;

	listSymbol.clear();
	strLine += CString("  ");	// 优化一下下面的代码处理流程

	int nState = 0;
	for (int i = 0; i < strLine.GetLength(); i++) {
		char _c = strLine[i];

		switch (nState) {
		case 0:		// 刚开始
			strNote.Empty();

			if (
				((_c >= 'a') && (_c <= 'z')) || ((_c >= 'A') && (_c <= 'Z')) ||
				(_c == '_') || (_c == '.') || (_c == '[')
				)
			{
				strNote += _c;
				nState = 1;  // 进入标准符号
			}
			else if (
				_c == '\''
				) {
				strNote += _c;
				nState = 2;	 // 进入字节内容
			}
			else if (
				_c == '\"'
				) {
				strNote += _c;
				nState = 3;	 // 进入文本内容
			}
			else if (
				(_c == '=') || (_c == '+') || (_c == '-')
				)
			{
				strNote += _c;
				nState = 4;	 // 进入符号
			}
			else if ((_c >= '0') && (_c <= '9')) {
				strNote += _c;
				nState = 5;	 // 进入数字
			}
			else if ((_c == ' ') || (_c == '\t'))
			{
				//  这里是合法的空格
			}
			else {
				printf("Unknow Symbol %c at %d of %s\n", _c, i, strLine.GetBuffer());
				strLine.ReleaseBuffer();
			}
			break;
		case 1:	// 标准符号
			if (
				((_c >= 'a') && (_c <= 'z')) || ((_c >= 'A') && (_c <= 'Z')) || ((_c >= '0') && (_c <= '9')) ||
				(_c == '_') || (_c == '&') || (_c == '/') || (_c == ':') || (_c == ';') || (_c == '.') || (_c == '[')
				)
			{
				strNote += _c;
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

			break;
		case 2:	// 进入字节内容

			if (bChangeSymbol) {
				strNote += _c;
				bChangeSymbol = false;
			}
			else {
				if ((_c == '\\'))
				{
					strNote += _c;
					bChangeSymbol = true;
				}
				else if ((_c == '\''))
				{
					strNote += _c;
					i--; nState = 0;
					strNote.Trim();
					listSymbol.push_back(strNote);
				}
				else {
					strNote += _c;
				}
			}

			break;

		case 3:	// 进入字串内容

			if (bChangeSymbol) {
				strNote += _c;
				bChangeSymbol = false;
			}
			else {
				if ((_c == '\\'))
				{
					strNote += _c;
					bChangeSymbol = true;
				}
				else if ((_c == '\"'))
				{
					strNote += _c;
					i--; nState = 0;
					strNote.Trim();
					listSymbol.push_back(strNote);
				}
				else {
					strNote += _c;
				}
			}
			break;

		case 4:	// 进入符号
		{
			CString strTmp = (strNote + _c);

			if ((strTmp.Compare("++") == 0) || (strTmp.Compare("+=") == 0) || (strTmp.Compare("--") == 0) || (strTmp.Compare("-=") == 0) || (strTmp.Compare("==") == 0)) {
				strNote += _c;
				nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

		}
		break;
		case 5:	// 进入数字
			if (
				((_c >= '0') && (_c <= '9')) ||
				(_c == 'x') || (_c == 'X') || (_c == 'l') || (_c == 'L') || (_c == '.')
				)
			{
				strNote += _c;
			}
			else {
				i--; nState = 0;
				strNote.Trim();
				listSymbol.push_back(strNote);
			}

			break;
		}
	}

	return listSymbol;
}

// 开始分析
BOOL CJavaClass::AnalyzeClassSmaliListString(std::vector<CString> listCode) {

	for (unsigned int i = 0; i < listCode.size(); i++) {
		CString strLine = listCode[i];
		if (strLine.Trim().GetLength() > 0) {

			if (strLine.Find("#") == 0) {						// 这个是注释行
				TRACE("处理 %s\n", strLine);
			}
			else if (strLine.Find(".class") == 0) {				// 找到当前Class的名称
				int nFindL = strLine.Find("L");
				strClassAttribute.Empty();
				if (nFindL > 0) {
					strFullClassName = GetTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));

					strClassAttribute = strLine.Left(nFindL);
					strClassAttribute = strClassAttribute.Right(strClassAttribute.GetLength() - strlen(".class"));
					strClassAttribute.Trim();
				}
				else {
					strFullClassName = CString("Unknow.Class.Name");
					TRACE("Class 名称处理错误\n");
				}

				// 获得包名和短类名
				GetPackageAndClassName(strFullClassName, strPackageName, strClassName);

				// 内部子类
				strClassName.Replace("$", ".");
			}
			else if (strLine.Find(".super") == 0) {				// 父类的名称
				int nFindL = strLine.Find("L");
				if (nFindL > 0) {
					strSuperName = GetTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
				}
				else {
					strSuperName = CString("Unknow.Class.Name");
					TRACE("Super 名称处理错误\n");
				}

				if (strSuperName == CString("java.lang.Object")) {
					strSuperName.Empty();
				} else {
					CString strTmpPackage;
					CString strTmpClass;
					AddImportClassToList(strSuperName);
					GetPackageAndClassName(strSuperName, strTmpPackage, strTmpClass);
					strSuperName = strTmpClass;
				}
			}
			else if (strLine.Find(".implements") == 0) {		// 实现
				int nFindL = strLine.Find("L");
				CString strImplement;
				if (nFindL > 0) {
					strImplement = GetTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
				}
				else {
					strImplement = CString("Unknow.Class.Name");
					TRACE("Implements 名称处理错误\n");
				}

				CString strTmpPackage;
				CString strTmpClass;

				AddImportClassToList(strImplement);
				GetPackageAndClassName(strImplement, strTmpPackage, strTmpClass);
				strImplement = strTmpClass;

				listStrImplements.push_back(strImplement);
			}
			else if (strLine.Find(".field") == 0) {				// 成员变量
				BOOL bHasEq = false;
				CString strNote;
				CString valType;
				CString valName;
				CString valValue;
				CString strAttr;

				std::vector<CString> listSymbol = GetFieldSymbolList(strLine);
				std::vector<CString>::iterator it;
				for (it = listSymbol.begin(); it != listSymbol.end(); it++) {
					strNote = (*it);

					if (strNote.Find(".field") == 0) {
						//
					}
					else if (strNote.Find("private") == 0) {
						strAttr += CString("private ");
					}
					else if (strNote.Find("static") == 0) {
						strAttr += CString("static ");
					}
					else if (strNote.Find("final") == 0) {
						strAttr += CString("final ");
					}
					else if (strNote.Find("public") == 0) {
						strAttr += CString("public ");
					}
					else if (strNote.Find("protected") == 0) {
						strAttr += CString("protected ");
					}
					else if (strNote.Find("volatile") == 0) {
						strAttr += CString("volatile ");
					}
					else if (strNote.Find("transient") == 0) {
						strAttr += CString("transient ");
					}
					else if (strNote.Find("synthetic") == 0) {
						strAttr += CString("synthetic ");
					}
					else if (strNote.Find("enum") == 0) {
						strAttr += CString("enum ");
					}
					else if (strNote.Find("=") == 0) {
						bHasEq = true;
					}
					else if ((strNote.Find(":") > 0) && (bHasEq == false)) {
						int nFindDot = strNote.Find(":");
						valName = strNote.Left(nFindDot);
						valType = strNote.Right(strNote.GetLength() - nFindDot - 1);
						valName.Trim();
						valType.Trim();
					}
					else if (bHasEq) {
						valValue = strNote;
					}
					else {
						TRACE1("未知符号 strNote = %s\n", strNote.GetBuffer());
						strNote.ReleaseBuffer();
					}
				}

				// 补充成 Java 的代码
				CString cppString = strAttr;

				CString strFullTypeName = GetTypeFromJava(valType);
				CString strTmpPackage;
				CString strTmpClass;
				AddImportClassToList(strFullTypeName);
				GetPackageAndClassName(strFullTypeName, strTmpPackage, strTmpClass);

				cppString += (strTmpClass + CString(" ") + valName);
				if (bHasEq) {
					cppString += (CString(" = ") + valValue);
				}
				cppString += CString(";");
				TRACE1("Cpp Code: %s\n", cppString);

				listStrFields.push_back(cppString);
			}
			else if (strLine.Find(".method") == 0) {				// 成员方法

																	// 将整个method部分的代码放入 listMethodInst 数组
				std::vector<CString> listMethodInst;
				listMethodInst.clear();
				listMethodInst.push_back(strLine);

				CJavaMethod cJavaMethod;

				TRACE("###################################################################################################\n");
				TRACE1("开始处理函数 %s\n", strLine);

				for (i++; i < listCode.size(); i++) {
					if (strLine.Trim().GetLength() > 0) {			// 过滤掉空行
						listMethodInst.push_back(strLine);
						if (strLine.Find(".end method") == 0) {		// 结束位置，处理完结束
							break;
						}
						else if (strLine.Find(".locals") == 0)
						{
						}
						else {
							if (strLine.Find(":") == 0) {
							}
							else {
							}
						}
					}
				}

				// 处理Method数组
				cJavaMethod.strSuperClass = strSuperName;
				cJavaMethod.strClassName = strClassName;
				cJavaMethod.ProcessInstList(this, listMethodInst);

				listJavaMethods.push_back(cJavaMethod);
			}
			else if (strLine.Find(".end method") == 0) {
				TRACE("未成对处理的 method 结束\n");
			}
			else {
				TRACE1("未处理的行 %s\n", strLine.GetBuffer());
				strLine.ReleaseBuffer();
			}
		}
	}

	this->listJavaCode.clear();

	// 添加包名
	if (strPackageName.GetLength() > 0) {
		listJavaCode.push_back(CString("package ") + strPackageName + CString(";"));
	}
	listJavaCode.push_back(CString(""));

	// 添加引入的Class
	for (unsigned int i = 0; i < listStrImportClass.size(); i++) {
		listJavaCode.push_back(CString("import ") + listStrImportClass[i] + CString(";"));
	}
	listJavaCode.push_back(CString(""));


	// 添加申明类名
	CString strClassDefine;
	if (strClassAttribute.GetLength() > 0) {
		strClassDefine = strClassAttribute + CString(" ");
	}
	strClassDefine += (CString("class ") + strClassName);
	if (strSuperName.GetLength() > 0) {
		strClassDefine += (CString(" extends ") + strSuperName);
	}
	if (listStrImplements.size() > 0) {
		strClassDefine += (CString(" implements "));
		for (unsigned int i = 0; i < listStrImplements.size(); i++) {
			if ((i + 1) < listStrImplements.size()) {
				strClassDefine += (listStrImplements[i] + CString(","));
			}
			else {
				strClassDefine += (listStrImplements[i]);
			}
		}
	}
	strClassDefine += CString(" {");
	listJavaCode.push_back(strClassDefine);

	// 增加变量申明
	listJavaCode.push_back(CString(""));
	for (unsigned int i = 0; i < listStrFields.size(); i++) {
		listJavaCode.push_back(CString("    ") + listStrFields[i]);
	}
	listJavaCode.push_back(CString(""));


	// 增加函数
	for (unsigned int i = 0; i < listJavaMethods.size(); i++) {
		listJavaCode.push_back(CString(""));
		for ( unsigned int j = 0; j < listJavaMethods[i].listCppStringCode.size(); j++)
		listJavaCode.push_back(CString("    ") + listJavaMethods[i].listCppStringCode[j]);
	}
	



	// 添加类结束
	listJavaCode.push_back(CString(""));
	listJavaCode.push_back(CString("}"));
	

	return TRUE;
}

