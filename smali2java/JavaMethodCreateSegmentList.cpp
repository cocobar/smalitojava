#include "stdafx.h"
#include "JavaClass.h"
#include "JavaMethod.h"
#include "JavaInstructItem.h"

#include <algorithm>


// ������������þ��ǽ�����Method�Ĵ���ȫ���ֶ�
void CJavaMethod::CreateSegmentList(std::vector<CString> listMethodInst)
{
	std::vector<CString>::iterator it;
	std::vector<CString> listSwitchData;
	int nLineNumber = 0;

	CJavaMethodSegment instSegment;

	listCatchData.clear();
	listSwitchData.clear();
	instSegment.clear();
	instSegment.listSegName.push_back(CString("main"));

	// ����ɨ��һ��.catch .catchall
	int tmpIndex = 0;
	for (it = listMethodInst.begin(), tmpIndex++; it != listMethodInst.end(); it++, tmpIndex++) {
		CString strLine = (*it);
		CString strInstString = strLine;

		std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strInstString);

		// .catchall {:try_start_0 .. :try_end_0} :catchall_0
		// .catch Ljava/io/IOException; {:try_start_0 .. :try_end_0} :catch_0

		if ((listI[0].Compare(".catch") == 0) || (listI[0].Compare(".catchall") == 0)) {

			CTryCatchNode tryCatchNode;

			tryCatchNode.clear();
			int nFindDotPost = strLine.Find("}");
			CString strLabel = strLine.Right(strLine.GetLength() - nFindDotPost - 1).Trim();			// �����catch֮��ִ�еı�ǩ

																										// ȡ���м�{xxxx} ������
			strLine = strLine.Left(nFindDotPost);
			nFindDotPost = strLine.Find("{");
			strLine = strLine.Right(strLine.GetLength() - nFindDotPost - 1);

			nFindDotPost = strLine.Find("..");
			CString strStartLabel = strLine.Left(nFindDotPost).Trim();									// ��ʼ��ǩ
			CString strEndLabel = strLine.Right(strLine.GetLength() - nFindDotPost - 2).Trim();			// ������ǩ

			int nFindTryCatchIndex = -1;
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				if ((listCatchData[i].strStartLabel.Compare(strStartLabel) == 0) || (listCatchData[i].strEndLabel.Compare(strEndLabel) == 0)) {
					nFindTryCatchIndex = i;
					break;
				}
			}

			if (nFindTryCatchIndex >= 0) {		// �Ѿ�������
				if ((listI[0].Compare(".catch") == 0)) {
					CCatchItemNode cCatchItemNode;
					nFindDotPost = listI[1].Find("{");
					CString strType = CJavaClass::GetCppTypeFromJava(listI[1].Left(nFindDotPost).Trim());
					cCatchItemNode.strCatchType = strType;
					cCatchItemNode.strCatchLabel = strLabel;
					listCatchData[nFindTryCatchIndex].listCatch.push_back(cCatchItemNode);
				}
				else if ((listI[0].Compare(".catchall") == 0)) {
					if (listCatchData[nFindTryCatchIndex].strFinallyLabel.IsEmpty()) {
						listCatchData[nFindTryCatchIndex].strFinallyLabel = strLabel;
					}
					else {
						printf("�ж��finally����\n");
						while (1) {};
					}
				}
			}
			else {

				tryCatchNode.clear();
				tryCatchNode.strStartLabel = strStartLabel;
				tryCatchNode.strEndLabel = strEndLabel;
				tryCatchNode.strName.Format("VirtualTry%d", tmpIndex);


				if ((listI[0].Compare(".catch") == 0)) {
					CCatchItemNode cCatchItemNode;
					nFindDotPost = listI[1].Find("{");
					CString strType = CJavaClass::GetCppTypeFromJava(listI[1].Left(nFindDotPost).Trim());
					cCatchItemNode.strCatchType = strType;
					cCatchItemNode.strCatchLabel = strLabel;

					tryCatchNode.listCatch.push_back(cCatchItemNode);
				}
				else if ((listI[0].Compare(".catchall") == 0)) {
					if (listCatchData[nFindTryCatchIndex].strFinallyLabel.IsEmpty()) {
						tryCatchNode.strFinallyLabel = strLabel;
					}
					else {
						printf("�ж��finally����\n");
						while (1) {};
					}
				}
				listCatchData.push_back(tryCatchNode);
			}
		}
	}

	// ��ɨ��һ��switch�ṹ
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if ((strLine.Find("sparse-switch") == 0) || (strLine.Find("packed-switch") == 0)) {		// ����ǰ��
			int nDotFind = strLine.Find(":");
			CString strSwitchData = strLine.Right(strLine.GetLength() - nDotFind);
			strSwitchData.Trim();
			listSwitchData.push_back(strSwitchData);
		}
		else if (strLine.Find(":") == 0) {
			// �ж��Ƿ��� switch �ܹ�
			bool bFindSwitch = false;
			if (listSwitchData.size() > 0) {
				std::vector<CString>::iterator switchit;
				for (switchit = listSwitchData.begin(); switchit != listSwitchData.end(); switchit++) {
					if (strLine.Compare(*switchit) == 0) {
						bFindSwitch = true;
						break;
					}
				}
			}
			if (bFindSwitch) {
				bool bPackedSwitch = false;
				bool bSparseSwitch = false;
				unsigned int nPackedSwitchData = 0;
				CJavaPackedSwitchStruct packedSwitchStruct;
				packedSwitchStruct.strSwitchName = strLine;

				for (it++, nLineNumber++; it != listMethodInst.end(); it++, nLineNumber++) {
					strLine = (*it);
					if (strLine.Find(".packed-switch") == 0) {
						// ��ʼ
						CString strData = strLine.Right(strLine.GetLength() - CString(".packed-switch").GetLength());
						strData.Trim();
						bPackedSwitch = true;
						nPackedSwitchData = GetDataFromHex(strData);
					}
					else if (strLine.Find(".end packed-switch") == 0) {
						// ����
						if (!bPackedSwitch) {
							printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
						}
						else {
							listPackedSwitch.push_back(packedSwitchStruct);
						}
						break;
					}
					else if (strLine.Find(".sparse-switch") == 0) {
						// ��ʼ
						bSparseSwitch = true;
					}
					else if (strLine.Find(".end sparse-switch") == 0) {
						if (!bSparseSwitch) {
							printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
						}
						else {
							listPackedSwitch.push_back(packedSwitchStruct);
						}
						break;
					}
					else {
						if (bPackedSwitch && !bSparseSwitch) {
							if (strLine.Find(":") == 0) {
								CJavaSwitchItem switchItem;
								switchItem.nData = nPackedSwitchData++;
								switchItem.strSegmentName = strLine;
								packedSwitchStruct.listItem.push_back(switchItem);
							}
							else {
								printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
							}
						}
						else if (!bPackedSwitch && bSparseSwitch) {
							CJavaSwitchItem switchItem;
							unsigned int nData = 0;
							CString strLableName;
							int nFindDot = strLine.Find("->");
							if (nFindDot > 0) {
								nData = GetDataFromHex(strLine.Left(nFindDot));
							}
							nFindDot = strLine.Find(":");
							if (nFindDot > 0) {
								strLableName = strLine.Right(strLine.GetLength() - nFindDot);
							}
							switchItem.nData = nData;						// �б�����
							switchItem.strSegmentName = strLableName;		// ��Ӧ������
							packedSwitchStruct.listItem.push_back(switchItem);
						}
						else {
							printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
						}
					}
				}
			}
		}
	}

	// ���� try �ṹָ��
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if (strLine.Find(":") == 0) {
			strLine.Trim();
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				if (listCatchData[i].strStartLabel.Compare(strLine) == 0) {
					CString strTmpInst;
					strTmpInst.Format("try %s, %s", listCatchData[i].strName, listCatchData[i].strEndLabel);
					it = listMethodInst.insert(it, strTmpInst);
					it++;
					break;
				}
			}
		}
	}

	// ���� catch �ṹ
	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++) {
		CString strLine = (*it);
		if (strLine.Find(":") == 0) {
			strLine.Trim();
			for (unsigned int i = 0; i < listCatchData.size(); i++) {
				for (unsigned int j = 0; j < listCatchData[i].listCatch.size(); j++) {
					if (listCatchData[i].listCatch[j].strCatchLabel.Compare(strLine) == 0) {
						CString strTmpInst;
						strTmpInst.Format("catch %s, %s", CString("e0"), listCatchData[i].listCatch[j].strCatchType);
						it++;
						it = listMethodInst.insert(it, strTmpInst);
						break;
					}
				}
			}
		}
	}

	for (it = listMethodInst.begin(); it != listMethodInst.end(); it++, nLineNumber++) {
		CString strLineName;
		strLineName.Format("line%d", nLineNumber);
		CString strLine = (*it);

		if (strLine.Find(".method") == 0) {
			// ��������
			this->strMethodName = this->GetMethodName(strLine);
			this->strReturnType = this->GetReturnTypeFromMethods(strLine);
			this->listMethodAttr = this->GetMethodAttribute(strLine);
			this->listParamType = this->GetSignatureFromMethods(strLine);

			// ����Method��Ϣ
			this->strMethodInfo = strLine;

			// ����Ҫ���ɲ����б�
			listInputParam.clear();
			int nPStart = 1;
			if (find(listMethodAttr.begin(), listMethodAttr.end(), CString("static")) != listMethodAttr.end()) {
				nPStart = 0;
			}
			else {
				CMethodParam cMethodParam;
				cMethodParam.strReg = CString("p0");
				cMethodParam.strTypeName = CString("this");
				cMethodParam.strValueName = CString("this");
				listInputParam.push_back(cMethodParam);
			}


			for (unsigned int i = 0; i < listParamType.size(); i++) {

				CMethodParam cMethodParam;

				cMethodParam.strReg.Format("p%d", nPStart);
				cMethodParam.strTypeName = listParamType[i];
				cMethodParam.strValueName.Format("paramP%d", nPStart);

				if ((listParamType[i].Compare("long") == 0) || (listParamType[i].Compare("double") == 0)) {
					nPStart += 2;
				}
				else {
					nPStart += 1;
				}

				listInputParam.push_back(cMethodParam);
			}
		}
		else if (strLine.Find(".end method") == 0) {
			// ��������
		}
		else if (strLine.Find(".locals") == 0) {
			// ���ر������壬 �ô�����v XX�ı��� ����Ҫת��� val �ı���
		}
		else if ((strLine.Find(".catch") == 0) || (strLine.Find(".catchall") == 0)) {
			// �������������ˣ�ǰ���Ѿ�������
		}

		else if ((strLine.Find("sparse-switch") == 0) || (strLine.Find("packed-switch") == 0)) {		// ����ǰ��
																										// packed-switch ֮��� default ���ֵĽ������֣���Ҫ��case����ͳ�ƹ�ͬ�Ľ���λ�ã�
																										// Ĭ��switch֮��Ҫ����һ���Σ��������Ծ������������ default ���ֻ��� switch ��Ĳ���, ͳһʹ�� next ��������
																										// switch ָ��		packed-switch ��ʵ������ʵ���Ͼ���һ�����飬��ֵԪ���ǵ�ֵַ����������ֵ = baseֵ(low) + n
																										// switch ָ��		sparse-switch ʵ�ü�ֵ��ʵ�֣�һ��key��Ӧһ����ֵַ
			int nDotFind = strLine.Find(":");
			CString strSwitchData = strLine.Right(strLine.GetLength() - nDotFind);
			strSwitchData.Trim();
			listSwitchData.push_back(strSwitchData);

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();

			instSegment.listStrNext.push_back(CString("switch->") + strSwitchData);
			instSegment.listStrNext.push_back(CString("default->") + strLineName);
			instSegment.SegmentNextType = CJavaMethodSegment::nextSwitch;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("return") == 0) {			// ����ǰ��
														// ����ָ��
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			// ����һ���¶�
			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("return"));
			instSegment.SegmentNextType = CJavaMethodSegment::nextReturn;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("throw") == 0) {			// ����ǰ��
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			// ����һ���¶�
			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("throw"));
			instSegment.SegmentNextType = CJavaMethodSegment::nextThrow;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("if-") == 0) {				// ����ǰ�Σ�����else�ķ����ǣ�������㿪ʼ��������������Ĳ���

			std::vector<CString> listInstSeg = CJavaInstructItem::GetListInstructionSymbol(strLine);
			CString strLabel = listInstSeg[listInstSeg.size() - 1];
			strLabel.Trim();

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("if-y->") + strLabel);		// �������Ҫ��ת����
			instSegment.listStrNext.push_back(CString("if-n->") + strLineName);		// �����Ĭ�ϲ���ת����
			instSegment.SegmentNextType = CJavaMethodSegment::nextIf;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("try") == 0) {

			std::vector<CString> listI = CJavaInstructItem::GetListInstructionSymbol(strLine);

			// ����ָ��
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);

			instSegment.listStrNext.clear();

			instSegment.listStrNext.push_back(CString("try->") + listI[1]);
			instSegment.listStrNext.push_back(CString("default->") + listI[2]);
			instSegment.SegmentNextType = CJavaMethodSegment::nextTryCatch;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find("goto") == 0) {		// ����ǰ��

			std::vector<CString> listInstSeg = CJavaInstructItem::GetListInstructionSymbol(strLine);
			CString strLabel = listInstSeg[listInstSeg.size() - 1];
			strLabel.Trim();

			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);


			instSegment.listStrNext.clear();
			instSegment.listStrNext.push_back(CString("goto->") + strLabel);
			instSegment.SegmentNextType = CJavaMethodSegment::nextGoto;

			// ����һ���¶�
			this->listMethodSegment.push_back(instSegment);
			instSegment.clear();
			instSegment.listSegName.push_back(strLineName);
		}
		else if (strLine.Find(".") == 0) {
			printf("δ�����\n");
		}
		else if (strLine.Find(":") == 0) {

			// ������ҵ��б��
			if (instSegment.listInstSn.size() > 0) {

				if (instSegment.listStrNext.size() == 0) {
					instSegment.listStrNext.push_back(CString("next->") + strLine);
					instSegment.SegmentNextType = CJavaMethodSegment::nextNormal;
				}

				// ����һ���¶�
				this->listMethodSegment.push_back(instSegment);
				instSegment.clear();
			}
			instSegment.listSegName.push_back(strLine);
			//-------------------------------------------------------

			// �ж��Ƿ��� switch �ܹ�
			bool bFindSwitch = false;
			if (listSwitchData.size() > 0) {
				std::vector<CString>::iterator switchit;
				for (switchit = listSwitchData.begin(); switchit != listSwitchData.end(); switchit++) {
					if (strLine.Compare(*switchit) == 0) {
						bFindSwitch = true;
						break;
					}
				}
			}

			// ����Ĵ���ֻ�����������õ�
			if (bFindSwitch) {
				bool bPackedSwitch = false;
				bool bSparseSwitch = false;
				for (it++, nLineNumber++; it != listMethodInst.end(); it++, nLineNumber++) {
					strLine = (*it);
					if (strLine.Find(".packed-switch") == 0) {
						bPackedSwitch = true;
					}
					else if (strLine.Find(".end packed-switch") == 0) {
						if (!bPackedSwitch) {
							printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
						}
						break;
					}
					else if (strLine.Find(".sparse-switch") == 0) {
						bSparseSwitch = true;
					}
					else if (strLine.Find(".end sparse-switch") == 0) {
						if (!bSparseSwitch) {
							printf("��ʽ�д��� %s %d\n", __FILE__, __LINE__);
						}
						break;
					}
				}
			}
		}
		else {
			int nInstSn = AddInstructToOriginalList(strLine);
			instSegment.listInstSn.push_back(nInstSn);
		}
	}
}