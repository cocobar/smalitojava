#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"


CJavaClass::CJavaClass()
{
}


CJavaClass::~CJavaClass()
{
}

CString CJavaClass::GetCppTypeFromJava(CString strType) {
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
		strTypeString = CString("unsigned char");
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
		strTypeString = CString("bool");
	}
	else if (strType.Find("L") == 0) {
		strTypeString = strType;
#if 1
		strTypeString.Replace('/', '_');
		strTypeString.Replace('$', '_');
		strTypeString.Replace(';', ' ');
#else
		strTypeString.Remove('/');
		strTypeString.Remove('$');
		strTypeString.Remove(';');
#endif
		strTypeString.Trim();
		strTypeString = strTypeString.Right(strTypeString.GetLength() - 1);
	}
	else {
		printf("δ֪������ %s \n", strType);
		strTypeString = CString("UnknowType");
	}

	// ��Cpp��vector��Ƕ���������
#if 1
	while (nArrayCount--) {
		strTypeString = CString("std::vector<") + strTypeString + CString(">");
	}
#else
	while (nArrayCount--) {
		strTypeString += CString("[]");
	}
#endif

	return strTypeString;
}

// ���������б�
std::vector<CString> CJavaClass::GetFieldSymbolList(CString strLine) {
	std::vector<CString> listSymbol;

	BOOL bInString = false;
	BOOL bChangeSymbol = false;
	CString strNote;

	listSymbol.clear();
	strLine += CString("  ");	// �Ż�һ������Ĵ��봦������

	int nState = 0;
	for (int i = 0; i < strLine.GetLength(); i++) {
		char _c = strLine[i];

		switch (nState) {
		case 0:		// �տ�ʼ
			strNote.Empty();

			if (
				((_c >= 'a') && (_c <= 'z')) || ((_c >= 'A') && (_c <= 'Z')) ||
				(_c == '_') || (_c == '.') || (_c == '[')
				)
			{
				strNote += _c;
				nState = 1;  // �����׼����
			}
			else if (
				_c == '\''
				) {
				strNote += _c;
				nState = 2;	 // �����ֽ�����
			}
			else if (
				_c == '\"'
				) {
				strNote += _c;
				nState = 3;	 // �����ı�����
			}
			else if (
				(_c == '=') || (_c == '+') || (_c == '-')
				)
			{
				strNote += _c;
				nState = 4;	 // �������
			}
			else if ((_c >= '0') && (_c <= '9')) {
				strNote += _c;
				nState = 5;	 // ��������
			}
			else if ((_c == ' ') || (_c == '\t'))
			{
				//  �����ǺϷ��Ŀո�
			}
			else {
				printf("Unknow Symbol %c at %d of %s\n", _c, i, strLine.GetBuffer());
				strLine.ReleaseBuffer();
			}
			break;
		case 1:	// ��׼����
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
		case 2:	// �����ֽ�����

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

		case 3:	// �����ִ�����

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

		case 4:	// �������
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
		case 5:	// ��������
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

// ��ʼ������ Smali �ļ�
BOOL CJavaClass::AnalyzeClassSmali(CString strSmaliFile) {

	CStdioFile cStdFile;

	if (cStdFile.Open(strSmaliFile, CFile::modeRead)) {
		CString strLine;

		while (cStdFile.ReadString(strLine)) {

			if (strLine.Trim().GetLength() > 0) {

				if (strLine.Find("#") == 0) {						// �����ע����
					printf("���� %s\n", strLine);
				}
				else if (strLine.Find(".class") == 0) {				// �ҵ���ǰClass������
					int nFindL = strLine.Find("L");
					if (nFindL > 0) {
						strClassName = GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
						//strClassName = CJavaClass::GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
					}
					else {
						strClassName = CString("Unknow_Class_Name");
						printf("Class ���ƴ������\n");
					}
				}
				else if (strLine.Find(".super") == 0) {				// ���������
					int nFindL = strLine.Find("L");
					if (nFindL > 0) {
						strSuperName = GetCppTypeFromJava(strLine.Right(strLine.GetLength() - nFindL));
					}
					else {
						strSuperName = CString("Unknow_Class_Name");
						printf("Super ���ƴ������\n");
					}
				}
				else if (strLine.Find(".implements") == 0) {		// ʵ��
					int nFindL = strLine.Find("L");
					CString strImplement;
					if (nFindL > 0) {
						strImplement = strLine.Right(strLine.GetLength() - nFindL);
					}
					else {
						strImplement = CString("Unknow_Class_Name");
						printf("Implements ���ƴ������\n");
					}

					listStrImplements.push_back(strImplement);
				}
				else if (strLine.Find(".field") == 0) {				// ��Ա����
					unsigned int accessFlags = 0;
					BOOL bHasEq = false;
					CString strNote;
					CString valType;
					CString valName;
					CString valValue;

					std::vector<CString> listSymbol = GetFieldSymbolList(strLine);
					std::vector<CString>::iterator it;
					for (it = listSymbol.begin(); it != listSymbol.end(); it++) {
						strNote = (*it);

						if (strNote.Find(".field") == 0) {
							//
						}
						else if (strNote.Find("private") == 0) {
							accessFlags |= ACC_PRIVATE;
						}
						else if (strNote.Find("static") == 0) {
							accessFlags |= ACC_STATIC;
						}
						else if (strNote.Find("final") == 0) {
							accessFlags |= ACC_FINAL;
						}
						else if (strNote.Find("public") == 0) {
							accessFlags |= ACC_PUBLIC;
						}
						else if (strNote.Find("protected") == 0) {
							accessFlags |= ACC_PROTECTED;
						}
						else if (strNote.Find("volatile") == 0) {
							accessFlags |= ACC_VOLATILE;
						}
						else if (strNote.Find("transient") == 0) {
							accessFlags |= ACC_TRANSIENT;
						}
						else if (strNote.Find("synthetic") == 0) {
							accessFlags |= ACC_SYNTHETIC;
						}
						else if (strNote.Find("enum") == 0) {
							accessFlags |= ACC_ENUM;
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
							printf("δ֪���� strNote = %s\n", strNote.GetBuffer());
							strNote.ReleaseBuffer();
						}
					}

					// �����C++�Ĵ���
					CString cppString;

					if (accessFlags & ACC_PRIVATE) {
						cppString += CString("private ");
					}

					if (accessFlags & ACC_STATIC) {
						cppString += CString("static ");
					}

					if (accessFlags & ACC_FINAL) {
						cppString += CString("const ");
					}

					cppString += (GetCppTypeFromJava(valType) + CString(" ") + valName);
					if (bHasEq) {
						cppString += (CString(" = ") + valValue);
					}
					cppString += CString(";");
					printf("Cpp Code: %s\n", cppString);

					listStrFields.push_back(cppString);
				}
				else if (strLine.Find(".method") == 0) {				// ��Ա����

																		// ������method���ֵĴ������ listMethodInst ����
					std::vector<CString> listMethodInst;
					listMethodInst.clear();
					listMethodInst.push_back(strLine);

					CJavaMethod cJavaMethod;

					printf("###################################################################################################\n");
					printf("��ʼ������ %s\n", strLine);

					while (cStdFile.ReadString(strLine)) {
						if (strLine.Trim().GetLength() > 0) {			// ���˵�����
							listMethodInst.push_back(strLine);
							if (strLine.Find(".end method") == 0) {		// ����λ�ã����������
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

					// ����Method����
					cJavaMethod.strSuperClass = strSuperName;
					cJavaMethod.strClassName = strClassName;
					cJavaMethod.ProcessInstList(this, listMethodInst);

					listJavaMethods.push_back(cJavaMethod);
				}
				else if (strLine.Find(".end method") == 0) {
					printf("δ�ɶԴ���� method ����\n");
				}
				else {
					printf("δ������� %s\n", strLine.GetBuffer());
					strLine.ReleaseBuffer();
				}
			}
		}
		cStdFile.Close();
	}
	return TRUE;
}
